#include <cmath>
#include <numeric>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFileInfo>
#include "calibration_module.h"
#include "mainwindow.h"

#ifdef __linux__
#include <arpa/inet.h>
#elif __APPLE__
#include <arpa/inet.h>
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
    func = c[0]*0.5*errorfunctionc((x[0]-c[1])/(sqrt(2)* c[2]));
}

void erfc_function_mirrored(const real_1d_array &c, const real_1d_array &x, double &func, void *ptr)
{
    func = c[0]*0.5*errorfunctionc((-x[0]+c[1])/(sqrt(2)* c[2]));
}

void erfc_grad(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    func = c[0]*0.5*errorfunctionc((x[0]-c[1])/(sqrt(2)* c[2]));
    grad[0] = 0.5*errorfunctionc((x[0]-c[1])/(sqrt(2)* c[2]));
    grad[1] = (c[0]*0.5*sqrt(2/pi())/c[2])*exp(-(x[0]-c[1])*(x[0]-c[1])/(2*c[2]*c[2]));
    grad[2] = (c[0]*0.5*sqrt(2/pi())*(x[0]-c[1])/(c[2]*c[2]))*exp(-(x[0]-c[1])*(x[0]-c[1])/(2*c[2]*c[2]));
}

void erfc_grad_mirrored(const real_1d_array &c, const real_1d_array &x, double &func, real_1d_array &grad, void *ptr)
{
    func = c[0]*0.5*errorfunctionc((-x[0]+c[1])/(sqrt(2)* c[2]));
    grad[0] = 0.5*errorfunctionc((-x[0]+c[1])/(sqrt(2)* c[2]));
    grad[1] = (c[0]*0.5*sqrt(2/pi())/c[2])*exp(-(-x[0]+c[1])*(-x[0]+c[1])/(2*c[2]*c[2]));
    grad[2] = (c[0]*0.5*sqrt(2/pi())*(-x[0]+c[1])/(c[2]*c[2]))*exp(-(-x[0]+c[1])*(-x[0]+c[1])/(2*c[2]*c[2]));
}


CalibrationModule::CalibrationModule(MainWindow *top, QObject *parent) :
    QObject(parent),
    m_mainWindow{top},
    m_dbg(false),
    m_udpSocket(nullptr),
    m_msg(new MessageHandler),
    m_ignore16(false)
{

    m_calibrationArray[0] = nullptr;
    m_calibrationArray[1] = nullptr;
    m_modeIndex = -1;
    m_data_modeIndex = -1;

    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlot1);
    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlot2);
    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlot3);
    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlot4);
    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlot5);
    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlot6);
    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlot7);
    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlot8);

    for(int n=0; n<8;n++)
    {
        plotVector[n]->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    }
    connect( m_mainWindow->m_daqWindow->ui->comboBoxFec, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updatePlot()));
    connect( m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updatePlot()));
    connect( m_mainWindow->m_daqWindow->ui->choicePlotTime, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updatePlot()));
    connect( m_mainWindow->m_daqWindow->ui->choiceBit, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updatePlot()));

    connect( m_mainWindow->m_daqWindow->ui->comboBoxRunMode, SIGNAL(currentIndexChanged(int)),
             this, SLOT(calibAndPlotChoices()));
    connect( m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType, SIGNAL(currentIndexChanged(int)),
             this, SLOT(calibAndPlotChoices()));
    connect( m_mainWindow->m_daqWindow->ui->choicePlotTime, SIGNAL(currentIndexChanged(int)),
             this, SLOT(calibAndPlotChoices()));

    m_mainWindow->m_daqWindow->ui->choicePlotTime->setVisible(true);
    m_mainWindow->m_daqWindow->ui->choicePlotTime->setEnabled(true);
    m_mainWindow->m_daqWindow->ui->choiceBit->setVisible(false);
    m_mainWindow->m_daqWindow->ui->choiceBit->setEnabled(false);
    m_mainWindow->m_daqWindow->ui->pushButtonStoreCorrections->setEnabled(true);
    m_mainWindow->m_daqWindow->ui->pushButtonSavePDF->setEnabled(true);
    m_mainWindow->m_daqWindow->ui->pushButtonCSV->setEnabled(true);

    m_mainWindow->m_daqWindow->ui->comboBoxRunMode->setCurrentIndex(0);
    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(0);
    //Fill choices
    m_mainWindow->m_daqWindow->ui->choicePlotTime->clear();
    m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("calibrated ADC");
    m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("corrections");
    m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("fit");
    m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("ADC distribution");
    m_mainWindow->m_daqWindow->ui->choicePlotTime->setVisible(true);
    m_mainWindow->m_daqWindow->ui->choicePlotTime->setEnabled(true);
    m_mainWindow->m_daqWindow->ui->label_vmm->setText("Display VMMs");
    m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 1-8"));

}


void CalibrationModule::calibAndPlotChoices()
{
    if(QObject::sender() == m_mainWindow->m_daqWindow->ui->comboBoxRunMode){
        m_mainWindow->m_calib->m_dataAvailable = false;
        m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(-1);
        m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->clear();
        if(m_mainWindow->m_daqWindow->ui->comboBoxRunMode->currentIndex() == 0)
        {
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("Offline ADC");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("Offline Time (BCID/TDC)");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("ADC");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("TDC");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("S-curve");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("Threshold");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("Pedestal");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("Pulser DAC");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("Threshold DAC");
            //ADC calib active
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(0);
            m_mainWindow->m_daqWindow->ui->pushButtonSavePDF->setEnabled(true);
            m_mainWindow->m_daqWindow->ui->pushButtonCSV->setEnabled(true);
            m_mainWindow->m_daqWindow->ui->pushButtonStoreCorrections->setEnabled(true);
            //Fill choices
            m_mainWindow->m_daqWindow->ui->choicePlotTime->clear();
            m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("calibrated ADC");
            m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("corrections");
            m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("fit");
            m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("ADC distribution");
            m_mainWindow->m_daqWindow->ui->choicePlotTime->setVisible(true);
            m_mainWindow->m_daqWindow->ui->choicePlotTime->setEnabled(true);
            // calibrated ADC active
            m_mainWindow->m_daqWindow->ui->choicePlotTime->setCurrentIndex(0);
            m_mainWindow->m_daqWindow->ui->choiceBit->setVisible(false);
            m_mainWindow->m_daqWindow->ui->choiceBit->setEnabled(false);
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxRunMode->currentIndex() == 1)
        {
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("Channels");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("ADC");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("TDC");
            m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->addItem("BCID");
            m_mainWindow->m_daqWindow->ui->pushButtonStoreCorrections->setEnabled(false);
            m_mainWindow->m_daqWindow->ui->pushButtonCSV->setEnabled(true);
            m_mainWindow->m_daqWindow->ui->pushButtonSavePDF->setEnabled(true);
            m_mainWindow->m_daqWindow->ui->choicePlotTime->setVisible(false);
            m_mainWindow->m_daqWindow->ui->choicePlotTime->setEnabled(false);
            m_mainWindow->m_daqWindow->ui->choiceBit->setVisible(false);
            m_mainWindow->m_daqWindow->ui->choiceBit->setEnabled(false);
        }
    }
    else if(QObject::sender() == m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType){
        m_mainWindow->m_calib->m_dataAvailable = false;
        m_mainWindow->m_daqWindow->ui->comboBoxFec->clear();
        if(m_mainWindow->m_daqWindow->ui->comboBoxRunMode->currentIndex() == 0) {
            //S-curve per channel for pre-selected VMM
            if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 4 ) {
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("CH 0-7"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("CH 8-15"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("CH 16-23"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("CH 24-31"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("CH 32-39"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("CH 40-47"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("CH 48-55"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("CH 56-63"));
                m_mainWindow->m_daqWindow->ui->label_vmm->setText("Display Channels");
            }
            //All others
            else {
                m_mainWindow->m_daqWindow->ui->label_vmm->setText("Display VMMs");
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 1-8"));
                if(m_vmmActs.size() >= 8 )
                {
                    m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 9-16"));
                }
                if(m_vmmActs.size() >= 16 )
                {
                    m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 17-24"));
                }
                if(m_vmmActs.size() >= 24)
                {
                    m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 25-32"));
                }
                if(m_vmmActs.size() >= 32 )
                {
                    m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 33-40"));
                }
                if(m_vmmActs.size() >= 40 )
                {
                    m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 41-48"));
                }
                if(m_vmmActs.size() >= 48 )
                {
                    m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 49-56"));
                }
                if(m_vmmActs.size() >= 56 )
                {
                    m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 57-64"));
                }
            }

            if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 0 || m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 1 ||
                    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 2 || m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 3 ||
                    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 5 ) {
                m_mainWindow->m_daqWindow->ui->pushButtonStoreCorrections->setEnabled(true);
            }
            else {
                m_mainWindow->m_daqWindow->ui->pushButtonStoreCorrections->setEnabled(false);
            }

            if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 0 ) {
                m_mainWindow->m_daqWindow->ui->choicePlotTime->clear();
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("calibrated ADC");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("corrections");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("fit");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("ADC distribution");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->setVisible(true);
                m_mainWindow->m_daqWindow->ui->choicePlotTime->setEnabled(true);
                m_mainWindow->m_daqWindow->ui->choicePlotTime->setCurrentIndex(0);
                m_mainWindow->m_daqWindow->ui->choiceBit->setVisible(false);
                m_mainWindow->m_daqWindow->ui->choiceBit->setEnabled(false);
            }
            else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 1 ) {
                m_mainWindow->m_daqWindow->ui->choicePlotTime->clear();
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("calibrated time");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("corrections");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("fit");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("TDC distribution");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->addItem("BCID percentage");
                m_mainWindow->m_daqWindow->ui->choicePlotTime->setVisible(true);
                m_mainWindow->m_daqWindow->ui->choicePlotTime->setEnabled(true);
                m_mainWindow->m_daqWindow->ui->choicePlotTime->setCurrentIndex(0);
                m_mainWindow->m_daqWindow->ui->choiceBit->setVisible(false);
                m_mainWindow->m_daqWindow->ui->choiceBit->setEnabled(false);
            }
            else {
                m_mainWindow->m_daqWindow->ui->choicePlotTime->setVisible(false);
                m_mainWindow->m_daqWindow->ui->choicePlotTime->setEnabled(false);
                m_mainWindow->m_daqWindow->ui->choiceBit->setVisible(false);
                m_mainWindow->m_daqWindow->ui->choiceBit->setEnabled(false);
            }

        }
    }
    else if(QObject::sender() == m_mainWindow->m_daqWindow->ui->choicePlotTime){
        if(!m_dataAvailable)
        {
            m_mainWindow->m_daqWindow->ui->choicePlotTime->setCurrentIndex(0);
            return;
        }
        if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 3) {
            m_mainWindow->m_daqWindow->ui->choiceBit->clear();
            if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 1 ) {

                for(int i=0;i < 2; i++) {
                    for(int n=0;n< 8; n++) {
                        QString txt = QStringLiteral("%1 ns").arg((i*25)+n*25/8.0);
                        m_mainWindow->m_daqWindow->ui->choiceBit->addItem(txt);
                    }
                }
            }
            else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 0 ) {
                for(int bit=0;bit< m_number_bits; bit++) {
                    QString txt = QStringLiteral("%1 DAC").arg(m_dac_setting[bit]);
                    m_mainWindow->m_daqWindow->ui->choiceBit->addItem(txt);
                }

            }
            m_mainWindow->m_daqWindow->ui->choiceBit->setVisible(true);
            m_mainWindow->m_daqWindow->ui->choiceBit->setEnabled(true);
        }
        else {
            m_mainWindow->m_daqWindow->ui->choiceBit->setVisible(false);
            m_mainWindow->m_daqWindow->ui->choiceBit->setEnabled(false);
        }

        // ADC and time calib (depending on plot choice
        if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() <= 1 && m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() > 1) {
            m_mainWindow->m_daqWindow->ui->comboBoxFec->clear();
            m_mainWindow->m_daqWindow->ui->label_vmm->setText("Display Channels");
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 0-7"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 8-15"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 16-23"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 24-31"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 32-39"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 40-47"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 48-55"));
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM" + QString::number(vmm) + ", CH 56-63"));
            }
        }
        else {
            m_mainWindow->m_daqWindow->ui->comboBoxFec->clear();
            m_mainWindow->m_daqWindow->ui->label_vmm->setText("Display VMMs");
            m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 1-8"));
            if(m_vmmActs.size() >= 8 )
            {
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 9-16"));
            }
            if(m_vmmActs.size() >= 16 )
            {
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 17-24"));
            }
            if(m_vmmActs.size() >= 24)
            {
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 25-32"));
            }
            if(m_vmmActs.size() >= 32 )
            {
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 33-40"));
            }
            if(m_vmmActs.size() >= 40 )
            {
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 41-48"));
            }
            if(m_vmmActs.size() >= 48 )
            {
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 49-56"));
            }
            if(m_vmmActs.size() >= 56 )
            {
                m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("VMM 57-64"));
            }
        }
        //All

    }
}


