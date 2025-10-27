#include <cmath>
#include <numeric>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileInfo>
#include "calibration_module.h"
#include "daq_window.h"

#ifdef __linux__
#include <arpa/inet.h>
#include <sys/stat.h>
#elif __APPLE__
#include <arpa/inet.h>
#include <sys/stat.h>
#elif _WIN32
#include <windows.h>
#include <winsock.h>
#endif

#include <numeric>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

using namespace alglib;



void erfc_function(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = c[0]*0.5*errorfunctionc((x[0]-c[1])/(sqrt(2)* c[2])) + c[3];
}

void erfc_function_mirrored(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = c[0]*0.5*errorfunctionc((-x[0]+c[1])/(sqrt(2)* c[2])) + c[3];
}

void erfc_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    func = c[0]*0.5*errorfunctionc((x[0]-c[1])/(sqrt(2)* c[2])) + c[3];
    grad[0] = 0.5*errorfunctionc((x[0]-c[1])/(sqrt(2)* c[2]));
    grad[1] = (c[0]*0.5*sqrt(2/pi())/c[2])*exp(-(x[0]-c[1])*(x[0]-c[1])/(2*c[2]*c[2]));
    grad[2] = (c[0]*0.5*sqrt(2/pi())*(x[0]-c[1])/(c[2]*c[2]))*exp(-(x[0]-c[1])*(x[0]-c[1])/(2*c[2]*c[2]));
    grad[3] = 1;
}

void erfc_grad_mirrored(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    func = c[0]*0.5*errorfunctionc((-x[0]+c[1])/(sqrt(2)* c[2])) + c[3];
    grad[0] = 0.5*errorfunctionc((-x[0]+c[1])/(sqrt(2)* c[2]));
    grad[1] = (c[0]*0.5*sqrt(2/pi())/c[2])*exp(-(-x[0]+c[1])*(-x[0]+c[1])/(2*c[2]*c[2]));
    grad[2] = (c[0]*0.5*sqrt(2/pi())*(-x[0]+c[1])/(c[2]*c[2]))*exp(-(-x[0]+c[1])*(-x[0]+c[1])/(2*c[2]*c[2]));
    grad[3] = 1;
}


CalibrationModule::CalibrationModule(DAQWindow *top, QObject *parent) :
    QObject(parent),
    m_daqWindow{top},
    m_dbg(false),
    m_udpSocket(nullptr),
    m_msg(new MessageHandler),
    m_ignore16(false)
{

    m_calibrationArray[0] = nullptr;
    m_calibrationArray[1] = nullptr;
    m_calibrationArray[2] = nullptr;
    m_modeIndex = -1;
    m_data_modeIndex = -1;

    plotVector.push_back(m_daqWindow->m_ui->customPlot1);
    plotVector.push_back(m_daqWindow->m_ui->customPlot2);
    plotVector.push_back(m_daqWindow->m_ui->customPlot3);
    plotVector.push_back(m_daqWindow->m_ui->customPlot4);
    plotVector.push_back(m_daqWindow->m_ui->customPlot5);
    plotVector.push_back(m_daqWindow->m_ui->customPlot6);
    plotVector.push_back(m_daqWindow->m_ui->customPlot7);
    plotVector.push_back(m_daqWindow->m_ui->customPlot8);


    colorMapVector.push_back( new QCPColorMap(m_daqWindow->m_ui->customPlot1->xAxis, m_daqWindow->m_ui->customPlot1->yAxis));
    colorMapVector.push_back( new QCPColorMap(m_daqWindow->m_ui->customPlot2->xAxis, m_daqWindow->m_ui->customPlot2->yAxis));
    colorMapVector.push_back( new QCPColorMap(m_daqWindow->m_ui->customPlot3->xAxis, m_daqWindow->m_ui->customPlot3->yAxis));
    colorMapVector.push_back( new QCPColorMap(m_daqWindow->m_ui->customPlot4->xAxis, m_daqWindow->m_ui->customPlot4->yAxis));
    colorMapVector.push_back( new QCPColorMap(m_daqWindow->m_ui->customPlot5->xAxis, m_daqWindow->m_ui->customPlot5->yAxis));
    colorMapVector.push_back( new QCPColorMap(m_daqWindow->m_ui->customPlot6->xAxis, m_daqWindow->m_ui->customPlot6->yAxis));
    colorMapVector.push_back( new QCPColorMap(m_daqWindow->m_ui->customPlot7->xAxis, m_daqWindow->m_ui->customPlot7->yAxis));
    colorMapVector.push_back( new QCPColorMap(m_daqWindow->m_ui->customPlot8->xAxis, m_daqWindow->m_ui->customPlot8->yAxis));


    colorScaleVector.push_back( new QCPColorScale(m_daqWindow->m_ui->customPlot1));
    colorScaleVector.push_back( new QCPColorScale(m_daqWindow->m_ui->customPlot2));
    colorScaleVector.push_back( new QCPColorScale(m_daqWindow->m_ui->customPlot3));
    colorScaleVector.push_back( new QCPColorScale(m_daqWindow->m_ui->customPlot4));
    colorScaleVector.push_back( new QCPColorScale(m_daqWindow->m_ui->customPlot5));
    colorScaleVector.push_back( new QCPColorScale(m_daqWindow->m_ui->customPlot6));
    colorScaleVector.push_back( new QCPColorScale(m_daqWindow->m_ui->customPlot7));
    colorScaleVector.push_back( new QCPColorScale(m_daqWindow->m_ui->customPlot8));
    /*
    m_daqWindow->m_ui->customPlot1->plotLayout()->addElement(0, 1, colorScaleVector[0]);
    m_daqWindow->m_ui->customPlot2->plotLayout()->addElement(0, 1, colorScaleVector[1]);
    m_daqWindow->m_ui->customPlot3->plotLayout()->addElement(0, 1, colorScaleVector[2]);
    m_daqWindow->m_ui->customPlot4->plotLayout()->addElement(0, 1, colorScaleVector[3]);
    m_daqWindow->m_ui->customPlot5->plotLayout()->addElement(0, 1, colorScaleVector[4]);
    m_daqWindow->m_ui->customPlot6->plotLayout()->addElement(0, 1, colorScaleVector[5]);
    m_daqWindow->m_ui->customPlot7->plotLayout()->addElement(0, 1, colorScaleVector[6]);
    m_daqWindow->m_ui->customPlot8->plotLayout()->addElement(0, 1, colorScaleVector[7]);
*/

    for(int n=0; n<8;n++)
    {
        colorScaleVector[n]->setMinimumSize(0, 0);
        colorScaleVector[n]->setMaximumSize(0, 0);
        colorScaleVector[n]->setVisible(false);
        colorMapVector[n]->setVisible(false);

        plotVector[n]->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    }
    m_daqWindow->m_ui->choicePlotTime->setVisible(true);
    m_daqWindow->m_ui->choicePlotTime->setEnabled(true);
    m_daqWindow->m_ui->choiceBit->setVisible(false);
    m_daqWindow->m_ui->choiceBit->setEnabled(false);
    m_daqWindow->m_ui->pushButtonStoreCorrections->setEnabled(true);
    m_daqWindow->m_ui->pushButtonSavePDF->setEnabled(true);
    m_daqWindow->m_ui->pushButtonCSV->setEnabled(true);

    m_daqWindow->m_ui->comboBoxRunMode->setCurrentIndex(0);
    m_daqWindow->m_ui->comboBoxCalibrationType->setCurrentIndex(0);
    //Fill choices
    m_daqWindow->m_ui->choicePlotTime->clear();
    m_daqWindow->m_ui->choicePlotTime->addItem("calibrated ADC");
    m_daqWindow->m_ui->choicePlotTime->addItem("corrections");
    m_daqWindow->m_ui->choicePlotTime->addItem("fit");
    m_daqWindow->m_ui->choicePlotTime->addItem("ADC distribution");
    m_daqWindow->m_ui->choicePlotTime->setVisible(true);
    m_daqWindow->m_ui->choicePlotTime->setEnabled(true);
    m_daqWindow->m_ui->label_vmm->setText("Display VMMs");
    m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 0-7"));

    connect( m_daqWindow->m_ui->comboBoxFec, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updatePlot()));
    connect( m_daqWindow->m_ui->choicePlotTime, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updatePlot()));
    connect( m_daqWindow->m_ui->choiceBit, SIGNAL(currentIndexChanged(int)),
            this, SLOT(updatePlot()));

    connect( m_daqWindow->m_ui->comboBoxRunMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(calibAndPlotChoices()));
    connect( m_daqWindow->m_ui->comboBoxCalibrationType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(calibAndPlotChoices()));
    connect( m_daqWindow->m_ui->choicePlotTime, SIGNAL(currentIndexChanged(int)),
            this, SLOT(calibAndPlotChoices()));

}

void CalibrationModule::AutomaticLatencyCalib(bool isReset) {
    m_daqWindow->m_ui->comboBoxRunMode->setCurrentIndex(0);
    m_isAutomatic = true;
    if(isReset) {
        m_daqWindow->m_ui->comboBoxCalibrationType->setCurrentIndex(10);
    }
    else {
        m_daqWindow->m_ui->comboBoxCalibrationType->setCurrentIndex(11);
    }
    StartCalibration();
}

void CalibrationModule::calibAndPlotChoices()
{
    if(QObject::sender() == m_daqWindow->m_ui->comboBoxRunMode){
        m_daqWindow->m_calib->m_dataAvailable = false;
        m_daqWindow->m_ui->comboBoxCalibrationType->setCurrentIndex(-1);
        m_daqWindow->m_ui->comboBoxCalibrationType->clear();
        if(m_daqWindow->m_ui->comboBoxRunMode->currentIndex() == 0)
        {
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Offline ADC");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Offline Time (BCID/TDC)");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Offline Timewalk (time/ADC)");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("ADC");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("TDC");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("S-curve");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Threshold");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Pedestal");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Pulser DAC");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Threshold DAC");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Reset latency");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("TP latency");
            //m_daqWindow->m_ui->comboBoxCalibrationType->addItem("ADC via I2C");
            //ADC calib active
            m_daqWindow->m_ui->comboBoxCalibrationType->setCurrentIndex(0);
            m_daqWindow->m_ui->pushButtonSavePDF->setEnabled(true);
            m_daqWindow->m_ui->pushButtonCSV->setEnabled(true);
            m_daqWindow->m_ui->pushButtonStoreCorrections->setText("Corrections (json)");
            m_daqWindow->m_ui->pushButtonStoreCorrections->setEnabled(true);
            //Fill choices
            m_daqWindow->m_ui->choicePlotTime->clear();
            m_daqWindow->m_ui->choicePlotTime->addItem("calibrated ADC");
            m_daqWindow->m_ui->choicePlotTime->addItem("corrections");
            m_daqWindow->m_ui->choicePlotTime->addItem("fit");
            m_daqWindow->m_ui->choicePlotTime->addItem("ADC distribution");
            m_daqWindow->m_ui->choicePlotTime->setVisible(true);
            m_daqWindow->m_ui->choicePlotTime->setEnabled(true);
            // calibrated ADC active
            m_daqWindow->m_ui->choicePlotTime->setCurrentIndex(0);
            m_daqWindow->m_ui->choiceBit->setVisible(false);
            m_daqWindow->m_ui->choiceBit->setEnabled(false);
        }
        else if(m_daqWindow->m_ui->comboBoxRunMode->currentIndex() == 1)
        {
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Channels");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("ADC");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("TDC");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("BCID");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("ADC spectrum");
            m_daqWindow->m_ui->comboBoxCalibrationType->addItem("Time distribution");
            m_daqWindow->m_ui->pushButtonStoreCorrections->setEnabled(false);
            m_daqWindow->m_ui->pushButtonCSV->setEnabled(true);
            m_daqWindow->m_ui->pushButtonSavePDF->setEnabled(true);
            m_daqWindow->m_ui->choicePlotTime->setVisible(false);
            m_daqWindow->m_ui->choicePlotTime->setEnabled(false);
            m_daqWindow->m_ui->choiceBit->setVisible(false);
            m_daqWindow->m_ui->choiceBit->setEnabled(false);
        }
    }
    else if(QObject::sender() == m_daqWindow->m_ui->comboBoxCalibrationType){
        m_daqWindow->m_calib->m_dataAvailable = false;
        if(m_daqWindow->m_ui->comboBoxRunMode->currentIndex() == 0) {
            //S-curve per channel for pre-selected VMM
            if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 5 ) {
                m_daqWindow->m_ui->label_vmm->setText("Display VMM");
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("selected VMM"));
            }
            else if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 10 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 11) {
                m_daqWindow->m_ui->label_vmm->setText("Latency calibration");
                m_daqWindow->m_ui->comboBoxFec->clear();
                if(g_clock_source<2) {
                    m_daqWindow->m_ui->comboBoxFec->addItem(QString("all assisters"));
                }
                else {
                    m_daqWindow->m_ui->comboBoxFec->addItem(QString("all FECs"));
                }
                m_daqWindow->m_ui->pushButtonSavePDF->setEnabled(true);
            }
            //All others
            else {
                AddVMMsToCombobox();
            }
            if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 10 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 11) {
                m_daqWindow->m_ui->pushButtonCSV->setEnabled(false);
            }
            else {
                m_daqWindow->m_ui->pushButtonCSV->setEnabled(true);
            }
            if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 0 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 1 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 2) {
                m_daqWindow->m_ui->pushButtonStoreCorrections->setText("Corrections (json)");
                m_daqWindow->m_ui->pushButtonStoreCorrections->setEnabled(true);
            }
            else if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 3 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 4 ||
                     m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 6 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 10 ||  m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 11) {
                m_daqWindow->m_ui->pushButtonStoreCorrections->setText("Corrections (GUI)");
                m_daqWindow->m_ui->pushButtonStoreCorrections->setEnabled(true);
            }
            else {
                m_daqWindow->m_ui->pushButtonStoreCorrections->setEnabled(false);
            }

            if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 0 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 1 ||
                m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 7) {
                m_daqWindow->m_ui->pushButtonLog->setEnabled(true);
            }
            else {
                m_daqWindow->m_ui->pushButtonLog->setEnabled(false);
            }

            if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 0 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 1 || m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 2) {
                m_daqWindow->m_ui->pushButtonApplyCalib->setEnabled(true);
            }
            else {
                m_daqWindow->m_ui->pushButtonApplyCalib->setEnabled(false);
            }

            if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 0 ) {
                m_daqWindow->m_ui->choicePlotTime->clear();
                m_daqWindow->m_ui->choicePlotTime->addItem("calibrated ADC");
                m_daqWindow->m_ui->choicePlotTime->addItem("corrections");
                m_daqWindow->m_ui->choicePlotTime->addItem("fit");
                m_daqWindow->m_ui->choicePlotTime->addItem("ADC distribution");
                m_daqWindow->m_ui->choicePlotTime->setVisible(true);
                m_daqWindow->m_ui->choicePlotTime->setEnabled(true);
                m_daqWindow->m_ui->choicePlotTime->setCurrentIndex(0);
                m_daqWindow->m_ui->choiceBit->setVisible(false);
                m_daqWindow->m_ui->choiceBit->setEnabled(false);
            }
            else if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 1 ) {
                m_daqWindow->m_ui->choicePlotTime->clear();
                m_daqWindow->m_ui->choicePlotTime->addItem("calibrated time");
                m_daqWindow->m_ui->choicePlotTime->addItem("corrections");
                m_daqWindow->m_ui->choicePlotTime->addItem("fit");
                m_daqWindow->m_ui->choicePlotTime->addItem("TDC distribution");
                m_daqWindow->m_ui->choicePlotTime->addItem("BCID percentage");
                m_daqWindow->m_ui->choicePlotTime->setVisible(true);
                m_daqWindow->m_ui->choicePlotTime->setEnabled(true);
                m_daqWindow->m_ui->choicePlotTime->setCurrentIndex(0);
                m_daqWindow->m_ui->choiceBit->setVisible(false);
                m_daqWindow->m_ui->choiceBit->setEnabled(false);
            }
            else if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 2 ) {
                m_daqWindow->m_ui->choicePlotTime->clear();
                m_daqWindow->m_ui->choicePlotTime->addItem("calibrated time walk");
                m_daqWindow->m_ui->choicePlotTime->addItem("corrections");
                m_daqWindow->m_ui->choicePlotTime->addItem("fit");
                m_daqWindow->m_ui->choicePlotTime->setVisible(true);
                m_daqWindow->m_ui->choicePlotTime->setEnabled(true);
                m_daqWindow->m_ui->choicePlotTime->setCurrentIndex(0);
                m_daqWindow->m_ui->choiceBit->setVisible(false);
                m_daqWindow->m_ui->choiceBit->setEnabled(false);
            }
            else if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 5 ) {
                m_daqWindow->m_ui->choicePlotTime->clear();
                m_daqWindow->m_ui->choicePlotTime->addItem("Fit results per chip");
                m_daqWindow->m_ui->choicePlotTime->addItem("S-curve/gauss per channel");
                m_daqWindow->m_ui->choicePlotTime->setVisible(true);
                m_daqWindow->m_ui->choicePlotTime->setEnabled(true);
                m_daqWindow->m_ui->choicePlotTime->setCurrentIndex(0);
                m_daqWindow->m_ui->choiceBit->setVisible(false);
                m_daqWindow->m_ui->choiceBit->setEnabled(false);
            }
            else {
                m_daqWindow->m_ui->choicePlotTime->setVisible(false);
                m_daqWindow->m_ui->choicePlotTime->setEnabled(false);
                m_daqWindow->m_ui->choiceBit->setVisible(false);
                m_daqWindow->m_ui->choiceBit->setEnabled(false);
            }

        }
        else {
            AddVMMsToCombobox();
        }

    }
    else if(QObject::sender() == m_daqWindow->m_ui->choicePlotTime){
        if(!m_dataAvailable)
        {
            m_daqWindow->m_ui->choicePlotTime->setCurrentIndex(0);
            return;
        }
        if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 3) {
            m_daqWindow->m_ui->choiceBit->clear();
            if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 1 ) {
                for(int n=0;n< 8; n++) {
                    QString txt = QStringLiteral("%1 ns").arg(n*g_clock_period/8.0);
                    m_daqWindow->m_ui->choiceBit->addItem(txt);
                }
            }
            else if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 0 ) {
                for(int bit=0;bit< m_number_bits; bit++) {
                    QString txt = QStringLiteral("%1 DAC").arg(m_dac_setting[bit]);
                    m_daqWindow->m_ui->choiceBit->addItem(txt);
                }
            }
            m_daqWindow->m_ui->choiceBit->setVisible(true);
            m_daqWindow->m_ui->choiceBit->setEnabled(true);
        }
        else {
            m_daqWindow->m_ui->choiceBit->setVisible(false);
            m_daqWindow->m_ui->choiceBit->setEnabled(false);
        }

        // S-curve (fit per channel)
        if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() == 5) {
            m_daqWindow->m_ui->comboBoxFec->clear();
            if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("CH 0-7"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("CH 8-15"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("CH 16-23"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("CH 24-31"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("CH 32-39"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("CH 40-47"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("CH 48-55"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("CH 56-63"));
                m_daqWindow->m_ui->label_vmm->setText("Display Channels");
            }
            else {
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("selected VMM"));
                m_daqWindow->m_ui->label_vmm->setText("Display VMM");
            }
        }
        // ADC, time and time walk calib(depending on plot choice)
        else if(m_daqWindow->m_ui->comboBoxCalibrationType->currentIndex() <= 2 && m_daqWindow->m_ui->choicePlotTime->currentIndex() > 1) {
            m_daqWindow->m_ui->comboBoxFec->clear();
            m_daqWindow->m_ui->label_vmm->setText("Display Channels");
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 0-7"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 8-15"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 16-23"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 24-31"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 32-39"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 40-47"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 48-55"));
                m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 56-63"));
            }
        }
        else {
            AddVMMsToCombobox();
        }
        //All

    }
}



void CalibrationModule::LoadSettings(QString name) {
    m_daqWindow->LoadConfig(name);
    m_daqWindow->m_daq.SendAll(false);
    QThread::msleep(1);
    m_daqWindow->m_ui->line_configFile->setText(m_configText);
}


void CalibrationModule::SaveSettings(QString name) {
    m_daqWindow->m_daq.SendAll(false);
    m_configText = m_daqWindow->m_ui->line_configFile->displayText();
    m_daqWindow->m_ui->line_configFile->setText(name);
    m_daqWindow->SaveConfig(name);
    QThread::msleep(1);
    m_daqWindow->m_ui->line_configFile->setText(m_configText);
}

void CalibrationModule::StopDataTaking()
{
    std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Stop Data Taking" << std::endl;
    m_start = 0;
    m_end = 0;
    m_daqWindow->m_ui->onACQ->setEnabled(true);
    m_daqWindow->m_ui->offACQ->setEnabled(true);
    m_daqWindow->m_ui->onACQ->setChecked(false);
    m_daqWindow->m_ui->offACQ->setEnabled(true);
    m_daqWindow->m_ui->Send->setEnabled(true);
    m_daqWindow->m_daq.ACQHandler(false);
    m_daqWindow->m_ui->offACQ->setStyleSheet("QPushButton { color: red; }");
    m_daqWindow->m_ui->onACQ->setStyleSheet("QPushButton { color: black; }");
    QThread::msleep(500);
    CloseDAQSocket();
    m_daqWindow->m_ui->pushButtonTakeData->setChecked(false);

}
void CalibrationModule::StartDataTaking()
{
    ConnectDAQSocket();
    m_daqWindow->m_daq.ACQHandler(false);
    m_daqWindow->m_ui->onACQ->setCheckable(true);
    m_daqWindow->m_ui->onACQ->setChecked(true);
    m_daqWindow->m_ui->offACQ->setChecked(false);
    m_daqWindow->m_ui->Send->setEnabled(false);

    m_daqWindow->m_daq.SendAll(false);
    m_daqWindow->m_daq.ACQHandler(true);

    m_daqWindow->m_ui->offACQ->setStyleSheet("QPushButton { color: black; }");
    m_daqWindow->m_ui->onACQ->setStyleSheet("QPushButton { color: green; }");

    m_daqWindow->m_ui->pushButtonTakeData->setChecked(true);
}
// ------------------------------------------------------------------------ //
void CalibrationModule::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
    // remove monitoring m_daqMonitor->LoadMessageHandler(msg());
}
int CalibrationModule::GetCalibrationModeIndex(QString mode)
{
    if(mode == "Offline ADC")
    {
        return 1;
    }
    else if(mode == "Offline Time (BCID/TDC)")
    {
        return 2;
    }
    else if(mode == "Offline Timewalk (time/ADC)")
    {
        return 3;
    }
    if(mode == "Online ADC")
    {
        return 4;
    }
    if(mode == "Online TDC")
    {
        return 5;
    }
    if(mode == "S-curve")
    {
        return 6;
    }
    if(mode == "Threshold")
    {
        return 7;
    }
    if(mode == "Pedestal")
    {
        return 8;
    }
    if(mode == "Pulser DAC")
    {
        return 9;
    }
    if(mode == "Threshold DAC")
    {
        return 10;
    }
    if(mode == "Reset latency")
    {
        return 11;
    }
    if(mode == "TP latency")
    {
        return 12;
    }
    if(mode == "Channels")
    {
        return 20;
    }
    if(mode == "ADC")
    {
        return 21;
    }
    if(mode == "TDC")
    {
        return 22;
    }
    if(mode == "BCID")
    {
        return 23;
    }
    if(mode == "ADC spectrum")
    {
        return 24;
    }
    if(mode == "Time distribution")
    {
        return 25;
    }


    return -1;
}

bool CalibrationModule::CheckModes()
{
    if(!m_dataAvailable || m_data_modeIndex != m_modeIndex || m_modeIndex < 1)
    {
        return false;
    }
    if(m_daqWindow->m_ui->comboBoxFec->currentIndex() == -1) {
        if(m_daqWindow->m_ui->comboBoxFec->count() == 0) {
            return false;
        }
        else {
            m_daqWindow->m_ui->comboBoxFec->setCurrentIndex(0);
        }
    }
    return true;
}

bool CalibrationModule::IsCalibration()
{
    if(m_runMode == "User")
    {
        return false;
    }
    return true;
}




void CalibrationModule::FitSCurve() {
    int fec = m_theFEC;
    int hybrid = m_theVMM / 2;
    int chip = m_theVMM % 2;
    for(unsigned int ch = 0; ch<64; ch++){
        real_2d_array x;
        x.setlength(m_number_bits,1);
        real_1d_array y;
        y.setlength(m_number_bits);
        real_1d_array w;
        w.setlength(m_number_bits);
        for(int n=0; n<m_number_bits;n++) {
            y[n] = 0;
            w[n] = 0;
            x[n][0] = 0;
        }
        double dac_min = 9999;
        double dac_max = 9999;
        bool startFound = false;
        bool endFound = false;
        int dataPoints = 0;

        for(int bit=0; bit<m_number_bits;bit++){
            int cnt = m_channel_y[ch][bit];
            double theTime = m_time[bit];
            if(theTime > 0) {
                double rate = cnt/(1000*theTime);
                //w[bit] = 1/cnt;
                x[bit][0] = m_dac_measured[fec][hybrid][chip][bit];
                y[bit] = rate;
                dataPoints++;
                if(m_scan_type == 0) {
                    if(rate > 9.5) {
                        dac_min = m_dac_measured[fec][hybrid][chip][bit];
                        startFound = true;
                    }
                    if(rate < 0.5 && m_dac_measured[fec][hybrid][chip][bit] > dac_min && dac_max == 9999) {
                        dac_max = m_dac_measured[fec][hybrid][chip][bit];
                        endFound = true;
                    }
                }
                else {
                    if(rate < 0.5) {
                        dac_min = m_dac_measured[fec][hybrid][chip][bit];
                        startFound = true;
                    }
                    if(rate > 9.5 && m_dac_measured[fec][hybrid][chip][bit] > dac_min && dac_max == 9999) {
                        dac_max = m_dac_measured[fec][hybrid][chip][bit];
                        endFound = true;
                    }
                }
            }
        }
        if(dataPoints == m_number_bits && startFound && endFound) {
            real_1d_array c;
            c.setlength(4);

            c[0] = 10.0;
            double sp_mean = dac_min + 0.5 * abs(dac_max - dac_min);
            double sp_sigma = abs(dac_max - dac_min) / 2.5;
            c[1] = sp_mean;
            c[2] = sp_sigma;
            c[3] = 0;


            double epsx = 0.000001;
            ae_int_t maxits = 0;
            ae_int_t info;
            lsfitstate state;
            lsfitreport rep;
            //lsfitcreatewfg(x, y, w, c, true, state);
            lsfitcreatefg(x, y, c, true, state);

            lsfitsetcond(state, epsx, maxits);
            if(m_scan_type == 0) {
                alglib::lsfitfit(state, erfc_function, erfc_grad);
            }
            else {
                alglib::lsfitfit(state, erfc_function_mirrored, erfc_grad_mirrored);
            }

            lsfitresults(state, info, c, rep);
            /*
            printf("%d\n", int(info));
            printf("scale %f\n", c[0]);
            printf("mean %f\n", c[1]);
            printf("sigma %f\n", c[2]);

            printf("error scale %f\n", rep.errpar[0]);
            printf("error mean %f\n", rep.errpar[1]);
            printf("error sigma %f\n", rep.errpar[2]);
            */

            m_fit_error_scale.push_back(rep.errpar[0]);
            m_fit_error_mean.push_back(rep.errpar[1]);
            m_fit_error_sigma.push_back(rep.errpar[2]);
            m_fit_scale.push_back(c[0]);
            m_fit_mean.push_back(c[1]);
            m_fit_sigma.push_back(c[2]);

        }
        else {
            m_fit_error_scale.push_back(-9999.0);
            m_fit_error_mean.push_back(-9999.0);
            m_fit_error_sigma.push_back(-9999.0);
            m_fit_scale.push_back(-9999.0);
            m_fit_mean.push_back(-9999.0);
            m_fit_sigma.push_back(-9999.0);
        }
    }
}

void CalibrationModule::FitErrorCurve() {
    if(m_calibrationArray[m_modeIndex-1] != nullptr)
    {
        while(m_calibrationArray[m_modeIndex-1]->count()) {
            m_calibrationArray[m_modeIndex-1]->pop_back();
        }
        delete m_calibrationArray[m_modeIndex-1];
    }
    m_calibrationArray[m_modeIndex-1] = new QJsonArray();
    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
        int fec = GetFEC(vmm);
        int hybrid = GetHybrid(vmm);
        int chip = GetVMM(vmm);


        for(unsigned int ch = 0; ch<64; ch++){
            real_2d_array x;
            x.setlength(m_number_bits,1);
            real_1d_array y;
            y.setlength(m_number_bits);
            real_1d_array w;
            w.setlength(m_number_bits);
            for(int n=0; n<m_number_bits;n++) {
                y[n] = 0;
                w[n] = 0;
                x[n][0] = 0;
            }

            double a_param;
            double b_param;
            double c_param;
            double d_param;
            int valid = 0;
            for(int bit =0; bit<m_number_bits; bit++){
                if(m_mean2[bit][fec][hybrid][chip][ch] != -9999.0 && m_mean[bit][fec][hybrid][chip][ch] != -9999.0) {
                    x[bit][0] =  m_mean2[bit][fec][hybrid][chip][ch];
                    //Force the time of the middle ADC value to zero
                    y[bit] =  m_mean[bit][fec][hybrid][chip][ch]-m_mean[(int)(m_number_bits/2)][fec][hybrid][chip][ch];
                    valid++;
                }
            }
            if(valid == m_number_bits) {
                real_1d_array c;
                c.setlength(4);

                c[0] = 30.0;
                c[1] = 0.0;
                c[2] = 350;
                c[3] = -5.0;

                double epsx = 0.000001;
                ae_int_t maxits = 0;
                ae_int_t info;
                lsfitstate state;
                lsfitreport rep;
                //lsfitcreatewfg(x, y, w, c, true, state);
                lsfitcreatefg(x, y, c, true, state);

                lsfitsetcond(state, epsx, maxits);
                alglib::lsfitfit(state, erfc_function, erfc_grad);
                //alglib::lsfitfit(state, erfc_function_mirrored, erfc_grad_mirrored);
                lsfitresults(state, info, c, rep);
                a_param=c[0];
                b_param=c[1];
                c_param=c[2];
                d_param=c[3];
                m_fit_a[fec][hybrid][chip].push_back(a_param);
                m_fit_b[fec][hybrid][chip].push_back(b_param);
                m_fit_c[fec][hybrid][chip].push_back(c_param);
                m_fit_d[fec][hybrid][chip].push_back(d_param);
                m_fit_error_rms[fec][hybrid][chip].push_back(rep.rmserror);
                m_fit_error_r2[fec][hybrid][chip].push_back(rep.r2);
            }
            else {
                a_param=-9999.0;
                b_param=-9999.0;
                c_param=-9999.0;
                d_param=-9999.0;
                m_fit_a[fec][hybrid][chip].push_back(a_param);
                m_fit_b[fec][hybrid][chip].push_back(b_param);
                m_fit_c[fec][hybrid][chip].push_back(c_param);
                m_fit_d[fec][hybrid][chip].push_back(d_param);
                m_fit_error_rms[fec][hybrid][chip].push_back(-9999.0);
                m_fit_error_r2[fec][hybrid][chip].push_back(-9999.0);
            }

            QJsonObject calibrationObject;
            QJsonArray aArray;
            QJsonArray bArray;
            QJsonArray cArray;
            QJsonArray dArray;
            aArray.push_back(a_param);
            bArray.push_back(b_param);
            cArray.push_back(c_param);
            dArray.push_back(d_param);
            int fecId = m_fecPosID[fec];
            calibrationObject.insert("fecID",fecId);
            calibrationObject.insert("vmmID",hybrid*2+chip);
            calibrationObject.insert("timewalk_a",aArray);
            calibrationObject.insert("timewalk_b",bArray);
            calibrationObject.insert("timewalk_c",cArray);
            calibrationObject.insert("timewalk_d",dArray);
            m_calibrationArray[m_modeIndex-1]->push_back(calibrationObject);
        }
    }
}


