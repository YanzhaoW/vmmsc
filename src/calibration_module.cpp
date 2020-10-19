#include <cmath>
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
    for(int n =0; n<maxModes; n++)
    {
        m_isCalibrated[n] = false;
    }
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
    connect( m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType, SIGNAL(currentIndexChanged(int)),
            this, SLOT(setPlotChoice()));
}

void CalibrationModule::SaveSettings() {
    m_mainWindow->m_daqs[0].SendAll();
    m_mainWindow->m_daqWindow->ui->line_configFile->setText("Calib_config");
    emit m_mainWindow->m_daqWindow->on_Button_save_clicked();
    QThread::usleep(1000);
    m_mainWindow->m_daqWindow->ui->line_configFile->setText("");
    QThread::sleep(1);
    m_mainWindow->m_daqWindow->LoadConfig("Calib_config");
}

void CalibrationModule::LoadSettings() {
    m_mainWindow->m_daqWindow->LoadConfig("Calib_config");
}

void CalibrationModule::StopDataTaking()
{
    m_start = 0;
    m_end = 0;
    m_mainWindow->m_daqWindow->ui->offACQ->setCheckable(true);
    m_mainWindow->m_daqWindow->ui->offACQ->setChecked(true);
    m_mainWindow->m_daqWindow->ui->onACQ->setChecked(false);
    m_mainWindow->m_daqWindow->ui->Send->setEnabled(true);
    m_mainWindow->m_daqs[0].ACQHandler(false);
    QThread::usleep(1000);
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
    if(!m_dataAvailable)
    {
        return false;
    }
    if(m_mainWindow->m_daqWindow->ui->comboBoxFec->currentIndex() == -1)
    {
        return false;
    }
    if(IsCalibration())
    {
        return m_isCalibrated[m_modeIndex];
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


    double* baseLine = new double[m_number_bits];

    for(int n=0; n< m_number_bits;n++)
    {
        baseLine[n] = 0;
    }
    int cnt = 0;

    for(int bit =0; bit<m_number_bits; bit++){
        int numVMMwithData = 0;
        for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);

            double baseLineChip = 0;
            long size = std::count_if(m_mean[bit][fec][hdmi][0][chip].begin(), m_mean[bit][fec][hdmi][0][chip].end(), [](double i){return ((i > -1.0));});
            if(size>0)
            {
                std::for_each(m_mean[bit][fec][hdmi][0][chip].begin(),m_mean[bit][fec][hdmi][0][chip].end(), [&](double x){if (x > -1.0) baseLineChip += x;});
                numVMMwithData++;
                baseLineChip = baseLineChip/static_cast<double>(size);
                baseLine[bit] += baseLineChip;
            }

        }
        baseLine[bit] = baseLine[bit]/numVMMwithData;
        std::cout << "Baseline " << bit << " " << baseLine[bit] << std::endl;
    }

    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
        int fec = GetFEC(vmm);
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);


        double slope=0.0;
        double offset=-1.0;

        for(unsigned int ch = 0; ch<64; ch++){
            std::vector<double> x;
            std::vector<double> y;
            slope=0.0;
            offset=-1.0;
            double sumX=0, sumY=0, sumXY=0, sumX2=0;
            for(int bit =0; bit<m_number_bits; bit++){
                //Offline Time
                if(m_modeIndex == 2)
                {
                    m_mean[bit][fec][hdmi][0][chip][ch] =  m_mean[bit][fec][hdmi][0][chip][ch] - baseLine[0];
                }
                double theMean = m_mean[bit][fec][hdmi][0][chip][ch];

                double theXValue = 0;
                //Offline ADC
                if(m_modeIndex == 1)
                {
                    theXValue = baseLine[bit];
                }
                //Offline Time
                else if(m_modeIndex == 2)
                {
                    theXValue = bit*3.125*TIME_FACTOR;
                }

                x.push_back(theXValue);
                y.push_back(theMean);
                sumX += theXValue;
                sumY += theMean;
                sumXY += theXValue * theMean;
                sumX2 += theXValue * theXValue;
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
                if(slope != 0 && offset != -1)
                {
                    slope = 1/slope;
                    slope = std::round(1000*slope)/1000;
                    meanOffset+=offset;
                    cnt++;
                }
            }
            m_slope[fec][hdmi][0][chip].push_back(slope);
            m_offset[fec][hdmi][0][chip].push_back(offset);
        }
    }
    if(cnt > 0)
    {
        meanOffset = meanOffset/cnt;
        for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int fecId = m_mainWindow->m_daqs[0].m_fecs[fec].GetIP_id();
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            QJsonObject calibrationObject;
            QJsonArray offsetArray;
            QJsonArray slopeArray;
            for(unsigned int ch = 0; ch<64; ch++){
                double slope = m_slope[fec][hdmi][0][chip][ch];
                double offset = m_offset[fec][hdmi][0][chip][ch];
                if(slope == 0.0 && offset == -1.0)
                {
                    offset = meanOffset;
                }
                else
                {
                    offset = offset - meanOffset;
                }
                offset = std::round(1000*offset)/1000;
                slopeArray.push_back(slope);
                offsetArray.push_back(offset);
            }
            calibrationObject.insert("fecID",fecId);
            calibrationObject.insert("vmmID",hdmi*2+chip);
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

    delete[] baseLine;
}