void CalibrationModule::SaveSettings() {
    m_mainWindow->m_daqs[0].SendAll(false);
    m_mainWindow->m_daqWindow->ui->line_configFile->setText("Calib_config");
    emit m_mainWindow->m_daqWindow->on_Button_save_clicked();
    QThread::usleep(1000);
    m_mainWindow->m_daqWindow->ui->line_configFile->setText("");
    QThread::usleep(1000);
    m_mainWindow->m_daqWindow->LoadConfig("Calib_config");
}

void CalibrationModule::LoadSettings() {
    m_mainWindow->m_daqWindow->LoadConfig("Calib_config");
}

void CalibrationModule::StopDataTaking()
{
    std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Stop Data Taking" << std::endl;
    m_start = 0;
    m_end = 0;
    m_mainWindow->m_daqWindow->ui->onACQ->setEnabled(true);
    m_mainWindow->m_daqWindow->ui->offACQ->setEnabled(true);
    m_mainWindow->m_daqWindow->ui->onACQ->setChecked(false);
    m_mainWindow->m_daqWindow->ui->offACQ->setEnabled(true);
    m_mainWindow->m_daqWindow->ui->Send->setEnabled(true);

    m_mainWindow->m_daqs[0].ACQHandler(false);
    QThread::msleep(500);
    CloseDAQSocket();
}
void CalibrationModule::StartDataTaking()
{
    emit m_mainWindow->m_daqWindow->ui->onACQ->clicked();
    QThread::usleep(1000);
    ConnectDAQSocket();
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(true);
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
    if(mode == "Online ADC")
    {
        return 3;
    }
    if(mode == "Online TDC")
    {
        return 4;
    }
    if(mode == "S-curve")
    {
        return 5;
    }
    if(mode == "Threshold")
    {
        return 6;
    }
    if(mode == "Pedestal")
    {
        return 7;
    }
    if(mode == "Pulser DAC")
    {
        return 8;
    }
    if(mode == "Threshold DAC")
    {
        return 9;
    }
    if(mode == "Channels")
    {
        return 10;
    }
    if(mode == "ADC")
    {
        return 11;
    }
    if(mode == "TDC")
    {
        return 12;
    }
    if(mode == "BCID")
    {
        return 13;
    }


    return -1;
}

bool CalibrationModule::CheckModes()
{
    if(!m_dataAvailable || m_data_modeIndex != m_modeIndex || m_modeIndex < 1)
    {
        return false;
    }
    if(m_mainWindow->m_daqWindow->ui->comboBoxFec->currentIndex() == -1) {
        if(m_mainWindow->m_daqWindow->ui->comboBoxFec->count() == 0) {
            return false;
        }
        else {
            m_mainWindow->m_daqWindow->ui->comboBoxFec->setCurrentIndex(0);
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
    int fec = m_theFEC-1;
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
            c.setlength(3);

            c[0] = 10.0;
            double sp_mean = dac_min + 0.5 * abs(dac_max - dac_min);
            double sp_sigma = abs(dac_max - dac_min) / 2.5;
            c[1] = sp_mean;
            c[2] = sp_sigma;


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
            m_fit_error_scale.push_back(-1);
            m_fit_error_mean.push_back(-1);
            m_fit_error_sigma.push_back(-1);
            m_fit_scale.push_back(-1);
            m_fit_mean.push_back(-1);
            m_fit_sigma.push_back(-1);
        }
    }
}