void CalibrationModule::Fit4PL() {
    if(m_calibrationArray[m_modeIndex-1] != nullptr)
    {
        while(m_calibrationArray[m_modeIndex-1]->count()) {
            m_calibrationArray[m_modeIndex-1]->pop_back();
        }
        delete m_calibrationArray[m_modeIndex-1];
    }
    m_calibrationArray[m_modeIndex-1] = new QJsonArray();
    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
        int fec = GetFEC(vmm);
        int hybrid = GetHybrid(vmm);
        int chip = GetVMM(vmm);

        QJsonArray aArray;
        QJsonArray bArray;
        QJsonArray cArray;
        QJsonArray dArray;

        for(unsigned int ch = 0; ch<64; ch++){
            ae_int_t points = m_number_bits;
            real_1d_array x;
            x.setlength(points);
            real_1d_array y;
            y.setlength(points);
            real_1d_array v;
            v.setlength(4);
            for(int n=0; n<m_number_bits;n++) {
                y[n] = 0;
                x[n] = 0;
            }
            double a;
            double b;
            double c;
            double d;

            int valid = 0;
            for(int bit =0; bit<m_number_bits; bit++){
                if(m_mean2[bit][fec][hybrid][chip][ch] != -9999.0 && m_mean[bit][fec][hybrid][chip][ch] != -9999.0) {
                    x[bit] =  m_mean2[bit][fec][hybrid][chip][ch];
                    //Force the time of the middle ADC value to zero
                    y[bit] =  m_mean[bit][fec][hybrid][chip][ch]-m_mean[(int)(m_number_bits/2)][fec][hybrid][chip][ch];
                    valid++;
                }
            }
            if(valid == m_number_bits) {
                lsfitreport rep;
                logisticfit4(x, y, points, a, b, c, d, rep);
                //limit the numbers to 3 significant digits
                a = std::round(1000*a)/1000;
                b = std::round(1000*b)/1000;
                c = std::round(1000*c)/1000;
                d = std::round(1000*d)/1000;

                m_fit_a[fec][hybrid][chip].push_back(a);
                m_fit_b[fec][hybrid][chip].push_back(b);
                m_fit_c[fec][hybrid][chip].push_back(c);
                m_fit_d[fec][hybrid][chip].push_back(d);
                m_fit_error_rms[fec][hybrid][chip].push_back(rep.rmserror);
                m_fit_error_r2[fec][hybrid][chip].push_back(rep.r2);
            }
            else {
                a=-9999.0;
                b=-9999.0;
                c=-9999.0;
                d=-9999.0;
                m_fit_a[fec][hybrid][chip].push_back(a);
                m_fit_b[fec][hybrid][chip].push_back(b);
                m_fit_c[fec][hybrid][chip].push_back(c);
                m_fit_d[fec][hybrid][chip].push_back(d);
                m_fit_error_rms[fec][hybrid][chip].push_back(-9999.0);
                m_fit_error_r2[fec][hybrid][chip].push_back(-9999.0);
            }
            aArray.push_back(a);
            bArray.push_back(b);
            cArray.push_back(c);
            dArray.push_back(d);
        }
        QJsonObject calibrationObject;
        int fecId = m_fecPosID[fec];
        calibrationObject.insert("fecID",fecId);
        calibrationObject.insert("vmmID",hybrid*2+chip);
        calibrationObject.insert("timewalk_a",aArray);
        calibrationObject.insert("timewalk_b",bArray);
        calibrationObject.insert("timewalk_c",cArray);
        calibrationObject.insert("timewalk_d",dArray);
        m_calibrationArray[m_modeIndex-1]->push_back(calibrationObject);
    }
}


void CalibrationModule::FitLinearData()
{
    if(m_calibrationArray[m_modeIndex-1] != nullptr)
    {
        while(m_calibrationArray[m_modeIndex-1]->count()) {
            m_calibrationArray[m_modeIndex-1]->pop_back();
        }
        delete m_calibrationArray[m_modeIndex-1];
    }
    m_calibrationArray[m_modeIndex-1] = new QJsonArray();


    double meanOffset = 0;
    double meanSlope = 0;

    std::vector<double> mean_offset_perChip;
    mean_offset_perChip.resize(m_vmmActs.size(), 0);

    std::vector<double> mean_slope_perChip;
    mean_slope_perChip.resize(m_vmmActs.size(), 0);

    int cntPerSystem = 0;
    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
        int fec = GetFEC(vmm);
        int hybrid = GetHybrid(vmm);
        int chip = GetVMM(vmm);

        int cntPerChip = 0;
        double slope=0.0;
        double offset=0;

        meanOffset = 0;
        meanSlope = 0;

        for(unsigned int ch = 0; ch<64; ch++){
            std::vector<double> x;
            std::vector<double> y;
            slope=0.0;
            offset=0.0;
            double sumX=0, sumY=0, sumXY=0, sumX2=0;

            //For each channel, fit a straight line through m_number_bits data points (for ADC calibration: DAC on x-axis, measured ADC on y-axis)
            //for time calibration: equidistant times, starting at 0 ns on x-axis, measured time on y-axis
            for(int bit =0; bit<m_number_bits; bit++){
                double theMean = 0;

                double theXValue = 0;
                //Offline ADC
                if(m_modeIndex == 1)
                {
                    //Since we do not know the correct ADC value for the x-axis, we use the measured pulse height in DAC
                    theXValue = m_dac_measured[fec][hybrid][chip][bit];
                    theMean = m_mean[bit][fec][hybrid][chip][ch];
                }
                //Offline Time
                else if(m_modeIndex == 2)
                {
                    //Y-values: TDC time
                    theMean = m_fit_y[bit][fec][hybrid][chip][ch];

                    int m =  m_fit_start_time[fec][hybrid][chip][ch]/(g_time_factor);
                    if(m_fit_start_time[fec][hybrid][chip][ch]/(g_time_factor) < 0) {
                        m = m - 1;
                    }
                    double firstTime = m*g_time_factor - m_fit_start_time[fec][hybrid][chip][ch];
                    theXValue = firstTime + bit*g_time_factor;


                }
                if(theMean != -9999.0) {
                    if(m_modeIndex < 3 || (m_modeIndex == 3 && bit < 3)) {
                        x.push_back(theXValue);
                        y.push_back(theMean);
                        sumX += theXValue;
                        sumY += theMean;
                        sumXY += theXValue * theMean;
                        sumX2 += theXValue * theXValue;
                    }
                }

            }
            unsigned long nPoints = x.size();
            if(nPoints > 0)
            {
                double xMean = sumX / static_cast<double>(nPoints);
                double yMean = sumY / static_cast<double>(nPoints);
                double denominator = sumX2 - sumX * xMean;
                if( std::fabs(denominator) > 1e-7 ) {
                    slope = (sumXY - sumX * yMean) / denominator;
                    offset = yMean - slope * xMean;
                }
                if(slope != 0)
                {
                    cntPerChip++;
                    cntPerSystem++;
                    meanSlope += slope;
                    meanOffset += offset;
                }

            }
            m_slope[fec][hybrid][chip].push_back(slope);
            m_offset[fec][hybrid][chip].push_back(offset);
        } //channel

        mean_offset_perChip[vmm] = meanOffset;
        mean_slope_perChip[vmm] = meanSlope;
        if(cntPerChip > 0) {
            mean_offset_perChip[vmm] = mean_offset_perChip[vmm]/cntPerChip;
            mean_slope_perChip[vmm] = mean_slope_perChip[vmm]/cntPerChip;
        }

    } // vmm



    if(cntPerSystem > 0)
    {

        for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int fecId = m_fecPosID[fec];
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            QJsonObject calibrationObject;
            QJsonArray offsetArray;
            QJsonArray slopeArray;
            if(m_modeIndex == 1) {
                m_dac_slope[fec][hybrid][chip] = mean_slope_perChip[vmm];
                m_dac_offset[fec][hybrid][chip] = mean_offset_perChip[vmm];

            }
            for(unsigned int ch = 0; ch<64; ch++){
                double slope = 1.0;
                double offset = 0;
                //Channel with data
                if(m_slope[fec][hybrid][chip][ch] != 0)
                {
                    if(m_modeIndex == 2) {
                        offset =  m_offset[fec][hybrid][chip][ch] - (m_fit_start_time[fec][hybrid][chip][ch])*m_slope[fec][hybrid][chip][ch];
                        slope = 1/m_slope[fec][hybrid][chip][ch];
                    }
                    else if(m_modeIndex == 1) {
                        offset =  m_offset[fec][hybrid][chip][ch] - m_dac_offset[fec][hybrid][chip]*m_slope[fec][hybrid][chip][ch]/m_dac_slope[fec][hybrid][chip];
                        slope = m_dac_slope[fec][hybrid][chip]/m_slope[fec][hybrid][chip][ch];
                    }
                }
                //limit the numbers to 3 significant digits
                slope = std::round(1000*slope)/1000;
                offset = std::round(1000*offset)/1000;
                //If the fit results in strange slope values, do not correct the channel
                if(slope < 0.0 || slope > 10) {
                    slope = 1.0;
                    offset = 0.0;
                }
                m_slope[fec][hybrid][chip][ch] = slope;
                m_offset[fec][hybrid][chip][ch] = offset;
                slopeArray.push_back(slope);
                offsetArray.push_back(offset);
            }
            calibrationObject.insert("fecID",fecId);
            calibrationObject.insert("fec",fec);
            calibrationObject.insert("vmmID",hybrid*2+chip);
            if(m_modeIndex == 1)
            {
                calibrationObject.insert("adc_offsets",offsetArray);
                calibrationObject.insert("adc_slopes",slopeArray);
            }
            else if(m_modeIndex == 2) {
                calibrationObject.insert("time_offsets",offsetArray);
                calibrationObject.insert("time_slopes",slopeArray);
            }
            m_calibrationArray[m_modeIndex-1]->push_back(calibrationObject);
        }
    }

}


void CalibrationModule::SaveToLog() {
    if(!m_dataAvailable || !IsCalibration()) return;
    if(! (m_modeIndex == 1 || m_modeIndex == 2 | m_modeIndex == 7)) {
        return;
    }
    double gain = 0;
    int polarity = 0;
    double peaktime = 0;
    double tac = 0;
    double bcclock = 0;
    int timing_at_threshold = 0;
    int fec = 0;
    int hybrid = 0;
    int chip = 0;
    QString fileName = "log.csv";
    QString measurementName;



    if(m_modeIndex == 1)
    {
        measurementName = "Offline_ADC";
    }
    else if(m_modeIndex == 2)
    {
        measurementName = "Offline_Time";
    }
    else if(m_modeIndex == 7)
    {
        measurementName = "Pedestal";
    }
    std::string stored_labels="";
    if(!m_hybrid_labels.empty()) {
        for(int n=0; n< m_hybrid_labels.size() - 1; n++) {
            stored_labels += m_hybrid_labels[n] + ",";
        }
        stored_labels += m_hybrid_labels[m_hybrid_labels.size()-1];
    }

    bool ok;
    QStringList list = QInputDialog::getText(nullptr, tr("Hybrid labels of connected hybrids"),
                                             tr("Please add hybrid labels (one per hybrid, not one per VMM) as comma separated list to write to the log!"),
                                             QLineEdit::Normal,stored_labels.c_str(),&ok).split(",");

    if(!ok) {
        return;
    }
    if( list.count() > 0) {
        m_hybrid_labels.clear();
        for(int n=0; n<list.count();n++) {
            //std::cout << n << " " << list.count() << " " << list[n].toStdString() << std::endl;
            m_hybrid_labels.push_back(list[n].toStdString());
        }
    }
    for(int n=list.count(); n<m_vmmActs.size()*0.5; n++) {
        m_hybrid_labels.push_back("");
    }

    if(m_outFile.is_open())
    {
        m_outFile.close();
    }
    struct stat buffer;
    QString theName = m_daqWindow->GetApplicationPath() + "/" + fileName;
    if(stat (theName.toStdString().c_str(), &buffer) == 0) {
        m_outFile.open(theName.toStdString().c_str(), std::ofstream::out|std::ofstream::app);
    }
    else {
        m_outFile.open(theName.toStdString().c_str(), std::ofstream::out);
        m_outFile << "date, hybrid_label, hybrid_id, test, gain, peaktime, bcclock, tac, polarity, timing_at_threshold, vmm0 details, vmm0 summary, vmm1 details, vmm1 summary, hybrid\n";
    }
    QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    std::vector<int> faultyChannels0;
    std::vector<int> faultyChannels1;
    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){

        fec = GetFEC(vmm);
        hybrid = GetHybrid(vmm);
        chip = GetVMM(vmm);

        unsigned long size=0;
        double norm =0;
        double mean=0;
        double median=0;
        double smallest=0;
        double largest=0;
        double smallestChannel=0;
        double largestChannel=0;
        double smallest10percent=0;
        double largest10percent=0;
        double stddev=0;
        bool isGoodVMM0 = true;
        bool isGoodVMM1 = true;
        if(m_modeIndex == 7) {
            size = m_mean[0][fec][hybrid][chip].size();
            norm = static_cast<double>(size);
            if(size>0)
            {
                mean = std::accumulate(m_mean[0][fec][hybrid][chip].begin(),m_mean[0][fec][hybrid][chip].end(),0ll)/norm;

                auto nth = m_mean[0][fec][hybrid][chip].begin() + (50*m_mean[0][fec][hybrid][chip].size())/100;
                std::nth_element(m_mean[0][fec][hybrid][chip].begin(), nth, m_mean[0][fec][hybrid][chip].end());
                median = *nth;

                nth = m_mean[0][fec][hybrid][chip].begin() + (10*m_mean[0][fec][hybrid][chip].size())/100;
                std::nth_element(m_mean[0][fec][hybrid][chip].begin(), nth, m_mean[0][fec][hybrid][chip].end());
                smallest10percent = *nth;

                nth = m_mean[0][fec][hybrid][chip].begin() + (90*m_mean[0][fec][hybrid][chip].size())/100;
                std::nth_element(m_mean[0][fec][hybrid][chip].begin(), nth, m_mean[0][fec][hybrid][chip].end());
                largest10percent = *nth;


                auto variance_func = [&mean, &size](double accumulator, const double& val) {
                    return accumulator + ((val - mean)*(val - mean) / (size - 1));
                };

                stddev = std::accumulate(m_mean[0][fec][hybrid][chip].begin(), m_mean[0][fec][hybrid][chip].end(), 0ll, variance_func);
                stddev = sqrt(stddev);

                std::vector<double>::iterator result = std::min_element(m_mean[0][fec][hybrid][chip].begin(), m_mean[0][fec][hybrid][chip].end());
                smallest = *result;
                smallestChannel = std::distance(m_mean[0][fec][hybrid][chip].begin(), result);
                result = std::max_element(m_mean[0][fec][hybrid][chip].begin(), m_mean[0][fec][hybrid][chip].end());
                largest = *result;
                largestChannel = std::distance(m_mean[0][fec][hybrid][chip].begin(), result);
            }
        }
        if(chip == 0) {
            faultyChannels0.clear();
            timing_at_threshold = m_timing_at_thr[fec][hybrid][chip];
            gain = m_gain[fec][hybrid][chip];
            polarity = m_polarity[fec][hybrid][chip];
            peaktime = m_shaping_time[fec][hybrid][chip];
            tac = m_tac_slope[fec][hybrid][chip];
            bcclock = 1000.0/g_clock_period;
            m_outFile << theDate.toStdString() << "," << m_hybrid_labels[vmm*0.5] << "," << m_hybrid_id[fec][hybrid] << "," << measurementName.toStdString() << "," << gain << "," << peaktime << "," << bcclock  << "," << tac << "," << polarity << "," << timing_at_threshold << ",";
            if(m_modeIndex == 1 || m_modeIndex == 2) {
                for(int ch = 0; ch<64; ch++){
                    if(m_slope[fec][hybrid][chip][ch] == 0) {
                        faultyChannels0.push_back(ch);
                    }
                }
                if(faultyChannels0.size() == 0) {
                    m_outFile << ",vmm0 number bad channels: 0,";
                }
                else {
                    m_outFile << "vmm0 bad channels: ";
                    for(auto err: faultyChannels0) {
                        m_outFile << err << " ";
                    }
                    m_outFile << ",vmm0 number bad channels: " << faultyChannels0.size() << ",";
                }
            }
            else {
                m_outFile << "mean (" << mean << " mV)  " << "median (" << median << " mV)  " << "stddev (" << stddev << " mV)  " << "smallest (" << smallest << " mV at channel " << smallestChannel <<")  "
                          << "smallest 10 percent (" << smallest10percent << " mV)  " << "biggest (" << largest << " mV at channel " << largestChannel <<")  "  << "biggest 10 percent (" << largest10percent << " mV),";
                std::string summary = "ok";
                if(largest - smallest > 10 || stddev > 5) {
                    summary = "bad";
                    isGoodVMM0 = false;
                }
                m_outFile << summary << ",";
            }
        }
        else {
            faultyChannels1.clear();
            if(m_modeIndex == 1 || m_modeIndex == 2) {
                for(int ch = 0; ch<64; ch++){
                    if(m_slope[fec][hybrid][chip][ch] == 0) {
                        faultyChannels1.push_back(ch);
                    }
                }
                if(faultyChannels1.size() == 0) {
                    m_outFile << ",vmm1 number bad channels: 0,";
                }
                else {
                    m_outFile << "vmm1 bad channels: ";
                    for(auto err: faultyChannels1) {
                        m_outFile << err << " ";
                    }
                    m_outFile << ",vmm1 number bad channels: " << faultyChannels1.size() << ",";
                }

                if((faultyChannels0.size() == 0) && (faultyChannels1.size() == 0)) {
                    m_outFile << "ok\n";
                }
                else {
                    m_outFile << "hybrid number bad channels: " << faultyChannels0.size() + faultyChannels1.size() << "\n";
                }
            }
            else {
                m_outFile << "mean (" << mean << " mV)  " << "median (" << median << " mV)  " << "stddev (" << stddev << " mV)  " << "smallest (" << smallest << " mV at channel " << smallestChannel <<")  "
                          << "smallest 10 percent (" << smallest10percent << " mV)  " << "biggest (" << largest << " mV at channel " << largestChannel <<")  "  << "biggest 10 percent (" << largest10percent << " mV),";
                std::string summary = "ok";
                if(largest - smallest > 10 || stddev > 5) {
                    summary = "bad";
                    isGoodVMM1 = false;
                }
                m_outFile << summary << ",";
                if(isGoodVMM0 == false || isGoodVMM1 == false) {
                    m_outFile << "bad\n";
                }
                else {
                    m_outFile << "ok\n";
                }

            }
        }
    }
    m_outFile.close();
}


void CalibrationModule::ApplyCalib(bool isTimewalk) {
    if( !IsCalibration() ) return;
    if( m_modeIndex > 3 ) return;
    if( (!m_dataAvailable && m_modeIndex <= 2) ) return;
    QString pathStr = m_daqWindow->GetApplicationPath() + "/../calibs";
    QString dirStr;
    if(m_modeIndex==3) {
        if(isTimewalk) {
            dirStr = QFileDialog::getOpenFileName(nullptr, tr("Select calibration file with time and ADC calibration"),pathStr, tr("Text (*.json)"));
        }
        else {
            dirStr = QFileDialog::getOpenFileName(nullptr, tr("Select calibration file with time walk calibration"), pathStr, tr("Text (*.json)"));
        }
    }
    else if(m_modeIndex==2) {
        dirStr = QFileDialog::getOpenFileName(nullptr, tr("Select calibration file with ADC calibration"), pathStr, tr("Text (*.json)"));
    }
    else if(m_modeIndex==1) {
        dirStr = QFileDialog::getOpenFileName(nullptr, tr("Select calibration file with time calibration"), pathStr, tr("Text (*.json)"));
    }

    if(dirStr == "") {
        return;
    }
    QString fname = dirStr.split("/").last();
    //std::cout << fname.toStdString() << " " <<  dirStr.toStdString() << std::endl;

    QMessageBox msgBox;
    QPushButton *hybridIDButton = msgBox.addButton(tr("Use hybrid ID"), QMessageBox::ActionRole);
    QPushButton *fecPositionButton = msgBox.addButton(tr("Use mapping (FEC, hybrid)"), QMessageBox::ActionRole);
    if(m_modeIndex==3 && isTimewalk) {
        msgBox.setText("ADC and time calibration will be used for timewalk calibration.");
    }
    else {
        msgBox.setText("Apply stored calibration to data.");
    }
    msgBox.setInformativeText("Do you want to apply the calibration using the hybrid ID or the mapping (FEC, hybrid)?");
    msgBox.exec();
    bool useHybridID = false;
    if (msgBox.clickedButton() == hybridIDButton) {
        useHybridID = true;
    }

    QFile file;
    file.setFileName(dirStr);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    QString val = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());

    QJsonObject jsonObject = doc.object();
    QJsonArray jsonArray = jsonObject["vmm_calibration"].toArray();

    foreach (const QJsonValue & value, jsonArray) {
        int fec = 0;
        int hybrid = 0;
        int chip = 0;
        QString hybridID = "";
        const auto& obj = value.toObject();
        int fecID = obj["fecID"].toInt();
        fec = m_fecIDPos[fecID];
        //std::cout << "FEC ID " << fecID << " " << fec << std::endl;

        hybridID = obj["hybridID"].toString();
        //std::cout << "hybridID " << hybridID.toStdString() << std::endl;

        chip = obj["vmmID"].toInt()%2;
        hybrid = obj["vmmID"].toInt()/2;
        if(useHybridID) {
            for(int f=0; f < FECS_PER_DAQ; f++) {
                for(int h=0; h < HYBRIDS_PER_FEC; h++) {
                    if(m_hybrid_id[f][h] == hybridID.toStdString()) {
                        fec = f;
                        hybrid = h;
                        break;
                    }
                }
            }
        }

        if(m_modeIndex == 1 || m_modeIndex == 3) {
            auto const & slopes = obj["adc_slopes"].toArray();
            auto const & offsets = obj["adc_offsets"].toArray();
            if(!slopes.empty() && !offsets.empty()) {
                m_file_adc_slope[fec][hybrid][chip].clear();
                m_file_adc_offset[fec][hybrid][chip].clear();
            }
            for(const auto &val: slopes) {
                m_file_adc_slope[fec][hybrid][chip].push_back(val.toDouble());
                //std::cout << "adc slopes " << fec << " " << hybrid << " " << chip << " " << val.toDouble() << std::endl;
            }
            for(const auto &val: offsets) {
                m_file_adc_offset[fec][hybrid][chip].push_back(val.toDouble());
                //std::cout << "adc offsets " << fec << " " << hybrid << " " << chip << " " << val.toDouble() << std::endl;
            }
        }
        if(m_modeIndex == 2 || m_modeIndex == 3) {
            auto const & slopes = obj["time_slopes"].toArray();
            auto const & offsets = obj["time_offsets"].toArray();
            if(!slopes.empty() && !offsets.empty()) {
                m_file_time_slope[fec][hybrid][chip].clear();
                m_file_time_offset[fec][hybrid][chip].clear();
            }
            for(const auto &val: slopes) {
                m_file_time_slope[fec][hybrid][chip].push_back(val.toDouble());
                //std::cout << "time slopes " << fec << " " << hybrid << " " << chip << " " << val.toDouble() << std::endl;
            }
            for(const auto &val: offsets) {
                m_file_time_offset[fec][hybrid][chip].push_back(val.toDouble());
                //std::cout << "time offsets " << fec << " " << hybrid << " " << chip << " " << val.toDouble() << std::endl;
            }

        }
        if(m_modeIndex == 3 && !isTimewalk) {
            auto const & arrayA = obj["timewalk_a"].toArray();
            auto const & arrayB = obj["timewalk_b"].toArray();
            auto const & arrayC = obj["timewalk_c"].toArray();
            auto const & arrayD = obj["timewalk_d"].toArray();
            if(!arrayA.empty() && !arrayB.empty() && !arrayC.empty() && !arrayD.empty()) {
                m_file_timewalk_a[fec][hybrid][chip].clear();
                m_file_timewalk_b[fec][hybrid][chip].clear();
                m_file_timewalk_c[fec][hybrid][chip].clear();
                m_file_timewalk_d[fec][hybrid][chip].clear();
            }
            for(const auto &val: arrayA) {
                m_file_timewalk_a[fec][hybrid][chip].push_back(val.toDouble());
                //std::cout << "timewalk a " << val.toDouble() << std::endl;
            }
            for(const auto &val: arrayB) {
                m_file_timewalk_b[fec][hybrid][chip].push_back(val.toDouble());
                //std::cout << "timewalk b " << val.toDouble() << std::endl;
            }
            for(const auto &val: arrayC) {
                m_file_timewalk_c[fec][hybrid][chip].push_back(val.toDouble());
                //std::cout << "timewalk c " << val.toDouble() << std::endl;
            }
            for(const auto &val: arrayD) {
                m_file_timewalk_d[fec][hybrid][chip].push_back(val.toDouble());
                //std::cout << "timewalk d " << val.toDouble() << std::endl;
            }
        }

    }
    PlotData();
}


void CalibrationModule::SaveCorrectionsEFU(QString name){
    /*
    int m_modeIndex=1;
    QJsonArray *m_calibrationArray[3];
    m_calibrationArray[0] = new QJsonArray();
    QString m_hybrid_id[8][8];
    for(int k=0;k<8;k++) {
        for(int n=0;n<8;n++) {
            m_hybrid_id[k][n] = "a08000e4fccca08034100820067044" + QString::number(k) + QString::number(n);
        }
    }

    QVector<int> m_fec;
    m_fec.push_back(0);
    m_fec.push_back(0);
    m_fec.push_back(0);
    m_fec.push_back(0);
    m_fec.push_back(0);
    m_fec.push_back(1);
    m_fec.push_back(1);
    m_fec.push_back(1);
    m_fec.push_back(1);
    m_fec.push_back(1);
    m_fec.push_back(2);
    m_fec.push_back(2);
    m_fec.push_back(2);
    m_fec.push_back(2);

    QVector<int> m_hybrid;
    m_hybrid.push_back(0);
    m_hybrid.push_back(1);
    m_hybrid.push_back(2);
    m_hybrid.push_back(3);
    m_hybrid.push_back(4);
    m_hybrid.push_back(0);
    m_hybrid.push_back(1);
    m_hybrid.push_back(2);
    m_hybrid.push_back(3);
    m_hybrid.push_back(4);
    m_hybrid.push_back(0);
    m_hybrid.push_back(1);
    m_hybrid.push_back(2);
    m_hybrid.push_back(3);
    for(int vmm = 0; vmm < 28; vmm++){
        int fec = m_fec[vmm/2];
        int hybrid = m_hybrid[vmm/2];
        int chip = vmm%2;
        QJsonObject calibrationObject;
        QJsonArray offsetArray;
        QJsonArray slopeArray;
        for(int ch = 0; ch<64; ch++){
            slopeArray.push_back(1.0);
            offsetArray.push_back(0.0);
        }
        calibrationObject.insert("fecID",fec*32);
        calibrationObject.insert("fec",fec);
        calibrationObject.insert("vmmID",hybrid*2+chip);
        calibrationObject.insert("adc_offsets",offsetArray);
        calibrationObject.insert("adc_slopes",slopeArray);
        m_calibrationArray[0]->push_back(calibrationObject);
    }
*/
    if(m_modeIndex == 1)
    {
        if( ! m_calibrationArray[0]) {
            return;
        }
        QString theName = name + "_calib";
        QString theDateTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
        QString theCalibDateTime = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
        QJsonObject globalObject;
        globalObject.insert("Detector","Freia");
        globalObject.insert("Version",1);
        globalObject.insert("Comment","v1: TDC and ADC corrections, no timewalk");
        QString theDate = QDateTime::currentDateTime().toString("yyyy MM dd");
        globalObject.insert("Date",theDate);
        globalObject.insert("Info","Generated by slowcontrol vmmdcs version 2023/11/04");
        QJsonArray history;
        history.push_back("2023-xx-yy Initial format");
        history.push_back("2023-10-10 remove unused hybrid index, add settings comment and header fields");
        globalObject.insert("History", history);
        QJsonArray tdcOffsetArray;
        QJsonArray tdcSlopeArray;
        QJsonArray calibrationArray;
        QJsonObject calibrationObject;
        QJsonObject arrayObject;
        for(int n=0; n<64;n++) {
            tdcOffsetArray.push_back(0);
            tdcSlopeArray.push_back(1);
        }

        int theVmmId= -1;
        int hybrid = -1;
        int vmm = -1;
        int fec = -1;
        foreach (const QJsonValue & value, *m_calibrationArray[0]) {
            const auto& obj = value.toObject();
            const auto& keys = obj.keys();
            QJsonArray adcOffsetArray;
            QJsonArray adcSlopeArray;
            for(const auto& key : keys){
                qDebug() << key;
                if(key == "vmmID") {
                    theVmmId = obj[key].toInt();
                    hybrid = theVmmId/2;
                    vmm = theVmmId%2;
                }
                if(key == "fec") {
                    fec = obj[key].toInt();
                }
                if(key == "adc_offsets") {
                    auto const & arr = obj[key].toArray();
                    for(const auto& v : arr){
                        adcOffsetArray.push_back(v.toDouble());
                    }
                }
                if(key == "adc_slopes") {
                    auto const & arr = obj[key].toArray();
                    for(const auto& v : arr) {
                        adcSlopeArray.push_back(v.toDouble());
                    }
                }
            }
            int gain = 1;
            int polarity = 0;
            if(vmm == 0) {
                gain = 2;
                polarity = 1;
                for(const auto& key : calibrationObject.keys()){
                    calibrationObject.remove(key);
                }
                calibrationObject.insert("HybridId",QString::fromStdString(m_hybrid_id[fec][hybrid]));
                //calibrationObject.insert("HybridId",m_hybrid_id[fec][hybrid]);
                calibrationObject.insert("CalibrationDate",theCalibDateTime);
            }
            QJsonObject vmmObject;
            QString settings;
            if(polarity == 0) {
                settings = "negative polarity";
            }
            else {
                 settings = "positive polarity";
            }
            switch(gain) {
            case 0:
                settings += ", gain 0.5 mV/fC";
                break;
            case 1:
                settings += ", gain 1.0 mV/fC";
                break;
            case 2:
                settings += ", gain 3.0 mV/fC";
                break;
            case 3:
                settings += ", gain 4.5 mV/fC";
                break;
            case 4:
                settings += ", gain 6.0 mV/fC";
                break;
            case 5:
                settings += ", gain 9.0 mV/fC";
                break;
            case 6:
                settings += ", gain 12.0 mV/fC";
                break;
            default:
                settings += ", gain 16.0 mV/fC";
            }
            vmmObject.insert("Settings", settings);
            vmmObject.insert("adc_offset",adcOffsetArray);
            vmmObject.insert("adc_slope",adcSlopeArray);
            vmmObject.insert("tdc_offset",tdcOffsetArray);
            vmmObject.insert("tdc_slope",tdcSlopeArray);
            if(vmm==0) {
                calibrationObject.insert("vmm0",vmmObject);
            }
            else {
                calibrationObject.insert("vmm1",vmmObject);
                arrayObject.insert("VMMHybridCalibration",calibrationObject);
                calibrationArray.push_back(arrayObject);
            }
        }
        globalObject.insert("Calibrations",calibrationArray);

        QString theDirectory = CreateDir("calibs", "");
        theName = theDirectory + "/" + theName + "_" + theDateTime + ".json";

        QFile jsonFile(theName);
        jsonFile.open(QFile::WriteOnly);
        QJsonDocument document(globalObject);
        jsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));
        jsonFile.close();
    }
}