void CalibrationModule::SavePlotsAsPDF(){
    int gain = 0;
    int polarity = 0;
    int peaktime = 0;
    int tac = 0;
    int bcclock = 0;
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
        int hdmi = 0;
        int chip = 0;

        if(m_modeIndex == 5) {
            fec = m_theFEC-1;
            hdmi = m_theVMM / 2;
            chip = m_theVMM % 2;
        }
        else {
            fec = GetFEC(idx);
            hdmi = GetHDMI(idx);
            chip = GetVMM(idx);
        }
        if(idx==start)
        {

            gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("gain");
            polarity = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("sp");
            peaktime = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("peaktime");
            tac = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("stc");
            bcclock = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].GetReg("CKBC");
        }
        QCustomPlot *plot = plotVector[idx%8];
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
            name += "_VMM" + QString::number(hdmi*2+chip);
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
        int hdmi = 0;
        int chip = 0;
        if(m_modeIndex != 5) {
            fec = GetFEC(static_cast<int>(idx));
            hdmi = GetHDMI(static_cast<int>(idx));
            chip = GetVMM(static_cast<int>(idx));
            title = "FEC " + QString::number(fec+1) + " VMM " + QString::number(hdmi*2+chip);
        }
        else {
            fec = m_theFEC-1;
            hdmi = m_theVMM / 2;
            chip = m_theVMM % 2;
            title = "FEC " + QString::number(fec+1) + " VMM " + QString::number(hdmi*2+chip) + " CH " + QString::number(idx);
        }
        unsigned int colorFactor = 0;

        if(m_modeIndex == 1) {
            title+= ": Mean ADC";
            colorFactor = 10;
            plot->yAxis->setRange(0.0, 1023.0);
        }
        else if(m_modeIndex == 2) {
            title+= ": Mean time";
            colorFactor = 10;
            plot->yAxis->setRange(-10.0, 60.0);
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
            title+= ": Rate [Hz]";
            plot->xAxis->setLabel("threshold [mV]");
            //plot->yAxis->setRange(0.0, m_rateLimit);
            plot->xAxis->setRange(ThresholdDAC_to_mV(m_minThreshold), ThresholdDAC_to_mV(m_maxThreshold));
        }
        else if(m_modeIndex == 6) {
            title+= ": Threshold [mV]";
            colorFactor = 5;
            plot->yAxis->setRange(0.0, 300.0);
        }
        else if(m_modeIndex == 7)
        {
            title+= ": Pedestal [mV]";
            colorFactor = 30;
            plot->yAxis->setRange(0.0, 350.0);
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
            plot->graph(0)->setData( QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_mean[0][fec][hdmi][0][chip]));
            plot->yAxis->setLabel(title);
            plot->legend->setVisible(true);
            plot->legend->setFont(QFont("Helvetica",9));
            plot->graph(0)->setName(title);
            plot->yAxis->rescale();
        }
        else
        {
            //Offline ADC or time
            if(m_modeIndex == 1 || m_modeIndex == 2)
            {
                for(int bit=0; bit<m_number_bits;bit++){
                    plot->addGraph();
                    plot->graph(2*bit)->setData(
                        QVector<double>::fromStdVector(m_x),
                        QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                    plot->graph(2*bit)->setPen(QPen(QColor(static_cast<unsigned int>(bit)*colorFactor)));
                    plot->legend->removeItem(plot->legend->itemCount()-1);
                    std::vector<double> y;
                    for(unsigned int ch=0; ch<64; ch++)
                    {
                        y.push_back((m_mean[bit][fec][hdmi][0][chip][ch] - m_offset[fec][hdmi][0][chip][ch])*  m_slope[fec][hdmi][0][chip][ch]);
                    }
                    plot->addGraph();
                    plot->graph(bit*2+1)->setData(
                        QVector<double>::fromStdVector(m_x),
                        QVector<double>::fromStdVector(y));
                    plot->graph(bit*2+1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                    plot->legend->removeItem(plot->legend->itemCount()-1);
                }
                plot->legend->setVisible(false);
            }
            //Online ADC or TDC or threshold
            else if(m_modeIndex == 3 || m_modeIndex == 4 || (m_modeIndex == 6 && m_isThresholdCalibration))
            {
                for(int bit=0; bit<m_number_bits;bit++){
                    plot->addGraph();
                    plot->graph(bit)->setData(
                        QVector<double>::fromStdVector(m_x),
                        QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                    plot->graph(bit)->setPen(QPen(QColor(static_cast<unsigned int>(bit)*colorFactor)));
                    plot->legend->removeItem(plot->legend->itemCount()-1);
                }
                int n_graph =plot->graphCount();
                plot->addGraph();
                plot->graph(n_graph)->setPen(QPen(Qt::red,4,Qt::SolidLine));
                plot->graph(n_graph)->setName("Best common value");
                plot->graph(n_graph)->setData( QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_y[fec][hdmi][0][chip]));
                plot->addGraph();
                plot->graph(n_graph+1)->setData(QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_calVal[fec][hdmi][0][chip] ));
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
                plot->graph(0)->setData( QVector<double>::fromStdVector(m_dac_x), QVector<double>::fromStdVector(m_channel_y[idx]));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
                plot->graph(0)->setLineStyle(QCPGraph::lsLine);
                plot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc, 3));
                plot->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
                double max = *max_element (m_channel_y[idx].begin(), m_channel_y[idx].end());
                m_max_value_x[static_cast<unsigned long>(idx)] = 0.0;
                m_min_value_x[static_cast<unsigned long>(idx)] = -1.0;
                for(unsigned long i = 0; i < m_channel_y[idx].size(); i++){
                    if(m_channel_y[idx][i] == max) {
                        m_max_value_x[idx] = ThresholdDAC_to_mV(m_minThreshold+static_cast<int>(i));
                    }
                    if(m_channel_y[idx][i] == 0.0 && m_min_value_x[idx]  == -1.0 && i > 0) {
                        m_min_value_x[idx]  = ThresholdDAC_to_mV(m_minThreshold+static_cast<int>(i));
                        std::cout << i << " " << idx << " " << m_min_value_x[idx]<< std::endl;
                    }
                }
                double width = m_min_value_x[idx] -  m_max_value_x[idx];
                plot->graph(0)->setName(QString("S-curve (edge width: ") + QString::number(width)  + " mV)");
                QCPItemLine * m_lineMin = new QCPItemLine(plot);//Vertical line
                m_lineMin->setLayer("overlay");
                m_lineMin->setPen(QPen(Qt::red,2,Qt::SolidLine));
                m_lineMin->setClipToAxisRect(true);
                m_lineMin->start->setCoords(m_min_value_x[idx], 0);
                m_lineMin->end->setCoords(m_min_value_x[idx], 0.2*max);
                QCPItemLine * m_lineMax = new QCPItemLine(plot);//Vertical line
                m_lineMax->setLayer("overlay");
                m_lineMax->setPen(QPen(Qt::red,2,Qt::SolidLine));
                m_lineMax->setClipToAxisRect(true);
                m_lineMax->start->setCoords(m_max_value_x[idx], 0);
                m_lineMax->end->setCoords(m_max_value_x[idx], 0.2*max);
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
                    QVector<double>::fromStdVector(m_mean[0][fec][hdmi][0][chip]));
                plot->graph(0)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
                plot->yAxis->rescale();
            }
            //Pedestal
            else if(m_modeIndex == 7)
            {
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                for(int bit=0; bit<m_number_bits;bit++){
                    plot->addGraph();
                    plot->graph(bit)->setName(QString("Pedestal"));
                    plot->graph(bit)->setData(
                        QVector<double>::fromStdVector(m_x),
                        QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                    plot->graph(bit)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
                }
            }

        }
        plot->replot();
    }
}