void CalibrationModule::FitOfflineCalibrationData()
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
                    theMean = m_fit_y[bit][fec][hybrid][chip][ch];

                    int m =  m_fit_start_time[fec][hybrid][chip][ch]/(3.125*TIME_FACTOR);
                    if(m_fit_start_time[fec][hybrid][chip][ch]/(3.125*TIME_FACTOR) < 0) {
                        m = m - 1;
                    }
                    double firstTime = m*3.125*TIME_FACTOR - m_fit_start_time[fec][hybrid][chip][ch];
                    theXValue = firstTime + bit*3.125*TIME_FACTOR;

                }
                if(theMean != -9999) {
                    x.push_back(theXValue);
                    y.push_back(theMean);
                    sumX += theXValue;
                    sumY += theMean;
                    sumXY += theXValue * theMean;
                    sumX2 += theXValue * theXValue;
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
            double slope_corr = 1;
            double mean_offset = 0;
            double slope = 0;
            double offset = 0;
            if(m_modeIndex == 1) {
                m_dac_slope[fec][hybrid][chip] = mean_slope_perChip[vmm];
                m_dac_offset[fec][hybrid][chip] = mean_offset_perChip[vmm];
                if(m_dac_slope[fec][hybrid][chip] > 0) {
                    slope_corr = 1/m_dac_slope[fec][hybrid][chip];
                }
                mean_offset =  m_dac_offset[fec][hybrid][chip];
            }
            else if(m_modeIndex == 2) {
                slope_corr = 1;
                mean_offset = 0;
            }
            for(unsigned int ch = 0; ch<64; ch++){
                //Channel with missing data
                if(m_slope[fec][hybrid][chip][ch] == 0.0)
                {
                    //in case the neighbouring channels have data, use it (gaps of 1 or 2 missing channels)
                    if(ch == 0) {
                        if( m_slope[fec][hybrid][chip][1] != 0.0) {
                            offset =  m_offset[fec][hybrid][chip][1] - mean_offset*m_slope[fec][hybrid][chip][1]*slope_corr;
                            slope = 1/(m_slope[fec][hybrid][chip][1] * slope_corr);
                        }
                        else if( m_slope[fec][hybrid][chip][2] != 0.0) {
                            offset =  m_offset[fec][hybrid][chip][2] - mean_offset*m_slope[fec][hybrid][chip][2]*slope_corr;
                            slope = 1/(m_slope[fec][hybrid][chip][2] * slope_corr);
                        }
                    }
                    else if(ch == 63) {
                        if( m_slope[fec][hybrid][chip][62] != 0.0) {
                            offset =  m_offset[fec][hybrid][chip][62] - mean_offset*m_slope[fec][hybrid][chip][62]*slope_corr;
                            slope = 1/(m_slope[fec][hybrid][chip][62] * slope_corr);

                        }
                        else if( m_slope[fec][hybrid][chip][61] != 0.0) {
                            offset =  m_offset[fec][hybrid][chip][61] - mean_offset*m_slope[fec][hybrid][chip][61]*slope_corr;
                            slope = 1/(m_slope[fec][hybrid][chip][61] * slope_corr);
                        }
                    }
                    else {
                        if( m_slope[fec][hybrid][chip][ch+1] != 0.0) {
                            if(m_slope[fec][hybrid][chip][ch-1] != 0.0) {
                                offset =  0.5*(m_offset[fec][hybrid][chip][ch+1]+m_offset[fec][hybrid][chip][ch-1]) - mean_offset*0.5*(m_slope[fec][hybrid][chip][ch+1]+m_slope[fec][hybrid][chip][ch-1])*slope_corr;
                                slope = 1/(0.5*(m_slope[fec][hybrid][chip][ch+1]+m_slope[fec][hybrid][chip][ch-1]) * slope_corr);
                            }
                            else if(ch>=2 && m_slope[fec][hybrid][chip][ch-2] != 0.0) {
                                offset =  0.5*(m_offset[fec][hybrid][chip][ch+1]+m_offset[fec][hybrid][chip][ch-2]) - mean_offset*0.5*(m_slope[fec][hybrid][chip][ch+1]+m_slope[fec][hybrid][chip][ch-2])*slope_corr;
                                slope = 1/(0.5*(m_slope[fec][hybrid][chip][ch+1]+m_slope[fec][hybrid][chip][ch-2]) * slope_corr);
                            }
                        }
                        else if( ch <= 61 && m_slope[fec][hybrid][chip][ch+2] != 0.0) {
                            if( m_slope[fec][hybrid][chip][ch-1] != 0.0) {
                                offset =  0.5*(m_offset[fec][hybrid][chip][ch+2]+m_offset[fec][hybrid][chip][ch-1]) - mean_offset*0.5*(m_slope[fec][hybrid][chip][ch+2]+m_slope[fec][hybrid][chip][ch-1])*slope_corr;
                                slope = 1/(0.5*(m_slope[fec][hybrid][chip][ch+2]+m_slope[fec][hybrid][chip][ch-1]) * slope_corr);
                            }
                        }
                    }

                }
                //Channel has data
                else
                {
                    if(m_modeIndex == 2) {
                        offset =  m_offset[fec][hybrid][chip][ch] - (m_fit_start_time[fec][hybrid][chip][ch])*m_slope[fec][hybrid][chip][ch];
                        slope = 1/m_slope[fec][hybrid][chip][ch];
                    }
                    else {
                        offset =  m_offset[fec][hybrid][chip][ch] - m_dac_offset[fec][hybrid][chip]*m_slope[fec][hybrid][chip][ch]/m_dac_slope[fec][hybrid][chip];
                        slope = m_dac_slope[fec][hybrid][chip]/m_slope[fec][hybrid][chip][ch];
                    }

                }

                //limit the numbers to 3 significant digits
                slope = std::round(1000*slope)/1000;
                offset = std::round(1000*offset)/1000;

                m_slope[fec][hybrid][chip][ch] = slope;
                m_offset[fec][hybrid][chip][ch] = offset;
                slopeArray.push_back(slope);
                offsetArray.push_back(offset);
            }
            calibrationObject.insert("fecID",fecId);
            calibrationObject.insert("vmmID",hybrid*2+chip);
            if(m_modeIndex == 1)
            {
                calibrationObject.insert("adc_offsets",offsetArray);
                calibrationObject.insert("adc_slopes",slopeArray);
            }
            else {
                calibrationObject.insert("time_offsets",offsetArray);
                calibrationObject.insert("time_slopes",slopeArray);
            }
            m_calibrationArray[m_modeIndex-1]->push_back(calibrationObject);
        }
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
    if(m_modeIndex == 5) {
        fec = m_theFEC-1;
        hybrid = m_theVMM / 2;
        chip = m_theVMM % 2;
        name = "S-curve";
        timing_at_threshold = m_timing_at_thr[fec][hybrid][chip];
        gain = m_gain[fec][hybrid][chip];
        polarity = m_polarity[fec][hybrid][chip];
        peaktime = m_shaping_time[fec][hybrid][chip];
        tac = m_tac_slope[fec][hybrid][chip];
        bcclock = m_bc_clock[fec][hybrid];
        QString theName = CreateFileName(name,polarity,gain,peaktime,tac,bcclock);
        if(m_scan_type == 0) {
            theName += "scanned_threshold.csv";
        }
        else {
            theName += "scanned_pulser.csv";
        }
        if(m_outFile.is_open())
        {
            m_outFile.close();
        }
        m_outFile.open(theName.toStdString().c_str(), std::ofstream::out);
        m_outFile << "fec,vmm,ch,pulser_dac,pulser_mV,threshold_dac,threshold_mV,time,counts,rate\n";
        for(int bit=0; bit<m_number_bits;bit++){
            for(unsigned int ch = 0; ch<64; ch++){
                double rate = 0;
                if( m_time[bit] > 0) {
                    rate = m_channel_y[ch][bit]/(1000*m_time[bit]);
                }
                if(m_scan_type == 0) {
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_pulser_dac << "," << m_pulser_mV << "," << m_dac_setting[bit] << ","
                              << m_dac_measured[fec][hybrid][chip][bit] << "," << m_time[bit] << "," << m_channel_y[ch][bit] << "," <<rate << "\n" ;
                }
                else {
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_dac_setting[bit] << "," << m_dac_measured[fec][hybrid][chip][bit] << "," << m_threshold_dac << ","
                              << m_threshold_mV << "," << m_time[bit] << "," << m_channel_y[ch][bit] << "," <<rate << "\n" ;
                }
            }
        }

        m_outFile << "fec,vmm,ch,fit scale, error scale, fit mean, error mean, fit sigma, error sigma\n";
        for(unsigned int ch = 0; ch<64; ch++){
            m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_fit_scale[ch] << "," <<  m_fit_error_scale[ch] << "," <<  m_fit_mean[ch] << ","
                      <<  m_fit_error_mean[ch] << "," <<  m_fit_sigma[ch] << "," << m_fit_error_sigma[ch] << "\n" ;
        }
        m_outFile.close();
    }
    else {
        QString name = "";
        if(m_modeIndex == 1)
        {
            name = "Offline_ADC";
        }
        else if(m_modeIndex == 2)
        {
            name = "Offline_Time";
        }
        else if(m_modeIndex == 3)
        {
            name = "Online_ADC";
        }
        else if(m_modeIndex == 4)
        {
            name = "Online_TDC";
        }
        else if(m_modeIndex == 6)
        {
            name = "Threshold";
        }
        else if(m_modeIndex == 7)
        {
            name = "Pedestal";
        }
        else if(m_modeIndex == 8)
        {
            name = "Pulser_DAC";
        }
        else if(m_modeIndex == 9)
        {
            name = "Threshold_DAC";
        }
        else if(m_modeIndex == 10)
        {
            name = "Counts_Channels";
        }
        else if(m_modeIndex == 11)
        {
            name = "Mean_ADC";
        }
        else if(m_modeIndex == 12)
        {
            name = "Mean_TDC";
        }
        else if(m_modeIndex == 13)
        {
            name = "Mean_BCID";
        }
        fec = GetFEC(0);
        hybrid = GetHybrid(0);
        chip = GetVMM(0);

        timing_at_threshold = m_timing_at_thr[fec][hybrid][chip];
        gain = m_gain[fec][hybrid][chip];
        polarity = m_polarity[fec][hybrid][chip];
        peaktime = m_shaping_time[fec][hybrid][chip];
        tac = m_tac_slope[fec][hybrid][chip];
        bcclock = m_bc_clock[fec][hybrid];

        QString theName = CreateFileName(name,polarity,gain,peaktime,tac,bcclock);
        theName += ".csv";

        if(m_outFile.is_open())
        {
            m_outFile.close();
        }
        m_outFile.open(theName.toStdString().c_str(), std::ofstream::out);
        for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){

            fec = GetFEC(vmm);
            hybrid = GetHybrid(vmm);
            chip = GetVMM(vmm);



            if(m_modeIndex == 1)
            {
                //<< cnt << "," << std::setprecision(6) << rate<< std::endl;
                m_outFile << "fec,vmm,ch,pulser_dac_setting,pulser_dac_measured,adc_measured,slope,offset,adc_corrected\n";
                for(int ch = 0; ch<64; ch++){

                    for(int bit=0; bit<m_number_bits;bit++){
                        m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_dac_setting[bit] << ","  << m_dac_measured[fec][hybrid][chip][bit] << ","
                                  << m_mean[bit][fec][hybrid][chip][ch] << "," << m_slope[fec][hybrid][chip][ch] << "," << m_offset[fec][hybrid][chip][ch]
                                     << "," << (m_mean[bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])* m_slope[fec][hybrid][chip][ch] << "\n" ;
                    }
                }
            }
            else if(m_modeIndex == 2)
            {
                m_outFile << "fec,vmm,ch,pulser_skew,time_slope_correction,time_offset_correction,";
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
                        m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << bit*3.125*TIME_FACTOR << ","  <<
                                     m_slope[fec][hybrid][chip][ch] << "," << m_offset[fec][hybrid][chip][ch] << ",";
                        double theTotalTime = 0;
                        int bcid = m_fit_start_bcid[fec][hybrid][chip][ch];
                        int z = 0;
                        for(int n=0; n<NUM_BCID; n++) {
                            m_outFile << m_most_common_BCID+bcid+n << ",";
                            m_outFile << m_percent_bcid[n][bit][fec][hybrid][chip][ch] << ",";
                            m_outFile << (bcid+n)*m_bc_period[fec][hybrid] << ",";


                            if(m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] != -9999) {
                                m_outFile << m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] << ",";
                                m_outFile << (m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])*  m_slope[fec][hybrid][chip][ch] << ",";
                                theTotalTime += m_percent_bcid[n][bit][fec][hybrid][chip][ch]*((bcid+n)*m_bc_period[fec][hybrid] + (m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])*  m_slope[fec][hybrid][chip][ch]);
                                z++;
                            }
                            else {
                                m_outFile << 0 << ",";
                                m_outFile << 0 << ",";
                            }
                        }
                        if(z==0) {
                            theTotalTime = -9999;
                        }

                        m_outFile << m_mean[bit][fec][hybrid][chip][ch] << "," << theTotalTime << "\n" ;
                    }
                }

            }
            else if(m_modeIndex == 3)
            {
                m_outFile << "fec,vmm,ch, ADC correction [mV], ADC, calibrated value, , best common value\n";
                for(int ch = 0; ch<64; ch++){
                    for(int bit=0; bit<m_number_bits;bit++){
                        m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << bit << "," << m_mean[bit][fec][hybrid][chip][ch]  << "," << m_calVal[fec][hybrid][chip][ch] << "," << m_y[fec][hybrid][chip][ch] << "\n" ;
                    }
                }

            }
            else if(m_modeIndex == 4)
            {
                m_outFile << "fec,vmm,ch, TDC correction [mV], TDC, calibrated value, best common value\n";
                for(int ch = 0; ch<64; ch++){
                    for(int bit=0; bit<m_number_bits;bit++){
                        m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << bit << "," << m_mean[bit][fec][hybrid][chip][ch]  << "," << m_calVal[fec][hybrid][chip][ch] << "," << m_y[fec][hybrid][chip][ch] << "\n" ;
                    }
                }
            }

            else if(m_modeIndex == 6)
            {
                name = "Threshold";
                if(m_isThresholdCalibration) {
                    m_outFile << "fec,vmm,ch, threshold correction [mV], threshold [mV], calibrated correction [mV], calibrated threshold [mV], best common value [mV]\n";
                    for(int ch = 0; ch<64; ch++){
                        for(int bit=0; bit<m_number_bits;bit++){
                            m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << bit << "," << m_mean[bit][fec][hybrid][chip][ch]  << "," << m_bitVal[fec][hybrid][chip][ch] << "," << m_calVal[fec][hybrid][chip][ch] << "," << m_y[fec][hybrid][chip][ch] << "\n" ;
                        }
                    }

                }
                else {
                    m_outFile << "fec,vmm,ch, threshold [mV]\n";
                    for(int ch = 0; ch<64; ch++){
                        m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                    }
                }

            }
            else if(m_modeIndex == 7)
            {
                m_outFile << "fec,vmm,ch, pedestal [mV]\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
            else if(m_modeIndex == 8)
            {
                m_outFile << "fec,vmm, pulser dac setting, pulser dac measured\n";
                for(int n=0; n<m_dac_setting.size();n++){
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << m_dac_setting[n] << "," << m_y[fec][hybrid][chip][n] << "\n" ;
                }
            }
            else if(m_modeIndex == 9)
            {
                m_outFile << "fec,vmm, threshold dac setting, threshold dac measured\n";
                for(int n=0; n<m_dac_setting.size();n++){
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << m_dac_setting[n] << "," << m_y[fec][hybrid][chip][n] << "\n" ;
                }
            }
            else if(m_modeIndex == 10)
            {
                m_outFile << "fec,vmm,ch, cnt\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
            else if(m_modeIndex == 11)
            {
                m_outFile << "fec,vmm,ch, mean ADC\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
            else if(m_modeIndex == 12)
            {
                m_outFile << "fec,vmm,ch, mean TDC\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
                }
            }
            else if(m_modeIndex == 13)
            {
                m_outFile << "fec,vmm,ch, mean BCID\n";
                for(int ch = 0; ch<64; ch++){
                    m_outFile << fec+1  << "," << hybrid*2+chip << "," << ch  << "," << m_mean[0][fec][hybrid][chip][ch] << "\n" ;
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
    int theChoice =  m_mainWindow->m_daqWindow->ui->comboBoxFec->currentIndex();
    int start = theChoice * 8;
    int end = (theChoice +1)*8;
    unsigned long limit = 0;
    //S-curve
    if(m_modeIndex == 5) {
        limit = 64;
    }
    else {
        limit = m_vmmActs.size();
    }
    for(int idx=start; idx< static_cast<int>(limit)&& idx <end; idx++){
        int fec = 0;
        int hybrid = 0;
        int chip = 0;

        if(m_modeIndex == 5) {
            fec = m_theFEC-1;
            hybrid = m_theVMM / 2;
            chip = m_theVMM % 2;
        }
        else {
            fec = GetFEC(idx);
            hybrid = GetHybrid(idx);
            chip = GetVMM(idx);
        }
        if(idx==start)
        {
            timing_at_threshold = m_timing_at_thr[fec][hybrid][chip];
            gain = m_gain[fec][hybrid][chip];
            polarity = m_polarity[fec][hybrid][chip];
            peaktime = m_shaping_time[fec][hybrid][chip];
            tac = m_tac_slope[fec][hybrid][chip];
            bcclock = m_bc_clock[fec][hybrid];
        }
        QCustomPlot *plot = plotVector[idx%8];
        QString name = "";

        if(m_modeIndex == 1)
        {
            name = "Offline_ADC";
            if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 1) {
                name+= "_corrections";
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 2) {
                name+= "_fit_CH" + QString::number(idx);
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 3) {
                name+= "_hist_CH" + QString::number(idx) + "_DAC" + QString::number(m_dac_measured[fec][hybrid][chip][m_mainWindow->m_daqWindow->ui->choiceBit->currentIndex()]);
            }
        }
        else if(m_modeIndex == 2)
        {
            name = "Offline_Time";
            if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 1) {
                name+= "_corrections";
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 2) {
                name+= "_fit_CH" + QString::number(idx);
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 3) {
                name+= "_hist_CH" + QString::number(idx) + "_" + QString::number(3.125*TIME_FACTOR*m_mainWindow->m_daqWindow->ui->choiceBit->currentIndex()) +"ns";
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 4) {
                name+= "_bcid_percent_CH" + QString::number(idx);
            }
        }
        else if(m_modeIndex == 3)
        {
            name = "Online_ADC";
        }
        else if(m_modeIndex == 4)
        {
            name = "Online_TDC";
        }
        else if(m_modeIndex == 5)
        {
            name = "S-curve_CH" + QString::number(idx);
        }
        else if(m_modeIndex == 6)
        {
            name = "Threshold";
        }
        else if(m_modeIndex == 7)
        {
            name = "Pedestal";
        }
        else if(m_modeIndex == 8)
        {
            name = "Pulser_DAC";
        }
        else if(m_modeIndex == 9)
        {
            name = "Threshold_DAC";
        }
        else if(m_modeIndex == 10)
        {
            name = "Counts_Channels";
        }
        else if(m_modeIndex == 11)
        {
            name = "Mean_ADC";
        }
        else if(m_modeIndex == 12)
        {
            name = "Mean_TDC";
        }
        else if(m_modeIndex == 13)
        {
            name = "Mean_BCID";
        }

        if(m_modeIndex == 5) {
            name += "_FEC" + QString::number(m_theFEC);
            name += "_VMM" + QString::number(m_theVMM);
        }
        else {
            name += "_FEC" + QString::number(fec+1);
            name += "_VMM" + QString::number(hybrid*2+chip);
        }

        QString theName = CreateFileName(name,polarity,gain,peaktime,tac,bcclock);
        theName += ".pdf";
        plot->savePdf(theName);
    }
}


void CalibrationModule::PlotData(){
    for(int i=0; i< plotVector.size();i++)
    {
        QCustomPlot *plot = plotVector[i];
        plot->clearItems();
        plot->clearPlottables();
        plot->clearGraphs();
        plot->yAxis->setLabel("");
        plot->xAxis->setLabel("");
        plot->legend->setVisible(false);
        plot->replot();
    }
    if(!CheckModes())
    {
        return;
    }
    unsigned long theChoice =  static_cast<unsigned long>(m_mainWindow->m_daqWindow->ui->comboBoxFec->currentIndex());
    unsigned long start = theChoice * 8;
    unsigned long end = (theChoice +1)*8;
    unsigned long limit = 0;
    //S-curve
    if(m_modeIndex == 5) {
        limit = 64;
    }
    else if((m_modeIndex == 1  || m_modeIndex == 2) && m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() > 1) {
        limit = 64*m_vmmActs.size();
    }
    else {
        limit = m_vmmActs.size();
    }
    for(unsigned long idx=start; idx<limit && idx <end; idx++){
        QCustomPlot *plot = plotVector[idx%8];

        plot->clearGraphs();
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
        if((m_modeIndex == 1  || m_modeIndex == 2) && m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() > 1) {
            fec = GetFEC(static_cast<int>(idx/64));
            hybrid = GetHybrid(static_cast<int>(idx/64));
            chip = GetVMM(static_cast<int>(idx/64));
            ch= idx%64;
            title = "FEC " + QString::number(fec+1) + " VMM " + QString::number(hybrid*2+chip) + " CH " + QString::number(ch);
        }
        else if(m_modeIndex != 5) {
            fec = GetFEC(static_cast<int>(idx));
            hybrid = GetHybrid(static_cast<int>(idx));
            chip = GetVMM(static_cast<int>(idx));
            title = "FEC " + QString::number(fec+1) + " VMM " + QString::number(hybrid*2+chip);
        }
        else {
            fec = m_theFEC-1;
            hybrid = m_theVMM / 2;
            chip = m_theVMM % 2;
            title = "FEC " + QString::number(fec+1) + " VMM " + QString::number(hybrid*2+chip) + " CH " + QString::number(idx);
        }
        unsigned int colorFactor = 0;

        if(m_modeIndex == 1) {
            title+= ": Mean ADC";
            colorFactor = 10;
            plot->yAxis->setRange(0.0, 1023.0);

            if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 0) {
                title+= ": Mean ADC";
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 1) {
                title+= ": ADC corrections (slop/offset)";
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 2) {
                title+= ": Fit (measured ADC vs DAC)";
                plot->xAxis->setLabel("measured DAC");
                plot->xAxis->setRange(m_dac_measured[fec][hybrid][chip][0], m_dac_measured[fec][hybrid][chip][m_number_bits-1]);
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 3) {
                title+= ": counts per ADC";
                plot->xAxis->setRange(0, 1024);
                plot->xAxis->setLabel("ADC");
            }
        }
        else if(m_modeIndex == 2) {
            colorFactor = 10;
            plot->yAxis->setRange(-10.0, 60.0);
            if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 0) {
                title+= ": Mean time";
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 1) {
                title+= ": Time corrections (slop/offset)";
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 2) {
                title+= ": Fit (measured time vs pulse shift)";
                plot->xAxis->setRange(0, m_number_bits*3.125*TIME_FACTOR);
                plot->xAxis->setLabel("pulse shift [ns]");
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 3) {
                title+= ": counts per TDC";
                plot->xAxis->setRange(0, 256);
                plot->xAxis->setLabel("TDC");
            }
            else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 4) {
                title+= ": BCID percentages [%]";
                plot->xAxis->setRange(0, m_number_bits*3.125*TIME_FACTOR);
                plot->xAxis->setLabel("pulse shift [ns]");
            }

        }
        else if(m_modeIndex == 3) {
            title+= ": Mean ADC";
            colorFactor = 5;
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 4) {
            title+= ": Mean TDC";
            colorFactor = 10;
            plot->yAxis->setRange(0.0, 255.0);
        }
        else if(m_modeIndex == 5)
        {
            title+= ": Rate [kHz]";
            if(m_scan_type == 0) {
                plot->xAxis->setLabel("threshold [mV]");
            }
            else {
                plot->xAxis->setLabel("pulser [mV]");
            }
            plot->xAxis->setRange(m_dac_measured[fec][hybrid][chip][0], m_dac_measured[fec][hybrid][chip][m_number_bits -1]);
        }
        else if(m_modeIndex == 6) {
            title+= ": Threshold [mV]";
            colorFactor = 5;
            plot->yAxis->setRange(0.0, 300.0);
        }
        else if(m_modeIndex == 7)
        {
            title+= ": Pedestal [mV]";
            plot->yAxis->setRange(0.0, 350.0);
        }
        else if(m_modeIndex == 8) {
            title+= ": Pulser DAC [mV]";
            plot->xAxis->setLabel("Pulser setting [DAC]");
            plot->xAxis->setRange(0.0, 1023.0);
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 9)
        {
            title+= ": Threshold DAC [mV]";
            plot->xAxis->setLabel("Threshold setting [DAC]");
            plot->xAxis->setRange(0.0, 1023.0);
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 10)
        {
            title+= ": Counts";
        }
        else if(m_modeIndex == 11)
        {
            title+= ": Mean ADC";
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 12)
        {
            title+= ": Mean TDC";
            plot->yAxis->setRange(0.0, 255.0);
        }
        else if(m_modeIndex == 13)
        {
            title+= ": Mean BCID";
            plot->yAxis->setRange(0.0, 4095.0);
        }


        plot->yAxis->setLabel(title);
        if(!IsCalibration() /*m_runMode == "User"  >= 10 */)
        {
            plot->addGraph();
            plot->graph(0)->setPen(pen);
            plot->graph(0)->setData( QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_mean[0][fec][hybrid][chip]));
            plot->yAxis->setLabel(title);
            plot->legend->setVisible(true);
            plot->legend->setFont(QFont("Helvetica",9));
            plot->graph(0)->setName(title);
            plot->yAxis->rescale();
        }
        else
        {
            //Offline ADC
            if(m_modeIndex == 1)
            {
                // calibrated time
                if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 0) {
                    for(int bit=0; bit<m_number_bits;bit++){
                        plot->addGraph();
                        plot->graph(bit*3)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(m_mean[bit][fec][hybrid][chip]));
                        plot->graph(bit*3)->setPen(QPen(Qt::black,1,Qt::SolidLine));
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
                        plot->graph(bit*3+1)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(y));
                        plot->graph(bit*3+1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                        if(bit > 0) {
                            plot->legend->removeItem(plot->legend->itemCount()-1);
                        }

                        plot->addGraph();
                        plot->graph(bit*3+2)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(y2));

                        plot->graph(bit*3+2)->setPen(QPen(Qt::blue,1,Qt::DashLine));
                        if(bit > 0) {
                            plot->legend->removeItem(plot->legend->itemCount()-1);
                        }


                    }
                    plot->graph(0)->setName("measured");
                    plot->graph(1)->setName("corrected");
                    plot->graph(2)->setName("DAC measured");
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                }
                // corrections
                else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 1) {
                    plot->addGraph();
                    std::vector<double> x;
                    std::vector<double> y_slope;
                    std::vector<double> y_offset;
                    for(unsigned int ch=0; ch<64; ch++)
                    {
                        if(!(m_slope[fec][hybrid][chip][ch] == 0.0 && m_offset[fec][hybrid][chip][ch] == -1.0)) {
                            x.push_back(ch);
                            y_offset.push_back(m_offset[fec][hybrid][chip][ch]);
                            y_slope.push_back(m_slope[fec][hybrid][chip][ch]);
                        }
                    }
                    plot->graph(0)->setData(
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(y_offset));
                    plot->graph(0)->setPen(QPen(Qt::blue,2,Qt::SolidLine));
                    plot->graph(0)->setName("offset");

                    plot->addGraph();
                    plot->graph(1)->setData(
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(y_slope));
                    plot->graph(1)->setPen(QPen(Qt::darkGreen,2,Qt::SolidLine));
                    plot->graph(1)->setName("slope");
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                    plot->yAxis->rescale();
                }
                // fit
                else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 2) {

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
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(y));
                    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
                    plot->graph(0)->setPen(QPen(Qt::blue));

                    plot->addGraph();
                    plot->graph(1)->setData(
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(yf));
                    plot->graph(1)->setPen(QPen(Qt::red));


                    plot->graph(0)->setName("measured");

                    plot->graph(1)->setName(QString("fit: " + QString::number(slope) + "* DAC + " + QString::number(offset)));

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);
                    plot->xAxis->setRange(m_dac_measured[fec][hybrid][chip][0], m_dac_measured[fec][hybrid][chip][m_number_bits-1]);
                    plot->yAxis->rescale();

                }
                //ADC distribution
                else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 3) {
                    int bit = m_mainWindow->m_daqWindow->ui->choiceBit->currentIndex();
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
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(y));
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
                if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 0) {

                    for(int bit=0; bit<m_number_bits;bit++){
                        plot->addGraph();
                        std::vector<double> x;
                        std::vector<double> xc;
                        std::vector<double> y;
                        std::vector<double> yc;
                        for(unsigned int ch=0; ch<64; ch++)
                        {
                            if(m_mean[bit][fec][hybrid][chip][ch] != -9999) {
                                x.push_back(ch);
                                y.push_back(m_mean[bit][fec][hybrid][chip][ch]);

                            }
                            double theTotalTime = 0;
                            int bcid = m_fit_start_bcid[fec][hybrid][chip][ch];
                            int z = 0;
                            for(int n=0; n<NUM_BCID; n++) {
                                if(m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] != -9999) {
                                    theTotalTime += m_percent_bcid[n][bit][fec][hybrid][chip][ch]*((bcid+n)*m_bc_period[fec][hybrid] + (m_mean_per_bcid[n][bit][fec][hybrid][chip][ch] - m_offset[fec][hybrid][chip][ch])*  m_slope[fec][hybrid][chip][ch]);
                                    z++;
                                }
                            }
                            if(z>0) {
                                xc.push_back(ch);
                                yc.push_back(theTotalTime);
                            }
                        }
                        plot->graph(bit*2)->setData(
                                    QVector<double>::fromStdVector(x),
                                    QVector<double>::fromStdVector(y));
                        plot->graph(bit*2)->setPen(QPen(Qt::black,1,Qt::SolidLine));
                        if(bit > 0) {
                            plot->legend->removeItem(plot->legend->itemCount()-1);
                        }
                        plot->addGraph();
                        plot->graph(bit*2+1)->setData(
                                    QVector<double>::fromStdVector(xc),
                                    QVector<double>::fromStdVector(yc));
                        plot->graph(bit*2+1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                        if(bit > 0) {
                            plot->legend->removeItem(plot->legend->itemCount()-1);
                        }
                    }
                    plot->graph(0)->setName("measured");
                    plot->graph(1)->setName("corrected");
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                }
                // corrections
                else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 1) {
                    plot->addGraph();
                    std::vector<double> x;
                    std::vector<double> y_slope;
                    std::vector<double> y_offset;
                    for(unsigned int ch=0; ch<64; ch++)
                    {
                        if(!(m_slope[fec][hybrid][chip][ch] == 0.0)) {
                            x.push_back(ch);
                            y_offset.push_back(m_offset[fec][hybrid][chip][ch]);
                            y_slope.push_back(m_slope[fec][hybrid][chip][ch]);
                        }
                    }
                    plot->graph(0)->setData(
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(y_offset));
                    plot->graph(0)->setPen(QPen(Qt::blue,2,Qt::SolidLine));


                    plot->addGraph();
                    plot->graph(1)->setData(
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(y_slope));
                    plot->graph(1)->setPen(QPen(Qt::darkGreen,2,Qt::SolidLine));
                    plot->graph(0)->setName("offset");
                    plot->graph(1)->setName("slope");
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                    plot->yAxis->rescale();
                }
                // fit
                else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 2) {
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
                    int m =  m_fit_start_time[fec][hybrid][chip][ch]/(3.125*TIME_FACTOR);
                    if(m_fit_start_time[fec][hybrid][chip][ch]/(3.125*TIME_FACTOR) < 0) {
                        m = m - 1;
                    }
                    double firstTime = m*3.125*TIME_FACTOR - m_fit_start_time[fec][hybrid][chip][ch];
                    for(int bit=0; bit<m_number_bits;bit++){
                        if(m_fit_y[bit][fec][hybrid][chip][ch] != -9999) {
                            y.push_back(m_fit_y[bit][fec][hybrid][chip][ch]);

                            double theTime = firstTime + bit*3.125*TIME_FACTOR;
                            x.push_back(theTime);
                            yf.push_back(offset + theTime*slope);
                        }
                    }
                    plot->graph(0)->setData(
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(y));
                    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
                    plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
                    plot->graph(0)->setName("measured");
                    plot->addGraph();
                    plot->graph(1)->setData(
                                QVector<double>::fromStdVector(x),
                                QVector<double>::fromStdVector(yf));
                    plot->graph(1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                    plot->graph(1)->setName(QString("fit: " + QString::number(slope) + "* t + " + QString::number(offset)));

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignLeft);
                    plot->xAxis->setRange(firstTime, firstTime + m_number_bits*3.125*TIME_FACTOR);
                    plot->yAxis->rescale();
                }
                //TDC distribution
                else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 3) {
                    int bit = m_mainWindow->m_daqWindow->ui->choiceBit->currentIndex();
                    if(bit == -1) {
                        bit = 0;
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    for(int n=0; n<NUM_BCID; n++) {
                        int bcid = m_fit_start_bcid[fec][hybrid][chip][ch]+m_most_common_BCID+n;

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
                                    QVector<double>::fromStdVector(x),
                                    QVector<double>::fromStdVector(y));
                        plot->graph(n)->setScatterStyle(QCPScatterStyle::ssDisc);

                        if(bcid-m_most_common_BCID == -2) {
                            plot->graph(n)->setPen(QPen(Qt::darkCyan));
                            plot->graph(n)->setBrush(QBrush(Qt::cyan));
                        }
                        else if(bcid-m_most_common_BCID == -1) {
                            plot->graph(n)->setPen(QPen(Qt::darkRed));
                            plot->graph(n)->setBrush(QBrush(Qt::red));
                        }
                        else if(bcid-m_most_common_BCID == 0) {
                            plot->graph(n)->setPen(QPen(Qt::darkGreen));
                            plot->graph(n)->setBrush(QBrush(Qt::green));
                        }
                        else if(bcid-m_most_common_BCID == 1) {
                            plot->graph(n)->setPen(QPen(Qt::darkBlue));
                            plot->graph(n)->setBrush(QBrush(Qt::blue));

                        }
                        else if(bcid-m_most_common_BCID == 2) {
                            plot->graph(n)->setPen(QPen(Qt::darkMagenta));
                            plot->graph(n)->setBrush(QBrush(Qt::magenta));

                        }
                        plot->graph(n)->setName(QString("BCID "+ QString::number(bcid)));
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                    plot->yAxis->rescale();
                }
                // BCID percent
                else if(m_mainWindow->m_daqWindow->ui->choicePlotTime->currentIndex() == 4) {

                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",9));
                    for(int n=0; n<NUM_BCID; n++) {
                        int bcid = m_fit_start_bcid[fec][hybrid][chip][ch]+m_most_common_BCID+n;
                        plot->addGraph();
                        std::vector<double> x;
                        std::vector<double> y;
                        for(int bit=0; bit<m_number_bits;bit++){

                            x.push_back(bit*3.125*TIME_FACTOR);
                            y.push_back(100*m_percent_bcid[n][bit][fec][hybrid][chip][ch]);
                        }

                        plot->graph(n)->setData(
                                    QVector<double>::fromStdVector(x),
                                    QVector<double>::fromStdVector(y));

                        if(bcid-m_most_common_BCID == -2) {
                            plot->graph(n)->setPen(QPen(Qt::darkCyan,2,Qt::SolidLine));
                        }
                        else if(bcid-m_most_common_BCID == -1) {
                            plot->graph(n)->setPen(QPen(Qt::red,2,Qt::SolidLine));

                        }
                        else if(bcid-m_most_common_BCID == 0) {
                            plot->graph(n)->setPen(QPen(Qt::darkGreen,2,Qt::SolidLine));

                        }
                        else if(bcid-m_most_common_BCID == 1) {
                            plot->graph(n)->setPen(QPen(Qt::blue,2,Qt::SolidLine));

                        }
                        else if(bcid-m_most_common_BCID == 2) {
                            plot->graph(n)->setPen(QPen(Qt::darkMagenta,2,Qt::SolidLine));

                        }
                        plot->graph(n)->setName(QString("BCID "+ QString::number(bcid)));
                    }
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                    plot->yAxis->rescale();

                }
            }
            //Online ADC or TDC or threshold
            else if(m_modeIndex == 3 || m_modeIndex == 4 || (m_modeIndex == 6 && m_isThresholdCalibration))
            {
                for(int bit=0; bit<m_number_bits;bit++){
                    plot->addGraph();
                    plot->graph(bit)->setData(
                                QVector<double>::fromStdVector(m_x),
                                QVector<double>::fromStdVector(m_mean[bit][fec][hybrid][chip]));
                    plot->graph(bit)->setPen(QPen(QColor(static_cast<unsigned int>(bit)*colorFactor)));
                    plot->legend->removeItem(plot->legend->itemCount()-1);
                }
                int n_graph =plot->graphCount();
                plot->addGraph();
                plot->graph(n_graph)->setPen(QPen(Qt::red,4,Qt::SolidLine));
                plot->graph(n_graph)->setName("Best common value");
                plot->graph(n_graph)->setData( QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_y[fec][hybrid][chip]));
                plot->addGraph();
                plot->graph(n_graph+1)->setData(QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_calVal[fec][hybrid][chip] ));
                plot->graph(n_graph+1)->setPen(QPen(Qt::green,4,Qt::SolidLine));
                plot->graph(n_graph+1)->setName("Calibrated curve");
                plot->yAxis->rescale();
            }
            //S-curve
            else if(m_modeIndex == 5)
            {
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
                plot->graph(0)->setData( QVector<double>::fromStdVector(x), QVector<double>::fromStdVector(y));
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
                errorBars->setData(QVector<double>::fromStdVector(errors));

                if(m_fit_scale[idx] != -1 && m_fit_mean[idx] != -1 && m_fit_sigma[idx] != -1) {
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
                    plot->graph(1)->setData( QVector<double>::fromStdVector(x), QVector<double>::fromStdVector(fit));
                    plot->graph(1)->setPen(QPen(Qt::red,2,Qt::SolidLine));
                    plot->graph(1)->setLineStyle(QCPGraph::lsLine);
                    plot->graph(1)->setName(QString("Fit\nScale ") + QString::number(m_fit_scale[idx],'f',2)  + " (+/- " + QString::number(m_fit_error_scale[idx],'f',5) + ")"
                                            + QString("\nMean ") + QString::number(m_fit_mean[idx],'f',2)  + " (+/- " + QString::number(m_fit_error_mean[idx],'f',5)  + ")"
                                            + QString("\nSigma ") + QString::number(m_fit_sigma[idx],'f',2)  + " (+/- " + QString::number(m_fit_error_sigma[idx],'f',5)  + ")");
                }
                plot->yAxis->rescale();
            }
            //Threshold measurement
            else if(m_modeIndex == 6 && !m_isThresholdCalibration)
            {
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                plot->addGraph();
                plot->graph(0)->setName(QString("Threshold"));
                plot->graph(0)->setData(
                            QVector<double>::fromStdVector(m_x),
                            QVector<double>::fromStdVector(m_mean[0][fec][hybrid][chip]));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
                plot->yAxis->rescale();
            }
            //Pedestal
            else if(m_modeIndex == 7)
            {
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                plot->addGraph();
                plot->graph(0)->setName(QString("Pedestal"));
                plot->graph(0)->setData(
                            QVector<double>::fromStdVector(m_x),
                            QVector<double>::fromStdVector(m_mean[0][fec][hybrid][chip]));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));

            }
            //Pulser DAC
            else if(m_modeIndex == 8)
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
                            QVector<double>::fromStdVector(m_dac_setting),
                            QVector<double>::fromStdVector(m_y[fec][hybrid][chip]));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
            }
            //Threshold DAC
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
                            QVector<double>::fromStdVector(m_dac_setting),
                            QVector<double>::fromStdVector(m_y[fec][hybrid][chip]));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
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
        if (m_mainWindow->m_daqs[0].GetFEC(fec) ){
            m_fecPosID[fec] = m_mainWindow->m_daqs[0].m_fecs[fec].GetID();
            m_fecIDPos[m_mainWindow->m_daqs[0].m_fecs[fec].GetID()] = fec;
            for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_FEC; hybrid++){
                if( m_mainWindow->m_daqs[0].m_fecs[fec].GetHybrid(hybrid) ){

                    for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                        if (m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetVMM(vmm) ){
                            m_vmmActs.push_back(fec*HYBRIDS_PER_FEC*VMMS_PER_HYBRID+
                                                hybrid*VMMS_PER_HYBRID +vmm);
                        }
                    }
                }

            }
        }
    }

    //();
}