void CalibrationModule::WriteSystemConfig() {
    //SaveCorrectionsEFU(g_instrument);
    bool ok;
    QString defaultName = "hybrid_mapping";
    QString theDirectory = CreateDir("calibs", "");
    QString theTime = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QString theDateTime = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");

    if(g_clock_source == 0 && g_slow_control==0 && g_instrument == "AMOR") {
        defaultName = g_instrument + "_config";
        QJsonObject globalObject;
        QJsonObject configObject;
        QJsonArray cassetteArray;
        globalObject.insert("Detector", "Freia");
        globalObject.insert("InstrumentGeometry", "AMOR");
        globalObject.insert("Version", 1);
        globalObject.insert("VersionStr", "v1: strip and wire thresholds per cassette, not per channel");
        globalObject.insert("Date", theDateTime);
        globalObject.insert("Info", "Generated by vmmdcs version 20231103");
        QJsonArray history;
        history.push_back("2023-09-12 Change hybrid order");
        history.push_back("2023-09-25 Added hybrid id's");
        history.push_back("2023-10-06 add info fields to config file, thresholds");
        globalObject.insert("History", history);
        globalObject.insert("Comment","layout for AMOR for the PSI visit 2023-10");
        globalObject.insert("WireChOffset",16);

        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if (m_daqWindow->m_daq.GetFEC(fec)){
                int ring = m_daqWindow->m_map_id_ring_fen[fec].first;
                int fen = m_daqWindow->m_map_id_ring_fen[fec].second;
                for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                    if(m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hyb)) {
                        QString hybridID = QString::fromStdString(m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].GetInfo("hybrid_id"));
                        //QString hybridID = "a08000e4fccca08034100820067044" + QString::number(fec)+ QString::number(hyb);
                        QString description = QString::fromStdString(m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].GetInfo("description"));
                        int cassette = 0;
                        if(description.toUpper().startsWith("CASSETTE")) {
                            cassette = description.mid(9,description.length()-9).toInt();
                        }
                        else {
                            if(description.length()<=2) {
                                cassette = description.mid(0,description.length()).toInt();
                            }
                        }

                        QJsonObject cassetteObject;
                        cassetteObject.insert("CassetteNumber",cassette);
                        cassetteObject.insert("Ring",ring);
                        cassetteObject.insert("FEN",fen);
                        cassetteObject.insert("Hybrid",hyb);
                        QJsonArray threshold0;
                        threshold0.push_back(0);
                        QJsonArray threshold1;
                        threshold1.push_back(0);
                        QJsonArray thresholds;
                        thresholds.push_back(threshold0);
                        thresholds.push_back(threshold1);
                        cassetteObject.insert("Thresholds",thresholds);
                        cassetteObject.insert("HybridId",hybridID);
                        cassetteArray.push_back(cassetteObject);
                    }
                }
            }
        }
        globalObject.insert("Config",cassetteArray);
        globalObject.insert("MaxPulseTimeNS",2071428570);
        globalObject.insert("MaxGapWire",1);
        globalObject.insert("MaxGapStrip",1);
        QString theName = theDirectory + "/" + defaultName + "_" + theTime + ".json";

        QFile jsonFile(theName);
        jsonFile.open(QFile::WriteOnly);
        QJsonDocument document(globalObject);
        jsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));
        jsonFile.close();
    }
    if(g_instrument == "NMX" || g_clock_source > 0) {

        QString theName = QInputDialog::getText(nullptr, tr("Create system configuration file (mapping of hybrids).."),
                                                tr("Below please state the name for the system calibration file"),
                                                QLineEdit::Normal,defaultName,&ok);
        QJsonObject globalObject;
        QJsonArray calibrationArray;
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if (m_daqWindow->m_daq.GetFEC(fec)){
                int fecID = m_fecPosID[fec];
                //Master (0) or Assister (1)
                if(g_slow_control < 2) {
                    int ring = m_daqWindow->m_map_id_ring_fen[fec].first;
                    int fen = m_daqWindow->m_map_id_ring_fen[fec].second;
                    fecID =  ring * 32 + fen;
                }
                for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                    QJsonObject calibrationObject;
                    if(m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hyb)) {
                        std::string hybridID = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hyb].GetInfo("hybrid_id");
                        //hybridID = "abcd";
                        if(hybridID == "ffffffffffffffffffffffffffffffff") {
                            QString theId = QString::fromStdString(hybridID) + "_" + QString::number(fecID) + "_" +  QString::number(hyb);
                            calibrationObject.insert("hybridID",theId);
                        }
                        else {
                            calibrationObject.insert("hybridID",QString::fromStdString(hybridID));
                        }
                        for(int n=0; n<2;n++) {
                            calibrationObject.insert("fecID",fecID);
                            calibrationObject.insert("vmmID",hyb*2+n);
                            calibrationArray.push_back(calibrationObject);
                        }
                    }
                }
            }
        }
        globalObject.insert("hybrid_mapping",calibrationArray);
        theName = theDirectory + "/" + theName + "_" + theTime + ".json";

        QFile jsonFile(theName);
        jsonFile.open(QFile::WriteOnly);
        QJsonDocument document(globalObject);
        jsonFile.write(document.toJson(QJsonDocument::JsonFormat::Indented));
        jsonFile.close();
    }

}


void CalibrationModule::JoinCalib() {
    QString pathStr = m_daqWindow->GetApplicationPath() + "/../calibs";
    bool ok;
    QString theName = QInputDialog::getText(nullptr, tr("Join JSON calibration files.."),
                                            tr("To create a joined calibration file, select the individual calibration files in this order:\n"
                                               "\t1. ADC calibration\n"
                                               "\t2. Time calibration\n"
                                               "\t3. Time-walk calibration\n"
                                               "If you do not want to add a particular calibration, just press cancel in the file dialog\n\n"
                                               "Below please state the name for the new joined calibration file"),
                                            QLineEdit::Normal,"joined_calib",&ok);

    QString dirStrADC = QFileDialog::getOpenFileName(nullptr, tr("Select ADC calibration file"), pathStr, tr("Text (*.json)"));
    QString dirStrTime = QFileDialog::getOpenFileName(nullptr, tr("Select time calibration file"),pathStr, tr("Text (*.json)"));
    QString dirStrTimewalk = QFileDialog::getOpenFileName(nullptr, tr("Select time walk calibration file"), pathStr, tr("Text (*.json)"));

    QJsonObject newGlobalObject;
    QJsonArray newCalibrationArray;
    bool adcFound = false;
    bool timeFound = false;
    if(dirStrADC != "") {
        QFile file;
        file.setFileName(dirStrADC);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString val = file.readAll();
        file.close();
        QJsonDocument oldDoc = QJsonDocument::fromJson(val.toUtf8());

        QJsonObject oldJsonObject = oldDoc.object();
        QJsonArray oldJsonArray = oldJsonObject["vmm_calibration"].toArray();

        foreach (const QJsonValue & value, oldJsonArray) {
            QJsonObject newCalibrationObject;
            QJsonArray newAdcOffsetArray;
            QJsonArray newAdcSlopeArray;


            int fec = 0;
            int hybrid = 0;
            int chip = 0;
            QString hybridID = "";
            const auto& obj = value.toObject();
            int fecID = obj["fecID"].toInt();
            fec = m_fecIDPos[fecID];
            //std::cout << "FEC ID " << fecID << " " << fec << std::endl;

            hybridID = obj["hybridID"].toString();
            //std::cout << "hybridID " << hybridID.toStdString() << std::endl;

            chip = obj["vmmID"].toInt()%2;
            hybrid = obj["vmmID"].toInt()/2;

            auto const & slopes = obj["adc_slopes"].toArray();
            auto const & offsets = obj["adc_offsets"].toArray();
            for(const auto &val: slopes) {
                newAdcSlopeArray.push_back(val.toDouble());
                //std::cout << "adc slopes " << val.toDouble() << std::endl;
            }
            for(const auto &val: offsets) {
                newAdcOffsetArray.push_back(val.toDouble());
                //std::cout << "adc offsets " << val.toDouble() << std::endl;
            }
            if(!newAdcSlopeArray.empty() && !newAdcOffsetArray.empty()) {
                newCalibrationObject.insert("fecID",fecID);
                newCalibrationObject.insert("hybridID",hybridID);
                newCalibrationObject.insert("vmmID",hybrid*2+chip);
                newCalibrationObject.insert("adc_slopes",newAdcSlopeArray);
                newCalibrationObject.insert("adc_offsets",newAdcOffsetArray);
                newCalibrationArray.push_back(newCalibrationObject);
                adcFound = true;
            }
        }
    }

    if(dirStrTime != "") {
        QFile file;
        file.setFileName(dirStrTime);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString val = file.readAll();
        file.close();
        QJsonDocument oldDoc = QJsonDocument::fromJson(val.toUtf8());

        QJsonObject oldJsonObject = oldDoc.object();
        QJsonArray oldJsonArray = oldJsonObject["vmm_calibration"].toArray();

        foreach (const QJsonValue & value, oldJsonArray) {
            QJsonObject newCalibrationObject;
            QJsonArray newTimeOffsetArray;
            QJsonArray newTimeSlopeArray;

            int fec = 0;
            int hybrid = 0;
            int chip = 0;
            QString hybridID = "";
            const auto& obj = value.toObject();
            int fecID = obj["fecID"].toInt();
            fec = m_fecIDPos[fecID];
            //std::cout << "FEC ID " << fecID << " " << fec << std::endl;

            hybridID = obj["hybridID"].toString();
            //std::cout << "hybridID " << hybridID.toStdString() << std::endl;

            chip = obj["vmmID"].toInt()%2;
            hybrid = obj["vmmID"].toInt()/2;

            auto const & slopes = obj["time_slopes"].toArray();
            auto const & offsets = obj["time_offsets"].toArray();
            for(const auto &val: slopes) {
                newTimeSlopeArray.push_back(val.toDouble());
                //std::cout << "adc slopes " << val.toDouble() << std::endl;
            }
            for(const auto &val: offsets) {
                newTimeOffsetArray.push_back(val.toDouble());
                //std::cout << "adc offsets " << val.toDouble() << std::endl;
            }
            if(!newTimeSlopeArray.empty() && !newTimeOffsetArray.empty()) {
                if(!adcFound) {
                    newCalibrationObject.insert("fecID",fecID);
                    newCalibrationObject.insert("hybridID",hybridID);
                    newCalibrationObject.insert("vmmID",hybrid*2+chip);
                }
                else {
                    for (int n = 0; n < newCalibrationArray.size(); n++)
                    {
                        QJsonObject theObject = newCalibrationArray.at(n).toObject();
                        int theFecID = theObject["fecID"].toInt();
                        QString theHybridID = theObject["hybridID"].toString();
                        int theVmmID = theObject["vmmID"].toInt();
                        if(theFecID == fecID && theHybridID == hybridID && theVmmID == hybrid*2+chip) {
                            theObject.insert("time_slopes",newTimeSlopeArray);
                            theObject.insert("time_offsets",newTimeOffsetArray);
                            newCalibrationArray.replace(n,theObject);
                        }

                    }

                }
            }
        }
    }

    if(dirStrTimewalk != "") {
        QFile file;
        file.setFileName(dirStrTimewalk);
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        QString val = file.readAll();
        file.close();
        QJsonDocument oldDoc = QJsonDocument::fromJson(val.toUtf8());

        QJsonObject oldJsonObject = oldDoc.object();
        QJsonArray oldJsonArray = oldJsonObject["vmm_calibration"].toArray();

        foreach (const QJsonValue & value, oldJsonArray) {
            QJsonObject newCalibrationObject;
            QJsonArray newTimewalkA_Array;
            QJsonArray newTimewalkB_Array;
            QJsonArray newTimewalkC_Array;
            QJsonArray newTimewalkD_Array;

            int fec = 0;
            int hybrid = 0;
            int chip = 0;
            QString hybridID = "";
            const auto& obj = value.toObject();
            int fecID = obj["fecID"].toInt();
            fec = m_fecIDPos[fecID];
            //std::cout << "FEC ID " << fecId << " " << fec << std::endl;

            hybridID = obj["hybridID"].toString();
            //std::cout << "hybridID " << hybridID.toStdString() << std::endl;

            chip = obj["vmmID"].toInt()%2;
            hybrid = obj["vmmID"].toInt()/2;
            //std::cout << "hybrid " << hybrid << " " << chip << std::endl;

            auto const & arrayA = obj["timewalk_a"].toArray();
            auto const & arrayB = obj["timewalk_b"].toArray();
            auto const & arrayC = obj["timewalk_c"].toArray();
            auto const & arrayD = obj["timewalk_d"].toArray();


            for(const auto &val: arrayA) {
                newTimewalkA_Array.push_back(val.toDouble());
            }
            for(const auto &val: arrayB) {
                newTimewalkB_Array.push_back(val.toDouble());
            }
            for(const auto &val: arrayC) {
                newTimewalkC_Array.push_back(val.toDouble());
            }
            for(const auto &val: arrayD) {
                newTimewalkD_Array.push_back(val.toDouble());
            }
            if(!newTimewalkA_Array.empty() && !newTimewalkB_Array.empty() && !newTimewalkC_Array.empty() && !newTimewalkD_Array.empty()) {
                if(!adcFound && !timeFound) {
                    newCalibrationObject.insert("fecID",fecID);
                    newCalibrationObject.insert("hybridID",hybridID);
                    newCalibrationObject.insert("vmmID",hybrid*2+chip);
                }
                else {
                    for (int n = 0; n < newCalibrationArray.size(); n++)
                    {
                        QJsonObject theObject = newCalibrationArray.at(n).toObject();
                        int theFecID = theObject["fecID"].toInt();
                        QString theHybridID = theObject["hybridID"].toString();
                        int theVmmID = theObject["vmmID"].toInt();
                        if(theFecID == fecID && theHybridID == hybridID && theVmmID == hybrid*2+chip) {
                            theObject.insert("timewalk_a",newTimewalkA_Array);
                            theObject.insert("timewalk_b",newTimewalkB_Array);
                            theObject.insert("timewalk_c",newTimewalkC_Array);
                            theObject.insert("timewalk_d",newTimewalkD_Array);
                            newCalibrationArray.replace(n,theObject);
                        }

                    }

                }
            }
        }
    }

    if(!newCalibrationArray.empty()) {
        QString theDirectory = CreateDir("calibs", "");
        QString theTime = QDateTime::currentDateTime().toString("hhmmss");
        theName = theDirectory + "/" + theName + "_" + theTime + ".json";

        QFile jsonFile(theName);
        jsonFile.open(QFile::WriteOnly);

        newGlobalObject.insert("vmm_calibration",newCalibrationArray);
        QJsonDocument document(newGlobalObject);
        jsonFile.write(document.toJson(QJsonDocument::JsonFormat::Compact));
        jsonFile.close();
    }
}