void CalibrationModule::setPlotChoice(){
    m_mainWindow->m_daqWindow->ui->comboBoxFec->clear();
    m_mainWindow->m_daqWindow->ui->label_vmm->clear();
    QString calibMode = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentText();
    m_modeIndex = GetCalibrationModeIndex(calibMode);

    //All except S-curve
    if(m_modeIndex != 5) {
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
    else {
        //S-curve
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


}

// ------------------------------------------------------------------------ //
void CalibrationModule::GetActiveVMMs(){
    m_vmmActs.clear();

    for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
        if (m_mainWindow->m_daqs[0].GetFEC(fec) ){
            for (unsigned short hdmi=0; hdmi < HDMIS_PER_FEC; hdmi++){
                if( m_mainWindow->m_daqs[0].m_fecs[fec].GetHDMI(hdmi) ){
                    for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_HDMI; hybrid++){
                        if (m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].GetHybrid(hybrid)){
                            for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                                if (m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].GetVMM(vmm) ){
                                    m_vmmActs.push_back(fec*HDMIS_PER_FEC*HYBRIDS_PER_HDMI*VMMS_PER_HYBRID+
                                                        hdmi*HYBRIDS_PER_HDMI*VMMS_PER_HYBRID +
                                                        hybrid*VMMS_PER_HYBRID
                                                        +vmm);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    setPlotChoice();
}

// ------------------------------------------------------------------------ //
void CalibrationModule::updatePlot(){
    GetSettings();
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
    for(int n =0; n<maxModes; n++)
    {
        m_isCalibrated[n] = false;
    }
    m_dataAvailable = false;
    emit m_mainWindow->m_daqWindow->ui->openConnection_2->clicked();
    QThread::usleep(1000);
    if(! (m_mainWindow->m_daqWindow->ui->connectionLabel_2->text()==QString("all alive"))) {
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
        m_mainWindow->m_daqWindow->ui->line_configFile->setText("Calib_config");
        SaveSettings();
    }

    GetActiveVMMs();
    InitializeDataStructures();

    //S-curve needs user input for parameters
    if(m_modeIndex == 5)
    {
        bool ok;
        QStringList list = QInputDialog::getText(nullptr, tr("S-curve input parameters"),
                                                 tr("Measurement with user settings (gain, polarity, test pulse height). The threshold is changed automatically.\nSet FEC(1-8), VMM(0-15), user sets test pulse height (0=auto, 1=user), scan width [threshold DAC], scan direction (0=up, 1=down):"),
                                                 QLineEdit::Normal,"1,0,0,60,0",&ok).split(",");
        int pulser_user = 0;
        int spanWidth = 0;
        if(list.count() != 5) {
            return;
        }
        else {
            m_theFEC = list[0].toInt();
            m_theVMM = list[1].toInt();
            pulser_user = list[2].toInt();
            spanWidth = list[3].toInt();
            m_theDirection = list[4].toInt();
        }
        if(m_theFEC > 8 || m_theFEC < 1 || m_theVMM > 15 || m_theVMM < 0
            || m_theDirection < 0 || m_theDirection > 1
            || pulser_user > 1 || pulser_user < 0
            || spanWidth < 20 || spanWidth > 200) {
            return;
        }

        int fec = m_theFEC-1;
        int hdmi = m_theVMM / 2;
        int chip = m_theVMM % 2;


        int polarity = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("sp");
        int peaktime = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("peaktime");
        int tac = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("stc");
        int bcclock = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].GetReg("CKBC");
        int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("gain");

        if(pulser_user == 0) {
            m_pulser_dac = static_cast<int>(std::round(0.5*(m_minPulseHeightTable[gain] +  m_maxPulseHeightTable[gain])));
        }
        else {
            m_pulser_dac = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("sdp_2");
        }
        double pulser_mV = PulserDAC_to_PulseHeight_mV(m_pulser_dac, gain);

        int threshold_DAC = Threshold_mV_to_DAC(pulser_mV);
        m_minThreshold = int(threshold_DAC-0.5*spanWidth);
        m_maxThreshold = int(threshold_DAC+0.5*spanWidth);
        if(m_theDirection == 0)
        {
            m_threshold = m_minThreshold;
        }
        else {
            m_threshold = m_maxThreshold;
        }

        for(int z=0; z < m_maxThreshold-m_minThreshold+1; z++)
        {
            m_dac_x.push_back(ThresholdDAC_to_mV(m_minThreshold+z));
            for(unsigned int ch = 0; ch<64; ch++){
                m_channel_y[ch].push_back(0);
                m_max_value_x.push_back(m_maxThreshold);
                m_min_value_x.push_back(m_minThreshold);
            }
        }

        QString name = CreateFileName("S-curve",polarity,gain,peaktime,tac, bcclock);
        name += ".txt";
        if(m_outFile.is_open())
        {
            m_outFile.close();
        }
        m_outFile.open(name.toStdString().c_str(), std::ofstream::out);
        m_outFile << "acq_time,fec,hdmi,chip,ch,gain,pulser_dac,pulse_mV,threshold_dac,threshold_mV,n,rate" << std::endl;
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



    m_mainWindow->m_daqWindow->ui->checkBoxGlobalDAQ->setChecked(true);
    emit m_mainWindow->m_daqWindow->ui->checkBoxGlobalDAQ->stateChanged(true);
    emit m_mainWindow->m_daqWindow->ui->trgPulser->clicked();
    QThread::usleep(1000);
    //m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(true);
    //m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(true);

    if(IsCalibration())
    {
        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);

            //S-curve (one VMM)
            if(m_modeIndex == 5) {
                int theFec = m_theFEC-1;
                int theHdmi = m_theVMM / 2;
                int theChip = m_theVMM % 2;
                if(fec == theFec && hdmi == theHdmi && theChip == chip)
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdp_2", m_pulser_dac);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdt", m_threshold);
                }
            }
            for(int ch = 0; ch<64; ch++){
                //Offline ADC and time, online ADC and TDC calibration
                if(m_modeIndex <= 4)
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",1,ch);
                }

                //threshold measurement
                if(m_modeIndex == 6)
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",0,ch);

                }
                //S-curve (one VMM)
                else if(m_modeIndex == 5)
                {
                    int theFec = m_theFEC-1;
                    int theHdmi = m_theVMM / 2;
                    int theChip = m_theVMM % 2;
                    if(fec == theFec && hdmi == theHdmi && theChip == chip)
                    {
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",0,ch);
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",1,ch);
                    }
                    else
                    {
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",1,ch);
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
                    }
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sd",0,ch);
                }
            }
        }

        m_mainWindow->m_daqs[0].SendAll();
    }
    if(m_modeIndex == 7)
    {
        MeasurePedestalOrThreshold(true, false);
    }
    else if(m_modeIndex == 6)
    {
        MeasurePedestalOrThreshold(false, m_isThresholdCalibration);
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
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            m_srs_timestamp_start[fec][hdmi][0][chip]=0;
            m_srs_timestamp_end[fec][hdmi][0][chip]=0;
            if(m_modeIndex == 1)
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].SetReg("TP_skew", 0);
                int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("gain");
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdt", m_thresholdTable[gain]);
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int step = (max - min)/(m_number_bits-1);
                std::string val = std::to_string(static_cast<int>(min+m_bitCount*step));
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdp_2",val);
            }
            else if(m_modeIndex == 2)
            {
                int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("gain");
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdt", m_thresholdTable[gain]);
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/2;
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdp_2",center);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].SetReg("TP_skew", m_bitCount*TIME_FACTOR);
            }
            else if(m_modeIndex == 3)
            {
                int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("gain");
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/(2);
                std::string val = std::to_string(static_cast<int>(center));
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdp_2",val);
                for(int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_10", m_bitCount,ch );
                }
            }
            else if(m_modeIndex == 4)
            {
                int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("gain");
                int min = m_minPulseHeightTable[gain];
                int max =  m_maxPulseHeightTable[gain];
                int center = (max + min)/(2);
                std::string val = std::to_string(static_cast<int>(center));
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdp_2",val);
                for(int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_8", m_bitCount,ch );
                }
            }
        }
    }
    //S-curve
    else if(m_modeIndex == 5)
    {
        m_bitCount = 0;
        if(m_theDirection == 0)
        {
            m_threshold++;
        }
        else {
            m_threshold--;
        }

        int fec = m_theFEC-1;
        int hdmi = m_theVMM / 2;
        int chip = m_theVMM % 2;
        m_srs_timestamp_start[fec][hdmi][0][chip]=0;
        m_srs_timestamp_end[fec][hdmi][0][chip]=0;
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdt", m_threshold);

    }
    m_mainWindow->m_daqs[0].SendAll();
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
        double delay_ms = delay_ns*0.000001;
        StopDataTaking();

        //S-curve
        if(m_modeIndex == 5)
        {
            std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Finished S-curve acquisition step "
                      <<  m_threshold << " after " << delay_ms<< " ms!" << std::endl;

            int fec = m_theFEC-1;
            int hdmi = m_theVMM / 2;
            int chip = m_theVMM % 2;

            int gain = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("gain");
            double pulseHeight = PulserDAC_to_PulseHeight_mV(m_pulser_dac, gain);
            double threshold_mV = ThresholdDAC_to_mV(m_threshold) ;

            for(unsigned int ch = 0; ch<64; ch++){
                double cnt = 0;
                double rate = 0;

                if(!m_data[m_bitCount][fec][hdmi][0][chip][ch].empty())
                {
                    cnt = m_data[m_bitCount][fec][hdmi][0][chip][ch][0];
                    if(delay_s > 0) {
                        rate = cnt/delay_s;
                    }
                }
                m_channel_y[ch][static_cast<unsigned long>(m_threshold-m_minThreshold)] = rate;
                m_data[m_bitCount][fec][hdmi][0][chip][ch].clear();

                m_outFile << delay_ns  << "," << fec << "," << hdmi <<"," << chip << "," << ch << ","
                          << m_gainTable[gain] << "," << m_pulser_dac << "," << pulseHeight << ","
                          << m_threshold << "," << threshold_mV << ","
                          << cnt << "," << std::setprecision(6) << rate<< std::endl;

            }
            if((m_theDirection == 0 && m_threshold == m_maxThreshold) || (m_theDirection == 1 && m_threshold == m_minThreshold))
            {
                continueCalibration = false;
            }
        }
        else {
            std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Finished calibration acquisition " <<
                m_bitCount << " after " << delay_ms  << " ms!"  << std::endl;
            //Finish calibration
            if(m_bitCount == m_number_bits-1) {
                continueCalibration = false;
            }
        }
        if(continueCalibration)
        {
            DoCalibrationStep();
            QThread::msleep(50);
            StartDataTaking();
        }
        else
        {
            if(m_outFile.is_open())
            {
                m_outFile.close();
            }
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
            m_dataAvailable = true;
            m_isCalibrated[m_modeIndex] = true;
            if(IsCalibration())
            {
                LoadSettings();
            }
            //Not for S-curve
            if(m_modeIndex != 5) {
                for(int bit =0; bit<m_number_bits; bit++){
                    for(int vmm =0; vmm < static_cast<int>(m_vmmActs.size()); vmm++){
                        int fec = GetFEC(vmm);
                        int hdmi = GetHDMI(vmm);
                        int chip = GetVMM(vmm);
                        for(unsigned int ch = 0; ch<64; ch++){
                            unsigned long size = m_data[bit][fec][hdmi][0][chip][ch].size();
                            double norm = static_cast<double>(size);
                            double mean = -1;
                            double median = -1;
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
                                mean = std::accumulate(m_data[bit][fec][hdmi][0][chip][ch].begin(),m_data[bit][fec][hdmi][0][chip][ch].end(),0.0)/norm;
                                auto nth = m_data[bit][fec][hdmi][0][chip][ch].begin() + (50*m_data[bit][fec][hdmi][0][chip][ch].size())/100;
                                std::nth_element(m_data[bit][fec][hdmi][0][chip][ch].begin(), nth, m_data[bit][fec][hdmi][0][chip][ch].end());
                                median = *nth;
                            }
                            if(m_modeIndex == 1 || m_modeIndex == 2) {
                                m_mean[bit][fec][hdmi][0][chip].push_back(median);
                            }
                            else {
                                m_mean[bit][fec][hdmi][0][chip].push_back(mean);
                            }

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
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);
        LoadSettings();
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("reset1", 1);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("reset2", 1);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->SendConfig(hdmi, 0, chip);
        QThread::msleep(100);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("reset1", 0);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("reset2", 0);
        m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->SendConfig(hdmi, 0, chip);
        QThread::msleep(100);

        m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ResetFEC();
        QThread::msleep(100);
        m_mainWindow->m_daqs[0].SendAll();
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
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);
        for(unsigned int ch=0; ch<64; ch++)
        {
            if(m_mean[0][fec][hdmi][0][chip][ch] > -1)
            {
                sorted_0mV.push_back(m_mean[0][fec][hdmi][0][chip][ch]);
            }
            if(m_mean[m_number_bits-1][fec][hdmi][0][chip][ch] > -1)
            {
                sorted_MAXmV.push_back(m_mean[m_number_bits-1][fec][hdmi][0][chip][ch]);
            }
        }

        if(sorted_0mV.size()>32 &&sorted_MAXmV.size()>32)
        {
            val =  SortVectors(sorted_0mV, sorted_MAXmV);
            for(unsigned int ch =0; ch<64; ch++){
                m_y[fec][hdmi][0][chip].push_back(val);
            }
            for(unsigned int ch =0; ch<64; ch++){
                double smallestDifference = 9999.;
                int bin_number = 0;
                double mean = 0;
                for(int bit = 0; bit<m_number_bits; bit++){
                    if(m_mean[bit][fec][hdmi][0][chip][ch]>=0)
                    {
                        double difference = pow(pow( m_y[fec][hdmi][0][chip][ch] - m_mean[bit][fec][hdmi][0][chip][ch],2),0.5);
                        if( difference < smallestDifference ) {
                            smallestDifference = difference;
                            bin_number = bit;
                            mean = m_mean[bit][fec][hdmi][0][chip][ch];
                        }
                    }
                    if(bit == m_number_bits-1){
                        m_bitVal[fec][hdmi][0][chip].push_back(bin_number);
                        m_calVal[fec][hdmi][0][chip].push_back(mean);
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
    GetSettings();
    if(m_modeIndex == 1 ||  m_modeIndex == 2)
    {
        if( ! m_calibrationArray[0] &&  !m_calibrationArray[1]) {
            return;
        }
        QString name = "vmm_calibration";
        int lastFEC = -1;
        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int fecId = m_mainWindow->m_daqs[0].m_fecs[fec].GetIP_id();
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            if(lastFEC != fecId) {
                name += "_FEC" + QString::number(fecId);
            }
            lastFEC = fecId;
            name += "_VMM" + QString::number(hdmi*2+chip);
        }
        QString theName = CreateFileName(name);
        QFile jsonFile(theName +  ".json");
        jsonFile.open(QFile::WriteOnly);

        QJsonObject globalObject;
        QJsonArray calibrationArray;
        for(int vmm = 0; vmm <static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int fecId = m_mainWindow->m_daqs[0].m_fecs[fec].GetIP_id();
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);

            QJsonObject calibrationObject;
            QJsonArray adcOffsetArray;
            QJsonArray adcSlopeArray;
            QJsonArray timeOffsetArray;
            QJsonArray timeSlopeArray;
            calibrationObject.insert("fecID",fecId);
            calibrationObject.insert("vmmID",hdmi*2+chip);
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
                    if(theVmmId == hdmi*2+chip &&  fecId == theFECId) {
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
                    if(theVmmId == hdmi*2+chip &&  fecId == theFECId) {
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
        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            for(int ch=0; ch<64; ch++)
            {
                int fec = GetFEC(vmm);
                int hdmi = GetHDMI(vmm);
                int chip = GetVMM(vmm);

                if(m_modeIndex == 3)
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_10", m_bitVal[fec][hdmi][0][chip][static_cast<unsigned int>(ch)]  , ch );
                }
                else if(m_modeIndex == 4) {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_8", m_bitVal[fec][hdmi][0][chip][static_cast<unsigned int>(ch)]  , ch );
                }
                else if(m_modeIndex == 6) {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "sd", m_bitVal[fec][hdmi][0][chip][static_cast<unsigned int>(ch)]  , ch );
                }
            }
        }
        SaveSettings();

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
        QString strIP = vmmip.toString();
        QString ip = strIP.mid(7,strIP.size()-7);
        Receive(buffer, size, ip);
    } // while loop

    return;
}

void CalibrationModule::GetSettings()
{
    m_BCID.clear();
    std::string text = m_mainWindow->m_daqWindow->ui->lineEditBCID->text().toStdString();

    if(text.size() > 0) {
        unsigned long pos = text.find("-");

        if(pos > 0)
        {
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
    else if(m_modeIndex == 5){
        m_number_bits = m_number_bits_s_curve;
    }
    else if(m_modeIndex == 6){
        m_number_bits = m_number_bits_threshold;
    }
    else if(m_modeIndex == 7){
        m_number_bits = m_number_bits_pedestal;
    }
    else {
        m_number_bits = 1;
    }
}

void CalibrationModule::InitializeDataStructures()
{
    mapIPFecId.clear();
    mapIPFirmware.clear();
    m_x.clear();
    m_dac_x.clear();
    m_max_value_x.clear();
    m_min_value_x.clear();
    for (unsigned int ch=0; ch < 64; ch++){
        m_channel_y[ch].clear();
        m_x.push_back(ch);
        m_max_value_x.push_back(0);
        m_min_value_x.push_back(0);
    }

    for(unsigned int bit = 0; bit < 32; bit++) {
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            for (unsigned short hdmi=0; hdmi < HDMIS_PER_FEC; hdmi++){
                for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_HDMI; hybrid++){
                    if (m_mainWindow->m_daqs[0].GetFEC(fec) &&  m_mainWindow->m_daqs[0].m_fecs[fec].GetHDMI(hdmi) && m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].GetHybrid(hybrid)){
                        int ckbc = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].GetReg("CKBC");
                        double period_ns = 0;
                        switch (ckbc)
                        {
                        case 0: period_ns = 12.5;
                            break;
                        case 1: period_ns = 12.5;
                            break;
                        case 2: period_ns = 25.0;
                            break;
                        case 3: period_ns = 50.0;
                            break;
                        case 4: period_ns = 100.0;
                            break;
                        case 5: period_ns = 200.0;
                            break;
                        case 6: period_ns = 400.0;
                            break;
                        case 7: period_ns = 800.0;
                            break;

                        }
                        m_bc_period[fec][hdmi][hybrid ]= period_ns;
                    }
                    for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                        if(bit == 0)
                        {
                            if (m_mainWindow->m_daqs[0].GetFEC(fec) &&  m_mainWindow->m_daqs[0].m_fecs[fec].GetHDMI(hdmi) &&
                                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].GetHybrid(hybrid) && m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].GetVMM(vmm) ){
                                int stc = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].m_vmms[vmm].GetRegister("stc");
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
                                m_tac_slope[fec][hdmi][hybrid][vmm] = tac_ns;
                            }
                            m_offset[fec][hdmi][hybrid][vmm].clear();
                            m_slope[fec][hdmi][hybrid][vmm].clear();
                            m_calVal[fec][hdmi][hybrid][vmm].clear();
                            m_bitVal[fec][hdmi][hybrid][vmm].clear();
                            m_y[fec][hdmi][hybrid][vmm].clear();
                            m_srs_timestamp_start[fec][hdmi][hybrid][vmm]=0;
                            m_srs_timestamp_end[fec][hdmi][hybrid][vmm]=0;

                        }
                        m_mean[bit][fec][hdmi][hybrid][vmm].clear();
                        m_allhitdata[bit][fec][hdmi][hybrid][vmm].clear();
                        for (unsigned int ch=0; ch < 64; ch++){
                            m_data[bit][fec][hdmi][hybrid][vmm][ch].clear();
                            //m_mean[bit][fec][hdmi][0][vmm].push_back(0);
                        }
                    }
                }
            }
        }
    }
}
void CalibrationModule::MeasurePedestalOrThreshold(bool isPedestal, bool isThresholdCalibration)
{
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(true);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(true);

    stringstream sx;
    if(isPedestal) {
        for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            for(int ch = 0; ch<64; ch++){
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("monitoring",std::to_string(ch));
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("smx",0,ch);
                m_mainWindow->m_daqs[0].SendAll();
                QThread::usleep(10000);
                int val = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hdmi, 0,chip, 2);
                m_mean[0][fec][hdmi][0][chip].push_back(val);
            }
        }
    }
    else {
        if(isThresholdCalibration) {
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                int fec = GetFEC(vmm);
                int hdmi = GetHDMI(vmm);
                int chip = GetVMM(vmm);

                for(int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("monitoring",std::to_string(ch));
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("smx",1,ch);
                    for(int bit=0; bit<m_number_bits;bit++){
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sd",bit,ch);
                        m_mainWindow->m_daqs[0].SendAll();
                        QThread::usleep(10000);
                        int val = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hdmi, 0,chip, 2);
                        m_mean[bit][fec][hdmi][0][chip].push_back(val);
                    }
                }

            }
        }
        else {
            for(int vmm=0; vmm<static_cast<int>(m_vmmActs.size()); vmm++){
                int fec = GetFEC(vmm);
                int hdmi = GetHDMI(vmm);
                int chip = GetVMM(vmm);

                for(int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("monitoring",std::to_string(ch));
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("smx",1,ch);
                    m_mainWindow->m_daqs[0].SendAll();
                    QThread::usleep(10000);
                    int val = m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hdmi, 0,chip, 2);
                    m_mean[0][fec][hdmi][0][chip].push_back(val);
                }
            }
        }
    }
    m_dataAvailable = true;
    if(isPedestal) {
        m_isCalibrated[7] = true;
    }
    else {
        m_isCalibrated[6] = true;
        if(isThresholdCalibration) {
            CalculateCorrections();
        }
    }
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
    PlotData();
}