// ------------------------------------------------------------------------ //
void CalibrationModule::updatePlot(){
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

// ------------------------------------------------------------------------ //
void CalibrationModule::StartCalibration(){
    m_numHits = 0;
    m_bitCount = -1;
    m_lastUdpTimeStamp=0;
    m_start = 0;
    m_end = 0;

    m_dataAvailable = false;
    emit m_mainWindow->m_daqWindow->ui->openConnection->clicked();
    QThread::usleep(1000);
    if(! (m_mainWindow->m_daqWindow->ui->connectionLabel->text()==QString("all alive"))) {
        std::cout<<"Communication couldn't be established! \n exit calibration"<<std::endl;
        m_mainWindow->m_daqWindow->ui->InfoScreen->setTextColor(Qt::red);
        m_mainWindow->m_daqWindow->ui->InfoScreen->append(QString("Communication couldn't be established! \n exit calibration"));
        m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
        return;
    }

    GetSettings();
    PlotData();

    if(IsCalibration())
    {

        if(m_modeIndex == 3 || m_modeIndex == 4 ||  m_modeIndex == 6) {
            m_mainWindow->m_daqWindow->ui->line_configFile->setText("Calib_config");
            SaveSettings();
        }
    }

    GetActiveVMMs();
    InitializeDataStructures();



    //Time calibration needs different scan length depending on bc_period, works only for 40 MHz
    if(m_modeIndex == 2) {

        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hybrid = GetHybrid(vmm);
            int bcclock = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetReg("CKBC");
            if(bcclock != 2) {
                int ret = QMessageBox::warning(nullptr, tr("Time calibration"),
                                               "For the time calibration, all hybrids have to use 40 MHz BC clock!\n",
                                               QMessageBox::Ok);
                return;
            }

            m_number_bits = m_number_bits_offline_time;
        }
    }
    //S-curve needs user input for parameters
    else if(m_modeIndex == 5)
    {
        bool ok;

        QStringList list = QInputDialog::getText(nullptr, tr("S-curve input parameters"),
                                                 tr("S-curve measurement on one VMM with user settings (gain, polarity, channel masking). On Windows, you can only measure around 8 channels, you have to mask the other channels.\n"
                                                    "Mandatory settings are: FEC=[1 to 8], VMM=[0 to 15]. The other settings are optionally, if they are not given the default 0 (automatically chosen) is used:\n"
                                                    "\tPULSE-HEIGHT=0 [0=automatically chosen (around 500 mV after per-amp), positive number=pulser DAC, -1=no test pulses (use noise)]\n"
                                                    "\tTHRESHOLD=0 [0=automatically chosen (around 500 mV), positive number=threshold DAC]\n"
                                                    "\tSCAN-WIDTH=0 [0=automatically chosen (60 steps with THRESHOLD in center), positive number=steps in scan interval (20-200)]\n"
                                                    "\tDIRECTION=0 [0=automatically chosen scan direction (up), 1=down]\n"
                                                    "\tSCAN-TYPE=0 [0=scan threshold, 1=scan pulse height]:"),
                                                 QLineEdit::Normal,"FEC=1,VMM=0,PULSE-HEIGHT=0,THRESHOLD=0,SCAN-WIDTH=0,DIRECTION=0,SCAN-TYPE=0",&ok).split(",");
        m_pulser_dac = 0;
        int scanWidth = 60;
        int threshold = 0;
        m_theFEC = -1;
        m_theVMM = -1;
        m_scan_type = 0;
        if(list.count() < 2) {
            int ret = QMessageBox::warning(nullptr, tr("S-curve input parameters"),
                                           "The FEC and the VMM have to be specified!\n",
                                           QMessageBox::Ok);
            return;
        }
        else {
            for(int n=0; n<list.count();n++) {
                QString str = list[n];
                QVector<QStringRef> words = str.splitRef('=');
                if(words.size() != 2) {
                    int ret = QMessageBox::warning(nullptr, tr("S-curve input parameters"),
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


        if(m_theFEC > 8 || m_theFEC < 1 || m_theVMM > 15 || m_theVMM < 0
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

        int fec = m_theFEC-1;
        int hybrid = m_theVMM / 2;
        int chip = m_theVMM % 2;


        int polarity = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("sp");
        int peaktime = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("peaktime");
        int tac = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("stc");
        int bcclock = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetReg("CKBC");
        int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("gain");

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

                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring","Pulser_DAC");
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp_2",m_pulser_dac);
                m_mainWindow->m_daqs[0].SendAll(false);
                QThread::usleep(10000);
                m_pulser_mV = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
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
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring","Threshold_DAC");
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt",m_threshold_dac);
            m_mainWindow->m_daqs[0].SendAll(false);
            QThread::usleep(10000);
            m_threshold_mV = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);

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
    else if(m_modeIndex == 6) {
        QMessageBox::StandardButton reply;

        reply = QMessageBox::question(nullptr, "Calibrate or measure threshold?", "Do you want to measure (no) or calibrate (yes)?", QMessageBox::Yes | QMessageBox::No );
        if(reply == QMessageBox::No) {
            m_isThresholdCalibration = false;
        }
        else {
            m_isThresholdCalibration = true;
        }
    }


    QThread::usleep(1000);

    if(IsCalibration())
    {

        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);

            for(int ch = 0; ch<64; ch++){
                //Offline ADC and time, online ADC and TDC calibration
                if(m_modeIndex <= 4)
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",1,ch);
                }

                //threshold measurement
                if(m_modeIndex == 6)
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm",0,ch);

                }
                //S-curve (one VMM)
                else if(m_modeIndex == 5)
                {
                    int theFec = m_theFEC-1;
                    int thehybrid = m_theVMM / 2;
                    int theChip = m_theVMM % 2;
                    if(fec == theFec && hybrid == thehybrid && theChip == chip)
                    {
                        if(m_pulser_dac == -1) {
                            m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                        }
                        else {
                            m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",1,ch);
                        }
                    }
                    else
                    {
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sm",1,ch);
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    }
                }
            }
        }
        //check config once
        m_mainWindow->m_daqs[0].SendAll(true);
    }
    if(m_modeIndex == 7)
    {
        MeasurePedestalOrThreshold(true, false);
    }
    else if(m_modeIndex == 6)
    {
        MeasurePedestalOrThreshold(false, m_isThresholdCalibration);
    }
    else if(m_modeIndex == 8)
    {
        MeasurePulserOrThresholdDAC(true);
    }
    else if(m_modeIndex == 9)
    {
        MeasurePulserOrThresholdDAC(false);
    }

    else {
        m_nodata_start = std::chrono::high_resolution_clock::now();
        DoCalibrationStep();
        QThread::usleep(100000);
        StartDataTaking();
    }
}

// ------------------------------------------------------------------------ //
void CalibrationModule::DoCalibrationStep(){
    m_bitCount++;
    //Offline or online ADC or time/TDC
    if(m_modeIndex <= 4) {

        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            m_srs_timestamp_start[fec][hybrid][chip]=0;
            m_srs_timestamp_end[fec][hybrid][chip]=0;
            if(m_modeIndex == 1)
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", 0);
                int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("gain");
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_thresholdTable[gain]);
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int step = (max - min)/(m_number_bits-1);
                std::string val = std::to_string(static_cast<int>(min+m_bitCount*step));
                if(vmm == 0) {
                    m_dac_setting.push_back(static_cast<int>(min+m_bitCount*step));
                }
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp_2",val);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring","Pulser_DAC");
                m_mainWindow->m_daqs[0].SendAll(false);
                QThread::usleep(10000);
                int measured = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                m_dac_measured[fec][hybrid][chip].push_back(measured);

            }
            else if(m_modeIndex == 2)
            {
                int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("gain");
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", m_thresholdTable[gain]);
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/2;
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp_2",center);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", m_bitCount*TIME_FACTOR);
            }
            else if(m_modeIndex == 3)
            {
                int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("gain");
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/(2);
                std::string val = std::to_string(static_cast<int>(center));
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp_2",val);
                for(int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "ADC0_10", m_bitCount,ch );
                }
            }
            else if(m_modeIndex == 4)
            {
                int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].GetRegister("gain");
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/(2);
                std::string val = std::to_string(static_cast<int>(center));
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp_2",val);
                for(int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "ADC0_8", m_bitCount,ch );
                }
            }
        }
    }
    //S-curve
    else if(m_modeIndex == 5)
    {

        if(m_theDirection == 0)
        {
            m_theIndex = m_bitCount;
        }
        else {
            m_theIndex = m_number_bits - 1 - m_bitCount;
        }


        int fec = m_theFEC-1;
        int hybrid = m_theVMM / 2;
        int chip = m_theVMM % 2;
        m_srs_timestamp_start[fec][hybrid][chip]=0;
        m_srs_timestamp_end[fec][hybrid][chip]=0;
        if(m_scan_type == 0) {
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring","Threshold_DAC");
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt", (int)m_dac_setting[m_theIndex]);
        }
        else {
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring","Pulser_DAC");
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp_2", (int)m_dac_setting[m_theIndex]);
        }
        m_mainWindow->m_daqs[0].SendAll(false);
        QThread::usleep(10000);
        int measured = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
        m_dac_measured[fec][hybrid][chip][m_theIndex] = measured;
    }
    m_mainWindow->m_daqs[0].SendAll(false);
    QThread::usleep(100000);
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
        if((IsCalibration() && m_modeIndex != 5 && nodata_duration >= 5000))
        {
            StopDataTaking();
            Reset();
            StartCalibration();
        }
    }
    bool continueCalibration = true;

    uint64_t delay_ns = m_end - m_start;
    //convert run value from ms to ns
    if(delay_ns >= static_cast<uint64_t>(m_mainWindow->m_daqWindow->ui->Runs->value())*1000000)
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
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
            m_dataAvailable = true;
            m_data_modeIndex = m_modeIndex;

            if(IsCalibration())
            {
                if(m_modeIndex == 3 || m_modeIndex == 4 ||  m_modeIndex == 6) {
                    LoadSettings();
                }
            }
            //Not for S-curve and time calibration
            if(m_modeIndex != 5 && m_modeIndex != 2) {
                for(int bit =0; bit<m_number_bits; bit++){
                    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
                        int fec = GetFEC(vmm);
                        int hybrid = GetHybrid(vmm);
                        int chip = GetVMM(vmm);
                        for(unsigned int ch = 0; ch<64; ch++){
                            unsigned long size = m_data[bit][fec][hybrid][chip][ch].size();
                            double norm = static_cast<double>(size);
                            double mean = -9999;
                            double median = -9999;
                            if(!IsCalibration())
                            {
                                mean = 0;
                            }

                            //Channels
                            if(m_modeIndex == 10)
                            {
                                norm = 1.0;
                            }
                            if(size>0)
                            {
                                mean = std::accumulate(m_data[bit][fec][hybrid][chip][ch].begin(),m_data[bit][fec][hybrid][chip][ch].end(),0.0)/norm;
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

                        }
                    }
                }
            }
            else if(m_modeIndex == 2) {
                //Determine most common BCID in measurements over all VMMs and channels
                long all_bcid_cnt = 0;
                int all_bcid_max = 0;
                int all_bcid_min = 0;
                m_most_common_BCID = 0;
                for(int i=0; i<4096; i++) {
                    long cnt = m_cnt_bcid[i];
                    if(cnt > all_bcid_cnt) {
                        m_most_common_BCID = i;
                        all_bcid_cnt = cnt;
                    }
                }
                //Accept in the subsequent search of the most common BCID per channels a range of +/- 2 BCIDs around the most common one
                if(m_most_common_BCID-(int)(NUM_BCID/2) >=0 ) {
                    all_bcid_min = m_most_common_BCID-(int)(NUM_BCID/2);
                }
                if(m_most_common_BCID+(int)(NUM_BCID/2) <= 4095 ) {
                    all_bcid_max = m_most_common_BCID+(int)(NUM_BCID/2);
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
                                m_mean_per_bcid[n][bit][fec][hybrid][chip].push_back(-9999);
                                m_percent_bcid[n][bit][fec][hybrid][chip].push_back(0);
                            }
                            m_mean[bit][fec][hybrid][chip].push_back(0);
                            m_fit_y[bit][fec][hybrid][chip].push_back(-9999);
                        }
                        m_fit_start_time[fec][hybrid][chip].push_back(-9999);
                        m_fit_start_bcid[fec][hybrid][chip].push_back(min-m_most_common_BCID);
                        if(max - min == NUM_BCID-1) {
                            //determine percentages for three BCIDs
                            for(int bit = 0; bit<m_number_bits; bit++){
                                int total = m_data[bit][fec][hybrid][chip][ch].size();
                                for(int bcid=min; bcid <= max; bcid++) {
                                    double meanTDC = -9999;
                                    double meanTime = -9999;
                                    auto it = std::partition(m_data[bit][fec][hybrid][chip][ch].begin(),m_data[bit][fec][hybrid][chip][ch].end(), [bcid](int val) { return (bcid*1000 <=val && bcid*1000+255 >= val); });
                                    long cnt = std::count_if(m_data[bit][fec][hybrid][chip][ch].begin(),it, [](int val) { return true; });
                                    if(cnt > 0) {
                                        long tdc = std::accumulate(m_data[bit][fec][hybrid][chip][ch].begin(),it,0);
                                        //remove the bcid from the combined data entry (bcid*1000+tdc)
                                        meanTDC = (tdc - bcid*1000*cnt);
                                        //calculate the mean tdc for the bit value and bcid
                                        meanTDC = (double)meanTDC/(double)cnt;
                                        //convert BCID and TDC to time
                                        meanTime = m_bc_period[fec][hybrid] - (meanTDC*m_tac_slope[fec][hybrid][chip])/255;
                                        m_percent_bcid[bcid-min][bit][fec][hybrid][chip][ch] =  (double)cnt/(double)total;
                                        m_mean[bit][fec][hybrid][chip][ch] +=  ((bcid-m_most_common_BCID)*m_bc_period[fec][hybrid] +  meanTime)*m_percent_bcid[bcid-min][bit][fec][hybrid][chip][ch];
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
                                            double theTime = bit*3.125*TIME_FACTOR + (0.5 - m_percent_bcid[bcid-min+1][bit][fec][hybrid][chip][ch])*3.125*TIME_FACTOR /(m_percent_bcid[bcid-min+1][bit+1][fec][hybrid][chip][ch] - m_percent_bcid[bcid-min+1][bit][fec][hybrid][chip][ch]);
                                            m_fit_start_time[fec][hybrid][chip][ch] = -((bcid+1-m_most_common_BCID)*m_bc_period[fec][hybrid]) + theTime;
                                            foundEdge = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if(foundEdge) {
                                //data from 50% point until end of bits
                                for(int n = theTimeIndex_50; n<m_number_bits; n++){
                                    m_fit_y[n-theTimeIndex_50][fec][hybrid][chip][ch] =  m_mean_per_bcid[theBCIDIndex_50][n][fec][hybrid][chip][ch];
                                }
                                //data from beginning to 50% point
                                for(int n = 2; n< 2+theTimeIndex_50; n++){
                                    m_fit_y[m_number_bits-theTimeIndex_50+n-2][fec][hybrid][chip][ch] = m_mean_per_bcid[theBCIDIndex_50-1][n][fec][hybrid][chip][ch];;
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
                        meanOffset += m_mean[0][fec][hybrid][chip][ch];
                        cnt++;
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
                            m_mean[bit][fec][hybrid][chip][ch] =  m_mean[bit][fec][hybrid][chip][ch] -  minMeanOffset;
                        }
                    }
                }
            }
            //Offline calibration ADC or Time
            if(m_modeIndex == 1 || m_modeIndex == 2)
            {
                FitOfflineCalibrationData();
            }
            //Online calibration ADC or TDC
            else if(m_modeIndex == 3 || m_modeIndex == 4)
            {
                CalculateCorrections();
            }
            //S-Curve
            else if(m_modeIndex == 5)
            {
                FitSCurve();
            }

            PlotData();
        }
    }
}