void CalibrationModule::SaveDataAsCSV() {
    if(!m_dataAvailable) return;
    double gain = 0;
    int polarity = 0;
    double peaktime = 0;
    double tac = 0;
    double bcclock = 0;
    int timing_at_threshold = 0;
    int fec = 0;
    int hybrid = 0;
    int chip = 0;
    QString name = "";
    if(m_modeIndex == 6) {
        fec = m_theFEC;
        QString fecId = "";
        //SRS (2)
        if(g_slow_control == 2){
            fecId = QString::number(m_fecPosID[m_theFEC]);
        }
        else {
            int ring = m_daqWindow->m_daq.m_fecs[m_theFEC].GetRegVal("ring");
            int fen = m_daqWindow->m_daq.m_fecs[m_theFEC].GetRegVal("fen");
            fecId = QString::number(ring).rightJustified(2, '0') + "_"+QString::number(fen).rightJustified(2, '0');
        }
        hybrid = m_theVMM / 2;
        chip = m_theVMM % 2;
        std::string hybridID = m_hybrid_id[fec][hybrid];
        QString theName = "S-curve";
        QString theFitName = "Fit_S-curve";
        timing_at_threshold = m_timing_at_thr[fec][hybrid][chip];
        gain = m_gain[fec][hybrid][chip];
        polarity = m_polarity[fec][hybrid][chip];
        peaktime = m_shaping_time[fec][hybrid][chip];
        tac = m_tac_slope[fec][hybrid][chip];
        bcclock = 1000.0/g_clock_period;

        QString theSettings = CreateFileName(polarity,gain,peaktime,tac);
        QString theDirectory = CreateDir("data", theSettings);

        QString theTime = QDateTime::currentDateTime().toString("hhmmss");

        if(m_scan_type == 0) {
            theName += "scanned_threshold";
            theFitName += "scanned_threshold";
        }
        else {
            theName += "scanned_pulser";
            theFitName += "scanned_pulser";
        }
        theName = theName + "_" + theTime + ".csv";
        theFitName = theFitName + "_" + theTime + ".csv";

        theName = theDirectory + "/" + theName;
        theFitName = theDirectory + "/" + theFitName;
        if(m_outFile.is_open())
        {
            m_outFile.close();
        }
        m_outFile.open(theName.toStdString().c_str(), std::ofstream::out);
        m_outFile << "hybrid_id, fec,vmm,ch,pulser_dac,pulser_mV,threshold_dac,threshold_mV,time,counts,rate\n";
        for(int bit=0; bit<m_number_bits;bit++){
            for(unsigned int ch = 0; ch<64; ch++){
                double rate = 0;
                if( m_time[bit] > 0) {
                    rate = m_channel_y[ch][bit]/(1000*m_time[bit]);
                }
                if(m_scan_type == 0) {
                    m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << m_pulser_dac << "," << m_pulser_mV << "," << m_dac_setting[bit] << ","
                              << m_dac_measured[fec][hybrid][chip][bit] << "," << m_time[bit] << "," << m_channel_y[ch][bit] << "," <<rate << "\n" ;
                }
                else {
                    m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << m_dac_setting[bit] << "," << m_dac_measured[fec][hybrid][chip][bit] << "," << m_threshold_dac << ","
                              << m_threshold_mV << "," << m_time[bit] << "," << m_channel_y[ch][bit] << "," <<rate << "\n" ;
                }
            }
        }
        m_outFile.close();
        m_outFile.open(theFitName.toStdString().c_str(), std::ofstream::out);
        m_outFile << "hybrid_id, fec,vmm,ch,fit scale, error scale, fit mean, error mean, fit sigma, error sigma\n";
        for(unsigned int ch = 0; ch<64; ch++){
            m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << m_fit_scale[ch] << "," <<  m_fit_error_scale[ch] << "," <<  m_fit_mean[ch] << ","
                      <<  m_fit_error_mean[ch] << "," <<  m_fit_sigma[ch] << "," << m_fit_error_sigma[ch] << "\n" ;
        }
        m_outFile.close();
    }
    else {
        QString theName = "";
        if(m_modeIndex == 1)
        {
            theName = "Offline_ADC";
        }
        else if(m_modeIndex == 2)
        {
            theName = "Offline_Time";
        }
        else if(m_modeIndex == 3)
        {
            theName = "Online_Timewalk";
        }
        else if(m_modeIndex == 4)
        {
            theName = "Online_ADC";
        }
        else if(m_modeIndex == 5)
        {
            theName = "Online_TDC";
        }
        else if(m_modeIndex == 7)
        {
            theName = "Threshold";
        }
        else if(m_modeIndex == 8)
        {
            theName = "Pedestal";
        }
        else if(m_modeIndex == 9)
        {
            theName = "Pulser_DAC";
        }
        else if(m_modeIndex == 10)
        {
            theName = "Threshold_DAC";
        }
        else if(m_modeIndex == 20)
        {
            theName = "Counts_Channels";
        }
        else if(m_modeIndex == 21)
        {
            theName = "Mean_ADC";
        }
        else if(m_modeIndex == 22)
        {
            theName = "Mean_TDC";
        }
        else if(m_modeIndex == 23)
        {
            theName = "Mean_BCID";
        }
        fec = GetFEC(0);
        hybrid = GetHybrid(0);
        chip = GetVMM(0);

        timing_at_threshold = m_timing_at_thr[fec][hybrid][chip];
        gain = m_gain[fec][hybrid][chip];
        polarity = m_polarity[fec][hybrid][chip];
        peaktime = m_shaping_time[fec][hybrid][chip];
        tac = m_tac_slope[fec][hybrid][chip];
        int dac = m_pulser_dac;
        int srat = m_timing_at_thr[fec][hybrid][chip];

        if(m_modeIndex != 2) {
            srat = -1;
            dac = -1;
        }


        QString theSettings = CreateFileName(polarity,gain,peaktime,tac,dac);
        QString theDirectory = CreateDir("data", theSettings);
        QString theTime = QDateTime::currentDateTime().toString("hhmmss");
        theName = theName + "_" + theTime + ".csv";
        theName = theDirectory + "/" + theName;


        if(m_outFile.is_open())
        {
            m_outFile.close();
        }
        m_outFile.open(theName.toStdString().c_str(), std::ofstream::out);
        for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){

            fec = GetFEC(vmm);
            hybrid = GetHybrid(vmm);
            chip = GetVMM(vmm);
            std::string hybridID = m_hybrid_id[fec][hybrid];
            QString fecId = "";
            //SRS (2)
            if(g_slow_control == 2){
                fecId = QString::number(m_fecPosID[fec]);
            }
            else {
                int ring = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("ring");
                int fen = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("fen");
                fecId = QString::number(ring).rightJustified(2, '0') + "_"+QString::number(fen).rightJustified(2, '0');
            }

            if(m_modeIndex == 1)
            {
                //<< cnt << "," << std::setprecision(6) << rate<< std::endl;
                m_outFile << "hybrid_id, fec,vmm,ch,pulser_dac_setting,pulser_dac_measured,adc_measured,slope,offset,adc_corrected\n";
                for(int ch = 0; ch<64; ch++){

                    for(int bit=0; bit<m_number_bits;bit++){
                        m_outFile << hybridID << "," << fecId.toStdString() << "," << hybrid*2+chip << "," << ch  << "," << m_dac_setting[bit] << ","  << m_dac_measured[fec][hybrid][chip][bit] << ","
                                  << m_mean[bit][fec][hybrid][chip][ch] << "," << m_slope[fec][hybrid][chip][ch] << "," << m_offset[fec][hybrid][chip][ch]
                                  << "," << (m_mean[bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])* m_slope[fec][hybrid][chip][ch] << "\n" ;
                    }
                }
            }
            else if(m_modeIndex == 2)
            {
                m_outFile << "hybrid_id, fec,vmm,ch,pulser_skew,time_slope_correction,time_offset_correction,";
                for(int n=0; n<NUM_BCID; n++) {
                    m_outFile << "bcid"<<n<<",";
                    m_outFile << "percentage"<<n<<",";
                    m_outFile << "measured_bcid_time"<<n<<",";
                    m_outFile << "measured_tdc_time"<<n<<",";
                    m_outFile << "corrected_tdc_time"<<n<<",";
                }
                m_outFile << "total_measured,";
                m_outFile << "total_corrected\n";

                for(int ch = 0; ch<64; ch++){
                    for(int bit=0; bit<m_number_bits;bit++){
                        m_outFile << hybridID << "," << fecId.toStdString() << "," << hybrid*2+chip << "," << ch  << "," << bit*g_time_factor << ","  <<
                            m_slope[fec][hybrid][chip][ch] << "," << m_offset[fec][hybrid][chip][ch] << ",";
                        double theTotalTime = 0;
                        int bcid = m_fit_start_bcid[fec][hybrid][chip][ch];
                        int z = 0;
                        for(int n=0; n<NUM_BCID; n++) {
                            m_outFile << m_reference_BCID+bcid+n << ",";
                            m_outFile << m_percent_bcid[n][bit][fec][hybrid][chip][ch] << ",";
                            m_outFile << (bcid+n)*g_clock_period << ",";


                            if(m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] != -9999.0) {
                                m_outFile << m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] << ",";
                                m_outFile << (m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])*  m_slope[fec][hybrid][chip][ch] << ",";
                                theTotalTime += m_percent_bcid[n][bit][fec][hybrid][chip][ch]*((bcid+n)*g_clock_period + (m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])*  m_slope[fec][hybrid][chip][ch]);
                                z++;
                            }
                            else {
                                m_outFile << 0 << ",";
                                m_outFile << 0 << ",";
                            }
                        }
                        if(z==0) {
                            theTotalTime = -9999.0;
                        }

                        m_outFile << m_mean[bit][fec][hybrid][chip][ch] << "," << theTotalTime << "\n" ;
                    }
                }

            }
            else if(m_modeIndex == 3)
            {
                m_outFile << "hybrid_id, fec,vmm,ch,";
                m_outFile << "pulser_dac_setting,pulser_dac_measured,adc_measured,time_measured,";
                m_outFile << "adc_correction_slope,adc_correction_offset,time_correction_slope,time_correction_offset,";
                m_outFile << "timewalk_correction_a,timewalk_correction_b,timewalk_correction_c,timewalk_correction_d,fit r2, fit rms\n";

                bool hasFileADC = false;
                if(m_file_adc_slope[fec][hybrid][vmm].size() == 64 && m_file_adc_offset[fec][hybrid][vmm].size() == 64 ) {
                    hasFileADC = true;
                }
                bool hasFileTime = false;
                if(m_file_time_slope[fec][hybrid][vmm].size() == 64 && m_file_time_offset[fec][hybrid][vmm].size() == 64 ) {
                    hasFileTime = true;
                }
                for(int ch = 0; ch<64; ch++){
                    for(int bit=0; bit<m_number_bits;bit++){
                        double a = 0;
                        double b = 1;
                        double c = 1;
                        double d = 0;
                        if(m_fit_a[fec][hybrid][chip][ch] > -9999.0) {
                            a = m_fit_a[fec][hybrid][chip][ch];
                        }
                        if(m_fit_b[fec][hybrid][chip][ch] > -9999.0) {
                            b = m_fit_b[fec][hybrid][chip][ch];
                        }
                        if(m_fit_c[fec][hybrid][chip][ch] > -9999.0) {
                            c = m_fit_c[fec][hybrid][chip][ch];
                        }
                        if(m_fit_d[fec][hybrid][chip][ch] > -9999.0) {
                            d = m_fit_d[fec][hybrid][chip][ch];
                        }
                        m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << ",";
                        m_outFile << m_dac_setting[bit] << ","  << m_dac_measured[fec][hybrid][chip][bit] << "," << m_mean2[bit][fec][hybrid][chip][ch] << ","  << m_mean[bit][fec][hybrid][chip][ch] << ",";
                        if(hasFileADC) {
                            m_outFile << m_file_adc_slope[fec][hybrid][chip][ch] << ","  << m_file_adc_offset[fec][hybrid][chip][ch] << ",";
                        }
                        else {
                            m_outFile << ",,";
                        }
                        if(hasFileTime) {
                            m_outFile << m_file_time_slope[fec][hybrid][chip][ch] << ","  << m_file_time_offset[fec][hybrid][chip][ch] << ",";
                        }
                        else {
                            m_outFile << ",,";
                        }
                        m_outFile << a << ","  << b << "," << c << ","  << d << ",";
                        m_outFile << m_fit_error_r2[fec][hybrid][chip][ch] << ","  << m_fit_error_rms[fec][hybrid][chip][ch] << "\n";
                    }
                }

            }
            else if(m_modeIndex == 4)
            {
                m_outFile << "hybrid_id, fec,vmm,ch, ADC correction [mV], ADC, calibrated value, , best common value\n";
                for(int ch = 0; ch<64; ch++){
                    for(int bit=0; bit<m_number_bits;bit++){
                        m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << bit << "," << m_mean[bit][fec][hybrid][chip][ch]  << "," << m_calVal[fec][hybrid][chip][ch] << "," << m_y[fec][hybrid][chip][ch] << "\n" ;
                    }
                }

            }
            else if(m_modeIndex == 5)
            {
                m_outFile << "hybrid_id, fec,vmm,ch, TDC correction [mV], TDC, calibrated value, best common value\n";
                for(int ch = 0; ch<64; ch++){
                    for(int bit=0; bit<m_number_bits;bit++){
                        m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << bit << "," << m_mean[bit][fec][hybrid][chip][ch]  << "," << m_calVal[fec][hybrid][chip][ch] << "," << m_y[fec][hybrid][chip][ch] << "\n" ;
                    }
                }
            }

            else if(m_modeIndex == 7)
            {
                name = "Threshold";
                if(m_isThresholdCalibration) {
                    m_outFile << "hybrid_id, fec,vmm,ch, threshold correction [mV], threshold [mV], calibrated correction [mV], calibrated threshold [mV], best common value [mV]\n";
                    for(int ch = 0; ch<64; ch++){
                        for(int bit=0; bit<m_number_bits;bit++){
                            m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << bit << "," << m_mean[bit][fec][hybrid][chip][ch]  << "," << m_bitVal[fec][hybrid][chip][ch] << "," << m_calVal[fec][hybrid][chip][ch] << "," << m_y[fec][hybrid][chip][ch] << "\n" ;
                        }
                    }

                }
                else {
                    m_outFile << "hybrid_id, fec,vmm,ch, threshold [mV]\n";
                    for(int ch = 0; ch<64; ch++){
                        m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                    }
                }

            }
            else if(m_modeIndex == 8)
            {
                m_outFile << "hybrid_id, fec,vmm,ch, pedestal [mV]\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << hybridID << "," << fecId.toStdString() << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
            else if(m_modeIndex == 9)
            {
                m_outFile << "hybrid_id, fec,vmm, pulser dac setting, pulser dac measured\n";
                for(int n=0; n<m_dac_setting.size();n++){
                    m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << m_dac_setting[n] << "," << m_y[fec][hybrid][chip][n] << "\n" ;
                }
            }
            else if(m_modeIndex == 10)
            {
                m_outFile << "hybrid_id, fec,vmm, threshold dac setting, threshold dac measured\n";
                for(int n=0; n<m_dac_setting.size();n++){
                    m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << m_dac_setting[n] << "," << m_y[fec][hybrid][chip][n] << "\n" ;
                }
            }
            else if(m_modeIndex == 20)
            {
                m_outFile << "hybrid_id, fec,vmm,ch, cnt\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
            else if(m_modeIndex == 21)
            {
                m_outFile << "hybrid_id, fec,vmm,ch, mean ADC\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
            else if(m_modeIndex == 22)
            {
                m_outFile << "hybrid_id, fec,vmm,ch, mean TDC\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
            else if(m_modeIndex == 23)
            {
                m_outFile << "hybrid_id, fec,vmm,ch, mean BCID\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << hybridID << "," << fecId.toStdString()  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
        }

        m_outFile.close();
    }
}

void CalibrationModule::SavePlotsAsPDF(){
    if(!m_dataAvailable) return;
    double gain = 0;
    int polarity = 0;
    double peaktime = 0;
    double tac = 0;
    double bcclock = 0;
    int timing_at_threshold = 0;

    unsigned long theChoice =  static_cast<unsigned long>(m_daqWindow->m_ui->comboBoxFec->currentIndex());
    unsigned long start = theChoice * 8;
    unsigned long end = (theChoice +1)*8;
    unsigned long limit = 0;
    //S-curve
    if(m_modeIndex == 6) {
        if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1 ) {
            limit = 64;
        }
        else {
            limit = 2;
        }
    }
    else if((m_modeIndex == 1  || m_modeIndex == 2 || m_modeIndex == 3) && m_daqWindow->m_ui->choicePlotTime->currentIndex() > 1) {
        limit = 64*m_vmmActs.size();
    }
    else if(m_modeIndex == 11  || m_modeIndex == 12) {
        limit = 1;
    }
    else {
        limit = m_vmmActs.size();
    }
    for(unsigned long idx=start; idx<limit && idx <end; idx++){
        int fec = 0;
        int hybrid = 0;
        int chip = 0;

        if((m_modeIndex == 1  || m_modeIndex == 2 || m_modeIndex == 3) && m_daqWindow->m_ui->choicePlotTime->currentIndex() > 1) {
            fec = GetFEC(static_cast<int>(idx/64));
            hybrid = GetHybrid(static_cast<int>(idx/64));
            chip = GetVMM(static_cast<int>(idx/64));
        }
        else if(m_modeIndex != 6) {
            fec = GetFEC(static_cast<int>(idx));
            hybrid = GetHybrid(static_cast<int>(idx));
            chip = GetVMM(static_cast<int>(idx));
        }
        else {
            fec = m_theFEC;
            hybrid = m_theVMM / 2;
            chip = m_theVMM % 2;
        }
        int fecId = m_fecPosID[fec];
        if(idx==start)
        {
            timing_at_threshold = m_timing_at_thr[fec][hybrid][chip];
            gain = m_gain[fec][hybrid][chip];
            polarity = m_polarity[fec][hybrid][chip];
            peaktime = m_shaping_time[fec][hybrid][chip];
            tac = m_tac_slope[fec][hybrid][chip];
            bcclock = 1000.0/g_clock_period;
        }
        QCustomPlot *plot = plotVector[idx%8];
        QString name = "";

        if(m_modeIndex == 1)
        {
            name = "Offline_ADC";
            if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                name+= "_corrections";
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {
                name+= "_fit_CH" + QString::number(idx);
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 3) {
                name+= "_hist_CH" + QString::number(idx) + "_DAC" + QString::number(m_dac_measured[fec][hybrid][chip][m_daqWindow->m_ui->choiceBit->currentIndex()]);
            }
        }
        else if(m_modeIndex == 2)
        {
            name = "Offline_Time";
            if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                name+= "_corrections";
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {
                name+= "_fit_CH" + QString::number(idx);
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 3) {
                name+= "_hist_CH" + QString::number(idx) + "_" + QString::number(g_time_factor*m_daqWindow->m_ui->choiceBit->currentIndex()) +"ns";
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 4) {
                name+= "_bcid_percent_CH" + QString::number(idx);
            }
        }
        else if(m_modeIndex == 3)
        {
            name = "Timewalk";
            if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                name+= "_corrections";
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {
                name+= "_fit_CH" + QString::number(idx);
            }
        }
        else if(m_modeIndex == 4)
        {
            name = "Online_ADC";
        }
        else if(m_modeIndex == 5)
        {
            name = "Online_TDC";
        }
        else if(m_modeIndex == 6)
        {
            name = "S-curve_CH" + QString::number(idx);
        }
        else if(m_modeIndex == 7)
        {
            name = "Threshold";
        }
        else if(m_modeIndex == 8)
        {
            name = "Pedestal";
        }
        else if(m_modeIndex == 9)
        {
            name = "Pulser_DAC";
        }
        else if(m_modeIndex == 10)
        {
            name = "Threshold_DAC";
        }
        else if(m_modeIndex == 20)
        {
            name = "Counts_Channels";
        }
        else if(m_modeIndex == 21)
        {
            name = "Mean_ADC";
        }
        else if(m_modeIndex == 22)
        {
            name = "Mean_TDC";
        }
        else if(m_modeIndex == 23)
        {
            name = "Mean_BCID";
        }

        name += "_" +  g_card_name + QString::number(fec)+ "_IP" + QString::number(fecId);
        if(m_modeIndex == 6) {
            name += "_VMM" + QString::number(m_theVMM);
        }
        else {
            name += "_VMM" + QString::number(hybrid*2+chip);
        }
        int dac = m_pulser_dac;
        int srat = m_timing_at_thr[fec][hybrid][chip];

        if(m_modeIndex != 2) {
            srat = -1;
            dac = -1;
        }
        QString theSettings = CreateFileName(polarity,gain,peaktime,tac,dac);
        QString theDirectory = CreateDir("plots", theSettings);
        QString theTime = QDateTime::currentDateTime().toString("hhmmss");
        if(m_modeIndex == 11 || m_modeIndex == 12) {
            if(m_modeIndex == 11)
            {
                name = "ResetLatency";
            }
            else if(m_modeIndex == 12)
            {
                name = "TPLatency";
            }
            name = theDirectory + "/" + name + "_" + theTime + ".pdf";
        }
        else {
            name = theDirectory + "/" + name + "_" + theTime + ".pdf";
        }


        plot->savePdf(name);
    }
}

QString CalibrationModule::CreateDir(QString type, QString name) {
    std::string fname = m_daqWindow->GetApplicationPath().toStdString();
    fname+="/../";
    fname += type.toStdString();
    //make directory for Plots, Data or Calib
    QDir dirType(QString::fromStdString(fname));
    if (!dirType.exists()) {
        dirType.mkpath(".");
    }
    QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd");
    fname += "/";
    fname += theDate.toStdString();
    //Make directory for the data
    QDir dirDate(QString::fromStdString(fname));
    if (!dirDate.exists()) {
        dirDate.mkpath(".");
    }
    if(name != "") {
        fname += "/";
        fname += name.toStdString();
        //Make directory for the setting
        QDir dirSettings(QString::fromStdString(fname));
        if (!dirSettings.exists()) {
            dirSettings.mkpath(".");
        }
    }
    return QString::fromStdString(fname);
}

void CalibrationModule::PlotData(){
    for(int i=0; i< plotVector.size();i++)
    {
        QCustomPlot *plot = plotVector[i];
        QCPColorMap *colorMap = colorMapVector[i];
        QCPColorScale *colorScale = colorScaleVector[i];

        colorMap->setVisible(false);
        colorScale->setVisible(false);
        colorScale->setMinimumSize(0, 0);
        colorScale->setMaximumSize(0, 0);
        QMargins margins;
        margins.setTop(0);
        margins.setBottom(0);
        margins.setRight(0);
        margins.setLeft(0);
        colorScale->setMargins(margins );
        if(plot->plotLayout()->hasElement(0,1)) {
            plot->plotLayout()->take(colorScale);
        }

        plot->clearItems();
        //plot->clearPlottables();
        plot->clearGraphs();
        plot->yAxis->setLabel("");
        plot->xAxis->setLabel("");
        plot->legend->setVisible(false);
        plot->legend->clearItems();
        plot->plotLayout()->simplify();
        //plot->yAxis2->setVisible(false);
        plot->replot();

    }
    if(!CheckModes())
    {
        return;
    }
    unsigned long theChoice =  static_cast<unsigned long>(m_daqWindow->m_ui->comboBoxFec->currentIndex());
    unsigned long start = theChoice * 8;
    unsigned long end = (theChoice +1)*8;
    unsigned long limit = 0;
    //S-curve
    if(m_modeIndex == 6) {
        if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1 ) {
            limit = 64;
        }
        else {
            limit = 2;
        }
    }
    else if((m_modeIndex == 1  || m_modeIndex == 2 || m_modeIndex == 3) && m_daqWindow->m_ui->choicePlotTime->currentIndex() > 1) {
        limit = 64*m_vmmActs.size();
    }
    //Latency calibration
    else if(m_modeIndex == 11 || m_modeIndex == 12) {
        limit = 1;
    }
    else {
        limit = m_vmmActs.size();
    }
    for(unsigned long idx=start; idx<limit && idx <end; idx++){
        QCustomPlot *plot = plotVector[idx%8];
        QCPColorMap *colorMap = colorMapVector[idx%8];
        QCPColorScale *colorScale  = colorScaleVector[idx%8];
        colorScale->setMaximumSize(0, 0);
        // give the axes some labels:
        plot->xAxis->setLabel("channel");
        // set axes ranges, so we see all data:
        plot->xAxis->setRange(0, 66);
        QPen pen(Qt::red,3,Qt::SolidLine);
        QString title = "";
        int fec = 0;
        int hybrid = 0;
        int chip = 0;
        int ch = 0;
        if((m_modeIndex == 1  || m_modeIndex == 2 || m_modeIndex == 3) && m_daqWindow->m_ui->choicePlotTime->currentIndex() > 1) {
            fec = GetFEC(static_cast<int>(idx/64));
            int fecId = m_fecPosID[fec];
            hybrid = GetHybrid(static_cast<int>(idx/64));
            chip = GetVMM(static_cast<int>(idx/64));
            ch= idx%64;
            //SRS (2)
            if(g_slow_control == 2){
                title = g_card_name + " " + QString::number(fec) + " (IP " +  QString::number(fecId) + ") Hybrid " + QString::number(hybrid) + "(VMM " + QString::number(chip) + ") CH " + QString::number(ch);
            }
            else {
                int ring = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("ring");
                int fen = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("fen");
                title = g_card_name + " " + QString::number(fec) + " (" +QString::number(ring).rightJustified(2, '0')
                        + "_"+QString::number(fen).rightJustified(2, '0') + ") Hybrid " + QString::number(hybrid) + "(VMM " + QString::number(chip) + ") CH " + QString::number(ch);
            }
        }
        else if(m_modeIndex == 11 || m_modeIndex == 12) {
            fec = GetFEC(static_cast<int>(idx));
            hybrid = GetHybrid(static_cast<int>(idx));
            chip = GetVMM(static_cast<int>(idx));
            //SRS (2)
            if(g_slow_control == 2){
                title = g_card_name + "s";
            }
            else {
                title = g_card_name + "s";
            }
        }
        else if(m_modeIndex != 6) {
            fec = GetFEC(static_cast<int>(idx));
            int fecId = m_fecPosID[fec];
            hybrid = GetHybrid(static_cast<int>(idx));
            chip = GetVMM(static_cast<int>(idx));
            //SRS (2)
            if(g_slow_control == 2){
                title = g_card_name + " " + QString::number(fec) + " (IP " +  QString::number(fecId) + ") Hybrid " + QString::number(hybrid) + "(VMM " + QString::number(chip) + ")";
            }
            else {
                int ring = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("ring");
                int fen = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("fen");
                title = g_card_name + " " + QString::number(fec) + " (" +QString::number(ring).rightJustified(2, '0')
                        + "_"+QString::number(fen).rightJustified(2, '0') + ") Hybrid " + QString::number(hybrid) + "(VMM " + QString::number(chip) + ")";
            }
        }
        else {
            fec = m_theFEC;
            int fecId = m_fecPosID[fec];
            hybrid = m_theVMM / 2;
            chip = m_theVMM % 2;
            //SRS (2)
            if(g_slow_control == 2){
                title = g_card_name + " " + QString::number(fec) + " (IP " +  QString::number(fecId) + ") Hybrid " + QString::number(hybrid) + "(VMM " + QString::number(chip) + ") CH " + QString::number(idx);
            }
            else {
                int ring = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("ring");
                int fen = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("fen");
                title = g_card_name + " " + QString::number(fec) + " (" +QString::number(ring).rightJustified(2, '0')
                        + "_"+QString::number(fen).rightJustified(2, '0') + ") Hybrid " + QString::number(hybrid) + "(VMM " + QString::number(chip) + ") CH " + QString::number(idx);
            }
        }
        unsigned int colorFactor = 0;

        if(m_modeIndex == 1) {
            title+= ": Mean ADC";
            colorFactor = 10;
            plot->yAxis->setRange(0.0, 1023.0);

            if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 0) {
                title+= ": Mean ADC";
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                title+= ": ADC corrections (slope/offset)";
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {
                title+= ": Fit (measured ADC vs DAC)";
                plot->xAxis->setLabel("measured DAC");
                plot->xAxis->setRange(m_dac_measured[fec][hybrid][chip][0], m_dac_measured[fec][hybrid][chip][m_number_bits-1]);
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 3) {
                title+= ": counts per ADC";
                plot->xAxis->setRange(0, 1024);
                plot->xAxis->setLabel("ADC");
            }
        }
        else if(m_modeIndex == 2) {
            colorFactor = 10;
            plot->yAxis->setRange(-10.0, 60.0);
            if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 0) {
                title+= ": Mean time";
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                title+= ": Time corrections (slope/offset)";
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {
                title+= ": Fit (measured time vs pulse shift)";
                plot->xAxis->setRange(0, m_number_bits*g_time_factor);
                plot->xAxis->setLabel("pulse shift [ns]");
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 3) {
                title+= ": counts per TDC";
                plot->xAxis->setRange(0, 256);
                plot->xAxis->setLabel("TDC");
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 4) {
                title+= ": BCID percentages [%]";
                plot->xAxis->setRange(0, m_number_bits*g_time_factor);
                plot->xAxis->setLabel("pulse shift [ns]");
            }

        }
        else if(m_modeIndex == 3) {
            colorFactor = 64;
            if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 0) {
                title+= ": Mean time [ns]";
                //plot->yAxis2->setLabel("charge [ADC]");
                plot->yAxis->setRange(-4, 18.0);
                //plot->yAxis2->setRange(0.0, 1023);
                //plot->yAxis2->setVisible(true);
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                title+= ": Timewalk corrections (slope/offset)";
                plot->yAxis->setRange(-10, 1000.0);
            }
            else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {
                title+= ": Fit (measured time vs ADC)";
                plot->xAxis->setLabel("charge [ADC]");
                plot->yAxis->setRange(-4, 18.0);
                plot->xAxis->setRange(0.0, 1023);
            }
        }
        else if(m_modeIndex == 4) {
            title+= ": Mean ADC";
            colorFactor = 5;
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 5) {
            title+= ": Mean TDC";
            colorFactor = 10;
            plot->yAxis->setRange(0.0, 255.0);
        }
        else if(m_modeIndex == 6)
        {
            if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1 ) {
                title+= ": Rate [kHz]";
                if(m_scan_type == 0) {
                    plot->xAxis->setLabel("threshold [mV]");
                }
                else {
                    plot->xAxis->setLabel("pulser [mV]");
                }
                plot->xAxis->setRange(m_dac_measured[fec][hybrid][chip][0], m_dac_measured[fec][hybrid][chip][m_number_bits -1]);
            }
            else {
                if(idx == 0) {
                    title+= ": Mean [mV]";

                }
                else {
                    title+= ": Sigma [mV]";
                }
            }
        }
        else if(m_modeIndex == 7) {
            if(m_isThresholdCalibration) {
                title+= ": Threshold - Pedestal [mV]";
            }
            else {
                title+= ": Threshold [mV]";
            }
            colorFactor = 5;
            plot->yAxis->setRange(0.0, 300.0);
        }
        else if(m_modeIndex == 8)
        {
            title+= ": Pedestal [mV]";
            plot->yAxis->setRange(0.0, 350.0);
        }
        else if(m_modeIndex == 9) {
            title+= ": Pulser DAC [mV]";
            plot->xAxis->setLabel("Pulser setting [DAC]");
            plot->xAxis->setRange(0.0, 1023.0);
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 10)
        {
            title+= ": Threshold DAC [mV]";
            plot->xAxis->setLabel("Threshold setting [DAC]");
            plot->xAxis->setRange(0.0, 1023.0);
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 11) {
            title+= ": BCID - BC counter";
            plot->xAxis->setLabel("reset latency");
            plot->xAxis->setRange(m_start_reset_latency, m_start_reset_latency+m_number_bits_offline_latency);
            plot->yAxis->setRange(-10.0, 10.0);
        }
        else if(m_modeIndex == 12) {
            title+= ": BCID - TP setting";
            plot->xAxis->setLabel("TP latency");
            plot->xAxis->setRange(m_start_tp_latency, m_start_tp_latency+m_number_bits_offline_latency);
            plot->yAxis->setRange(-10.0, 10.0);
        }
        else if(m_modeIndex == 20)
        {
            title+= ": Counts";
        }
        else if(m_modeIndex == 21)
        {
            title+= ": ADC";
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 22)
        {
            title+= ": Mean TDC";
            plot->yAxis->setRange(0.0, 255.0);
        }
        else if(m_modeIndex == 23)
        {
            title+= ": Mean BCID";
            plot->yAxis->setRange(0.0, 4095.0);
        }
        else if(m_modeIndex == 24)
        {
            title+= ": Counts ADC";
            plot->xAxis->setRange(0.0, 1023);
            plot->xAxis->setLabel("ADC");
        }
        else if(m_modeIndex == 25)
        {
            title+= ": Counts Time";
            plot->xAxis->setRange(0.0, m_daqWindow->m_ui->Runs->value());
            plot->xAxis->setLabel("time [ms]");
        }

        plot->yAxis->setLabel(title);
        if(!IsCalibration() /*m_runMode == "User"  >= 20 */)
        {
            plot->addGraph();
            plot->graph(0)->setPen(pen);
            if(m_modeIndex == 24) {
                plot->graph(0)->setData( QVector<double>(m_adcs.begin(), m_adcs.end()), QVector<double>(m_adc_data[fec][hybrid][chip].begin(), m_adc_data[fec][hybrid][chip].end()));
            }
            else if(m_modeIndex == 25) {
                plot->graph(0)->setData( QVector<double>(m_times.begin(), m_times.end()), QVector<double>(m_adc_data[fec][hybrid][chip].begin(), m_adc_data[fec][hybrid][chip].end()));
            }
            else if(m_modeIndex == 20) {
                plot->graph(0)->setData( QVector<double>(m_x.begin(), m_x.end()), QVector<double>(m_channel_data[fec][hybrid][chip].begin(), m_channel_data[fec][hybrid][chip].end()));
            }
            else if(m_modeIndex == 21) {
                plot->plotLayout()->addElement(0, 1, colorScale);
                colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
                colorMap->setColorScale(colorScale); // associate the color map with the color scale
                colorScale->axis()->setLabel("Counts ADC values");

                colorMap->data()->setSize(64, 1024);
                colorMap->data()->setRange(QCPRange(0, 63), QCPRange(0, 1023));
                colorMap->setGradient(QCPColorGradient::gpSpectrum);
                const QCPColorGradient &grd = colorMap->gradient();
                //grd.SetNanColor(Qt::white);
                colorScale->setMaximumSize(50, 200);
                colorMap->setVisible(true);
                colorScale->setVisible(true);

                for (int chNo=0; chNo<64; chNo++) {
                    for (int adc=0; adc<1024; adc++) {
                        const int num_items = std::count(m_data[m_bitCount][fec][hybrid][chip][chNo].cbegin(), m_data[m_bitCount][fec][hybrid][chip][chNo].cend(), adc);
                        colorMap->data()->setCell(chNo, adc, num_items);
                    }
                }

                colorMap->rescaleDataRange(true);

                plot->rescaleAxes();
                plot->replot();
                plot->yAxis->setLabel(title);
            }
            else {
                plot->graph(0)->setData( QVector<double>(m_x.begin(), m_x.end()), QVector<double>(m_mean[0][fec][hybrid][chip].begin(), m_mean[0][fec][hybrid][chip].end()));
            }
            if(m_modeIndex != 21) {
                plot->yAxis->setLabel(title);
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                plot->graph(0)->setName(title);
                plot->yAxis->rescale();
            }
        }
        else
        {
            //Offline ADC
            if(m_modeIndex == 1)
            {
                if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 0) {
                    int numPlots = 3;
                    if(m_file_adc_offset[fec][hybrid][chip].size() == 64 && m_file_adc_offset[fec][hybrid][chip].size() == 64) {
                        numPlots = 4;
                    }
                    for(int bit=0; bit<m_number_bits;bit++){

                        plot->addGraph();
                        plot->graph(bit*numPlots)->setData(
                            QVector<double>(m_x.begin(), m_x.end()),
                            QVector<double>(m_mean[bit][fec][hybrid][chip].begin(), m_mean[bit][fec][hybrid][chip].end()));
                        plot->graph(bit*numPlots)->setPen(QPen(Qt::black,1,Qt::SolidLine));
                        if(bit > 0) {
                            plot->legend->removeItem(plot->legend->itemCount()-1);
                        }
                        std::vector<double> y;
                        std::vector<double> y2;
                        for(unsigned int ch=0; ch<64; ch++)
                        {
                            double adc = (m_mean[bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])*  m_slope[fec][hybrid][chip][ch];
                            y.push_back(adc);
                            y2.push_back(m_dac_measured[fec][hybrid][chip][bit]);
                        }
                        plot->addGraph();
                        plot->graph(bit*numPlots+1)->setData(
                            QVector<double>(m_x.begin(), m_x.end()),
                            QVector<double>(y.begin(), y.end()));
                        plot->graph(bit*numPlots+1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                        if(bit > 0) {
                            plot->graph(bit*numPlots+1)->removeFromLegend();
                            //plot->legend->removeItem(plot->legend->itemCount()-1);
                        }

                        plot->addGraph();
                        plot->graph(bit*numPlots+2)->setData(
                            QVector<double>(m_x.begin(), m_x.end()),
                            QVector<double>(y2.begin(), y2.end()));

                        plot->graph(bit*numPlots+2)->setPen(QPen(Qt::blue,1,Qt::DashLine));
                        if(bit > 0) {
                            plot->graph(bit*numPlots+2)->removeFromLegend();
                            //plot->legend->removeItem(plot->legend->itemCount()-1);
                        }

                        if(numPlots == 4) {
                            std::vector<double> y3;
                            for(unsigned int ch=0; ch<64; ch++)
                            {
                                double adc = (m_mean[bit][fec][hybrid][chip][ch] - m_file_adc_offset[fec][hybrid][chip][ch])*  m_file_adc_slope[fec][hybrid][chip][ch];
                                y3.push_back(adc);
                            }
                            plot->addGraph();
                            plot->graph(bit*numPlots+3)->setData(
                                QVector<double>(m_x.begin(), m_x.end()),
                                QVector<double>(y3.begin(), y3.end()));
                            plot->graph(bit*numPlots+3)->setPen(QPen(Qt::magenta,1,Qt::SolidLine));
                            if(bit > 0) {
                                plot->graph(bit*numPlots+2)->removeFromLegend();
                                //plot->legend->removeItem(plot->legend->itemCount()-1);
                            }
                        }
                    }
                    plot->graph(0)->setName("measured");
                    plot->graph(1)->setName("corrected");
                    plot->graph(2)->setName("DAC measured");
                    if(numPlots == 4) {
                        plot->graph(3)->setName("corrected from file");
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
                }
                // corrections
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                    int numPlots = 2;
                    if(m_file_adc_offset[fec][hybrid][chip].size() == 64 && m_file_adc_offset[fec][hybrid][chip].size() == 64) {
                        numPlots = 4;
                    }
                    plot->addGraph();
                    std::vector<double> x;
                    std::vector<double> y_slope;
                    std::vector<double> y_offset;
                    for(unsigned int ch=0; ch<64; ch++)
                    {
                        //if(!(m_slope[fec][hybrid][chip][ch] == 0.0 && m_offset[fec][hybrid][chip][ch] == -1.0)) {
                        x.push_back(ch);
                        y_offset.push_back(m_offset[fec][hybrid][chip][ch]);
                        y_slope.push_back(m_slope[fec][hybrid][chip][ch]);
                        //}
                    }
                    plot->graph(0)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(m_offset[fec][hybrid][chip].begin(), m_offset[fec][hybrid][chip].end()));
                    plot->graph(0)->setPen(QPen(Qt::blue,2,Qt::SolidLine));
                    plot->graph(0)->setName("offset");

                    plot->addGraph();
                    plot->graph(1)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(m_slope[fec][hybrid][chip].begin(), m_slope[fec][hybrid][chip].end()));
                    plot->graph(1)->setPen(QPen(Qt::darkGreen,2,Qt::SolidLine));
                    plot->graph(1)->setName("slope");
                    if(numPlots == 4) {
                        plot->addGraph();
                        plot->graph(2)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(m_file_adc_offset[fec][hybrid][chip].begin(), m_file_adc_offset[fec][hybrid][chip].end()));
                        plot->graph(2)->setPen(QPen(Qt::cyan,2,Qt::DotLine));
                        plot->graph(2)->setName("offset from file");

                        plot->addGraph();
                        plot->graph(3)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(m_file_adc_slope[fec][hybrid][chip].begin(), m_file_adc_slope[fec][hybrid][chip].end()));
                        plot->graph(3)->setPen(QPen(Qt::green,2,Qt::DotLine));
                        plot->graph(3)->setName("slope from file");
                    }



                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->yAxis->rescale();
                }
                // fit
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {

                    std::vector<double> x;
                    std::vector<double> y;
                    std::vector<double> yf;
                    double slope  = 0.0;
                    double offset  = 0.0;

                    if(m_slope[fec][hybrid][chip][ch] != 0.0) {
                        slope = m_dac_slope[fec][hybrid][chip]/m_slope[fec][hybrid][chip][ch];
                        offset = m_offset[fec][hybrid][chip][ch] + m_dac_offset[fec][hybrid][chip]*slope/m_dac_slope[fec][hybrid][chip];
                    }
                    for(int bit=0; bit<m_number_bits;bit++){
                        x.push_back(m_dac_measured[fec][hybrid][chip][bit]);
                        y.push_back(m_mean[bit][fec][hybrid][chip][ch]);
                        yf.push_back(offset + m_dac_measured[fec][hybrid][chip][bit]*slope);
                    }
                    plot->addGraph();
                    plot->graph(0)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(y.begin(), y.end()));
                    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
                    plot->graph(0)->setPen(QPen(Qt::blue));

                    plot->addGraph();
                    plot->graph(1)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(yf.begin(), yf.end()));
                    plot->graph(1)->setPen(QPen(Qt::red));


                    plot->graph(0)->setName("measured");

                    plot->graph(1)->setName(QString("fit: " + QString::number(slope) + "* DAC + " + QString::number(offset)));

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);
                    plot->xAxis->setRange(m_dac_measured[fec][hybrid][chip][0], m_dac_measured[fec][hybrid][chip][m_number_bits-1]);
                    plot->yAxis->rescale();

                }
                //ADC distribution
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 3) {
                    int bit = m_daqWindow->m_ui->choiceBit->currentIndex();
                    if(bit == -1) {
                        bit = 0;
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    std::vector<double> x;
                    std::vector<double> y;
                    int min = 1023;
                    int max = 0;
                    for(int adc = 0; adc < 1024; adc++) {
                        long cnt = std::count(m_data[bit][fec][hybrid][chip][ch].begin(),m_data[bit][fec][hybrid][chip][ch].end(), adc);
                        x.push_back(adc);
                        y.push_back(cnt);
                        if(cnt > 10) {
                            if(adc > max) {
                                max = adc;
                            }
                            if(adc < min) {
                                min = adc;
                            }
                        }
                    }
                    plot->xAxis->setRange(0.99*min, 1.01*max);
                    plot->addGraph();

                    plot->graph(0)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(y.begin(), y.end()));
                    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
                    plot->graph(0)->setPen(QPen(Qt::darkRed));
                    plot->graph(0)->setBrush(QBrush(Qt::red));
                    plot->legend->removeItem(plot->legend->itemCount()-1);
                    plot->legend->setVisible(false);
                    plot->yAxis->rescale();

                }
            }
            //Offline time
            else if(m_modeIndex == 2)
            {
                // calibrated time
                if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 0) {
                    int numPlots = 2;
                    if(m_file_time_offset[fec][hybrid][chip].size() == 64 && m_file_time_offset[fec][hybrid][chip].size() == 64) {
                        numPlots = 3;
                    }
                    for(int bit=0; bit<m_number_bits;bit++){
                        plot->addGraph();
                        std::vector<double> x;
                        std::vector<double> xc;
                        std::vector<double> y;
                        std::vector<double> yc;
                        for(unsigned int ch=0; ch<64; ch++)
                        {
                            //if(m_mean[bit][fec][hybrid][chip][ch] != -9999.0) {
                            x.push_back(ch);
                            y.push_back(m_mean[bit][fec][hybrid][chip][ch]);

                            //}
                            double theTotalTime = 0;
                            int bcid = m_fit_start_bcid[fec][hybrid][chip][ch];
                            int z = 0;
                            for(int n=0; n<NUM_BCID; n++) {
                                //if(m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] != -9999.0) {
                                theTotalTime += m_percent_bcid[n][bit][fec][hybrid][chip][ch]*((bcid+n)*g_clock_period + (m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])*  m_slope[fec][hybrid][chip][ch]);
                                z++;
                                //}
                            }
                            //if(z>0) {
                            xc.push_back(ch);
                            yc.push_back(theTotalTime);
                            //}
                        }
                        plot->graph(bit*numPlots)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(y.begin(), y.end()));
                        plot->graph(bit*numPlots)->setPen(QPen(Qt::black,1,Qt::SolidLine));
                        if(bit > 0) {
                            plot->legend->removeItem(plot->legend->itemCount()-1);
                        }
                        plot->addGraph();
                        plot->graph(bit*numPlots+1)->setData(
                            QVector<double>(xc.begin(), xc.end()),
                            QVector<double>(yc.begin(), yc.end()));
                        plot->graph(bit*numPlots+1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                        if(bit > 0) {
                            plot->legend->removeItem(plot->legend->itemCount()-1);
                        }
                        if(numPlots == 3) {
                            std::vector<double> y3;
                            for(unsigned int ch=0; ch<64; ch++)
                            {
                                double theTotalTime = 0;
                                int bcid = m_fit_start_bcid[fec][hybrid][chip][ch];
                                int z = 0;
                                for(int n=0; n<NUM_BCID; n++) {
                                    theTotalTime += m_percent_bcid[n][bit][fec][hybrid][chip][ch]*((bcid+n)*g_clock_period + (m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] - m_file_time_offset[fec][hybrid][chip][ch])*  m_file_time_slope[fec][hybrid][chip][ch]);
                                    z++;
                                }
                                y3.push_back(theTotalTime);
                            }
                            plot->addGraph();
                            plot->graph(bit*numPlots+2)->setData(
                                QVector<double>(x.begin(), x.end()),
                                QVector<double>(y3.begin(), y3.end()));
                            plot->graph(bit*numPlots+2)->setPen(QPen(Qt::magenta,1,Qt::SolidLine));
                            if(bit > 0) {
                                plot->legend->removeItem(plot->legend->itemCount()-1);
                            }
                        }
                    }
                    plot->graph(0)->setName("measured");
                    plot->graph(1)->setName("corrected");
                    if(numPlots == 3) {
                        plot->graph(2)->setName("corrected from file");
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
                }
                // corrections
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                    int numPlots = 2;
                    if(m_file_time_offset[fec][hybrid][chip].size() == 64 && m_file_time_offset[fec][hybrid][chip].size() == 64) {
                        numPlots = 4;
                    }
                    plot->addGraph();
                    std::vector<double> x;
                    std::vector<double> y_slope;
                    std::vector<double> y_offset;
                    for(unsigned int ch=0; ch<64; ch++)
                    {
                        //if(!(m_slope[fec][hybrid][chip][ch] == 0.0 && m_offset[fec][hybrid][chip][ch] == -1.0)) {
                        x.push_back(ch);
                        y_offset.push_back(m_offset[fec][hybrid][chip][ch]);
                        y_slope.push_back(m_slope[fec][hybrid][chip][ch]);
                        //}
                    }
                    plot->graph(0)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(m_offset[fec][hybrid][chip].begin(), m_offset[fec][hybrid][chip].end()));
                    plot->graph(0)->setPen(QPen(Qt::blue,2,Qt::SolidLine));
                    plot->graph(0)->setName("offset");

                    plot->addGraph();
                    plot->graph(1)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(m_slope[fec][hybrid][chip].begin(), m_slope[fec][hybrid][chip].end()));
                    plot->graph(1)->setPen(QPen(Qt::darkGreen,2,Qt::SolidLine));
                    plot->graph(1)->setName("slope");
                    if(numPlots == 4) {
                        plot->addGraph();
                        plot->graph(2)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(m_file_time_offset[fec][hybrid][chip].begin(), m_file_time_offset[fec][hybrid][chip].end()));
                        plot->graph(2)->setPen(QPen(Qt::cyan,2,Qt::DotLine));
                        plot->graph(2)->setName("offset from file");

                        plot->addGraph();
                        plot->graph(3)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(m_file_time_slope[fec][hybrid][chip].begin(), m_file_time_slope[fec][hybrid][chip].end()));
                        plot->graph(3)->setPen(QPen(Qt::green,2,Qt::DotLine));
                        plot->graph(3)->setName("slope from file");
                    }

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->yAxis->rescale();
                }
                // fit
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {
                    plot->addGraph();
                    std::vector<double> x;
                    std::vector<double> y;
                    std::vector<double> yf;
                    double slope = 0;
                    double offset = 0;

                    if(m_slope[fec][hybrid][chip][ch] != 0.0) {
                        slope = 1/m_slope[fec][hybrid][chip][ch];
                        offset =  m_offset[fec][hybrid][chip][ch] + (m_fit_start_time[fec][hybrid][chip][ch])*slope;

                    }
                    int m =  m_fit_start_time[fec][hybrid][chip][ch]/(g_time_factor);
                    if(m_fit_start_time[fec][hybrid][chip][ch]/(g_time_factor) < 0) {
                        m = m - 1;
                    }
                    double firstTime = m*g_time_factor - m_fit_start_time[fec][hybrid][chip][ch];
                    for(int bit=0; bit<m_number_bits;bit++){
                        if(m_fit_y[bit][fec][hybrid][chip][ch] != -9999.0) {
                            y.push_back(m_fit_y[bit][fec][hybrid][chip][ch]);

                            double theTime = firstTime + bit*g_time_factor;
                            x.push_back(theTime);
                            yf.push_back(offset + theTime*slope);
                        }
                    }
                    plot->graph(0)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(y.begin(), y.end()));
                    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
                    plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
                    plot->graph(0)->setName("measured");
                    plot->addGraph();
                    plot->graph(1)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(yf.begin(), yf.end()));
                    plot->graph(1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                    plot->graph(1)->setName(QString("fit: " + QString::number(slope) + "* t + " + QString::number(offset)));

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);
                    plot->xAxis->setRange(firstTime, firstTime + m_number_bits*g_time_factor);
                    plot->yAxis->rescale();
                }
                //TDC distribution
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 3) {
                    int bit = m_daqWindow->m_ui->choiceBit->currentIndex();
                    if(bit == -1) {
                        bit = 0;
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    for(int n=0; n<NUM_BCID; n++) {
                        int bcid = m_fit_start_bcid[fec][hybrid][chip][ch]+m_reference_BCID+n;

                        plot->addGraph();
                        std::vector<double> x;
                        std::vector<double> y;
                        auto it = std::partition(m_data[bit][fec][hybrid][chip][ch].begin(), m_data[bit][fec][hybrid][chip][ch].end(), [bcid](int val) { return (bcid*1000 <=val && bcid*1000+255 >= val); });
                        for(int tdc = 0; tdc < 256; tdc++) {
                            long cnt = std::count(m_data[bit][fec][hybrid][chip][ch].begin(),it, bcid*1000+tdc);
                            x.push_back(tdc);
                            y.push_back(cnt);
                        }

                        plot->graph(n)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(y.begin(), y.end()));
                        plot->graph(n)->setScatterStyle(QCPScatterStyle::ssDisc);

                        if(bcid-m_reference_BCID == -2) {
                            plot->graph(n)->setPen(QPen(Qt::darkCyan));
                            plot->graph(n)->setBrush(QBrush(Qt::cyan));
                        }
                        else if(bcid-m_reference_BCID == -1) {
                            plot->graph(n)->setPen(QPen(Qt::darkRed));
                            plot->graph(n)->setBrush(QBrush(Qt::red));
                        }
                        else if(bcid-m_reference_BCID == 0) {
                            plot->graph(n)->setPen(QPen(Qt::darkGreen));
                            plot->graph(n)->setBrush(QBrush(Qt::green));
                        }
                        else if(bcid-m_reference_BCID == 1) {
                            plot->graph(n)->setPen(QPen(Qt::darkBlue));
                            plot->graph(n)->setBrush(QBrush(Qt::blue));

                        }
                        else if(bcid-m_reference_BCID == 2) {
                            plot->graph(n)->setPen(QPen(Qt::darkMagenta));
                            plot->graph(n)->setBrush(QBrush(Qt::magenta));

                        }
                        plot->graph(n)->setName(QString("BCID "+ QString::number(bcid)));
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->yAxis->rescale();
                }
                // BCID percent
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 4) {

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    for(int n=0; n<NUM_BCID; n++) {
                        int bcid = m_fit_start_bcid[fec][hybrid][chip][ch]+m_reference_BCID+n;
                        plot->addGraph();
                        std::vector<double> x;
                        std::vector<double> y;
                        for(int bit=0; bit<m_number_bits;bit++){

                            x.push_back(bit*g_time_factor);
                            y.push_back(100*m_percent_bcid[n][bit][fec][hybrid][chip][ch]);
                        }

                        plot->graph(n)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(y.begin(), y.end()));

                        if(bcid-m_reference_BCID == -2) {
                            plot->graph(n)->setPen(QPen(Qt::darkCyan,2,Qt::SolidLine));
                        }
                        else if(bcid-m_reference_BCID == -1) {
                            plot->graph(n)->setPen(QPen(Qt::red,2,Qt::SolidLine));

                        }
                        else if(bcid-m_reference_BCID == 0) {
                            plot->graph(n)->setPen(QPen(Qt::darkGreen,2,Qt::SolidLine));

                        }
                        else if(bcid-m_reference_BCID == 1) {
                            plot->graph(n)->setPen(QPen(Qt::blue,2,Qt::SolidLine));

                        }
                        else if(bcid-m_reference_BCID == 2) {
                            plot->graph(n)->setPen(QPen(Qt::darkMagenta,2,Qt::SolidLine));

                        }
                        plot->graph(n)->setName(QString("BCID "+ QString::number(bcid)));
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->xAxis->setRange(0, m_number_bits*g_time_factor);
                    plot->yAxis->rescale();

                }
            }
            //Offline Time walk
            else if(m_modeIndex == 3)
            {
                // calibrated time
                if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 0) {
                    int numPlots = 2;
                    if(m_file_timewalk_a[fec][hybrid][chip].size() == 64 && m_file_timewalk_b[fec][hybrid][chip].size() == 64 && m_file_timewalk_c[fec][hybrid][chip].size() == 64 && m_file_timewalk_d[fec][hybrid][chip].size() == 64) {
                        numPlots = 3;
                    }
                    double plot_correction = 0;
                    int n = 0;
                    for(int ch=0; ch<64; ch++) {
                        if(m_mean[m_number_bits-1][fec][hybrid][chip][ch] != -9999.0) {
                            plot_correction += m_mean[(int)(m_number_bits/2)][fec][hybrid][chip][ch];
                            n++;
                        }
                    }
                    plot_correction = plot_correction/n;
                    n = 0;
                    for(int bit=0; bit<m_number_bits;bit++){
                        if(bit%4==0) {
                            std::vector<double> y0;
                            std::vector<double> y2;
                            std::vector<double> y3;
                            for(int ch =0; ch<64; ch++){
                                if(m_mean[m_number_bits-1][fec][hybrid][chip][ch] != -9999.0) {
                                    y0.push_back(m_mean[bit][fec][hybrid][chip][ch] - plot_correction);
                                }
                                else {
                                    y0.push_back(0);
                                }
                                if(m_mean2[bit][fec][hybrid][chip][ch] != -9999 && m_fit_a[fec][hybrid][chip][ch]!= -9999 && m_fit_b[fec][hybrid][chip][ch]!= -9999 && m_fit_c[fec][hybrid][chip][ch]!= -9999 && m_fit_d[fec][hybrid][chip][ch]!= -9999) {
                                    double corrected_time = logisticcalc4(m_mean2[bit][fec][hybrid][chip][ch], m_fit_a[fec][hybrid][chip][ch], m_fit_b[fec][hybrid][chip][ch], m_fit_c[fec][hybrid][chip][ch], m_fit_d[fec][hybrid][chip][ch]);
                                    //double corrected_time = m_fit_a[fec][hybrid][chip][ch]*0.5*errorfunctionc((m_mean2[bit][fec][hybrid][chip][ch]-m_fit_b[fec][hybrid][chip][ch])/(sqrt(2)* m_fit_c[fec][hybrid][chip][ch] )) + m_fit_d[fec][hybrid][chip][ch];
                                    y2.push_back(m_mean[bit][fec][hybrid][chip][ch] - plot_correction - corrected_time);
                                }
                                else {
                                    y2.push_back(0);
                                }
                                if(numPlots == 3) {
                                    if(m_mean2[bit][fec][hybrid][chip][ch] != -9999 && m_file_timewalk_a[fec][hybrid][chip][ch]!= -9999 && m_file_timewalk_b[fec][hybrid][chip][ch]!= -9999 && m_file_timewalk_c[fec][hybrid][chip][ch]!= -9999 && m_file_timewalk_d[fec][hybrid][chip][ch]!= -9999) {
                                        double corrected_time = logisticcalc4(m_mean2[bit][fec][hybrid][chip][ch], m_file_timewalk_a[fec][hybrid][chip][ch], m_file_timewalk_b[fec][hybrid][chip][ch], m_file_timewalk_c[fec][hybrid][chip][ch], m_file_timewalk_d[fec][hybrid][chip][ch]);
                                        y3.push_back(m_mean[bit][fec][hybrid][chip][ch] - plot_correction - corrected_time);
                                    }
                                    else {
                                        y3.push_back(0);
                                    }
                                }
                            }


                            //Time
                            plot->addGraph(plot->xAxis, plot->yAxis);
                            plot->graph(n*numPlots)->setData(
                                QVector<double>(m_x.begin(), m_x.end()),
                                QVector<double>(y0.begin(), y0.end()));
                            plot->graph(n*numPlots)->setPen(QPen(Qt::black,1,Qt::SolidLine));
                            if(n > 0) {
                                plot->legend->removeItem(plot->legend->itemCount()-1);
                            }
                            /*
                            //ADC
                            plot->addGraph(plot->xAxis, plot->yAxis2);
                            plot->graph(bit*numPlots+1)->setData(
                                        QVector<double>(m_x.begin(), m_x.end()),
                                        QVector<double>(m_mean2[bit][fec][hybrid][chip].begin(), m_mean2[bit][fec][hybrid][chip].end()));
                            plot->graph(bit*numPlots+1)->setPen(QPen(Qt::blue,1,Qt::DashLine));

                            if(bit > 0) {
                                plot->legend->removeItem(plot->legend->itemCount()-1);
                            }
                            */

                            plot->addGraph(plot->xAxis, plot->yAxis);
                            plot->graph(n*numPlots+1)->setData(
                                QVector<double>(m_x.begin(), m_x.end()),
                                QVector<double>(y2.begin(), y2.end()));
                            plot->graph(n*numPlots+1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                            if(n > 0) {
                                plot->legend->removeItem(plot->legend->itemCount()-1);
                            }


                            if(numPlots == 3) {
                                plot->addGraph(plot->xAxis, plot->yAxis);
                                plot->graph(n*numPlots+2)->setData(
                                    QVector<double>(m_x.begin(), m_x.end()),
                                    QVector<double>(y3.begin(), y3.end()));
                                plot->graph(n*numPlots+2)->setPen(QPen(Qt::magenta,1,Qt::SolidLine));
                                if(n > 0) {
                                    plot->legend->removeItem(plot->legend->itemCount()-1);
                                }
                            }
                            n++;
                        }
                    }
                    plot->graph(0)->setName(QString("measured: " + QString::number(m_dac_setting[0]) + " DAC - " + QString::number(m_dac_setting[m_bitCount - 1]) + " DAC"));
                    //plot->graph(1)->setName("ADC");
                    plot->graph(1)->setName(QString("time-walk corrected: " + QString::number(m_dac_setting[0]) + " DAC - " + QString::number(m_dac_setting[m_bitCount - 1]) + " DAC"));
                    if(numPlots == 3) {
                        plot->graph(2)->setName("corrected from file");
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                }
                // corrections
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1) {
                    int numPlots = 4;
                    if(m_file_timewalk_a[fec][hybrid][chip].size() == 64 && m_file_timewalk_b[fec][hybrid][chip].size() == 64 && m_file_timewalk_c[fec][hybrid][chip].size() == 64 && m_file_timewalk_d[fec][hybrid][chip].size() == 64) {
                        numPlots = 8;
                    }

                    std::vector<double> x;
                    for(unsigned int ch=0; ch<64; ch++)
                    {
                        x.push_back(ch);
                    }
                    plot->addGraph();
                    plot->graph(0)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(m_fit_a[fec][hybrid][chip].begin(), m_fit_a[fec][hybrid][chip].end()));
                    plot->graph(0)->setPen(QPen(Qt::blue,2,Qt::SolidLine));
                    plot->graph(0)->setName("a");

                    plot->addGraph();
                    plot->graph(1)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(m_fit_b[fec][hybrid][chip].begin(), m_fit_b[fec][hybrid][chip].end()));
                    plot->graph(1)->setPen(QPen(Qt::darkGreen,2,Qt::SolidLine));
                    plot->graph(1)->setName("b");

                    plot->addGraph();
                    plot->graph(2)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(m_fit_c[fec][hybrid][chip].begin(), m_fit_c[fec][hybrid][chip].end()));
                    plot->graph(2)->setPen(QPen(Qt::green,2,Qt::SolidLine));
                    plot->graph(2)->setName("c");

                    plot->addGraph();
                    plot->graph(3)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(m_fit_d[fec][hybrid][chip].begin(), m_fit_d[fec][hybrid][chip].end()));
                    plot->graph(3)->setPen(QPen(Qt::cyan,2,Qt::SolidLine));
                    plot->graph(3)->setName("d");


                    if(numPlots == 8) {
                        plot->addGraph();
                        plot->graph(4)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(m_file_timewalk_a[fec][hybrid][chip].begin(), m_file_timewalk_a[fec][hybrid][chip].end()));
                        plot->graph(4)->setPen(QPen(Qt::blue,2,Qt::SolidLine));
                        plot->graph(4)->setName("a from file");

                        plot->addGraph();
                        plot->graph(5)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(m_file_timewalk_b[fec][hybrid][chip].begin(), m_file_timewalk_b[fec][hybrid][chip].end()));
                        plot->graph(5)->setPen(QPen(Qt::darkGreen,2,Qt::SolidLine));
                        plot->graph(5)->setName("b from file");

                        plot->addGraph();
                        plot->graph(6)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(m_file_timewalk_c[fec][hybrid][chip].begin(), m_file_timewalk_c[fec][hybrid][chip].end()));
                        plot->graph(6)->setPen(QPen(Qt::green,2,Qt::SolidLine));
                        plot->graph(6)->setName("c from file");

                        plot->addGraph();
                        plot->graph(7)->setData(
                            QVector<double>(x.begin(), x.end()),
                            QVector<double>(m_file_timewalk_d[fec][hybrid][chip].begin(), m_file_timewalk_d[fec][hybrid][chip].end()));
                        plot->graph(7)->setPen(QPen(Qt::cyan,2,Qt::SolidLine));
                        plot->graph(7)->setName("d from file");
                    }

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    //plot->yAxis->rescale();
                }
                // fit
                else if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 2) {

                    std::vector<double> x;
                    std::vector<double> xf;
                    std::vector<double> y;
                    std::vector<double> yf;
                    if(m_fit_a[fec][hybrid][chip][ch] != -9999.0 && m_fit_b[fec][hybrid][chip][ch] != -9999.0 && m_fit_c[fec][hybrid][chip][ch] != -9999.0  && m_fit_d[fec][hybrid][chip][ch] != -9999.0 ) {
                        for(int adc=0; adc<1023;adc+=50) {
                            double fittedTime = logisticcalc4(adc, m_fit_a[fec][hybrid][chip][ch], m_fit_b[fec][hybrid][chip][ch], m_fit_c[fec][hybrid][chip][ch], m_fit_d[fec][hybrid][chip][ch]);
                            //double fittedTime = m_fit_a[fec][hybrid][chip][ch]*0.5*errorfunctionc((adc-m_fit_b[fec][hybrid][chip][ch])/(sqrt(2)* m_fit_c[fec][hybrid][chip][ch] )) + m_fit_d[fec][hybrid][chip][ch];
                            xf.push_back(adc);
                            yf.push_back(fittedTime);
                        }
                        for(int bit=0; bit<m_number_bits;bit++) {
                            x.push_back(m_mean2[bit][fec][hybrid][chip][ch]);
                            y.push_back(m_mean[bit][fec][hybrid][chip][ch]-m_mean[(int)(m_number_bits/2)][fec][hybrid][chip][ch]);
                        }
                    }
                    else {
                        for(int adc=0; adc<1023;adc+=50){
                            xf.push_back(0);
                            yf.push_back(0);
                        }
                        for(int bit=0; bit<m_number_bits;bit++) {
                            x.push_back(0);
                            y.push_back(0);
                        }
                    }
                    plot->addGraph();
                    plot->graph(0)->setData(
                        QVector<double>(x.begin(), x.end()),
                        QVector<double>(y.begin(), y.end()));
                    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
                    plot->graph(0)->setPen(QPen(Qt::blue));

                    plot->addGraph();
                    plot->graph(1)->setData(
                        QVector<double>(xf.begin(), xf.end()),
                        QVector<double>(yf.begin(), yf.end()));
                    plot->graph(1)->setPen(QPen(Qt::red));
                    plot->graph(0)->setName("measured");
                    plot->graph(1)->setName(QString("4PL-fit:  a=") + QString::number( m_fit_a[fec][hybrid][chip][ch],'f',2)  + QString(", b=") + QString::number( m_fit_b[fec][hybrid][chip][ch],'f',2)
                                            + QString("\nc=") + QString::number( m_fit_c[fec][hybrid][chip][ch],'f',2)  + QString(", d=") + QString::number( m_fit_d[fec][hybrid][chip][ch],'f',2)
                                            + QString("\nr2=") + QString::number( m_fit_error_r2[fec][hybrid][chip][ch],'f',2)  + QString(", rms=") + QString::number( m_fit_error_rms[fec][hybrid][chip][ch],'f',5));

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
                    //plot->yAxis->rescale();
                }

            }
            //Online ADC or TDC or threshold
            else if(m_modeIndex == 4 || m_modeIndex == 5 || (m_modeIndex == 7 && m_isThresholdCalibration))
            {
                for(int bit=0; bit<m_number_bits;bit++){
                    plot->addGraph();
                    plot->graph(bit)->setData(
                        QVector<double>(m_x.begin(), m_x.end()),
                        QVector<double>(m_mean[bit][fec][hybrid][chip].begin(), m_mean[bit][fec][hybrid][chip].end()));
                    plot->graph(bit)->setPen(QPen(QColor(static_cast<unsigned int>(bit)*colorFactor)));
                    plot->legend->removeItem(plot->legend->itemCount()-1);
                }
                int n_graph =plot->graphCount();
                plot->addGraph();
                plot->graph(n_graph)->setPen(QPen(Qt::red,4,Qt::SolidLine));
                plot->graph(n_graph)->setName("Best common value");
                plot->graph(n_graph)->setData( QVector<double>(m_x.begin(), m_x.end()), QVector<double>(m_y[fec][hybrid][chip].begin(), m_y[fec][hybrid][chip].end()));
                plot->addGraph();
                plot->graph(n_graph+1)->setData(QVector<double>(m_x.begin(), m_x.end()), QVector<double>(m_calVal[fec][hybrid][chip].begin(), m_calVal[fec][hybrid][chip].end()));
                plot->graph(n_graph+1)->setPen(QPen(Qt::green,4,Qt::SolidLine));
                plot->graph(n_graph+1)->setName("Calibrated curve");
                plot->yAxis->rescale();
            }
            //S-curve
            else if(m_modeIndex == 6)
            {
                if(m_daqWindow->m_ui->choicePlotTime->currentIndex() == 1 ) {
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->addGraph();
                    std::vector<double> x;
                    std::vector<double> y;
                    for(int bit=0; bit<m_number_bits;bit++){
                        int cnt = m_channel_y[idx][bit];
                        double theTime = m_time[bit];
                        if(theTime > 0) {
                            x.push_back(m_dac_measured[fec][hybrid][chip][bit]);
                            y.push_back(cnt/(1000*theTime));
                        }
                    }
                    plot->graph(0)->setData( QVector<double>(x.begin(), x.end()), QVector<double>(y.begin(), y.end()));
                    plot->graph(0)->setPen(QPen(Qt::blue,2,Qt::SolidLine));
                    plot->graph(0)->setLineStyle(QCPGraph::lsLine);
                    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
                    plot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
                    plot->graph(0)->setName(QString("measured"));


                    QCPErrorBars *errorBars = new QCPErrorBars(plot->xAxis, plot->yAxis);
                    errorBars->removeFromLegend();
                    errorBars->setAntialiased(false);
                    errorBars->setDataPlottable(plot->graph(0));
                    errorBars->setPen(QPen(QColor(180,180,180)));
                    std::vector< double > errors;
                    for(int bit=0; bit<m_number_bits;bit++) {
                        errors.push_back(sqrt(m_channel_y[idx][bit])/(1000*m_time[bit]));
                    }
                    errorBars->setData(QVector<double>(errors.begin(), errors.end()));

                    if(m_fit_scale[idx] != -9999.0 && m_fit_mean[idx] != -9999.0 && m_fit_sigma[idx] != -9999.0) {
                        plot->addGraph();
                        std::vector<double> fit;
                        std::vector<double> x;
                        double step = (m_dac_measured[fec][hybrid][chip][m_number_bits-1] - m_dac_measured[fec][hybrid][chip][0])/(m_number_bits*20-1);
                        for(int bit=0; bit<m_number_bits*20;bit++) {
                            double theXVal = m_dac_measured[fec][hybrid][chip][0]+step*bit;
                            x.push_back(theXVal);
                            double val = 0;
                            if(m_scan_type == 0) {
                                val = m_fit_scale[idx]*0.5*errorfunctionc((theXVal-m_fit_mean[idx])/(sqrt(2)* m_fit_sigma[idx] ));
                            }
                            else {
                                val = m_fit_scale[idx]*0.5*errorfunctionc((-theXVal+m_fit_mean[idx])/(sqrt(2)* m_fit_sigma[idx] ));
                            }
                            fit.push_back(val);
                        }
                        plot->graph(1)->setData( QVector<double>(x.begin(), x.end()), QVector<double>(fit.begin(), fit.end()));
                        plot->graph(1)->setPen(QPen(Qt::red,2,Qt::SolidLine));
                        plot->graph(1)->setLineStyle(QCPGraph::lsLine);
                        plot->graph(1)->setName(QString("Fit\nScale ") + QString::number(m_fit_scale[idx],'f',2)  + " (+/- " + QString::number(m_fit_error_scale[idx],'f',5) + ")"
                                                + QString("\nMean ") + QString::number(m_fit_mean[idx],'f',2)  + " (+/- " + QString::number(m_fit_error_mean[idx],'f',5)  + ")"
                                                + QString("\nSigma ") + QString::number(m_fit_sigma[idx],'f',2)  + " (+/- " + QString::number(m_fit_error_sigma[idx],'f',5)  + ")");
                    }
                }
                else {
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    plot->addGraph();
                    std::vector<double> x;
                    std::vector<double> y_mean;
                    std::vector<double> y_sigma;
                    std::vector<double> error_mean;
                    std::vector<double> error_sigma;
                    for(int ch=0; ch < 64; ch++) {
                        y_mean.push_back(m_fit_mean[ch]);
                        y_sigma.push_back(m_fit_sigma[ch]);
                        error_mean.push_back(m_fit_error_mean[ch]);
                        error_sigma.push_back(m_fit_error_sigma[ch]);
                    }
                    QCPErrorBars *errorBars = new QCPErrorBars(plot->xAxis, plot->yAxis);
                    errorBars->removeFromLegend();
                    errorBars->setAntialiased(false);
                    errorBars->setDataPlottable(plot->graph(0));
                    errorBars->setPen(QPen(QColor(180,180,180)));

                    if(idx == 0) {
                        plot->graph(0)->setData( QVector<double>(m_x.begin(), m_x.end()), QVector<double>(y_mean.begin(), y_mean.end()));
                        plot->graph(0)->setName(QString("Fit mean"));
                        errorBars->setData(QVector<double>(error_mean.begin(), error_mean.end()));
                    }
                    else {
                        plot->graph(0)->setData( QVector<double>(m_x.begin(), m_x.end()), QVector<double>(y_sigma.begin(), y_sigma.end()));
                        plot->graph(0)->setName(QString("Fit sigma"));
                        errorBars->setData(QVector<double>(error_sigma.begin(), error_sigma.end()));
                    }

                    plot->graph(0)->setPen(QPen(Qt::blue,2,Qt::SolidLine));
                    plot->graph(0)->setLineStyle(QCPGraph::lsLine);
                    plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
                    plot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));


                }
                plot->yAxis->rescale();
            }
            //Threshold measurement
            else if(m_modeIndex == 7 && !m_isThresholdCalibration)
            {
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                plot->addGraph();
                plot->graph(0)->setName(QString("Total Threshold"));
                plot->graph(0)->setData(
                    QVector<double>(m_x.begin(), m_x.end()),
                    QVector<double>(m_mean[0][fec][hybrid][chip].begin(), m_mean[0][fec][hybrid][chip].end()));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
                plot->addGraph();
                plot->graph(1)->setName(QString("Channel Threshold"));
                plot->graph(1)->setData(
                    QVector<double>(m_x.begin(), m_x.end()),
                    QVector<double>(m_mean[1][fec][hybrid][chip].begin(), m_mean[1][fec][hybrid][chip].end()));
                plot->graph(1)->setPen(QPen(Qt::darkGreen,1,Qt::SolidLine));
                plot->addGraph();
                plot->graph(2)->setName(QString("Global Threshold"));
                plot->graph(2)->setData(
                    QVector<double>(m_x.begin(), m_x.end()),
                    QVector<double>(m_mean[2][fec][hybrid][chip].begin(), m_mean[2][fec][hybrid][chip].end()));
                plot->graph(2)->setPen(QPen(Qt::red,1,Qt::DashLine));
                plot->yAxis->rescale();
            }
            //Pedestal
            else if(m_modeIndex == 8)
            {
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                plot->addGraph();
                plot->graph(0)->setName(QString("Pedestal"));
                plot->graph(0)->setData(
                    QVector<double>(m_x.begin(), m_x.end()),
                    QVector<double>(m_mean[0][fec][hybrid][chip].begin(), m_mean[0][fec][hybrid][chip].end()));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));

            }
            //Pulser DAC
            else if(m_modeIndex == 9)
            {
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                double slope_high = (m_y[fec][hybrid][chip][10] - m_y[fec][hybrid][chip][2])/(m_dac_setting[10] - m_dac_setting[2]);
                double offset_high = m_y[fec][hybrid][chip][10] - slope_high*m_dac_setting[10];

                double slope_low = (m_y[fec][hybrid][chip][1] - m_y[fec][hybrid][chip][0])/(m_dac_setting[1] - m_dac_setting[0]);
                double offset_low = m_y[fec][hybrid][chip][0];
                plot->addGraph();
                plot->graph(0)->setName(QString("measured DAC [mV]\n(slope low: ") + QString::number(slope_low,'f',2)  + ", offset low " + QString::number(offset_low,'f',2) + ",\n"
                                        + QString("slope high: ") + QString::number(slope_high,'f',2)  + QString(", offset high ") + QString::number(offset_high,'f',2) + ")");
                plot->graph(0)->setData(
                    QVector<double>(m_dac_setting.begin(), m_dac_setting.end()),
                    QVector<double>(m_y[fec][hybrid][chip].begin(), m_y[fec][hybrid][chip].end()));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
            }
            //Threshold DAC
            else if(m_modeIndex == 10)
            {
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                double slope_high = (m_y[fec][hybrid][chip][10] - m_y[fec][hybrid][chip][2])/(m_dac_setting[10] - m_dac_setting[2]);
                double offset_high = m_y[fec][hybrid][chip][10] - slope_high*m_dac_setting[10];

                double slope_low = (m_y[fec][hybrid][chip][1] - m_y[fec][hybrid][chip][0])/(m_dac_setting[1] - m_dac_setting[0]);
                double offset_low = m_y[fec][hybrid][chip][0];
                plot->addGraph();
                plot->graph(0)->setName(QString("measured DAC [mV]\n(slope low: ") + QString::number(slope_low,'f',2)  + ", offset low " + QString::number(offset_low,'f',2) + ",\n"
                                        + QString("slope high: ") + QString::number(slope_high,'f',2)  + QString(", offset high ") + QString::number(offset_high,'f',2) + ")");
                plot->graph(0)->setData(
                    QVector<double>(m_dac_setting.begin(), m_dac_setting.end()),
                    QVector<double>(m_y[fec][hybrid][chip].begin(), m_y[fec][hybrid][chip].end()));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
            }
            //Latency calibration
            else if(m_modeIndex == 11 || m_modeIndex == 12)
            {
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                int start = m_start_reset_latency;
                QVector<double> m_x;
                if(m_modeIndex == 12) {
                    start = m_start_tp_latency;
                }
                for(int bit=0; bit<m_number_bits;bit++){
                    m_x.push_back(start+bit);
                }
                int idxFec = -1;
                for(int fec=0; fec<FECS_PER_DAQ; fec++) {
                    if (m_daqWindow->m_daq.GetFEC(fec)){
                        int n = m_fecLatencyReset[fec];
                        if(m_modeIndex == 12) {
                            n = m_fecLatencyTP[fec];
                        }
                        if(n > -1) {
                            idxFec++;
                            QVector<double> m_y;
                            for(int bit=0; bit<m_number_bits;bit++){
                                m_y.push_back(m_mean2[bit][fec][0][0][0]);
                            }
                            plot->addGraph();
                            if(g_slow_control == 2){
                                int fecId = m_fecPosID[fec];
                                title = g_card_name + " " + QString::number(fec) + " (IP " +  QString::number(fecId) + ") CH " + QString::number(ch);
                            }
                            else {
                                int ring = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("ring");
                                int fen = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("fen");
                                title = g_card_name + " " + QString::number(fec) + " (" +QString::number(ring).rightJustified(2, '0')
                                        + "_"+QString::number(fen).rightJustified(2, '0') + ") CH " + QString::number(ch);
                            }
                            plot->graph(idxFec)->setName(title);
                            plot->graph(idxFec)->setData(m_x, m_y);
                            if(idxFec == 0) {
                                plot->graph(idxFec)->setScatterStyle(QCPScatterStyle::ssDisc);
                            }
                            else if(idxFec == 1) {
                                plot->graph(idxFec)->setScatterStyle(QCPScatterStyle::ssTriangle);
                            }
                            else if(idxFec == 2) {
                                plot->graph(idxFec)->setScatterStyle(QCPScatterStyle::ssCross);
                            }
                            else if(idxFec == 3) {
                                plot->graph(idxFec)->setScatterStyle(QCPScatterStyle::ssDiamond);
                            }
                            else if(idxFec == 4) {
                                plot->graph(idxFec)->setScatterStyle(QCPScatterStyle::ssCircle);
                            }
                            else if(idxFec == 5) {
                                plot->graph(idxFec)->setScatterStyle(QCPScatterStyle::ssStar);
                            }
                            else if(idxFec == 6) {
                                plot->graph(idxFec)->setScatterStyle(QCPScatterStyle::ssTriangleInverted);
                            }
                            else if(idxFec == 7) {
                                plot->graph(idxFec)->setScatterStyle(QCPScatterStyle::ssSquare);
                            }
                            plot->graph(idxFec)->setPen(QPen(QColor(0,0,100),2,Qt::SolidLine));
                        }
                    }
                }
            }

        }
        plot->replot();
    }
}