// ------------------------------------------------------------------------ //
void CalibrationModule::Receive(const char* buffer, long size, QString ip)
{
    stringstream sx;
    m_lastUdpTimeStamp = m_commonData.m_udpTimeStamp;
    m_numHitsInFrame=0;
    m_numHitsInFrame = Receive_VMM3(buffer, size, mapIPFecId[ip]);

    //std::cout << ip.toStdString() << " " << m_bitCount << " " <<  m_numHitsInFrame << std::endl;
    AccumulateData();
}

int CalibrationModule::GetFEC(int vmmId) {
    return m_vmmActs[static_cast<unsigned long>(vmmId)]/(HDMIS_PER_FEC*HYBRIDS_PER_HDMI*VMMS_PER_HYBRID);
}

int CalibrationModule::GetHDMI(int vmmId) {
    int hdmi =  m_vmmActs[static_cast<unsigned long>(vmmId)]%(HDMIS_PER_FEC*HYBRIDS_PER_HDMI*VMMS_PER_HYBRID);
    return hdmi/2;
}

int CalibrationModule::GetVMM(int vmmId) {
    int hdmi =  m_vmmActs[static_cast<unsigned long>(vmmId)]%(HDMIS_PER_FEC*HYBRIDS_PER_HDMI*VMMS_PER_HYBRID);
    return hdmi%2;
}