void CalibrationModule::Reset()
{
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
    QThread::usleep(1000);
    std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Hard Reset of VMMs!" << std::endl;
    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
        int fec = GetFEC(vmm);
        int hybrid = GetHybrid(vmm);
        int chip = GetVMM(vmm);
        //LoadSettings();
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("reset1", 1);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("reset2", 1);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->SendConfig(hybrid, chip);
        QThread::msleep(100);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("reset1", 0);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("reset2", 0);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->SendConfig(hybrid, chip);
        QThread::msleep(100);

        m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ResetFEC();
        QThread::msleep(100);
        m_mainWindow->m_daqs[0].SendAll(false);
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
            if(m_mean[0][fec][hybrid][chip][ch] > -1)
            {
                sorted_0mV.push_back(m_mean[0][fec][hybrid][chip][ch]);
            }
            if(m_mean[m_number_bits-1][fec][hybrid][chip][ch] > -1)
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
    if(m_modeIndex == 1 ||  m_modeIndex == 2)
    {
        if( ! m_calibrationArray[0] &&  !m_calibrationArray[1]) {
            return;
        }
        QString name = "vmm_calibration";
        int lastFEC = -1;
        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int fecId = m_fecPosID[fec];
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            if(lastFEC != fec+1) {
                name += "_FEC" + QString::number(fecId);
            }
            lastFEC = fec+1;
            name += "_VMM" + QString::number(hybrid*2+chip);
        }
        QString theName = CreateFileName(name);
        QFile jsonFile(theName +  ".json");
        jsonFile.open(QFile::WriteOnly);

        QJsonObject globalObject;
        QJsonArray calibrationArray;
        for(int vmm = 0; vmm <static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int fecId = m_fecPosID[fec];
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);

            QJsonObject calibrationObject;
            QJsonArray adcOffsetArray;
            QJsonArray adcSlopeArray;
            QJsonArray timeOffsetArray;
            QJsonArray timeSlopeArray;
            calibrationObject.insert("fecID",fecId);
            calibrationObject.insert("vmmID",hybrid*2+chip);
            int theVmmId = 0;
            int theFECId = 0;
            if(m_calibrationArray[0]) {
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
                        break;
                    }
                }
            }

            if(m_calibrationArray[1]) {
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
                        break;
                    }
                }
            }


            if(adcOffsetArray.empty()) {
                for(int i=0; i< 64; i++) {
                    adcOffsetArray.push_back(0);
                }
            }
            if(timeOffsetArray.empty()) {
                for(int i=0; i< 64; i++) {
                    timeOffsetArray.push_back(0);
                }
            }
            if(adcSlopeArray.empty()) {
                for(int i=0; i< 64; i++) {
                    adcSlopeArray.push_back(1);
                }
            }
            if(timeSlopeArray.empty()) {
                for(int i=0; i< 64; i++) {
                    timeSlopeArray.push_back(1);
                }
            }


            calibrationObject.insert("time_offsets",timeOffsetArray);
            calibrationObject.insert("time_slopes",timeSlopeArray);
            calibrationObject.insert("adc_offsets",adcOffsetArray);
            calibrationObject.insert("adc_slopes",adcSlopeArray);
            calibrationArray.push_back(calibrationObject);
        }

        globalObject.insert("vmm_calibration",calibrationArray);
        QJsonDocument document(globalObject);
        jsonFile.write(document.toJson(QJsonDocument::JsonFormat::Compact));
        jsonFile.close();

    }
    else
    {
        if(m_modeIndex == 3 || m_modeIndex == 4 ||  m_modeIndex == 6) {
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                for(int ch=0; ch<64; ch++)
                {
                    int fec = GetFEC(vmm);
                    int hybrid = GetHybrid(vmm);
                    int chip = GetVMM(vmm);

                    if(m_modeIndex == 3)
                    {
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "ADC0_10", m_bitVal[fec][hybrid][chip][static_cast<unsigned int>(ch)]  , ch );
                    }
                    else if(m_modeIndex == 4) {
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "ADC0_8", m_bitVal[fec][hybrid][chip][static_cast<unsigned int>(ch)]  , ch );
                    }
                    else if(m_modeIndex == 6) {
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi( "sd", m_bitVal[fec][hybrid][chip][static_cast<unsigned int>(ch)]  , ch );
                    }
                }
            }
            SaveSettings();
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
    QString qtext = m_mainWindow->m_daqWindow->ui->lineEditBCID->text();
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
    if(m_mainWindow->m_daqWindow->ui->comboBoxRunMode->currentIndex() == 0)
    {
        m_runMode = "Calibration";
    }
    else if(m_mainWindow->m_daqWindow->ui->comboBoxRunMode->currentIndex() == 1){
        m_runMode = "User";
    }
    QString calibMode = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentText();
    m_modeIndex = GetCalibrationModeIndex(calibMode);
    if(m_modeIndex == 1){
        m_number_bits = m_number_bits_offline_adc;
    }
    else if(m_modeIndex == 2){
        m_number_bits = m_number_bits_offline_time;
    }
    else if(m_modeIndex == 3){
        m_number_bits = m_number_bits_adc;
    }
    else if(m_modeIndex == 4){
        m_number_bits = m_number_bits_tdc;
    }
    else if(m_modeIndex == 6){
        m_number_bits = m_number_bits_threshold;
    }
    else {
        m_number_bits = 1;
    }

}