// ------------------------------------------------------------------------ //
void CalibrationModule::GetActiveVMMs(){
    m_vmmActs.clear();
    m_fecPosID.clear();
    m_fecIDPos.clear();

    for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
        if (m_daqWindow->m_daq.GetFEC(fec) ){
            m_fecPosID[fec] = m_daqWindow->m_daq.m_fecs[fec].GetID();
            m_fecIDPos[m_daqWindow->m_daq.m_fecs[fec].GetID()] = fec;
            //std::cout << "m_daqWindow->m_daq.m_fecs[fec].GetID() " << m_daqWindow->m_daq.m_fecs[fec].GetID()
            //          << " fec " << fec << std::endl;
            for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_FEC; hybrid++){
                if( m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid) ){

                    for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                        m_vmmActs.push_back(fec*HYBRIDS_PER_FEC*VMMS_PER_HYBRID+
                                            hybrid*VMMS_PER_HYBRID +vmm);
                    }
                }

            }
        }
    }

    //();
}

// ------------------------------------------------------------------------ //
void CalibrationModule::updatePlot(){
    //std::cout << "Update Plot" << std::endl;
    PlotData();
}

double CalibrationModule::ThresholdDAC_to_mV(int dac) {
    return 0.801*dac+25.182;
}

int CalibrationModule::Threshold_mV_to_DAC(double mV) {
    return static_cast<int>(std::round((mV-25.182)/0.801));
}