int CalibrationModule::Parse_VMM3(uint32_t data1, uint16_t data2, int fecId) {
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
            int hdmi = vmmid/2;
            int chip = vmmid%2;
            double tac_slope_ns = m_tac_slope[fecId][hdmi][0][chip];
            double bc_period_ns = m_bc_period[fecId][hdmi][0];

            double theTdc = tdc;
            double theTime =  ((bcid+1)*bc_period_ns - theTdc*tac_slope_ns/255);

            if(m_srs_timestamp_start[fecId][hdmi][0][chip] > 0)
            {
                uint64_t delay = m_srs_timestamp_end[fecId][hdmi][0][chip] - m_srs_timestamp_start[fecId][hdmi][0][chip];
                if(delay <= static_cast<uint64_t>(m_mainWindow->m_daqWindow->ui->Runs->value())*1000000)
                {
                    //ADC calibrations or user settings average ADC
                    if(m_modeIndex == 1 || m_modeIndex == 3  || m_modeIndex == 11)
                    {
                        m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(adc);
                    }
                    //Time calibration
                    else if(m_modeIndex == 2 )
                    {
                        m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(theTime);
                    }
                    //TDC calibration or user settings average TDC per channel
                    else if(m_modeIndex == 4|| m_modeIndex == 12)
                    {
                        m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(tdc);
                    }
                    //S-curve
                    else if(m_modeIndex == 5)
                    {
                        if(m_data[m_bitCount][fecId][hdmi][0][chip][chNo].empty())
                        {
                            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(1);
                        }
                        else {
                            m_data[m_bitCount][fecId][hdmi][0][chip][chNo][0]++;
                        }
                    }
                    //User settings average bcid per channel
                    else if(m_modeIndex == 13)
                    {
                        m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(bcid);
                    }
                    //User settings counts per channel
                    else if(m_modeIndex == 10)
                    {
                        m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(1);
                    }
                    QVector<double> hit;
                    hit.push_back(chNo);
                    hit.push_back(bcid);
                    hit.push_back(tdc);
                    hit.push_back(adc);
                    hit.push_back(double(overThreshold));
                    m_allhitdata[m_bitCount][fecId][hdmi][0][chip].push_back(hit);
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
        int hdmi = vmmid/2;
        int chip = vmmid%2;
        uint64_t delay = m_srs_timestamp_end[fecId][hdmi][0][chip] - m_srs_timestamp_start[fecId][hdmi][0][chip];
        if(delay <= static_cast<uint64_t>(m_mainWindow->m_daqWindow->ui->Runs->value())*1000000)
        {
            //first timestamp/marker for this VMM
            if(m_srs_timestamp_start[fecId][hdmi][0][chip] == 0)
            {
                //42 bit timestamp, giving number of 40 MHz clock cycles
                m_srs_timestamp_start[fecId][hdmi][0][chip] = timestamp_42bit*25;
                m_start = timestamp_42bit*25;
                m_numHits=0;
            }
            m_srs_timestamp_end[fecId][hdmi][0][chip] = timestamp_42bit*25;
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
    m_commonData.m_fecId = (((m_commonData.m_dataId & 0xF0) >> 4)-1);
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
QString CalibrationModule::CreateFileName(QString name, int polarity, int gain, int peaktime, int tac, int bcclock)
{
    QString theName = m_mainWindow->GetApplicationPath() + "/" + name;
    QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    if(polarity != -1)
    {
        QString strPolarity = m_polarityTable[polarity];
        theName = theName + "_" + strPolarity;
    }
    if(gain != -1)
    {
        QString strGain = QString::number(m_gainTable[gain]) + "mVfC";
        theName = theName + "_" + strGain;
    }
    if(peaktime != -1)
    {
        QString strPeaktime = QString::number(m_peaktimeTable[peaktime]) + "ns";
        theName = theName + "_" + strPeaktime;
    }
    if(tac != -1)
    {
        QString strTac = QString::number(m_tacTable[tac]) + "ns";
        theName = theName + "_" + strTac;
    }
    if(bcclock != -1)
    {
        QString strBcclock = m_bcclock_table[bcclock] + "MHz";
        theName = theName + "_" + strBcclock;
    }
    theName = theName + "_" + theDate;
    return theName;
}