void CalibrationModule::InitializeDataStructures()
{
    m_theIndex = 0;
    m_pulser_dac = 0;
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
    m_most_common_BCID = 0;
    for(unsigned int bit = 0; bit < 32; bit++) {
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_FEC; hybrid++){
                if (m_mainWindow->m_daqs[0].GetFEC(fec) &&  m_mainWindow->m_daqs[0].m_fecs[fec].GetHybrid(hybrid)){
                    int ckbc = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetReg("CKBC");
                    double period_ns = 0;
                    double clock = 0;
                    switch (ckbc)
                    {
                    case 0: period_ns = 12.5;
                        clock = 80;
                        break;
                    case 1: period_ns = 12.5;
                        clock = 80;
                        break;
                    case 2: period_ns = 25.0;
                        clock = 40;
                        break;
                    case 3: period_ns = 50.0;
                        clock = 20;
                        break;
                    case 4: period_ns = 100.0;
                        clock = 10;
                        break;
                    case 5: period_ns = 200.0;
                        clock = 5;
                        break;
                    case 6: period_ns = 400.0;
                        clock = 2.5;
                        break;
                    case 7: period_ns = 800.0;
                        clock = 1.25;
                        break;
                    }
                    m_bc_period[fec][hybrid]= period_ns;
                    m_bc_clock[fec][hybrid]= clock;
                }
                for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){

                    if(bit == 0)
                    {
                        m_dac_measured[fec][hybrid][vmm].clear();

                        if (m_mainWindow->m_daqs[0].GetFEC(fec) &&  m_mainWindow->m_daqs[0].m_fecs[fec].GetHybrid(hybrid) && m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetVMM(vmm) ){
                            int stc = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("stc");
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
                        if (m_mainWindow->m_daqs[0].GetFEC(fec) &&  m_mainWindow->m_daqs[0].m_fecs[fec].GetHybrid(hybrid) && m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetVMM(vmm) ){
                            int st = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("peaktime");
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
                        if (m_mainWindow->m_daqs[0].GetFEC(fec) &&  m_mainWindow->m_daqs[0].m_fecs[fec].GetHybrid(hybrid) && m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetVMM(vmm) ){
                            int sg = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("gain");
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

                        if (m_mainWindow->m_daqs[0].GetFEC(fec) &&  m_mainWindow->m_daqs[0].m_fecs[fec].GetHybrid(hybrid) && m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetVMM(vmm) ){
                            bool polarity = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("sp");
                            if(polarity) {
                                m_polarity[fec][hybrid][vmm] = 1;
                            }
                            else {
                                m_polarity[fec][hybrid][vmm] = 0;
                            }

                        }
                        if (m_mainWindow->m_daqs[0].GetFEC(fec) &&  m_mainWindow->m_daqs[0].m_fecs[fec].GetHybrid(hybrid) && m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].GetVMM(vmm) ){
                            bool timing = m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].GetRegister("srat");
                            if(timing) {
                                m_timing_at_thr[fec][hybrid][vmm] = 1;
                            }
                            else {
                                m_timing_at_thr[fec][hybrid][vmm] = 0;
                            }

                        }


                        m_offset[fec][hybrid][vmm].clear();
                        m_slope[fec][hybrid][vmm].clear();
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
                    m_fit_y[bit][fec][hybrid][vmm].clear();
                    m_allhitdata[bit][fec][hybrid][vmm].clear();
                    for(int n=0; n<NUM_BCID; n++) {
                        m_mean_per_bcid[n][bit][fec][hybrid][vmm].clear();
                        m_percent_bcid[n][bit][fec][hybrid][vmm].clear();
                    }

                    for (unsigned int ch=0; ch < 64; ch++){
                        m_data[bit][fec][hybrid][vmm][ch].clear();
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

    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(true);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(true);

    for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
        int fec = GetFEC(vmm);
        int hybrid = GetHybrid(vmm);
        int chip = GetVMM(vmm);

        for(int n=0; n<M; n++) {
            if(measurePulser) {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring","Pulser_DAC");
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdp_2",(int)m_dac_setting[n]);
            }
            else {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring", "Threshold_DAC");
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sdt",(int)m_dac_setting[n]);
            }
            m_mainWindow->m_daqs[0].SendAll(false);
            QThread::usleep(10000);
            int val = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);

            m_y[fec][hybrid][chip].push_back(val);
            std::cout << m_dac_setting[n] << ", " << val << std::endl;
        }
    }

    m_dataAvailable = true;
    m_data_modeIndex = m_modeIndex;
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
    PlotData();
}


void CalibrationModule::MeasurePedestalOrThreshold(bool isPedestal, bool isThresholdCalibration)
{
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(true);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(true);

    stringstream sx;
    if(isPedestal) {
        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hybrid = GetHybrid(vmm);
            int chip = GetVMM(vmm);
            for(int ch = 0; ch<64; ch++){
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring",std::to_string(ch));
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("smx",0,ch);
                m_mainWindow->m_daqs[0].SendAll(false);
                QThread::usleep(10000);
                int val = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                m_mean[0][fec][hybrid][chip].push_back(val);
            }
        }
    }
    else {
        if(isThresholdCalibration) {
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                int fec = GetFEC(vmm);
                int hybrid = GetHybrid(vmm);
                int chip = GetVMM(vmm);

                for(int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring",std::to_string(ch));
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("smx",1,ch);
                    for(int bit=0; bit<m_number_bits;bit++){
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("sd",bit,ch);
                        m_mainWindow->m_daqs[0].SendAll(false);
                        QThread::usleep(1000);
                        int val = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hybrid,chip, 2);
                        m_mean[bit][fec][hybrid][chip].push_back(val);
                    }
                }

            }
        }
        else {
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                int fec = GetFEC(vmm);
                int hybrid = GetHybrid(vmm);
                int chip = GetVMM(vmm);

                for(int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("monitoring",std::to_string(ch));
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("st",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hybrids[hybrid].m_vmms[chip].SetRegi("smx",1,ch);
                    m_mainWindow->m_daqs[0].SendAll(false);
                    QThread::usleep(10000);
                    int val = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hybrid, chip, 2);
                    m_mean[0][fec][hybrid][chip].push_back(val);
                }
            }
        }
    }
    m_dataAvailable = true;
    m_data_modeIndex = m_modeIndex;
    if(!isPedestal) {
        if(isThresholdCalibration) {
            CalculateCorrections();
        }
    }
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
    PlotData();
}