double CalibrationModule::PulserDAC_to_mV(int dac) {
    return  (0.796*dac+30.615);
}

double CalibrationModule::PulserDAC_to_PulseHeight_mV(int dac, int gain_idx) {
    double m_pulseSlope[8] = {0.129,0.245,0.728,1.107,1.476,2.166,2.925,3.94};
    double m_pulseOffset[8] = {172.1,176.5,175,162,156.5,163.5,151.33,141.33};
    return dac*m_pulseSlope[gain_idx]+m_pulseOffset[gain_idx];
}

int CalibrationModule::PulseHeight_mV_to_PulserDAC( double pulseHeight, int gain_idx) {
    double m_pulseSlope[8] = {0.129,0.245,0.728,1.107,1.476,2.166,2.925,3.94};
    double m_pulseOffset[8] = {172.1,176.5,175,162,156.5,163.5,151.33,141.33};
    return static_cast<int>(std::round(pulseHeight/(m_pulseSlope[gain_idx]+m_pulseOffset[gain_idx])));
}

void CalibrationModule::AddVMMsToCombobox() {
    m_daqWindow->m_ui->comboBoxFec->clear();
    m_daqWindow->m_ui->label_vmm->setText("Display VMMs");
    m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 0-7"));
    if(m_vmmActs.size() >= 8 )
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 8-15"));
    }
    if(m_vmmActs.size() >= 16 )
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 16-23"));
    }
    if(m_vmmActs.size() >= 24)
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 24-31"));
    }
    if(m_vmmActs.size() >= 32 )
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 32-39"));
    }
    if(m_vmmActs.size() >= 40 )
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 40-47"));
    }
    if(m_vmmActs.size() >= 48 )
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 48-55"));
    }
    if(m_vmmActs.size() >= 56 )
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 56-63"));
    }
    if(m_vmmActs.size() >= 64 )
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 64-71"));
    }
    if(m_vmmActs.size() >= 72 )
    {
        m_daqWindow->m_ui->comboBoxFec->addItem(QString("VMM 72-79"));
    }
}

// ------------------------------------------------------------------------ //
void CalibrationModule::StartCalibration(){
    m_numHits = 0;
    m_bitCount = -1;
    m_start = 0;
    m_end = 0;
    m_dataAvailable = false;
    m_pulser_dac = 0;
    m_threshold = -1;
    m_theChannel = 8;
    emit m_daqWindow->m_ui->openConnection->clicked();
    QThread::msleep(1);
    if(! (g_connection_ok)) {
        int ret = QMessageBox::critical(nullptr, tr("FEC communication"),
                                        "Communication couldn't be established!\nexit calibration!",
                                        QMessageBox::Ok);
        m_daqWindow->m_ui->pushButtonTakeData->setChecked(false);
        return;
    }

    GetSettings();
    PlotData();

    GetActiveVMMs();
    if(m_modeIndex != 11 && m_modeIndex != 12) {
        AddVMMsToCombobox();
    }
    InitializeDataStructures();
    if(IsCalibration())
    {
        SaveSettings("Latest_config");
        if(m_modeIndex == 1) {
            bool ok;
            QString theValue = QInputDialog::getText(nullptr, tr("ADC calibration"),
                                                     tr("Do you want to set the threshold DAC [1-1023]\n(-1 means the values are automatically chosen, 0 takes threshold active in slow control)?"),
                                                     QLineEdit::Normal,"-1",&ok);
            if(ok) {
                if(theValue.toDouble()>=0) {
                    m_threshold = theValue.toDouble();
                }
            }
        }
        else if(m_modeIndex == 2) {
            m_pulser_dac = -1;
            m_bcid_percentage = 0.90;
            if(g_clock_source==0) {
                m_bcid_percentage = 0.80;
            }
            unsigned long tp_offset = 4096;
            unsigned long tp_offset_last = 4096;
            for(int fec=0; fec < FECS_PER_DAQ; fec++) {
                if( m_daqWindow->m_daq.m_fecActs[fec == true]) {
                    tp_offset_last = tp_offset;
                    tp_offset = m_daqWindow->m_daq.m_fecs[fec].GetRegVal("tp_offset_first");
                    if(tp_offset_last != tp_offset && tp_offset_last != 4096) {
                        QMessageBox::warning(nullptr, tr("Time calibration"),
                                             "All FECs/assisters need the same setting for the offset of the first TP!\n",
                                             QMessageBox::Ok);
                        return;
                    }
                }
            }
            m_reference_BCID = tp_offset;
            //std::cout << "reference bcid " << m_reference_BCID << std::endl;
            bool ok;
            QStringList list = QInputDialog::getText(nullptr, tr("Time calibration"),
                                                     tr("Do you want to set the pulser DAC value [1-1023], the threshold DAC [1-1023] and the BCID percentage [0.0-1.0] for the fit\n(-1 means the values are automatically chosen, Threshold=0 takes threshold active in slow control)?"),
                                                     QLineEdit::Normal,"DAC=-1,THR=-1,BCID_PERCENT=-1",&ok).split(",");

			if (ok && list.count() == 3)
			{
				QString str = list[0];
				QVector<QStringView> words = QStringView{str}.split('=');
				QString param = words[0].toString();
				QString val = words[1].toString();
				if (param == "DAC")
				{
					m_pulser_dac = val.toInt();
				}

				str = list[1];
				words = QStringView{str}.split('=');
				param = words[0].toString();
				val = words[1].toString();
				if (param == "THR")
				{
					if (val.toDouble() >= 0)
					{
						m_threshold = val.toDouble();
					}
				}

				str = list[2];
				words = QStringView{str}.split('=');
				param = words[0].toString();
				val = words[1].toString();
				if (param == "BCID_PERCENT")
				{
					m_bcid_percentage = val.toDouble();
				}
			}
		}
        else if(m_modeIndex == 3){
            ApplyCalib(true);
        }
    }




    //S-curve needs user input for parameters
    if(m_modeIndex == 6)
    {
        bool ok;

        QStringList list = QInputDialog::getText(nullptr, tr("S-curve input parameters"),
                                                 tr("S-curve measurement on one VMM with user settings (gain, polarity, channel masking). On Windows, you can only measure around 8 channels, you have to mask the other channels.\n"
                                                    "Mandatory settings are: FEC-ID (as defined in the slow control, not IP address)=[0 to 7], VMM=[0 to 15]. The other settings are optionally, if they are not given the default 0 (automatically chosen) is used:\n"
                                                    "\tPULSE-HEIGHT=0 [0=automatically chosen (around 500 mV after per-amp), positive number=pulser DAC, -1=no test pulses (use noise)]\n"
                                                    "\tTHRESHOLD=0 [0=automatically chosen (around 500 mV), positive number=threshold DAC]\n"
                                                    "\tSCAN-WIDTH=0 [0=automatically chosen (60 steps with THRESHOLD in center), positive number=steps in scan interval (20-200)]\n"
                                                    "\tDIRECTION=0 [0=automatically chosen scan direction (up), 1=down]\n"
                                                    "\tSCAN-TYPE=0 [0=scan threshold, 1=scan pulse height]:"),
                                                 QLineEdit::Normal,"FEC=0,VMM=0,PULSE-HEIGHT=0,THRESHOLD=0,SCAN-WIDTH=0,DIRECTION=0,SCAN-TYPE=0",&ok).split(",");
        m_pulser_dac = 0;
        int scanWidth = 60;
        int threshold = 0;
        m_theFEC = -1;
        m_theVMM = -1;
        m_scan_type = 0;
        if(list.count() < 2) {
            QMessageBox::warning(nullptr, tr("S-curve input parameters"),
                                 "The FEC and the VMM have to be specified!\n",
                                 QMessageBox::Ok);
            return;
        }
        else {
            for(int n=0; n<list.count();n++) {
                QString str = list[n];
				QVector<QStringView> words = QStringView{str}.split('=');
				if (words.size() != 2)
				{
					QMessageBox::warning(nullptr, tr("S-curve input parameters"),
                                         "The correct format is PARAMETER=VALUE!\n",
                                         QMessageBox::Ok);
                    return;
				}
				QString param = words[0].toString();
                QString val = words[1].toString();
                if(param == "FEC") {
                    m_theFEC = val.toInt();
                }
                else if(param == "VMM") {
                    m_theVMM = val.toInt();
                }
                else if(param == "PULSE-HEIGHT") {
                    m_pulser_dac = val.toInt();
                }
                else if(param == "THRESHOLD") {
                    threshold = val.toInt();
                }
                else if(param == "SCAN-WIDTH") {
                    if(val.toInt() > 0) {
                        scanWidth = val.toInt();
                    }
                }
                else if(param == "DIRECTION") {
                    m_theDirection = val.toInt();
                }
                else if(param == "SCAN-TYPE") {
                    m_scan_type = val.toInt();
                }
            }
        }


        if(m_theFEC > 7 || m_theFEC < 0 || m_theVMM > 15 || m_theVMM < 0
            || m_theDirection < 0 || m_theDirection > 1
            || m_scan_type < 0 || m_scan_type > 1
            || m_pulser_dac < -1 || m_pulser_dac > 1023
            || ((m_scan_type == 1) && (m_pulser_dac == -1))
            || threshold < 0 || threshold > 1023
            || scanWidth < 20 || scanWidth > 200) {
            int ret = QMessageBox::warning(nullptr, tr("S-curve input parameters"),
                                           "Incorrect range or combination of paramters!\n",
                                           QMessageBox::Ok);
            return;
        }

        int fec = m_theFEC;
        int hybrid = m_theVMM / 2;
        int chip = m_theVMM % 2;

        int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");

        m_threshold_dac = 0;
        m_threshold_mV = 0;

        int w = 60;
        if(scanWidth > 0) {
            w = scanWidth;
        }

        int min = 0;
        int max = 0;
        double pulser_mV = 0;

        // scan threshold, set pulser_dac
        if(m_scan_type == 0) {
            if(m_pulser_dac < 0) {
                //Set pulser height to pedestal
                pulser_mV = 170;
            }
            else {
                if(m_pulser_dac == 0) {
                    m_pulser_dac = static_cast<int>(std::round(0.5*(m_minPulseHeightTable[gain] +  m_maxPulseHeightTable[gain])));
                }
                //Determine pulser mV to determine threshold
                pulser_mV = PulserDAC_to_PulseHeight_mV(m_pulser_dac, gain);
                //std::cout << "pulser_mV " << pulser_mV << std::endl;
                //std::cout << "m_pulser_dac " << m_pulser_dac << std::endl;

                //64=Pulser DAC
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",64);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10",m_pulser_dac);
                m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                m_pulser_mV = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                //std::cout << "measured pulser_mV " << m_pulser_mV << std::endl;
            }
            //Automatically chosen threshold from pulser dac
            m_threshold_dac = Threshold_mV_to_DAC(pulser_mV);
            //If threshold is set manually, use it
            if(threshold > 0) {
                m_threshold_dac  = threshold;
            }
            //Determine scan range of threshold
            min = std::max(0,int(m_threshold_dac-0.5*w));
            max = std::min(1023,int(m_threshold_dac+0.5*w));
            std::cout << "Threshold min max " << min << " " << max << std::endl;
        }
        // scan pulser, set threshold dac
        else {
            if(m_pulser_dac == 0) {
                m_pulser_dac = static_cast<int>(std::round(0.5*(m_minPulseHeightTable[gain] +  m_maxPulseHeightTable[gain])));
            }
            //Determine pulser mV to determine threshold
            m_pulser_mV = PulserDAC_to_PulseHeight_mV(m_pulser_dac, gain);

            //Automatically chosen threshold from pulser dac
            m_threshold_dac = Threshold_mV_to_DAC(m_pulser_mV);
            //If threshold is set manually, use it
            if(threshold > 0) {
                m_threshold_dac  = threshold;
            }
            //65=Threshold DAC
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",65);
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt",m_threshold_dac);
            m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
            m_threshold_mV = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);

            //Determine scan range of pulser
            min = std::max(0,int(m_pulser_dac-0.5*w));
            max = std::min(1023,int(m_pulser_dac+0.5*w));
        }
        m_number_bits = max - min + 1;

        m_dac_setting.clear();
        m_dac_measured[fec][hybrid][chip].clear();
        for(unsigned int ch = 0; ch<64; ch++){
            for(int bit=0; bit < m_number_bits; bit++) {
                m_channel_y[ch].push_back(0);
                if(ch == 0) {
                    m_dac_setting.push_back(min+bit);
                    m_dac_measured[fec][hybrid][chip].push_back(0);
                }
            }
        }
    }
    else if(m_modeIndex == 7) {
        QMessageBox msgBox;

        msgBox.setWindowTitle(tr("Calibrate or measure threshold?"));
        msgBox.setText(tr("Do you want to measure or calibrate?"));
        QAbstractButton* pButtonYes = msgBox.addButton(tr("Calibrate"), QMessageBox::YesRole);
        msgBox.addButton(tr("Measure"), QMessageBox::NoRole);
        msgBox.setIcon(QMessageBox::Question);
        msgBox.exec();
        if (msgBox.clickedButton()==pButtonYes) {
            m_isThresholdCalibration = true;
        }
        else {
            m_isThresholdCalibration = false;
        }
    }
    else if(m_modeIndex == 11 || (m_modeIndex == 12 && !m_isAutomatic)) {
        bool ok;
        QStringList list = QInputDialog::getText(nullptr, tr("Latence calibration"),
                                                 tr("Please select one working channel for all VMM [0-63], and choose the threshold DAC [1-1023]\n(-1 means the values are automatically chosen, Threshold=0 takes threshold active in slow control)?"),
                                                 QLineEdit::Normal,"CH=-1,THR=-1",&ok).split(",");
        if(ok && list.count() == 2) {
            QString str = list[0];
			QVector<QStringView> words = QStringView{str}.split('=');
			QString param = words[0].toString();
			QString val = words[1].toString();
            if(param == "CH") {
                if(val.toInt() > -1) {
                    m_theChannel = val.toInt();
                }
            }

            str = list[1];
			words = QStringView{str}.split('=');
			param = words[0].toString();
            val = words[1].toString();
            if(param.toStdString() == "THR") {
                if(val.toDouble()>=0) {
                    m_threshold = val.toDouble();
                }
            }
        }
    }


    QThread::msleep(1);

    if(IsCalibration())
    {

        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            if(chip%2==0) {
                if(m_modeIndex != 6) {
                    //Read Hybrid ID
                    m_hybrid_id[fec][hybrid] = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadI2C(hybrid,0).toStdString();
                }
                else {
                    int theFec = m_theFEC;
                    int thehybrid = m_theVMM / 2;
                    m_hybrid_id[fec][hybrid] = m_daqWindow->m_daq.m_fecs[theFec].m_fecConfigModule->ReadI2C(thehybrid,0).toStdString();
                }
            }
            for(int ch = 0; ch<64; ch++){
                //Offline ADC and time, online ADC and TDC calibration
                if(m_modeIndex <= 5)
                {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm",0,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",1,ch);
                    int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                    if(gain<=1) {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sth",1,ch);
                    }
                    else {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sth",0,ch);
                    }
                }
                else if(m_modeIndex == 11 || m_modeIndex == 12)
                {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm",1,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                    if(gain<=1) {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sth",1,ch);
                    }
                    else {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sth",0,ch);
                    }
                }
                //threshold measurement
                if(m_modeIndex == 7)
                {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm",0,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sth",0,ch);

                }
                //S-curve (one VMM)
                else if(m_modeIndex == 6)
                {
                    int theFec = m_theFEC;
                    int thehybrid = m_theVMM / 2;
                    int theChip = m_theVMM % 2;
                    int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                    if(gain<=1) {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sth",1,ch);
                    }
                    else {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sth",0,ch);
                    }
                    if(fec == theFec && hybrid == thehybrid && theChip == chip)
                    {
                        if(m_pulser_dac == -1) {
                            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                        }
                        else {
                            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",1,ch);
                        }
                    }
                    else
                    {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm",1,ch);
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);

                    }
                }
            }
        }
        //check config once
        m_daqWindow->m_daq.SendAll(true);
    }
    if(m_modeIndex == 8)
    {
        MeasurePedestalOrThreshold(true, false);
        PlotData();
    }
    else if(m_modeIndex == 7)
    {
        MeasurePedestalOrThreshold(false, m_isThresholdCalibration);
        PlotData();
    }
    else if(m_modeIndex == 9)
    {
        MeasurePulserOrThresholdDAC(true);
        PlotData();
    }
    else if(m_modeIndex == 10)
    {
        MeasurePulserOrThresholdDAC(false);
        PlotData();
    }
    else {
        if(IsCalibration())
        {
            DoCalibrationStep();
        }
        else {
            m_bitCount = 0;
        }
        QThread::msleep(1);
        StartDataTaking();
        m_packetCounter = 0;
        m_nodata_start = std::chrono::high_resolution_clock::now();
    }
}

// ------------------------------------------------------------------------ //
void CalibrationModule::DoCalibrationStep(){
    m_bitCount++;
    //Offline or online ADC or time/TDC
    if(m_modeIndex <= 5 || m_modeIndex == 11 || m_modeIndex == 12) {

        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            m_srs_timestamp_start[fec][hybrid][chip]=0;
            m_srs_timestamp_end[fec][hybrid][chip]=0;
            if(m_modeIndex == 1)
            {
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", 0);
                m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigHybrid(hybrid);
                int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                if(m_threshold == -1) {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_thresholdTable[gain]);
                }
                else if(m_threshold > 0) {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_threshold);
                }
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int step = (max - min)/(m_number_bits-1);
                int val = static_cast<int>(min+m_bitCount*step);
                if(vmm == 0) {
                    m_dac_setting.push_back(val);
                }
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10",val);
                //64=PulserDAC
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",64);
                m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                int measured = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                //std::cout << "pulser Dac - vmm " << (int) val << " " << (int)vmm << ", hybrid " << (int) hybrid << ", fec " << (int)fec << ": " << measured  << std::endl;
                m_dac_measured[fec][hybrid][chip].push_back(measured);

            }
            else if(m_modeIndex == 2)
            {
                int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                if(m_threshold == -1) {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_thresholdTable[gain]);
                }
                else if(m_threshold > 0) {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_threshold);
                }
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/2;
                if( m_pulser_dac == -1) {
                    m_pulser_dac = center;
                }
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10",m_pulser_dac);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", m_bitCount);
            }
            else if(m_modeIndex == 3)
            {
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", 0);
                m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigHybrid(hybrid);
                int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_thresholdTable[gain]);
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int step = (max - min)/(m_number_bits-1);
                int val = static_cast<int>(min+m_bitCount*step);
                if(vmm == 0) {
                    m_dac_setting.push_back(val);
                }
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10",val);
                //64=PulserDAC
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",64);
                m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                int measured = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                m_dac_measured[fec][hybrid][chip].push_back(measured);
            }
            else if(m_modeIndex == 4)
            {
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", 0);
                int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/(2);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10",center);
                for(int ch = 0; ch<64; ch++){
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "sz10b", m_bitCount,ch );
                }
            }
            else if(m_modeIndex == 5)
            {
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", 0);
                int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/(2);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10",center);
                for(int ch = 0; ch<64; ch++){
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "sz08b", m_bitCount,ch );
                }
            }
            //Reset latency and TP latency
            else if(m_modeIndex == 11 || m_modeIndex == 12)
            {
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", 4);
                int gain = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sg");
                if(m_threshold == -1) {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_thresholdTable[gain]);
                }
                else if(m_threshold > 0) {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_threshold);
                }
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/2;
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10",center);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",1,m_theChannel);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm",0,m_theChannel);
                m_daqWindow->m_daq.m_fecs[fec].SetReg("debug_data_format",  true);
                if(m_modeIndex == 11) {
                    m_daqWindow->m_daq.m_fecs[fec].SetReg("latency_reset", static_cast<unsigned long>(m_start_reset_latency+m_bitCount));
                }
                else {
                    m_daqWindow->m_daq.m_fecs[fec].SetReg("tp_offset_first", (unsigned long)100);
                    m_daqWindow->m_daq.m_fecs[fec].SetReg("tp_latency", static_cast<unsigned long>(m_start_tp_latency+m_bitCount));
                }
            }
        }
    }
    //S-curve
    else if(m_modeIndex == 6)
    {

        if(m_theDirection == 0)
        {
            m_theIndex = m_bitCount;
        }
        else {
            m_theIndex = m_number_bits - 1 - m_bitCount;
        }


        int fec = m_theFEC;
        int hybrid = m_theVMM / 2;
        int chip = m_theVMM % 2;
        m_srs_timestamp_start[fec][hybrid][chip]=0;
        m_srs_timestamp_end[fec][hybrid][chip]=0;
        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", 0);
        m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigHybrid(hybrid);
        if(m_scan_type == 0) {
            //65="Threshold_DAC"
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",65);
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", (int)m_dac_setting[m_theIndex]);
        }
        else {
            //64="Pulser_DAC"
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",64);
            m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10", (int)m_dac_setting[m_theIndex]);
        }
        m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
        int measured = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
        m_dac_measured[fec][hybrid][chip][m_theIndex] = measured;
    }
}


// ------------------------------------------------------------------------ //
void CalibrationModule::AccumulateData(){

    stringstream sx;
    //Data received
    if(m_numHitsInFrame > 0)
    {
        m_nodata_start = std::chrono::high_resolution_clock::now();
    }
    //No data received
    else
    {
        m_nodata_end = std::chrono::high_resolution_clock::now();
        auto nodata_duration = std::chrono::duration_cast<std::chrono::milliseconds>( m_nodata_end - m_nodata_start ).count();
        //If for ADC or TDC calibrations there is not data for longer than 10 s, hard reset the VMMs
        if((IsCalibration() && m_modeIndex != 6 && nodata_duration >= 20000))
        {
            StopDataTaking();
            Reset();
            StartCalibration();
        }
    }
    bool continueCalibration = true;

    uint64_t delay_ns = 0;
    if(m_end >= m_start) {
        delay_ns = m_end - m_start;
    }
    //convert run value from ms to ns
    if(delay_ns >= static_cast<uint64_t>(m_daqWindow->m_ui->Runs->value())*1000000)
    {
        double delay_s = delay_ns*0.000000001;
        m_time.push_back(delay_s);
        double delay_ms = delay_ns*0.000001;



        StopDataTaking();


        std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Finished calibration acquisition " <<
            m_bitCount << " after " << delay_ms  << " ms!"  << std::endl;
        //Finish calibration
        if(m_bitCount == m_number_bits-1) {
            continueCalibration = false;
        }

        if(continueCalibration)
        {
            DoCalibrationStep();
            QThread::msleep(50);
            StartDataTaking();
        }
        else
        {
            m_daqWindow->m_ui->pushButtonTakeData->setCheckable(false);
            m_daqWindow->m_ui->pushButtonTakeData->setChecked(false);
            m_dataAvailable = true;
            m_data_modeIndex = m_modeIndex;

            //Not for S-curve and time calibration
            if(m_modeIndex != 6 && m_modeIndex != 2) {
                for(int bit =0; bit<m_number_bits; bit++){
                    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
                        int fec = GetFEC(vmm);
                        int hybrid = GetHybrid(vmm);
                        int chip = GetVMM(vmm);
                        for(unsigned int ch = 0; ch<64; ch++){
                            unsigned long size = m_data[bit][fec][hybrid][chip][ch].size();
                            double norm = static_cast<double>(size);
                            double mean = -9999.0;
                            double mean2 = -9999.0;
                            double median = -9999.0;
                            if(!IsCalibration())
                            {
                                mean = 0;
                            }

                            //Channels
                            if(m_modeIndex == 20)
                            {
                                norm = 1.0;
                            }
                            if(size>0)
                            {
                                mean = std::accumulate(m_data[bit][fec][hybrid][chip][ch].begin(),m_data[bit][fec][hybrid][chip][ch].end(),0ll)/norm;
                                mean2 = std::accumulate(m_data2[bit][fec][hybrid][chip][ch].begin(),m_data2[bit][fec][hybrid][chip][ch].end(),0ll)/norm;
                                auto nth = m_data[bit][fec][hybrid][chip][ch].begin() + (50*m_data[bit][fec][hybrid][chip][ch].size())/100;
                                std::nth_element(m_data[bit][fec][hybrid][chip][ch].begin(), nth, m_data[bit][fec][hybrid][chip][ch].end());
                                median = *nth;
                            }
                            if(m_modeIndex == 1) {
                                m_mean[bit][fec][hybrid][chip].push_back(median);
                            }
                            else {
                                m_mean[bit][fec][hybrid][chip].push_back(mean);
                            }
                            m_mean2[bit][fec][hybrid][chip].push_back(mean2);
                        }
                    }
                }

            }
            else if(m_modeIndex == 2) {
                //Determine most common BCID in measurements over all VMMs and channels
                long all_bcid_cnt = 0;
                int all_bcid_max = 0;
                int all_bcid_min = 0;
                int measured_most_common_BCID = 0;
                for(int i=0; i<4096; i++) {
                    long cnt = m_cnt_bcid[i];
                    if(cnt > all_bcid_cnt) {
                        if(m_reference_BCID == -1) {
                            m_reference_BCID = i;
                        }
                        measured_most_common_BCID = i;
                        all_bcid_cnt = cnt;
                    }
                }
                //std::cout << "reference bcid " << m_reference_BCID << std::endl;

                //Accept in the subsequent search of the most common BCID per channels a range of +/- 2 BCIDs around the most common one
                if(measured_most_common_BCID-(int)(NUM_BCID/2) >=0 ) {
                    all_bcid_min = measured_most_common_BCID-(int)(NUM_BCID/2);
                }
                if(measured_most_common_BCID+(int)(NUM_BCID/2) <= 4095 ) {
                    all_bcid_max = measured_most_common_BCID+(int)(NUM_BCID/2);
                }
                for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
                    int fec = GetFEC(vmm);
                    int hybrid = GetHybrid(vmm);
                    int chip = GetVMM(vmm);

                    for(unsigned int ch = 0; ch<64; ch++){
                        long max_cnt = 0;
                        int the_bcid = 0;
                        int max = 0;
                        int min = 0;

                        //Determine most common BCID per channel
                        for(int bcid = all_bcid_min; bcid <= all_bcid_max; bcid++) {
                            int bcid_cnt = 0;
                            for(int bit = 0; bit<m_number_bits; bit++){
                                int cnt = std::count_if(m_data[bit][fec][hybrid][chip][ch].begin(),
                                                        m_data[bit][fec][hybrid][chip][ch].end(),
                                                        [bcid](int val) { return bcid*1000 <=val && bcid*1000+255 >= val; });
                                bcid_cnt += cnt;
                            }
                            if(bcid_cnt > max_cnt) {
                                max_cnt = bcid_cnt;
                                the_bcid = bcid;
                            }
                        }
                        //Accept data in the range of +/- 2 BCID aroud the most common BCID per channel
                        if(the_bcid - (int)(NUM_BCID/2) >= 0) {
                            min = the_bcid - (int)(NUM_BCID/2);
                        }
                        else {
                            min = 0;
                        }
                        if(the_bcid + (int)(NUM_BCID/2) <= 4095) {
                            max = the_bcid + (int)(NUM_BCID/2);
                        }
                        else {
                            max = 4095;
                        }
                        for(int bit = 0; bit<m_number_bits; bit++){
                            for(int n = 0; n<NUM_BCID; n++){
                                m_mean_per_bcid[n][bit][fec][hybrid][chip].push_back(-9999.0);
                                m_percent_bcid[n][bit][fec][hybrid][chip].push_back(0);
                            }
                            m_mean[bit][fec][hybrid][chip].push_back(0);
                            m_fit_y[bit][fec][hybrid][chip].push_back(-9999.0);
                        }
                        m_fit_start_time[fec][hybrid][chip].push_back(-9999.0);
                        m_fit_start_bcid[fec][hybrid][chip].push_back(-9999.0);
                        if(max - min == NUM_BCID-1) {
                            //determine percentages for three BCIDs
                            for(int bit = 0; bit<m_number_bits; bit++){
                                int total = m_data[bit][fec][hybrid][chip][ch].size();
                                for(int bcid=min; bcid <= max; bcid++) {
                                    double meanTDC = -9999.0;
                                    double meanTime = -9999.0;
                                    auto it = std::partition(m_data[bit][fec][hybrid][chip][ch].begin(),m_data[bit][fec][hybrid][chip][ch].end(), [bcid](int val) { return (bcid*1000 <=val && bcid*1000+255 >= val); });
                                    long cnt = std::count_if(m_data[bit][fec][hybrid][chip][ch].begin(),it, [](int val) { return true; });
                                    if(cnt > 0) {
                                        //Algorithm to calculate mean TDC
                                        long tdc = std::accumulate(m_data[bit][fec][hybrid][chip][ch].begin(),it,0ll);
                                        //remove the bcid from the combined data entry (bcid*1000+tdc)
                                        meanTDC = (tdc - bcid*1000*cnt);
                                        //calculate the mean tdc for the bit value and bcid
                                        meanTDC = (double)meanTDC/(double)cnt;

                                        //convert BCID and TDC to time
                                        //since the TDC ramp is stopped by the falling edge of the BCCLock, one has to use 1.5*bc_period (50% duty cycle)
                                        meanTime = 1.5*g_clock_period - (meanTDC*m_tac_slope[fec][hybrid][chip])/255;
                                        m_percent_bcid[bcid-min][bit][fec][hybrid][chip][ch] =  (double)cnt/(double)total;
                                        m_mean[bit][fec][hybrid][chip][ch] +=  ((bcid-m_reference_BCID)*g_clock_period +  meanTime)*m_percent_bcid[bcid-min][bit][fec][hybrid][chip][ch];
                                        m_mean_per_bcid[bcid-min][bit][fec][hybrid][chip][ch] = meanTime;
                                    }
                                }
                            }

                            bool foundEdge = false;
                            int theBCIDIndex_50 = 0;
                            int theTimeIndex_50 = 0;

                            //look for edge (change of bcid)
                            for(int bcid=min; bcid < max; bcid++) {
                                if(!foundEdge) {
                                    for(int bit = 0; bit<m_number_bits-1; bit++){
                                        if(m_percent_bcid[bcid-min][bit][fec][hybrid][chip][ch] >= 0.5 && m_percent_bcid[bcid-min][bit+1][fec][hybrid][chip][ch] <= 0.5
                                            && m_percent_bcid[bcid-min+1][bit][fec][hybrid][chip][ch] <= 0.5 && m_percent_bcid[bcid-min+1][bit+1][fec][hybrid][chip][ch] >= 0.5) {
                                            theBCIDIndex_50 = bcid-min+1;
                                            theTimeIndex_50 = bit;
                                            //Determine the time at which 50% of BCIDs belong to one BCID and 50% belong to BCID+1
                                            double theTime = bit*g_time_factor + (0.5 - m_percent_bcid[bcid-min+1][bit][fec][hybrid][chip][ch])*g_time_factor /(m_percent_bcid[bcid-min+1][bit+1][fec][hybrid][chip][ch] - m_percent_bcid[bcid-min+1][bit][fec][hybrid][chip][ch]);
                                            //Store bcid where 50% edge occurs
                                            m_fit_start_bcid[fec][hybrid][chip][ch] = min-m_reference_BCID;
                                            m_fit_start_time[fec][hybrid][chip][ch] = -((bcid+1-m_reference_BCID)*g_clock_period) + theTime;
                                            foundEdge = true;
                                            break;
                                        }
                                        //Fix for cases with strange BC distribution (no crossing point at 50%)
                                        else if(m_percent_bcid[bcid-min][bit][fec][hybrid][chip][ch] >= 0.5 && m_percent_bcid[bcid-min][bit+1][fec][hybrid][chip][ch] <= 0.5) {
                                            theBCIDIndex_50 = bcid-min+1;
                                            theTimeIndex_50 = bit;
                                            double theTime = bit*g_time_factor + (0.5 - m_percent_bcid[bcid-min+1][bit][fec][hybrid][chip][ch])*g_time_factor /(m_percent_bcid[bcid-min+1][bit+1][fec][hybrid][chip][ch] - m_percent_bcid[bcid-min+1][bit][fec][hybrid][chip][ch]);
                                            m_fit_start_time[fec][hybrid][chip][ch] = -((bcid+1-m_reference_BCID)*g_clock_period) + theTime;
                                            //Store bcid where 50% edge occurs
                                            m_fit_start_bcid[fec][hybrid][chip][ch] = min-m_reference_BCID;
                                            foundEdge = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if(foundEdge) {
                                //data from 50% point until end of bits
                                for(int n = theTimeIndex_50; n<m_number_bits; n++){
                                    if(m_percent_bcid[theBCIDIndex_50][n][fec][hybrid][chip][ch] >= m_bcid_percentage) {
                                        m_fit_y[n-theTimeIndex_50][fec][hybrid][chip][ch] =  m_mean_per_bcid[theBCIDIndex_50][n][fec][hybrid][chip][ch];
                                    }
                                    else {
                                        m_fit_y[n-theTimeIndex_50][fec][hybrid][chip][ch] = -9999.0;
                                    }
                                }

                                //data from beginning to 50% point
                                //ignore the first two data points, they are the same as the last two data points (bit 0 is identical to bit 8, bit 1 to bit 10)
                                for(int n = 2; n< 2+theTimeIndex_50; n++){
                                    if(m_percent_bcid[theBCIDIndex_50-1][n][fec][hybrid][chip][ch] >= m_bcid_percentage) {
                                        m_fit_y[m_number_bits-theTimeIndex_50+n-2][fec][hybrid][chip][ch] = m_mean_per_bcid[theBCIDIndex_50-1][n][fec][hybrid][chip][ch];
                                    }
                                    else {
                                        m_fit_y[m_number_bits-theTimeIndex_50+n-2][fec][hybrid][chip][ch] = -9999.0;
                                    }
                                }
                            }
                        }
                    }
                }
                double minMeanOffset = 99999999;
                double meanOffset = 0;
                for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
                    int fec = GetFEC(vmm);
                    int hybrid = GetHybrid(vmm);
                    int chip = GetVMM(vmm);
                    int cnt = 0;
                    for(unsigned int ch = 0; ch<64; ch++){
                        if(m_mean[0][fec][hybrid][chip][ch] != -9999.0) {
                            meanOffset += m_mean[0][fec][hybrid][chip][ch];
                            cnt++;
                        }
                    }
                    meanOffset = meanOffset/cnt;
                    if(meanOffset < minMeanOffset) {
                        minMeanOffset = meanOffset;
                    }
                }
                for(int bit = 0; bit<m_number_bits; bit++){
                    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
                        int fec = GetFEC(vmm);
                        int hybrid = GetHybrid(vmm);
                        int chip = GetVMM(vmm);
                        for(unsigned int ch = 0; ch<64; ch++){
                            if(m_mean[bit][fec][hybrid][chip][ch] != -9999.0) {
                                m_mean[bit][fec][hybrid][chip][ch] =  m_mean[bit][fec][hybrid][chip][ch] -  minMeanOffset;
                            }
                        }
                    }
                }
            }
            //Offline calibration ADC or Time
            if(m_modeIndex == 1 || m_modeIndex == 2)
            {
                FitLinearData();
            }
            else if(m_modeIndex == 3)
            {
                Fit4PL();
                //FitErrorCurve();
            }
            //Online calibration ADC or TDC
            else if(m_modeIndex == 4 || m_modeIndex == 5)
            {
                CalculateCorrections();
            }
            //S-Curve
            else if(m_modeIndex == 6)
            {
                FitSCurve();
            }
            //Latency calibration
            else if(m_modeIndex == 11) {
                CalculateLatency(true);
            }
            //Latency calibration
            else if(m_modeIndex == 12) {
                CalculateLatency(false);
            }
            PlotData();
            if(IsCalibration())
            {
                //Restore the saved settings
                if(m_modeIndex == 4 || m_modeIndex == 5) {
                    SaveSettings("Calib_config");
                }
                else {
                    LoadSettings("Latest_config");
                }
            }
            if(m_isAutomatic && (m_modeIndex == 11 || m_modeIndex == 12)) {
                SaveCorrections();
            }
            m_daqWindow->LoadFECSettings();
            m_daqWindow->LoadHybridSettings();
            m_daqWindow->LoadVMMSettings();
            m_daqWindow->LoadVMMChannelSettings();
        }
    }
}