// ------------------------------------------------------------------------ //
void CalibrationModule::Receive(const char* buffer, long size, int fecId)
{
    stringstream sx;
    m_lastUdpTimeStamp = m_commonData.m_udpTimeStamp;
    m_numHitsInFrame=0;
    m_numHitsInFrame = Receive_VMM3(buffer, size, fecId);

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

int CalibrationModule::Parse_VMM3(uint32_t data1, uint16_t data2, int fecId) {
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
            GetMessageHandler()(sx,"calibration_module::Parse_VMM3"); sx.str("");
        }

        auto it = find (m_BCID.begin(), m_BCID.end(), bcid);
        if ((m_BCID.empty() || it != m_BCID.end()))
        {
            int hybrid = vmmid/2;
            int chip = vmmid%2;

            if(m_srs_timestamp_start[fec][hybrid][chip] > 0)
            {
                uint64_t delay = m_srs_timestamp_end[fec][hybrid][chip] - m_srs_timestamp_start[fec][hybrid][chip];
                if(delay <= static_cast<uint64_t>(m_mainWindow->m_daqWindow->ui->Runs->value())*1000000)
                {
                    //ADC calibrations or user settings average ADC
                    if(m_modeIndex == 1 || m_modeIndex == 3  || m_modeIndex == 11)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(adc);
                    }
                    //Time calibration
                    else if(m_modeIndex == 2 )
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(bcid*1000+tdc);
                        m_cnt_bcid[bcid]++;
                    }
                    //TDC calibration or user settings average TDC per channel
                    else if(m_modeIndex == 4|| m_modeIndex == 12)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(tdc);
                    }
                    //S-curve
                    else if(m_modeIndex == 5)
                    {
                        m_channel_y[chNo][m_theIndex]++;
                    }
                    //User settings average bcid per channel
                    else if(m_modeIndex == 13)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(bcid);
                    }
                    //User settings counts per channel
                    else if(m_modeIndex == 10)
                    {
                        m_data[m_bitCount][fec][hybrid][chip][chNo].push_back(1);
                    }
                    QVector<double> hit;
                    hit.push_back(chNo);
                    hit.push_back(bcid);
                    hit.push_back(tdc);
                    hit.push_back(adc);
                    hit.push_back(double(overThreshold));
                    if(m_bitCount < MAX_BITS) {
                        m_allhitdata[m_bitCount][fec][hybrid][chip].push_back(hit);
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
        uint64_t delay = m_srs_timestamp_end[fec][hybrid][chip] - m_srs_timestamp_start[fec][hybrid][chip];
        if(delay <= static_cast<uint64_t>(m_mainWindow->m_daqWindow->ui->Runs->value())*1000000)
        {
            //first timestamp/marker for this VMM
            if(m_srs_timestamp_start[fec][hybrid][chip] == 0)
            {
                //42 bit timestamp, giving number of 40 MHz clock cycles
                m_srs_timestamp_start[fec][hybrid][chip] = timestamp_42bit*25;
                m_start = timestamp_42bit*25;
                m_numHits=0;
            }
            m_srs_timestamp_end[fec][hybrid][chip] = timestamp_42bit*25;
            m_end = timestamp_42bit*25;
        }

        if(IsDbgActive()) {
            sx.str("");
            sx << "SRS Marker fecId " << static_cast<int>(m_commonData.m_fecId) << ", vmmId " << vmmid << ", timestamp lower 10bit: " <<  timestamp_lower_10bit
               << ", timestamp upper 32bit: " <<  timestamp_upper_32bit
               << ", timestamp 42bit: " <<  timestamp_42bit << "\n";
            GetMessageHandler()(sx,"calibration_module::Parse_VMM3"); sx.str("");
        }
        return 0;
    }
}

int CalibrationModule::Receive_VMM3(const char *buffer, long size, int fecId) {
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
        int res = Parse_VMM3(data1, data2, fecId );
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

uint32_t CalibrationModule::Reversebits32(uint32_t x) {
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return ((x >> 16) | (x << 16));
}

uint16_t CalibrationModule::Reversebits16(uint16_t x) {
    uint32_t temp = Reversebits32(x);
    return (temp >> 16);
}

uint32_t CalibrationModule::Gray2bin32(uint32_t num) {
    num = num ^ (num >> 16);
    num = num ^ (num >> 8);
    num = num ^ (num >> 4);
    num = num ^ (num >> 2);
    num = num ^ (num >> 1);
    return num;
}

QString CalibrationModule::CreateFileName(QString name, int polarity, double gain, double peaktime, double tac, double bcclock, int srat)
{
    QString theName = m_mainWindow->GetApplicationPath() + "/" + name;
    QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    QString str;
    if(polarity == 0)
    {
        str = QString("%1").arg("negative");
        theName = theName + "_" + str;
    }
    else if (polarity == 1) {
        str = QString("%1").arg("positive");
        theName = theName + "_" + str;

    }

    if(gain != -1)
    {
        str = QString("%1%2").arg(gain).arg("mVfC");
        theName = theName + "_" + str;
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
    if(bcclock != -1)
    {
        str = QString("%1%2").arg(bcclock).arg("MHz");
        theName = theName + "_" + str;
    }
    if(srat == 0)
    {
        str = QString("%1").arg("timing_peak");
        theName = theName + "_" + str;
    }
    else if(srat == 1) {
        str = QString("%1").arg("timing_threshold");
        theName = theName + "_" + str;
    }

    theName = theName + "_" + theDate;
    return theName;
}