void CalibrationModule::CalculateLatency(bool isReset) {
    for(int bit = 0; bit<m_number_bits; bit++){
        double fecLatency[FECS_PER_DAQ] = {0,0,0,0,0,0,0,0};
        double nValues[FECS_PER_DAQ] = {0,0,0,0,0,0,0,0};
        for(int vmm = 0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            if(m_mean[bit][fec][hybrid][chip][m_theChannel] > -9999.0) {
                fecLatency[fec] += m_mean[bit][fec][hybrid][chip][m_theChannel];
                nValues[fec]++;
            }
        }
        for(int fec=0; fec<FECS_PER_DAQ;fec++) {
            m_mean2[bit][fec][0][0].clear();
            if(nValues[fec] > 0) {
                fecLatency[fec] = fecLatency[fec]/nValues[fec];
                m_mean2[bit][fec][0][0].push_back(fecLatency[fec]);
            }
        }
    }

    for(int fec=0; fec<FECS_PER_DAQ;fec++) {
        double minVal = 9999.0;
        int minBit = 0;
        for(int bit = 0; bit<m_number_bits; bit++){
            if(!m_mean2[bit][fec][0][0].empty() && std::abs(m_mean2[bit][fec][0][0][0]) < std::abs(minVal)) {
                minVal = m_mean2[bit][fec][0][0][0];
                minBit = bit;

            }
        }
        if(isReset) {
            if(minVal < 9999.0) {
                m_fecLatencyReset[fec] = minBit;
            }
            else {
                m_fecLatencyReset[fec] = -1;
            }
        }
        else {
            if(minVal < 9999.0) {
                m_fecLatencyTP[fec] = minBit;
            }
            else {
                m_fecLatencyTP[fec] = -1;
            }
        }
    }
}


void CalibrationModule::Reset()
{
    m_daqWindow->m_ui->pushButtonTakeData->setChecked(false);
    m_daqWindow->m_ui->pushButtonTakeData->setCheckable(false);
    QThread::msleep(1);
    std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Hard Reset of VMMs!" << std::endl;
    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
        int fec = GetFEC(vmm);
        int hybrid = GetHybrid(vmm);
        int chip = GetVMM(vmm);
        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("reset1", 1);
        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("reset2", 1);
        m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid, chip);
        QThread::msleep(100);
        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("reset1", 0);
        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("reset2", 0);
        m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid, chip);
        QThread::msleep(100);

        m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ResetFEC();
        QThread::msleep(100);
        m_daqWindow->m_daq.m_fecs[fec].SendAll(false);
    }
}

double CalibrationModule::SortVectors( vector<double>& sortedMin, vector<double>& sortedMax){
    stringstream sx;
    std::sort(sortedMin.begin(), sortedMin.end());
    std::sort(sortedMax.begin(), sortedMax.end());
    unsigned long last = sortedMax.size()-1;
    unsigned int state = 0;
    unsigned int check = 0;
    while(sortedMin[check-state] < 0 || (sortedMin[check-state]-sortedMax[last-check]) < 0 ){
        if(state == 0){
            state =1;
            check+=1;
        }
        else if(state == 1){
            state =0;
        }
    }
    if(IsDbgActive()) {
        sx <<"Best common value "<<(sortedMin[check-state]+sortedMax[last-check])/2<<" value of high: "<< sortedMax[check-state]<<", value of low "<<sortedMin[last-check]<<std::endl;
        GetMessageHandler()(sx,"calibration_module::AccumulateData"); sx.str("");
    }
    return ((sortedMin[check-state]+sortedMax[last-check])/2);
}

void CalibrationModule::CalculateCorrections(){
    stringstream sx;
    // Function to calculate the best common ADC, TDC value or threshold for all channels per VMM

    for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
        vector<double> sorted_0mV;
        vector<double> sorted_MAXmV;
        double val =  0;
        int fec = GetFEC(vmm);
        int hybrid = GetHybrid(vmm);
        int chip = GetVMM(vmm);
        for(unsigned int ch=0; ch<64; ch++)
        {
            if(m_mean[0][fec][hybrid][chip][ch]!= -9999.0)
            {
                sorted_0mV.push_back(m_mean[0][fec][hybrid][chip][ch]);
            }
            if(m_mean[m_number_bits-1][fec][hybrid][chip][ch] != -9999.0)
            {
                sorted_MAXmV.push_back(m_mean[m_number_bits-1][fec][hybrid][chip][ch]);
            }
        }

        if(sorted_0mV.size()>32 &&sorted_MAXmV.size()>32)
        {
            val =  SortVectors(sorted_0mV, sorted_MAXmV);
            for(unsigned int ch =0; ch<64; ch++){
                m_y[fec][hybrid][chip].push_back(val);
            }
            for(unsigned int ch =0; ch<64; ch++){
                double smallestDifference = 9999.;
                int bin_number = 0;
                double mean = 0;
                for(int bit = 0; bit<m_number_bits; bit++){
                    if(m_mean[bit][fec][hybrid][chip][ch]>=0)
                    {
                        double difference = pow(pow( m_y[fec][hybrid][chip][ch] - m_mean[bit][fec][hybrid][chip][ch],2),0.5);
                        if( difference < smallestDifference ) {
                            smallestDifference = difference;
                            bin_number = bit;
                            mean = m_mean[bit][fec][hybrid][chip][ch];
                        }
                    }
                    if(bit == m_number_bits-1){
                        m_bitVal[fec][hybrid][chip].push_back(bin_number);
                        m_calVal[fec][hybrid][chip].push_back(mean);
                    }
                }

            }
        }
    }
}

void CalibrationModule::ConnectDAQSocket()
{
    stringstream sx;
    int daqport = 6006;
    if(g_clock_source <= 1) {
        daqport = 9000;
    }
    if(!m_udpSocket) {
        GetMessageHandler()("Initializing DAQ socket...","calibration_module::connectDAQSocket");
        m_udpSocket = new QUdpSocket();
        connect(m_udpSocket, SIGNAL(readyRead()), this, SLOT(readEvent()));
    }
    //if(m_udpSocket->state() == QAbstractSocket::UnconnectedState) {
    if(m_udpSocket->state() != m_udpSocket->BoundState) {
        if(IsDbgActive()){
            sx << "About to re-bind DAQ socket";
            GetMessageHandler()(sx,"calibration_module::connectDAQSocket"); sx.str("");
        }
        bool bnd = m_udpSocket->bind(daqport, QUdpSocket::ShareAddress);
        if(!bnd) {
            sx << "ERROR Unable to re-bind DAQ socket to port " << daqport;
            GetMessageHandler()(sx, "calibration_module::connectDAQSocket"); sx.str("");
            if(IsDbgActive()) {
                sx << "Closing and disconnecting DAQ socket";
                GetMessageHandler()(sx,"calibration_module::connectDAQSocket"); sx.str("");
            }
            m_udpSocket->close();
            m_udpSocket->disconnectFromHost();
        } // not bnd correctly
        else {
            if(IsDbgActive()) {
                sx << "DAQ socket successfully bound to port " << daqport;
                GetMessageHandler()(sx,"calibration_module::connectDAQSocket"); sx.str("");
            }
        } // bnd ok
    }

}

void CalibrationModule::SaveCorrections(){
    if(m_modeIndex == 1 ||  m_modeIndex == 2 ||  m_modeIndex == 3)
    {
        if(g_clock_source == 0 && g_slow_control==0) {
            SaveCorrectionsEFU(g_instrument);
        }

        int dac = -1;
        if( ! m_calibrationArray[0] &&  !m_calibrationArray[1] && ! m_calibrationArray[2]) {
            return;
        }
        QString theName = "vmm_calibration";
        if(m_modeIndex == 1) {
            theName += "_ADC";
        }
        else if(m_modeIndex == 2) {
            theName += "_time";
            dac = m_pulser_dac;
        }
        else if(m_modeIndex == 3) {
            theName += "_timewalk";
        }
        int lastFEC = -1;
        double gain = 0;
        int polarity = 0;
        double peaktime =0;
        double tac = 0;
        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int fecId = m_fecPosID[fec];
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            if(lastFEC != fecId) {
                theName += "_FEC" + QString::number(fecId);
            }
            lastFEC = fecId;
            if(chip == 0 && m_vmmActs.size() <= 4) {
                if(QString::fromStdString(m_hybrid_id[fec][hybrid])== "ffffffffffffffffffffffffffffffff") {
                    theName +=  "_ffffffffffffffffffffffffffffffff_" +  QString::number(fecId) + "_" +  QString::number(hybrid);
                }
                else {
                    theName += "_" + QString::fromStdString(m_hybrid_id[fec][hybrid]);
                }
            }
            if(vmm == 0) {
                theName += "_VMM" + QString::number(hybrid*2+chip);
            }
            else {
                theName += "_" + QString::number(hybrid*2+chip);
            }
            gain = m_gain[fec][hybrid][chip];
            polarity = m_polarity[fec][hybrid][chip];
            peaktime = m_shaping_time[fec][hybrid][chip];
            tac = m_tac_slope[fec][hybrid][chip];

        }
        if(m_modeIndex != 2) {
            dac = -1;
        }

        QString theSettings = CreateFileName(polarity,gain,peaktime,tac,dac);
        QString theDirectory = CreateDir("calibs", theSettings);
        QString theTime = QDateTime::currentDateTime().toString("hhmmss");
        theName = theDirectory + "/" + theName + "_" + theTime + ".json";

        QFile jsonFile(theName);
        jsonFile.open(QFile::WriteOnly);

        QJsonObject globalObject;
        QJsonObject globalObjectSingle;
        QJsonArray calibrationArray;
        QJsonArray calibrationArraySingle;
        for(int vmm = 0; vmm <static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int fecId = m_fecPosID[fec];
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            if(chip==0) {
                globalObjectSingle = QJsonObject();
                calibrationArraySingle = QJsonArray();
            }
            QJsonObject calibrationObject;
            QJsonArray adcOffsetArray;
            QJsonArray adcSlopeArray;
            QJsonArray timeOffsetArray;
            QJsonArray timeSlopeArray;
            QJsonArray timeWalkArrayA;
            QJsonArray timeWalkArrayB;
            QJsonArray timeWalkArrayC;
            QJsonArray timeWalkArrayD;
            calibrationObject.insert("fecID",fecId);
            calibrationObject.insert("hybridID",QString::fromStdString(m_hybrid_id[fec][hybrid]));
            calibrationObject.insert("vmmID",hybrid*2+chip);

            QJsonObject calibrationObjectSingle;
            calibrationObjectSingle.insert("hybridID",QString::fromStdString(m_hybrid_id[fec][hybrid]));
            calibrationObjectSingle.insert("vmmID",chip);
            int theVmmId = 0;
            int theFECId = 0;
            QString theHybridId = QString::fromStdString(m_hybrid_id[fec][hybrid]);
            if(theHybridId == "ffffffffffffffffffffffffffffffff") {
                theHybridId += "_" +  QString::number(fecId) + "_" +  QString::number(hybrid);
            }

            if(m_calibrationArray[0] && m_modeIndex == 1) {
                foreach (const QJsonValue & value, *m_calibrationArray[0]) {
                    QJsonArray tempOffsetArray;
                    QJsonArray tempSlopeArray;
                    const auto& obj = value.toObject();
                    const auto& keys = obj.keys();
                    for(const auto& key : keys){
                        if(key == "vmmID") {
                            theVmmId = obj[key].toInt();
                        }
                        if(key == "fecID") {
                            theFECId = obj[key].toInt();
                        }
                        if(key == "adc_offsets") {

                            auto const & arr = obj[key].toArray();
                            for(const auto& v : arr){
                                tempOffsetArray.push_back(v.toDouble());
                            }
                        }
                        else if(key == "adc_slopes") {
                            auto const & arr = obj[key].toArray();
                            for(const auto& v : arr) {
                                tempSlopeArray.push_back(v.toDouble());
                            }
                        }
                    }
                    if(theVmmId == hybrid*2+chip &&  m_fecPosID[fec] == theFECId) {
                        adcOffsetArray = tempOffsetArray;
                        adcSlopeArray = tempSlopeArray;
                        calibrationObjectSingle.insert("adc_offsets",adcOffsetArray);
                        calibrationObjectSingle.insert("adc_slopes",adcSlopeArray);
                        break;
                    }
                }
            }

            if(m_calibrationArray[1] && m_modeIndex == 2) {
                foreach (const QJsonValue & value, *m_calibrationArray[1]) {
                    QJsonArray tempOffsetArray;
                    QJsonArray tempSlopeArray;
                    const auto& obj = value.toObject();
                    const auto& keys = obj.keys();
                    for(const auto& key : keys){
                        if(key == "vmmID") {
                            theVmmId = obj[key].toInt();
                        }
                        if(key == "fecID") {
                            theFECId = obj[key].toInt();
                        }
                        if(key == "hybridID") {
                            theHybridId = obj[key].toString();
                        }
                        if(key == "time_offsets") {
                            auto const & arr = obj[key].toArray();
                            for(const auto& v : arr){
                                tempOffsetArray.push_back(v.toDouble());
                            }
                        }
                        else if(key == "time_slopes") {
                            auto const & arr = obj[key].toArray();
                            for(const auto& v : arr) {
                                tempSlopeArray.push_back(v.toDouble());
                            }
                        }
                    }
                    if(theVmmId == hybrid*2+chip &&  m_fecPosID[fec] == theFECId) {
                        timeOffsetArray = tempOffsetArray;
                        timeSlopeArray = tempSlopeArray;
                        calibrationObjectSingle.insert("time_offsets",timeOffsetArray);
                        calibrationObjectSingle.insert("time_slopes",timeSlopeArray);
                        break;
                    }
                }
            }

            if(m_calibrationArray[2] && m_modeIndex == 3) {
                foreach (const QJsonValue & value, *m_calibrationArray[2]) {
                    QJsonArray tempArrayA;
                    QJsonArray tempArrayB;
                    QJsonArray tempArrayC;
                    QJsonArray tempArrayD;
                    const auto& obj = value.toObject();
                    const auto& keys = obj.keys();
                    for(const auto& key : keys){
                        if(key == "vmmID") {
                            theVmmId = obj[key].toInt();
                        }
                        if(key == "fecID") {
                            theFECId = obj[key].toInt();
                        }
                        if(key == "hybridID") {
                            theHybridId = obj[key].toString();
                        }
                        if(key == "timewalk_a") {
                            auto const & arr = obj[key].toArray();
                            for(const auto& v : arr){
                                double val = 0;
                                if(v.toDouble() > -9999.0) {
                                    val = v.toDouble();
                                }
                                tempArrayA.push_back(val);
                            }
                        }
                        else if(key == "timewalk_b") {
                            auto const & arr = obj[key].toArray();
                            for(const auto& v : arr) {
                                double val = 1;
                                if(v.toDouble() > -9999.0) {
                                    val = v.toDouble();
                                }
                                tempArrayB.push_back(val);
                            }
                        }
                        else if(key == "timewalk_c") {
                            auto const & arr = obj[key].toArray();
                            for(const auto& v : arr){
                                double val = 1;
                                if(v.toDouble() > -9999.0) {
                                    val = v.toDouble();
                                }
                                tempArrayC.push_back(val);
                            }
                        }
                        else if(key == "timewalk_d") {
                            auto const & arr = obj[key].toArray();
                            for(const auto& v : arr) {
                                double val = 0;
                                if(v.toDouble() > -9999.0) {
                                    val = v.toDouble();
                                }
                                tempArrayD.push_back(val);
                            }
                        }
                    }
                    if(theVmmId == hybrid*2+chip &&  m_fecPosID[fec] == theFECId) {
                        timeWalkArrayA = tempArrayA;
                        timeWalkArrayB = tempArrayB;
                        timeWalkArrayC = tempArrayC;
                        timeWalkArrayD = tempArrayD;
                        calibrationObjectSingle.insert("timewalk_a",timeWalkArrayA);
                        calibrationObjectSingle.insert("timewalk_b",timeWalkArrayB);
                        calibrationObjectSingle.insert("timewalk_c",timeWalkArrayC);
                        calibrationObjectSingle.insert("timewalk_d",timeWalkArrayD);
                        break;
                    }
                }
            }

            if(m_calibrationArray[0] && m_modeIndex == 1) {
                calibrationObject.insert("adc_offsets",adcOffsetArray);
                calibrationObject.insert("adc_slopes", adcSlopeArray);
            }
            else if(m_calibrationArray[1] && m_modeIndex == 2) {
                calibrationObject.insert("time_offsets",timeOffsetArray);
                calibrationObject.insert("time_slopes",timeSlopeArray);
            }
            else if(m_calibrationArray[2] && m_modeIndex == 3) {
                calibrationObject.insert("timewalk_a",timeWalkArrayA);
                calibrationObject.insert("timewalk_b",timeWalkArrayB);
                calibrationObject.insert("timewalk_c",timeWalkArrayC);
                calibrationObject.insert("timewalk_d",timeWalkArrayD);
            }
            calibrationArray.push_back(calibrationObject);
            calibrationArraySingle.push_back(calibrationObjectSingle);


            //Single Hybrid Files
            if(chip==1) {
                QString str;
                if(m_calibrationArray[0] && m_modeIndex == 1) {
                    str = "vmm_adc_calibration_";
                }
                else if(m_calibrationArray[1] && m_modeIndex == 2) {
                    str = "vmm_time_calibration_";
                }
                else if(m_calibrationArray[2] && m_modeIndex == 3) {
                    str = "vmm_timewalk_calibration_";
                }
                globalObjectSingle.insert("vmm_calibration",calibrationArraySingle);

                str = theDirectory + "/" + str  +  theHybridId + "_" + theTime + ".json";
                QFile jsonFileSingle(str);
                jsonFileSingle.open(QFile::WriteOnly);
                QJsonDocument documentSingle(globalObjectSingle);
                jsonFileSingle.write(documentSingle.toJson(QJsonDocument::JsonFormat::Compact));
                jsonFileSingle.close();
            }
        }

        globalObject.insert("vmm_calibration",calibrationArray);
        QJsonDocument document(globalObject);
        jsonFile.write(document.toJson(QJsonDocument::JsonFormat::Compact));
        jsonFile.close();
    }
    else
    {
        if(m_modeIndex == 4 || m_modeIndex == 5 ||  m_modeIndex == 7 || m_modeIndex == 11 || m_modeIndex == 12) {
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                int fec = GetFEC(vmm);
                int hybrid = GetHybrid(vmm);
                int chip = GetVMM(vmm);
                for(int ch=0; ch<64; ch++)
                {
                    if(m_modeIndex == 4)
                    {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "sz10b", m_bitVal[fec][hybrid][chip][static_cast<unsigned int>(ch)]  , ch );
                    }
                    else if(m_modeIndex == 5) {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "sz08b", m_bitVal[fec][hybrid][chip][static_cast<unsigned int>(ch)]  , ch );
                    }
                    else if(m_modeIndex == 7) {
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "sd", m_bitVal[fec][hybrid][chip][static_cast<unsigned int>(ch)]  , ch );
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("smx",0,ch);
                    }
                }
                if(m_modeIndex == 11) {
                    m_daqWindow->m_daq.m_fecs[fec].SetReg("debug_data_format",  false);
                    if( m_fecLatencyReset[fec] > -1.0) {
                        m_daqWindow->m_daq.m_fecs[fec].SetReg("latency_reset", (unsigned long)(m_start_reset_latency+m_fecLatencyReset[fec]));
                    }
                }
                else if(m_modeIndex == 12) {
                    m_daqWindow->m_daq.m_fecs[fec].SetReg("debug_data_format",  false);
                    if( m_fecLatencyTP[fec] > -1.0) {
                        m_daqWindow->m_daq.m_fecs[fec].SetReg("tp_latency", (unsigned long)(m_start_tp_latency+m_fecLatencyTP[fec]));
                    }
                }
            }
            SaveSettings("Calib_config");
            LoadSettings("Calib_config");
        }

    }
}

// ------------------------------------------------------------------------ //
void CalibrationModule::CloseDAQSocket()
{
    // close the socket
    if(IsDbgActive())GetMessageHandler()("Closing DAQ socket", "calibration_module::closeDAQSocket");
    m_udpSocket->close();
    m_udpSocket->disconnectFromHost();
}
// ------------------------------------------------------------------------ //
void CalibrationModule::readEvent()
{
    stringstream sx;
    QHostAddress vmmip;
    quint16 port;
    QByteArray datagram;
    datagram.clear();
    while(m_udpSocket->hasPendingDatagrams()) {
        datagram.resize(static_cast<int>(m_udpSocket->pendingDatagramSize()));
        //m_udpSocket->readDatagram(datagram.data(), datagram.size(), &vmmip);
        long size = m_udpSocket->pendingDatagramSize();
        m_udpSocket->readDatagram(datagram.data(), size, &vmmip, &port);
        char *buffer=datagram.data();
        long fecid = (vmmip.toIPv4Address() & 0x000000FF);
        Receive(buffer, size, fecid);
    } // while loop

    return;
}

void CalibrationModule::GetSettings()
{
    m_BCID.clear();
    QString qtext = m_daqWindow->m_ui->lineEditBCID->text();
    std::string text = qtext.toStdString();
    if(text.size() > 0) {
        if(qtext.contains('-')) {
            int pos = text.find("-");
            std::string start = text.substr(0, pos);
            std::string end = text.substr(pos+1, text.length()-pos);
            int startChannel = stoi(start);
            int endChannel = stoi(end);
            for(int ch=startChannel; ch<=endChannel;ch++)
            {
                m_BCID.push_back(ch);
            }
        }
        else
        {
            std::istringstream iss(text);
            std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                             std::istream_iterator<std::string>());
            for(auto x: results)
            {
                m_BCID.push_back(stoi(x));
            }
        }
    }
    if(m_daqWindow->m_ui->comboBoxRunMode->currentIndex() == 0)
    {
        m_runMode = "Calibration";
    }
    else if(m_daqWindow->m_ui->comboBoxRunMode->currentIndex() == 1){
        m_runMode = "User";
    }
    QString calibMode = m_daqWindow->m_ui->comboBoxCalibrationType->currentText();
    m_modeIndex = GetCalibrationModeIndex(calibMode);
    if(m_modeIndex == 1){
        m_number_bits = m_number_bits_offline_adc;
    }
    else if(m_modeIndex == 2){
        m_number_bits = m_number_bits_offline_time;
    }
    else if(m_modeIndex == 3){
        m_number_bits = m_number_bits_offline_timewalk;
    }
    else if(m_modeIndex == 4){
        m_number_bits = m_number_bits_adc;
    }
    else if(m_modeIndex == 5){
        m_number_bits = m_number_bits_tdc;
    }
    else if(m_modeIndex == 7){
        m_number_bits = m_number_bits_threshold;
    }
    else if(m_modeIndex == 11){
        m_number_bits = m_number_bits_offline_latency;
    }
    else if(m_modeIndex == 12){
        m_number_bits = m_number_bits_offline_latency;
    }
    else {
        m_number_bits = 1;
    }

}

void CalibrationModule::InitializeDataStructures()
{
    m_theIndex = 0;
    m_pulser_mV = 0;
    m_x.clear();
    m_dac_setting.clear();
    m_fit_error_scale.clear();
    m_fit_error_mean.clear();
    m_fit_error_sigma.clear();
    m_fit_scale.clear();
    m_fit_mean.clear();
    m_fit_sigma.clear();
    m_time.clear();

    for (unsigned int ch=0; ch < 64; ch++){
        m_channel_y[ch].clear();
        m_x.push_back(ch);
    }
    for(int i=0; i<4096;i++) {
        m_cnt_bcid[i] = 0;
    }
    m_reference_BCID = -1;
    m_adcs.clear();
    m_times.clear();
    m_total_channels = 0;
    double timeBin_ms = static_cast<double>(m_daqWindow->m_ui->Runs->value())/1024.0;
    for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
        m_fecLatencyReset[fec]=-1;
        m_fecLatencyTP[fec]=-1;;

        for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_FEC; hybrid++){
            if (m_daqWindow->m_daq.GetFEC(fec) &&  m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid)){
                m_total_channels +=128;
            }
            for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                m_adc_data[fec][hybrid][vmm].clear();
                m_channel_data[fec][hybrid][vmm].clear();
                for(int n=0; n< 1024; n++) {
                    if(fec == 0 && hybrid ==0 && vmm==0) {
                        m_adcs.push_back(n);
                        m_times.push_back(timeBin_ms*n);
                    }
                    m_adc_data[fec][hybrid][vmm].push_back(0);
                    if(n<=63) {
                        m_channel_data[fec][hybrid][vmm].push_back(0);
                    }

                }
            }
        }
    }


    for(unsigned int bit = 0; bit < 32; bit++) {
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_FEC; hybrid++){
                if(bit == 0) {
                    m_hybrid_id[fec][hybrid] = "0";
                }
                for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                    m_fit_function[bit][fec][hybrid][vmm].clear();
                    if(bit == 0)
                    {

                        m_dac_measured[fec][hybrid][vmm].clear();

                        if (m_daqWindow->m_daq.GetFEC(fec) &&  m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid)  ){
                            int stc = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("stc");
                            double tac_ns = 0;
                            switch (stc)
                            {
                            case 0: tac_ns = 60.0;
                                break;
                            case 1: tac_ns = 100.0;
                                break;
                            case 2: tac_ns = 350.0;
                                break;
                            case 3: tac_ns = 650.0;
                                break;
                            }
                            m_tac_slope[fec][hybrid][vmm] = tac_ns;
                        }
                        if (m_daqWindow->m_daq.GetFEC(fec) &&  m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid) ){
                            int st = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("st");
                            double shaping_time_ns = 0;
                            switch (st)
                            {
                            case 0: shaping_time_ns = 200.0;
                                break;
                            case 1: shaping_time_ns = 100.0;
                                break;
                            case 2: shaping_time_ns = 50.0;
                                break;
                            case 3: shaping_time_ns = 25.0;
                                break;
                            }
                            m_shaping_time[fec][hybrid][vmm] = shaping_time_ns;
                        }
                        if (m_daqWindow->m_daq.GetFEC(fec) &&  m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid)){
                            int sg = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("sg");
                            double gain = 0;
                            switch (sg)
                            {
                            case 0: gain = 0.5;
                                break;
                            case 1: gain = 1.0;
                                break;
                            case 2: gain = 3.0;
                                break;
                            case 3: gain = 4.5;
                                break;
                            case 4: gain = 6.0;
                                break;
                            case 5: gain = 9.0;
                                break;
                            case 6: gain = 12.0;
                                break;
                            case 7: gain = 16.0;
                                break;

                            }
                            m_gain[fec][hybrid][vmm] = gain;
                        }

                        if (m_daqWindow->m_daq.GetFEC(fec) &&  m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid) ){
                            bool polarity = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("sp");
                            if(polarity) {
                                m_polarity[fec][hybrid][vmm] = 1;
                            }
                            else {
                                m_polarity[fec][hybrid][vmm] = 0;
                            }

                        }
                        if (m_daqWindow->m_daq.GetFEC(fec) &&  m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid) ){
                            bool timing = m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("srat");
                            if(timing) {
                                m_timing_at_thr[fec][hybrid][vmm] = 1;
                            }
                            else {
                                m_timing_at_thr[fec][hybrid][vmm] = 0;
                            }

                        }
                        m_fit_a[fec][hybrid][vmm].clear();
                        m_fit_b[fec][hybrid][vmm].clear();
                        m_fit_c[fec][hybrid][vmm].clear();
                        m_fit_d[fec][hybrid][vmm].clear();
                        m_fit_error_r2[fec][hybrid][vmm].clear();
                        m_fit_error_rms[fec][hybrid][vmm].clear();

                        m_offset[fec][hybrid][vmm].clear();
                        m_slope[fec][hybrid][vmm].clear();
                        m_file_adc_offset[fec][hybrid][vmm].clear();
                        m_file_adc_slope[fec][hybrid][vmm].clear();
                        m_file_time_offset[fec][hybrid][vmm].clear();
                        m_file_time_slope[fec][hybrid][vmm].clear();
                        m_file_timewalk_a[fec][hybrid][vmm].clear();
                        m_file_timewalk_b[fec][hybrid][vmm].clear();
                        m_file_timewalk_c[fec][hybrid][vmm].clear();
                        m_file_timewalk_d[fec][hybrid][vmm].clear();

                        m_calVal[fec][hybrid][vmm].clear();
                        m_bitVal[fec][hybrid][vmm].clear();
                        m_y[fec][hybrid][vmm].clear();
                        m_srs_timestamp_start[fec][hybrid][vmm]=0;
                        m_srs_timestamp_end[fec][hybrid][vmm]=0;
                        m_dac_slope[fec][hybrid][vmm]=0;
                        m_dac_offset[fec][hybrid][vmm]=0;
                        m_fit_start_time[fec][hybrid][vmm].clear();
                        m_fit_start_bcid[fec][hybrid][vmm].clear();
                    }

                    m_mean[bit][fec][hybrid][vmm].clear();
                    m_mean2[bit][fec][hybrid][vmm].clear();
                    m_fit_y[bit][fec][hybrid][vmm].clear();
                    for(int n=0; n<NUM_BCID; n++) {
                        m_mean_per_bcid[n][bit][fec][hybrid][vmm].clear();
                        m_percent_bcid[n][bit][fec][hybrid][vmm].clear();
                    }

                    for (unsigned int ch=0; ch < 64; ch++){
                        m_data[bit][fec][hybrid][vmm][ch].clear();
                        m_data2[bit][fec][hybrid][vmm][ch].clear();
                    }
                }
            }

        }
    }
}

void CalibrationModule::MeasurePulserOrThresholdDAC(bool measurePulser)
{
    const int M = 11;
    m_dac_setting.clear();
    for(int i=0; i<M; i++) {
        m_dac_setting.push_back(i*100);
    }

    m_daqWindow->m_ui->pushButtonTakeData->setChecked(true);
    m_daqWindow->m_ui->pushButtonTakeData->setCheckable(true);
    std::cout << "Please wait - measuring..." << std::endl;
    m_daqWindow->m_daq.SendAll(false);
    for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
        int fec = GetFEC(vmm);
        int hybrid = GetHybrid(vmm);
        int chip = GetVMM(vmm);
        for(int n=0; n<M; n++) {
            if(measurePulser) {
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",64);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp10",(int)m_dac_setting[n]);
            }
            else {
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0", 65);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt",(int)m_dac_setting[n]);
            }
            m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
            int val = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
            m_y[fec][hybrid][chip].push_back(val);
            //std::cout << m_dac_setting[n] << ", " << val << std::endl;
        }
        std::cout << "VMM " << (int)vmm << " done!" << std::endl;
    }

    m_dataAvailable = true;
    m_data_modeIndex = m_modeIndex;
    m_daqWindow->m_ui->pushButtonTakeData->setChecked(false);
    m_daqWindow->m_ui->pushButtonTakeData->setCheckable(false);
}


void CalibrationModule::MeasurePedestalOrThreshold(bool isPedestal, bool isThresholdCalibration)
{
    m_daqWindow->m_ui->pushButtonTakeData->setChecked(true);
    m_daqWindow->m_ui->pushButtonTakeData->setCheckable(true);

    stringstream sx;
    std::cout << "Please wait - measuring..." << std::endl;
    m_daqWindow->m_daq.SendAll(false);
    if(isPedestal) {
        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            for(int ch = 0; ch<64; ch++){
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",ch);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("smx",0,ch);
                m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                int val = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                m_mean[0][fec][hybrid][chip].push_back(val);
            }
            std::cout << "VMM " << (int)vmm << " done!" << std::endl;
        }
    }
    else {
        if(isThresholdCalibration) {
            bool ok;
            qint32 deltaThreshold = QInputDialog::getInt(nullptr, tr("Channel threshold calibration"),
                                                         tr("Please enter desired channel threshold level [mV] above pedestal.\nExample: In case of a pedstal of 170 mV, a value of 20 mV leads to a threshold of 190 mV."),
                                                         40, 0, 1000, 1, &ok);
            if(!ok) {
                return;
            }
            const int M = 11;
            m_dac_setting.clear();
            for(int i=0; i<M; i++) {
                m_dac_setting.push_back(i*100);
            }

            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                int fec = GetFEC(vmm);
                int hybrid = GetHybrid(vmm);
                int chip = GetVMM(vmm);
                //Measure threshold DAC
                std::vector<int> thresholds;
                for(int n=0; n<M; n++) {
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0", 65);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt",(int)m_dac_setting[n]);
                    m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                    int val = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                    thresholds.push_back(val);
                }
                double meanPedestal = 0;
                int n=0;
                //Measure pedestal
                for(int ch = 0; ch<64; ch++){
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("smx",0,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                    int val = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                    m_mean2[0][fec][hybrid][chip].push_back(val);
                    if(val > 150 && val < 200) {
                        meanPedestal += val;
                        n++;
                    }
                }
                meanPedestal = meanPedestal/n;
                //Calculate global threshold that is equal to pedestal + deltaThreshold
                double slope_high = (thresholds[10] - thresholds[2])/(m_dac_setting[10] - m_dac_setting[2]);
                double offset_high = thresholds[10] - slope_high*m_dac_setting[10];
                int dac = 0;
                double thr = meanPedestal + deltaThreshold+20;
                dac =  (thr - offset_high)/slope_high;
                //Set global threshold
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt",(int)dac);
                for(int ch = 0; ch<64; ch++){
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("smx",1,ch);
                    for(int bit=0; bit<m_number_bits;bit++){
                        m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sd",bit,ch);
                        m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                        int val = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid,chip, 2);
                        int diff = val - m_mean2[0][fec][hybrid][chip][ch];
                        m_mean[bit][fec][hybrid][chip].push_back(diff);
                    }
                    m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                    std::cout << "VMM " << (int)vmm << " channel " << (int)ch  << " done!" << std::endl;
                }

            }
            CalculateCorrections();
        }
        else {
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                int fec = GetFEC(vmm);
                int hybrid = GetHybrid(vmm);
                int chip = GetVMM(vmm);
                m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0", 65);
                m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                int global_threshold = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                for(int ch = 0; ch<64; ch++){
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm5_sm0",ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("smx",1,ch);
                    m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                    int val = m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                    m_mean[0][fec][hybrid][chip].push_back(val);
                    m_mean[1][fec][hybrid][chip].push_back(val- global_threshold);
                    m_mean[2][fec][hybrid][chip].push_back(global_threshold);
                }
                m_daqWindow->m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hybrid,chip,false);
                std::cout << "VMM " << (int)vmm << " done!" << std::endl;
            }
        }
    }
    m_dataAvailable = true;
    m_data_modeIndex = m_modeIndex;

    m_daqWindow->m_ui->pushButtonTakeData->setChecked(false);
    m_daqWindow->m_ui->pushButtonTakeData->setCheckable(false);
}



// ------------------------------------------------------------------------ //
void CalibrationModule::Receive(const char* buffer, long size, int IP)
{
    stringstream sx;
    m_numHitsInFrame=0;
    if(g_clock_source <= 1) {
        m_numHitsInFrame = Receive_VMM3(buffer, size, IP);
    }
    else {
        m_numHitsInFrame = Receive_VMM3_SRS(buffer, size, IP);
    }

    AccumulateData();
}

int CalibrationModule::GetFEC(int vmmId) {
    return m_vmmActs[static_cast<unsigned long>(vmmId)]/(HYBRIDS_PER_FEC*VMMS_PER_HYBRID);
}

int CalibrationModule::GetHybrid(int vmmId) {
    int hybrid =  m_vmmActs[static_cast<unsigned long>(vmmId)]%(HYBRIDS_PER_FEC*VMMS_PER_HYBRID);
    return hybrid/2;
}

int CalibrationModule::GetVMM(int vmmId) {
    int hybrid =  m_vmmActs[static_cast<unsigned long>(vmmId)]%(HYBRIDS_PER_FEC*VMMS_PER_HYBRID);
    return hybrid%2;
}

int CalibrationModule::Parse_VMM3(uint32_t header, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data4, int readoutIndex) {

    int ring  =  (header >>24) & 0xFF;
    int fen =  (header >>16) & 0xFF;

    //int length =  (header & 0xFF)*256 + (header >> 8) & 0xFF;
    uint16_t fecId =
        static_cast<uint8_t>(ring / 2) * 32 + fen;

    int fec =  m_fecIDPos[fecId];
    //std::cout << ring << " " << fen << " " << fecId << " " << fec << std::endl;
    uint64_t timestamp_high = ntohl(data1);
    uint64_t timestamp_low = ntohl(data2);
    uint16_t bcid = ntohs(data3 >> 16) & 0xFFF;
    uint16_t overThreshold = (ntohs(data3 & 0xFFFF) >> 15) & 0x01;
    uint16_t adc = ntohs(data3 & 0xFFFF) & 0x3FF;
    uint8_t geoid = (data4 >> 24) & 0x3F;
    uint8_t tdc = (data4 >> 16) & 0xFF;
    uint8_t vmmid = (data4 >> 8) & 0x1F;
    uint8_t chNo = (data4) & 0x3F;
    uint16_t bc_counter_high = geoid & 0x0F;
    uint16_t bc_counter_low = tdc;
    uint16_t bc_counter = 256*bc_counter_high + bc_counter_low;

    stringstream sx;
    if(IsDbgActive()) {
        sx.str("");
        sx << "SRS Data fecId " << static_cast<int>(fecId)
           << ", vmmId " << static_cast<int>(vmmid)
           << ", chNo: " <<  static_cast<int>(chNo)
           << ", bcid: " <<  static_cast<int>(bcid)
           << ", tdc: " <<  static_cast<int>(tdc)
           << ", adc: " <<  static_cast<int>(adc)
           << ", overThreshold: " <<  static_cast<int>(overThreshold)
           << ", timestamp_high: " << static_cast<int>(timestamp_high)
           << ", timestamp_low: " << static_cast<int>(timestamp_low)
           << "\n";
        GetMessageHandler()(sx,"calibration_module::Parse_VMM3"); sx.str("");
    }
    int hybrid = vmmid/2;
    int chip = vmmid%2;


    if(readoutIndex == 0) {
        if(m_start == 0)  {
            m_start =  timestamp_high * 1000000000 +  timestamp_low * 0.5 * g_clock_period;
            m_numHits=0;
        }
    }
    uint64_t runLength = static_cast<uint64_t>(m_daqWindow->m_ui->Runs->value())*1000000;
    uint64_t cntPerChannel = 50;
    uint64_t nowTime = timestamp_high * 1000000000 +  timestamp_low * 0.5 * g_clock_period;
    if( m_start > 0 && nowTime > m_end) {
        if(IsCalibration() && m_modeIndex != 6  && m_numHits >= cntPerChannel*m_total_channels) {
            m_end = nowTime ;
        }
        else {
            m_end = nowTime ;
        }
    }

    uint64_t delay_ns = 0;
    if(m_end >= m_start) {
        delay_ns = m_end - m_start;
    }
    //convert run value from ms to ns
    if(delay_ns >= runLength)
    {
        return 1;
    }

    bool bcidFilter = true;
    if(!m_BCID.empty()) {
        auto it = find (m_BCID.begin(), m_BCID.end(), bcid);
        if(it == m_BCID.end()) {
            bcidFilter = false;
        }
    }
    if (bcidFilter)
    {
        //ADC calibrations or user settings average ADC
        if(m_modeIndex == 1 || m_modeIndex == 4  || m_modeIndex == 21)
        {
            m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(adc);
        }
        //Time calibration
        else if(m_modeIndex == 2)
        {
            m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(bcid*1000+tdc);
            m_cnt_bcid[bcid]++;
        }
        //Timewalk
        else if(m_modeIndex == 3 )
        {
            double adc_slope = 1;
            double adc_offset = 0;
            if(m_file_adc_offset[fec][hybrid][chip].size() == 64 && m_file_adc_slope[fec][hybrid][chip].size() == 64) {
                adc_slope = m_file_adc_slope[fec][hybrid][chip][chNo];
                adc_offset = m_file_adc_offset[fec][hybrid][chip][chNo];
            }
            double time_slope = 1;
            double time_offset = 0;
            if(m_file_time_offset[fec][hybrid][chip].size() == 64 && m_file_time_slope[fec][hybrid][chip].size() == 64) {
                time_slope = m_file_time_slope[fec][hybrid][chip][chNo];
                time_offset = m_file_time_offset[fec][hybrid][chip][chNo];
            }
            double theADC = (adc - adc_offset)*adc_slope;
            m_data2[m_bitCount][fec][hybrid][chip][chNo].push_back(theADC);
            double theTime = bcid*g_clock_period + (1.5*g_clock_period -  m_tac_slope[fec][hybrid][chip]*tdc/255.0 - time_offset)*time_slope;
            m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(theTime);
        }
        //TDC calibration or user settings average TDC per channel
        else if(m_modeIndex == 5|| m_modeIndex == 22)
        {
            m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(tdc);
        }
        //S-curve
        else if(m_modeIndex == 6)
        {
            m_channel_y[chNo][m_theIndex]++;
        }
        //Reset Latency
        else if(m_modeIndex == 11)
        {
            m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(static_cast<int>(bcid)-static_cast<int>(bc_counter));
        }
        //TP Latency
        else if(m_modeIndex == 12)
        {
            m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(static_cast<int>(bcid)-100);
        }
        //User settings average bcid per channel
        else if(m_modeIndex == 23)
        {
            m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(bcid);
        }
        //User settings counts per channel
        else if(m_modeIndex == 20)
        {
            m_channel_data[fec][hybrid][chip][chNo]++;
        }
        //User settings ADC distribution
        else if(m_modeIndex == 24)
        {
            if(adc >= 0 && adc <=1023) {
                m_adc_data[fec][hybrid][chip][adc]++;
            }
        }
        //User settings Time
        else if(m_modeIndex == 25)
        {
            double theTime = m_end - m_start + (1.5*g_clock_period -  m_tac_slope[fec][hybrid][chip]*tdc/255.0);
            uint64_t timeBin_ns = static_cast<uint64_t>(m_daqWindow->m_ui->Runs->value())*1000000/1024.0;
            int timeIndex = (int)(theTime/timeBin_ns);
            if(timeIndex >= 0 && timeIndex <=1023) {
                //std::cout << timeIndex << " " << timeBin_ns << " " << (int)hybrid << " " << (int)chip << " " << (int)chNo << std::endl;
                m_adc_data[fec][hybrid][chip][timeIndex]++;
            }
        }
        m_numHits++;
    }

    return 1;
}

int CalibrationModule::Receive_VMM3(const char *buffer, long size, int IP) {
    stringstream sx;
    if (size < m_ESSHeaderSize) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "IP " << IP << ": Undersize data: " << size << " words!";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }


    m_ESSheader.m_padding = (*reinterpret_cast<const uint8_t *>(&buffer[0]));
    m_ESSheader.m_version = (*reinterpret_cast<const uint8_t *>(&buffer[1]));
    m_ESSheader.m_cookie1 = (*reinterpret_cast<const uint8_t *>(&buffer[2]));
    m_ESSheader.m_cookie2 = (*reinterpret_cast<const uint8_t *>(&buffer[3]));
    m_ESSheader.m_cookie3 = (*reinterpret_cast<const uint8_t *>(&buffer[4]));
    m_ESSheader.m_type = (*reinterpret_cast<const uint8_t *>(&buffer[5]));
    m_ESSheader.m_length = (*reinterpret_cast<const uint16_t *>(&buffer[6]));
    m_ESSheader.m_outputQ = (*reinterpret_cast<const uint8_t *>(&buffer[8]));
    m_ESSheader.m_timeSrc = (*reinterpret_cast<const uint8_t *>(&buffer[9]));
    m_ESSheader.m_pulseT_high = (*reinterpret_cast<const uint32_t *>(&buffer[10]));
    m_ESSheader.m_pulseT_low = (*reinterpret_cast<const uint32_t *>(&buffer[14]));
    m_ESSheader.m_prevPT_high = (*reinterpret_cast<const uint32_t *>(&buffer[18]));
    m_ESSheader.m_prevPT_low = (*reinterpret_cast<const uint32_t *>(&buffer[22]));
    m_ESSheader.m_seqNo = (*reinterpret_cast<const uint32_t *>(&buffer[26]));



    //    std::cout <<  "m_ESSheader.m_padding  " <<  m_ESSheader.m_padding  << std::endl;
    //    std::cout <<  "m_ESSheader.m_version  " <<  m_ESSheader.m_version  << std::endl;
    //    std::cout <<  "m_ESSheader.m_cookie1  " <<  m_ESSheader.m_cookie1  << std::endl;
    //    std::cout <<  "m_ESSheader.m_cookie2  " <<  m_ESSheader.m_cookie2  << std::endl;
    //    std::cout <<  "m_ESSheader.m_cookie3  " <<  m_ESSheader.m_cookie3  << std::endl;
    //    std::cout <<  "m_ESSheader.m_type  " <<  m_ESSheader.m_type  << std::endl;
    //    std::cout <<  "m_ESSheader.m_length  " <<  m_ESSheader.m_length  << std::endl;
    //    std::cout <<  "m_ESSheader.m_outputQ  " <<  m_ESSheader.m_outputQ  << std::endl;
    //    std::cout <<  "m_ESSheader.m_timeSrc  " <<  m_ESSheader.m_timeSrc  << std::endl;
    //    std::cout <<  "m_ESSheader.m_pulseT_high  " <<  m_ESSheader.m_pulseT_high  << std::endl;
    //    std::cout <<  "m_ESSheader.m_pulseT_low  " <<  m_ESSheader.m_pulseT_low  << std::endl;
    //    std::cout <<  "m_ESSheader.m_prevPT_high  " <<  m_ESSheader.m_prevPT_high  << std::endl;
    //    std::cout <<  "m_ESSheader.m_prevPT_low  " <<  m_ESSheader.m_prevPT_low  << std::endl;
    //    std::cout <<  "m_ESSheader.m_seqNo  " <<  m_ESSheader.m_seqNo  << std::endl;


    if (size < m_ESSHeaderSize + m_headerSizeAssister + m_hitSize_VMM3a) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "Undersized payload\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }

    auto dataLength = size - m_ESSHeaderSize;
    if ((dataLength % (m_headerSizeAssister + m_hitSize_VMM3a)) != 0) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "Invalid data length " << dataLength << "\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }
    int readoutIndex = 0;
    m_packetCounter++;
    if(m_packetCounter >= 3) {
        while (dataLength >= (m_headerSizeAssister + m_hitSize_VMM3a)) {
            auto dataHeaderOffset = m_ESSHeaderSize + (m_headerSizeAssister + m_hitSize_VMM3a) * readoutIndex;
            auto dataOffset = dataHeaderOffset + m_headerSizeAssister;

            const uint32_t header = (*reinterpret_cast<const uint32_t *>(&buffer[dataHeaderOffset]));
            const uint32_t data1 = (*reinterpret_cast<const uint32_t *>(&buffer[dataOffset]));
            const uint32_t data2 = (*reinterpret_cast<const uint32_t *>(&buffer[dataOffset+4]));
            const uint32_t data3 = (*reinterpret_cast<const uint32_t *>(&buffer[dataOffset+8]));
            const uint32_t data4 = (*reinterpret_cast<const uint32_t *>(&buffer[dataOffset+12]));

            int res = Parse_VMM3(ntohl(header),  ntohl(data1),  ntohl(data2),  ntohl(data3),  ntohl(data4), readoutIndex);
            readoutIndex++;
            dataLength -=(m_headerSizeAssister + m_hitSize_VMM3a);

        }
    }
    return readoutIndex;
}




QString CalibrationModule::CreateFileName(int polarity, double gain, double peaktime, double tac, int dac)
{
    //QString theName = m_daqWindow->GetApplicationPath() + "/" + name;
    //QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    QString theName="";
    QString str;
    if(gain != -1)
    {
        str = QString("%1%2").arg(gain).arg("mVfC");
        theName = str;
    }
    if(peaktime != -1)
    {
        str = QString("%1%2").arg(peaktime).arg("ns");
        theName = theName + "_" + str;
    }
    if(tac != -1)
    {
        str = QString("%1%2").arg(tac).arg("ns");
        theName = theName + "_" + str;
    }
    if(polarity == 0)
    {
        str = QString("%1").arg("negative");
        theName = theName + "_" + str;
    }
    else if (polarity == 1) {
        str = QString("%1").arg("positive");
        theName = theName + "_" + str;

    }
    if(dac != -1)
    {
        str = QString("%1%2").arg(dac).arg("DAC");
        theName = theName + "_" + str;
    }

    //theName = theName + "_" + theDate;

    //if(theName.length()>=245) {
    //    theName = m_daqWindow->GetApplicationPath() + "/vmm_calibration_" + theDate;
    //}
    return theName;
}

int CalibrationModule::Parse_VMM3_SRS(uint32_t data1, uint16_t data2, int fecId) {
    int fec =  m_fecIDPos[fecId];
    stringstream sx;
    int dataflag = (data2 >> 15) & 0x1;
    if (dataflag) {

        uint8_t overThreshold = (data2 >> 14) & 0x01;
        uint8_t chNo = (data2 >> 8) & 0x3f;
        uint8_t tdc = data2 & 0xff;
        uint8_t vmmid = (data1 >> 22) & 0x1F;
        uint8_t triggerOffset = (data1 >> 27) & 0x1F;
        uint16_t adc = (data1 >> 12) & 0x3FF;
        uint32_t bcid = Gray2bin32(data1 & 0xFFF);
        uint16_t bc_counter_high = adc & 0x00F;
        uint16_t bc_counter_low = tdc;
        uint16_t bc_counter = 256*bc_counter_high + bc_counter_low;
        if(IsDbgActive()) {
            sx.str("");
            sx << "SRS Data fecId " << static_cast<int>(fecId)
               << ", vmmId " << static_cast<int>(vmmid)
               << ", chNo: " <<  static_cast<int>(chNo)
               << ", bcid: " <<  static_cast<int>(bcid)
               << ", tdc: " <<  static_cast<int>(tdc)
               << ", adc: " <<  static_cast<int>(adc)
               << ", overThreshold: " <<  static_cast<int>(overThreshold)
               << ", triggerOffset: " << static_cast<int>(triggerOffset)
               << "\n";
            GetMessageHandler()(sx,"calibration_module::Parse_VMM3_SRS"); sx.str("");
        }

        bool bcidFilter = true;
        if(!m_BCID.empty()) {
            auto it = find (m_BCID.begin(), m_BCID.end(), bcid);
            if(it == m_BCID.end()) {
                bcidFilter = false;
            }
        }
        if (bcidFilter)
        {
            int hybrid = vmmid/2;
            int chip = vmmid%2;

            if(m_srs_timestamp_start[fec][hybrid][chip] > 0 && m_srs_timestamp_end[fec][hybrid][chip] >= m_srs_timestamp_start[fec][hybrid][chip])
            {
                uint64_t delay = m_srs_timestamp_end[fec][hybrid][chip] - m_srs_timestamp_start[fec][hybrid][chip];
                if(delay <= static_cast<uint64_t>(m_daqWindow->m_ui->Runs->value())*1000000)
                {
                    //ADC calibrations or user settings average ADC
                    if(m_modeIndex == 1 || m_modeIndex == 4  || m_modeIndex == 21)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(adc);
                    }
                    //Time calibration
                    else if(m_modeIndex == 2 )
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(bcid*1000+tdc);
                        m_cnt_bcid[bcid]++;
                    }
                    //Timewalk
                    else if(m_modeIndex == 3 )
                    {
                        double adc_slope = 1;
                        double adc_offset = 0;
                        if(m_file_adc_offset[fec][hybrid][chip].size() == 64 && m_file_adc_slope[fec][hybrid][chip].size() == 64) {
                            adc_slope = m_file_adc_slope[fec][hybrid][chip][chNo];
                            adc_offset = m_file_adc_offset[fec][hybrid][chip][chNo];
                        }
                        double time_slope = 1;
                        double time_offset = 0;
                        if(m_file_time_offset[fec][hybrid][chip].size() == 64 && m_file_time_slope[fec][hybrid][chip].size() == 64) {
                            time_slope = m_file_time_slope[fec][hybrid][chip][chNo];
                            time_offset = m_file_time_offset[fec][hybrid][chip][chNo];
                        }
                        double theADC = (adc - adc_offset)*adc_slope;
                        m_data2[m_bitCount][fec][hybrid][chip][chNo].push_back(theADC);
                        double theTime = bcid*g_clock_period + (1.5*g_clock_period -  m_tac_slope[fec][hybrid][chip]*tdc/255.0 - time_offset)*time_slope;
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(theTime);
                    }
                    //TDC calibration or user settings average TDC per channel
                    else if(m_modeIndex == 5|| m_modeIndex == 22)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(tdc);
                    }
                    //S-curve
                    else if(m_modeIndex == 6)
                    {
                        m_channel_y[chNo][m_theIndex]++;
                    }
                    //Reset Latency
                    else if(m_modeIndex == 11)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(static_cast<int>(bcid)-static_cast<int>(bc_counter));
                    }
                    //TP Latency
                    else if(m_modeIndex == 12)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(static_cast<int>(bcid)-100);
                    }
                    //User settings average bcid per channel
                    else if(m_modeIndex == 23)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(bcid);
                    }
                    //User settings counts per channel
                    else if(m_modeIndex == 20)
                    {
                        m_channel_data[fec][hybrid][chip][chNo]++;
                    }
                    //User settings ADC distribution
                    else if(m_modeIndex == 24)
                    {
                        if(adc >= 0 && adc <=1023) {
                            m_adc_data[fec][hybrid][chip][adc]++;
                        }
                    }
                    //User settings Time
                    else if(m_modeIndex == 25)
                    {
                        double timeSRS = m_srs_timestamp_end[fec][hybrid][chip] - m_srs_timestamp_start[fec][hybrid][chip];
                        double timeChip = triggerOffset*4096*g_clock_period  + bcid*g_clock_period + (1.5*g_clock_period -  m_tac_slope[fec][hybrid][chip]*tdc/255.0);
                        double theTime = timeSRS + timeChip;
                        int timeBin_ns = static_cast<uint64_t>(m_daqWindow->m_ui->Runs->value())*1000000/1024;
                        int timeIndex = (int)(theTime/timeBin_ns);
                        if(timeIndex >= 0 && timeIndex <=1023) {
                            m_adc_data[fec][hybrid][chip][timeIndex]++;
                        }
                    }
                    m_numHits++;
                }
            }
        }
        return 1;
    } else {
        /// Marker
        uint8_t vmmid = (data2 >> 10) & 0x1F;
        uint64_t timestamp_lower_10bit = data2 & 0x03FF;
        uint64_t timestamp_upper_32bit = data1;
        uint64_t timestamp_42bit = (timestamp_upper_32bit << 10) + timestamp_lower_10bit;
        int hybrid = vmmid/2;
        int chip = vmmid%2;
        if(m_start == 0) {
            m_start = timestamp_42bit*g_clock_period;
        }
        //first timestamp/marker for this VMM
        if(m_srs_timestamp_start[fec][hybrid][chip] == 0)
        {
            //42 bit timestamp, giving number of 40 MHz clock cycles
            m_srs_timestamp_start[fec][hybrid][chip] = timestamp_42bit*g_clock_period;
            m_numHits=0;
        }
        m_srs_timestamp_end[fec][hybrid][chip] = timestamp_42bit*g_clock_period;
        m_end = timestamp_42bit*g_clock_period;

        if(IsDbgActive()) {
            sx.str("");
            sx << "SRS Marker fecId " << static_cast<int>(m_commonData.m_fecId) << ", vmmId " << vmmid << ", timestamp lower 10bit: " <<  timestamp_lower_10bit
               << ", timestamp upper 32bit: " <<  timestamp_upper_32bit
               << ", timestamp 42bit: " <<  timestamp_42bit << "\n";
            GetMessageHandler()(sx,"calibration_module::Parse_VMM3_SRS"); sx.str("");
        }
        return 0;
    }
}

int CalibrationModule::Receive_VMM3_SRS(const char *buffer, long size, int fecId) {
    int numHitsInFrame = 0;
    stringstream sx;
    if (size < 4) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "Undersize data: " << size << " words!";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }

    const struct SRSHeader_VMM3 *hdr = reinterpret_cast<const SRSHeader_VMM3 *>(buffer);
    m_commonData.m_frameCounter = ntohl(hdr->m_frameCounter);
    if (m_commonData.m_frameCounter == 0xfafafafa) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "End of frame!";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }
    m_commonData.m_lastFrameCounter = m_commonData.m_frameCounter;

    if (m_commonData.m_lastFrameCounter > 0) {
        int64_t fcDiff = static_cast<int64_t>(m_commonData.m_frameCounter) - static_cast<int64_t>(m_commonData.m_lastFrameCounter);
        if (fcDiff < 0) {
            if(IsDbgActive()) {
                sx.str("");
                sx << "Overflow: frame counter " << m_commonData.m_frameCounter
                   << ", last frame counter " << m_commonData.m_lastFrameCounter <<
                    ", difference " << fcDiff <<
                    ", correction " << m_commonData.m_frameCounter + 0xFFFFFFFF << "\n";
                GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
            }
            m_commonData.m_frameCounter = m_commonData.m_frameCounter + 0xFFFFFFFF;
        }
        if (fcDiff > 1) {
            if(IsDbgActive()) {
                sx.str("");
                sx << "Lost frame(s), frame counter " << m_commonData.m_frameCounter << ", last frame counter " << m_commonData.m_lastFrameCounter  << "\n";
                GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
            }
        }
    }
    if (size < m_SRSHeaderSize_VMM3 + m_hitAndMarkerSize_VMM3) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "Undersized payload\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }
    m_commonData.m_dataId = ntohl(hdr->m_dataId);
    if ((m_commonData.m_dataId & 0xffffff00) != 0x564d3300) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "Unknown data " << m_commonData.m_dataId << "\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }
    m_commonData.m_fecId = (((m_commonData.m_dataId & 0xF0) >> 4));
    m_commonData.m_udpTimeStamp = ntohl(hdr->m_udpTimeStamp);
    m_commonData.m_offsetOverflow = ntohl(hdr->m_offsetOverflow);
    auto dataLength = size - m_SRSHeaderSize_VMM3;
    if ((dataLength % m_hitAndMarkerSize_VMM3) != 0) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "Invalid data length " << dataLength << "\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }
    int readoutIndex = 0;
    while (dataLength >= m_hitAndMarkerSize_VMM3) {
        auto data1Offset = m_SRSHeaderSize_VMM3 + m_hitAndMarkerSize_VMM3 * readoutIndex;
        auto data2Offset = data1Offset + m_data1Size;

        const uint32_t data1 = htonl(*reinterpret_cast<const uint32_t *>(&buffer[data1Offset]));
        const uint16_t data2 = htons(*reinterpret_cast<const uint16_t *>(&buffer[data2Offset]));
        int res = Parse_VMM3_SRS(data1, data2, fecId );
        if (res == 1) { // This was data
            numHitsInFrame++;
        }
        readoutIndex++;
        dataLength -= m_hitAndMarkerSize_VMM3;
        if (numHitsInFrame == m_maxHits_VMM3 && dataLength > 0) {
            if(IsDbgActive()) {
                sx.str("");
                sx << "Data overflow, skipping " << dataLength << " bytes\n";
                GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
            }
            break;
        }
    }
    return numHitsInFrame;
}


uint32_t CalibrationModule::Gray2bin32(uint32_t num) {
    num = num ^ (num >> 16);
    num = num ^ (num >> 8);
    num = num ^ (num >> 4);
    num = num ^ (num >> 2);
    num = num ^ (num >> 1);
    return num;
}
