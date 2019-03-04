#include <arpa/inet.h>
#include <cmath>
#include <QJsonDocument>
#include <QJsonArray>
#include "calibration_module.h"




CalibrationModule::CalibrationModule(MainWindow *top, QObject *parent) :
    QObject(parent),
    m_mainWindow{top},
    m_dbg(false),
    m_udpSocket(nullptr),
    m_msg(new MessageHandler),
    m_ignore16(false)
{
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


}

void CalibrationModule::StopDataTaking()
{
    m_mainWindow->m_daqWindow->ui->offACQ->setCheckable(true);
    m_mainWindow->m_daqWindow->ui->offACQ->setChecked(true);
    m_mainWindow->m_daqWindow->ui->onACQ->setChecked(false);
    m_mainWindow->m_daqWindow->ui->Send->setEnabled(true);
    m_mainWindow->m_daqs[0].ACQHandler(false);
    usleep(1000);
    CloseDAQSocket();

}

void CalibrationModule::StartDataTaking()
{

    emit m_mainWindow->m_daqWindow->ui->onACQ->clicked();usleep(1000);
    ConnectDAQSocket();
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
    else if(mode == "Threshold")
    {
        return 3;
    }
    if(m_runMode == "Calibration" && mode == "ADC")
    {
        return 4;
    }
    if(m_runMode == "Calibration" && mode == "TDC")
    {
        return 5;
    }

    if(mode == "Channels")
    {
        return 6;
    }
    if(m_runMode == "User" && mode == "ADC")
    {
        return 7;
    }
    if(m_runMode == "User" && mode == "TDC")
    {
        return 8;
    }
    if(m_runMode == "User" && mode == "BCID")
    {
        return 9;
    }
    if(m_runMode == "User" && mode == "Pedestal")
    {
        return 10;
    }
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

    if(m_runMode == "Calibration")
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
    if(m_modeIndex != 1 && m_modeIndex != 2)
    {
        return;
    }
    if(m_jsonObject)
    {
        delete m_jsonObject;
    }

    m_jsonObject = new QJsonObject();

    QJsonArray calibrationArray;


    double meanOffset = 0;

    double baseLine[m_bitCount];
    for(int n=0; n< m_bitCount;n++)
    {
        baseLine[n] = 0;
    }
    int cnt = 0;

    for(int bit =0; bit<m_number_bits; bit++){
        int numVMMwithData = 0;
        for(int vmm =0; vmm < m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);

            double baseLineChip = 0;
            int size = std::count_if(m_mean[bit][fec][hdmi][0][chip].begin(), m_mean[bit][fec][hdmi][0][chip].end(), [](double i){return ((i != -1.0));});
            if(size>0)
            {
                std::for_each(m_mean[bit][fec][hdmi][0][chip].begin(),m_mean[bit][fec][hdmi][0][chip].end(), [&](double x){if (x!=-1.0) baseLineChip += x;});
                numVMMwithData++;
                baseLineChip = baseLineChip/size;
                baseLine[bit] += baseLineChip;
            }

        }
        baseLine[bit] = baseLine[bit]/numVMMwithData;
        std::cout << "Baseline " << bit << " " << baseLine[bit] << std::endl;
    }

    for(int vmm =0; vmm < m_vmmActs.size(); vmm++){
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

                /*
                //if(hdmi == 7 && chip == 1 )
                if(bit == 0)
                {
                    m_outFile << "fit," << hdmi << "," << chip << "," << ch << "," << bit << "," << theMean << std::endl;
                }
                */
                double theXValue = 0;
                //Offline ADC
                if(m_modeIndex == 1)
                {
                    theXValue = baseLine[bit];
                }
                //Offline Time
                else if(m_modeIndex == 2)
                {
                    theXValue = bit*3.125;
                }

                x.push_back(theXValue);
                y.push_back(theMean);
                sumX += theXValue;
                sumY += theMean;
                sumXY += theXValue * theMean;
                sumX2 += theXValue * theXValue;
            }

            int nPoints = x.size();
            if(nPoints > 0)
            {
                double xMean = sumX / nPoints;
                double yMean = sumY / nPoints;
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

        for(int vmm =0; vmm < m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
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
            calibrationObject.insert("fecID",fec+1);
            calibrationObject.insert("vmmID",hdmi*2+chip);
            calibrationObject.insert("offsets",offsetArray);
            calibrationObject.insert("slopes",slopeArray);
            calibrationArray.push_back(calibrationObject);

        }

        if(m_modeIndex == 1){
            m_jsonObject->insert("vmm_adc_calibration",calibrationArray);
        }
        else if(m_modeIndex == 2)
        {
            m_jsonObject->insert("vmm_time_calibration",calibrationArray);
        }

    }

}

void CalibrationModule::SavePlotsAsPDF(){
    for(int vmm=0; vmm<m_vmmActs.size(); vmm++){
        int fec = GetFEC(vmm);
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);

        int theFec =  m_mainWindow->m_daqWindow->ui->comboBoxFec->currentIndex()/2;
        int selectedRangeStart = 0;
        int selectedRangeStop = 3;
        if(m_mainWindow->m_daqWindow->ui->comboBoxFec->currentIndex()%2 == 1 )
        {
            selectedRangeStart = 4;
            selectedRangeStop = 7;
        }
        //Changed to put all VMMs on one tab
        //if(fec == theFec && hdmi >= selectedRangeStart && hdmi <= selectedRangeStop)
        if(hdmi == 0 || hdmi == 2 || hdmi == 3 || hdmi == 7)
        {
            int index = hdmi;
            if(hdmi == 2)
            {
                index = 1;
            }
            else if(hdmi == 3)
            {
                index = 2;
            }

            //Changed to put all VMMs on one tab
            //QCustomPlot *plot = plotVector[hdmi*2+chip];
            QCustomPlot *plot = plotVector[index*2+chip];

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
                name = "Threshold";
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
                name = "Counts_Channels";
            }
            else if(m_modeIndex == 7)
            {
                name = "Mean_ADC";
            }
            else if(m_modeIndex == 8)
            {
                name = "Mean_TDC";
            }
            else if(m_modeIndex == 9)
            {
                name = "Mean_BCID";
            }
            else if(m_modeIndex == 10)
            {
                name = "Pedestal";
            }

            QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");

            name += "_FEC" + QString::number(fec);
            name += "_VMM" + QString::number(hdmi*2+chip);
            name += ("_" + theDate + ".pdf");
            plot->savePdf(name);
        }
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
    for(int vmm=0; vmm<m_vmmActs.size(); vmm++){
        int fec = GetFEC(vmm);
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);

        int theFec =  m_mainWindow->m_daqWindow->ui->comboBoxFec->currentIndex()/2;
        int selectedRangeStart = 0;
        int selectedRangeStop = 3;
        if(m_mainWindow->m_daqWindow->ui->comboBoxFec->currentIndex()%2 == 1 )
        {
            selectedRangeStart = 4;
            selectedRangeStop = 7;
        }
        //Changed to put all VMMs on one tab
        //if(fec == theFec && hdmi >= selectedRangeStart && hdmi <= selectedRangeStop)
        if(hdmi == 0 || hdmi == 2 || hdmi == 3 || hdmi == 7)
        {
            int index = hdmi;
            if(hdmi == 2)
            {
                index = 1;
            }
            else if(hdmi == 3)
            {
                index = 2;
            }

            //Changed to put all VMMs on one tab
            //QCustomPlot *plot = plotVector[hdmi*2+chip];
            QCustomPlot *plot = plotVector[index*2+chip];

            plot->clearGraphs();
            // give the axes some labels:
            plot->xAxis->setLabel("channel");
            // set axes ranges, so we see all data:
            plot->xAxis->setRange(0, 66);
            QPen pen(Qt::red,3,Qt::SolidLine);

            QString title = "VMM " + QString::number(hdmi*2+chip);

            double colorFactor = 0;


            if(m_modeIndex == 1) {
                title+= ": Mean ADC";
                colorFactor = 10;
            }
            else if(m_modeIndex == 4) {
                title+= ": Mean ADC";
                colorFactor = 5;
            }
            else if(m_modeIndex == 2) {
                title+= ": Mean time";
                colorFactor = 10;
            }
            else if(m_modeIndex == 3) {
                title+= ": Threshold";
                colorFactor = 5;
            }
            else if(m_modeIndex == 5) {
                title+= ": Mean TDC";
                colorFactor = 10;
            }
            else if(m_modeIndex == 6)
            {
                title+= ": Counts";
            }
            else if(m_modeIndex == 7)
            {
                title+= ": Mean ADC";
            }
            else if(m_modeIndex == 8)
            {
                title+= ": Mean TDC";
            }
            else if(m_modeIndex == 9)
            {
                title+= ": Mean BCID";
            }
            else if(m_modeIndex == 10)
            {
                title+= ": Pedestal [mV]";
                colorFactor = 30;
            }

            plot->yAxis->setLabel(title);
            if(m_modeIndex >= 6  &&  m_modeIndex <= 9)
            {

                plot->addGraph();
                plot->graph(0)->setPen(pen);
                plot->graph(0)->setData( QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_mean[0][fec][hdmi][0][chip]));

                plot->yAxis->setLabel(title);
                plot->legend->setVisible(true);
                plot->legend->setFont(QFont("Helvetica",9));
                plot->graph(0)->setName(title);
                plot->yAxis->rescale();
                plot->replot();

            }
            else
            {
                if(m_modeIndex == 1 || m_modeIndex == 2)
                {
                    for(int bit=0; bit<m_number_bits;bit++){
                        plot->addGraph();
                        plot->graph(2*bit)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                        plot->graph(2*bit)->setPen(QPen(QColor(bit*colorFactor)));
                        plot->legend->removeItem(plot->legend->itemCount()-1);
                        std::vector<double> y;
                        for(int ch=0; ch<64; ch++)
                        {
                            y.push_back((m_mean[bit][fec][hdmi][0][chip][ch] - m_offset[fec][hdmi][0][chip][ch])*  m_slope[fec][hdmi][0][chip][ch]);
                            //std::cout << ch << " " << m_mean[bit][fec][hdmi][0][chip][ch] << std::endl;
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

                else if(m_modeIndex == 3)
                {
                    for(int bit=0; bit<m_number_bits;bit++){
                        plot->addGraph();
                        plot->graph(bit)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                        plot->graph(bit)->setPen(QPen(QColor(bit*colorFactor)));
                        plot->legend->removeItem(plot->legend->itemCount()-1);
                    }
                    plot->legend->setVisible(false);
                }
                else
                {
                    plot->legend->setVisible(true);
                    plot->legend->setFont(QFont("Helvetica",8));
                    for(int bit=0; bit<m_number_bits;bit++){
                        plot->addGraph();
                        plot->graph(bit)->setName(QString("Pedestal"));
                        plot->graph(bit)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                        plot->graph(bit)->setPen(QPen(Qt::blue,1,Qt::SolidLine));
                        if(bit > 0)
                        {
                            plot->legend->removeItem(plot->legend->itemCount()-1);
                        }
                    }
                    for(int bit=m_number_bits; bit<2*m_number_bits;bit++){
                        plot->addGraph();
                        if(bit == m_number_bits)
                        {
                            plot->graph(bit)->setName(QString("Threshold DAC, SD 0 mV"));
                        }
                        else if(bit == m_number_bits + 1)
                        {
                            plot->graph(bit)->setName(QString("Threshold DAC, SD 15 mV"));
                        }
                        else {
                            plot->graph(bit)->setName(QString("Threshold DAC, SD 30 mV"));
                        }

                        plot->graph(bit)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                        plot->graph(bit)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                        //plot->legend->removeItem(plot->legend->itemCount()-1);
                    }
                }


                plot->legend->setFont(QFont("Helvetica",9));
                plot->yAxis->rescale();
                plot->replot();

            }
        }

    }

}


// ------------------------------------------------------------------------ //
void CalibrationModule::GetActiveVMMs(){
    m_vmmActs.clear();
    m_mainWindow->m_daqWindow->ui->comboBoxFec->clear();

    for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
        if (m_mainWindow->m_daqs[0].GetFEC(fec) ){
            m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("FEC%0 VMM 0-7").arg(fec));
            m_mainWindow->m_daqWindow->ui->comboBoxFec->addItem(QString("FEC%0 VMM 8-15").arg(fec));
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
}
// ------------------------------------------------------------------------ //
void CalibrationModule::updatePlot(){
    GetSettings();
    PlotData();
}

// ------------------------------------------------------------------------ //

void CalibrationModule::StartCalibration(){
    m_numHits = 0;
    m_bitCount=-1;
    m_lastUdpTimeStamp=0;
    m_vmmIndex = 0;
    for(int n =0; n<maxModes; n++)
    {
        m_isCalibrated[n] = false;
    }
    m_dataAvailable = false;
    emit m_mainWindow->m_daqWindow->ui->openConnection_2->clicked();usleep(1000);
    if(! (m_mainWindow->m_daqWindow->ui->connectionLabel_2->text()==QString("all alive"))) {
        std::cout<<"Communication couldn't be established! \n exit calibration"<<std::endl;
        m_mainWindow->m_daqWindow->ui->InfoScreen->setTextColor(Qt::red);
        m_mainWindow->m_daqWindow->ui->InfoScreen->append(QString("Communication couldn't be established! \n exit calibration"));
        m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
        return;
    }


    GetSettings();
    PlotData();
    if(m_runMode != "User")
    {
        if(!IsCalibration())
        {
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
            return;
        }
    }
    if( m_mainWindow->m_daqWindow->ui->comboBoxRunMode->currentIndex() == 0)
    {
        m_mainWindow->m_daqWindow->ui->line_configFile->setText("Calib_config");
    }
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(true);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(true);
    emit m_mainWindow->m_daqWindow->ui->Button_save->clicked();usleep(1000);
    m_mainWindow->m_daqWindow->ui->line_configFile->setText("");

    GetActiveVMMs();

    InitializeDataStructures();


    for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
        if (m_mainWindow->m_daqs[0].GetFEC(fec) ){
            QMap<QString, QString> registers;
            m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadSystemRegisters(registers);
            m_mainWindow->m_daqs[0].m_fecs[fec].SetFirmwareVersion(registers["FirmwareVers"]);
            mapIPFirmware.emplace(registers["FECip"], registers["FirmwareVers"]);
            mapIPFecId.emplace(registers["FECip"],fec);
            std::cout << registers["FECip"].toStdString() << " " << fec << std::endl;

        }
    }

    /*
    QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
    QString name = "time_offset_slope";
    name += ("_" + theDate + ".txt");

    if(m_outFile.is_open())
    {
        m_outFile.close();
    }
    m_outFile.open(name.toStdString().c_str(), std::ofstream::out);
   */

    m_mainWindow->m_daqWindow->ui->checkBoxGlobalDAQ->setChecked(true);
    emit m_mainWindow->m_daqWindow->ui->checkBoxGlobalDAQ->stateChanged(true);
    emit m_mainWindow->m_daqWindow->ui->trgPulser->clicked();usleep(1000);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(true);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(true);


    if(m_runMode != "User")
    {
        for(unsigned int vmm=0; vmm<m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            //Save settings that are modified during calibration
            m_old_sdp2[vmm] = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("sdp_2");
            m_old_sdt[vmm] = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].GetRegister("sdt");
            m_old_TP_skew[vmm] = m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].GetReg("TP_skew");

            //Load general calibration settings
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].LoadCalibSettings();

            //prepare setting for threshold calibration
            //disable all test pulses
            //unmask all channels
            //set threshold correction sd to middle of the range (15 mV)
            if(m_modeIndex == 3)
            {
              for(unsigned int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",1,ch);
                    if(vmm == 0)
                    {
                        m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",0,ch);
                    }
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sd",15,ch);
                }

            }
        }
        m_mainWindow->m_daqs[0].SendAll();
    }
    if(m_modeIndex == 10)
    {
        MeasurePedestal();
    }
    else {
        m_nodata_start = std::chrono::high_resolution_clock::now();
        DoCalibrationStep();
        StartDataTaking();
    }



}

// ------------------------------------------------------------------------ //

void CalibrationModule::DoCalibrationStep(){


    if(!IsCalibration())
    {
        m_bitCount=0;
    }
    //Calibration
    else
    {
        if(m_modeIndex != 3 )
        {
            m_bitCount++;
        }
        else {
            if(m_bitCount == -1)
            {
                m_bitCount=0;
            }
        }
        for(unsigned int vmm=0; vmm<m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);

            if(m_modeIndex == 1)
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].SetReg("TP_skew", 0);
                std::string val = std::to_string(300+m_bitCount*50);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdp_2",val);
            }
            else if(m_modeIndex == 2)
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].SetReg("TP_skew", m_bitCount);
            }
            else if(m_modeIndex == 3 && m_vmmIndex == vmm)
            {
                int fec = GetFEC(m_vmmIndex);
                int hdmi = GetHDMI(m_vmmIndex);
                int chip = GetVMM(m_vmmIndex);
                int numberOfMaskedChannels = 0;
                if(m_numHits > 0)
                {

                    for(int chNo = 0; chNo < 64; chNo++)
                    {
                        if(m_data[m_bitCount][fec][hdmi][0][chip][chNo].size() > 0)
                        {
                            //mask channel that has had hits
                            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",1,chNo);
                            numberOfMaskedChannels++;
                            if(m_bitCount == 1)
                            {
                                std::cout <<numberOfMaskedChannels <<  " MASKED channels XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Masking chip "
                                         << hdmi << ", " << chip << ", channel " << chNo << ", at threshold " << m_minThreshold+m_threshold << std::endl;
                            }
                            else {
                                std::cout <<numberOfMaskedChannels <<  " MASKED channels XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Masking chip "
                                        << hdmi << ", " << chip << ", channel " << chNo << ", at threshold " << m_maxThreshold-m_threshold << std::endl;
                            }
                        }
                    }
                }
                if(m_minThreshold+m_threshold == m_maxThreshold || numberOfMaskedChannels == 64)
                {
                    for(int chNo = 0; chNo < 64; chNo++)
                    {
                        //mask channels of last vmm
                         m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",1,chNo);
                    }
                    //new VMM
                    //if(m_vmmIndex < m_vmmActs.size()-1)
                    //{
                    //    m_vmmIndex++;
                    //}
                    //last VMM finished, change direction
                    //else {
                        m_vmmIndex = 0;
                        m_bitCount+=1;
                    //}
                    fec = GetFEC(m_vmmIndex);
                    hdmi = GetHDMI(m_vmmIndex);
                    chip = GetVMM(m_vmmIndex);

                    for(int chNo = 0; chNo < 64; chNo++)
                    {
                        //unmask channels of new vmm
                         m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",0,chNo);

                    }
                    m_threshold = 0;
                }
                else {
                    m_threshold++;
                }

                fec = GetFEC(m_vmmIndex);
                hdmi = GetHDMI(m_vmmIndex);
                chip = GetVMM(m_vmmIndex);

                //set new threshold
                if(m_bitCount == 1)
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdt", m_minThreshold+m_threshold);
                    std::cout << "Calibration step: " << m_vmmIndex << " " << m_bitCount << " " << m_minThreshold+m_threshold << std::endl;
                }
                else {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdt", m_maxThreshold-m_threshold);
                    std::cout << "Calibration step: " << m_vmmIndex << " " << m_bitCount << " " <<  m_maxThreshold-m_threshold << std::endl;
                }


            }
            else if(m_modeIndex == 4)
            {
                for(unsigned int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_10", m_bitCount,ch );
                }
            }
            else if(m_modeIndex == 5)
            {
                for(unsigned int ch = 0; ch<64; ch++){
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_8", m_bitCount,ch );
                }
            }
        }
        m_mainWindow->m_daqs[0].SendAll();
        if(m_modeIndex == 3 && m_vmmIndex == 0)
        {
            usleep(10000);
        }
    }
    m_start = std::chrono::high_resolution_clock::now();
}

// ------------------------------------------------------------------------ //
void CalibrationModule::AccumulateData(){
    stringstream sx;

    //Data received
    if(m_numHits > 0)
    {
        m_nodata_start = std::chrono::high_resolution_clock::now();
    }
    //No data received
    else
    {
        m_nodata_end = std::chrono::high_resolution_clock::now();
        auto nodata_duration = std::chrono::duration_cast<std::chrono::milliseconds>( m_nodata_end - m_nodata_start ).count();
        //If for no longer than 10 s no data was received, hard reset the VMMs
        if((m_modeIndex != 3 && nodata_duration >= 10000))
        {
            StopDataTaking();
            m_bitCount = -1;
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
            usleep(1000);
            std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Too many errors!" << std::endl;
            std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Hard Reset of VMMs!" << std::endl;
            for(int vmm =0; vmm < m_vmmActs.size(); vmm++){
                int fec = GetFEC(vmm);
                int hdmi = GetHDMI(vmm);
                int chip = GetVMM(vmm);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdp_2", m_old_sdp2[vmm]);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].SetReg("TP_skew", m_old_TP_skew[vmm]);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdt", m_old_sdt[vmm]);


                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("reset1", 1);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("reset2", 1);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->SendConfig(hdmi, 0, chip);
                usleep(1000);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("reset1", 0);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("reset2", 0);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->SendConfig(hdmi, 0, chip);
                usleep(1000);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ResetFEC(false);
                usleep(1000);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ResetFEC(true);
                usleep(1000);
                m_mainWindow->m_daqs[0].SendAll();
            }
            StartCalibration();
        }

    }


    m_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( m_end - m_start );
    //If acquisition took longer than predefined time, finish calibration step
    if(duration.count() >=m_mainWindow->m_daqWindow->ui->Runs->value()){
        StopDataTaking();
        std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Finished calibration acquisition after " << duration.count() << " ms!" << std::endl;

        //if((m_bitCount == m_number_bits-1 && m_modeIndex != 3 )|| (m_modeIndex == 3 && m_numHits > 0 && m_vmmIndex == m_vmmActs.size() -1 && m_channelIndex == 63)){
        if((m_bitCount == m_number_bits-1) && (m_modeIndex != 3  || (m_modeIndex == 3  && m_vmmIndex == 0 &&  m_maxThreshold-m_threshold == m_minThreshold)))
        {

            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
            m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
            m_dataAvailable = true;
            m_isCalibrated[m_modeIndex] = true;

            for(int bit =0; bit<m_number_bits; bit++){

                for(int vmm =0; vmm < m_vmmActs.size(); vmm++){
                    int fec = GetFEC(vmm);
                    int hdmi = GetHDMI(vmm);
                    int chip = GetVMM(vmm);
                    if(m_runMode != "User")
                    {
                        if(bit == 0)
                        {
                            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdp_2", m_old_sdp2[vmm]);
                            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].SetReg("TP_skew", m_old_TP_skew[vmm]);
                            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sdt", m_old_sdt[vmm]);
                            for(unsigned int ch = 0; ch<64; ch++){
                                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
                                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sm",0,ch);
                                //m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sd",15,ch);
                            }


                        }
                    }

                    for(unsigned int ch = 0; ch<64; ch++){
                        int size = m_data[bit][fec][hdmi][0][chip][ch].size();
                        double mean = -1;
                        if(m_runMode == "User")
                        {
                            mean = 0;
                        }
                        //Channels
                        if(m_modeIndex == 6)
                        {
                            if(size>0)
                            {
                                mean = std::accumulate(m_data[bit][fec][hdmi][0][chip][ch].begin(),m_data[bit][fec][hdmi][0][chip][ch].end(),0.0);
                            }
                        }
                        else {
                            if(size>0)
                            {
                                mean = std::accumulate(m_data[bit][fec][hdmi][0][chip][ch].begin(),m_data[bit][fec][hdmi][0][chip][ch].end(),0.0)/size;
                            }
                        }

                        m_mean[bit][fec][hdmi][0][chip].push_back(mean);
                        //std::cout << "mean," << fec << "," << hdmi << "," << chip << "," << ch << "," << bit << ", " << mean << std::endl;
                    }

                }

            }
            m_mainWindow->m_daqs[0].SendAll();
            if(m_runMode == "Calibration")
            {
                if(m_modeIndex == 1 || m_modeIndex == 2)
                {
                    FitOfflineCalibrationData();
                }
                else if(m_modeIndex > 3)
                {
                    CalculateCorrections();
                }
                /*
                if(m_outFile.is_open())
                {
                    m_outFile.close();
                }
                */
            }
            PlotData();

        }
        else{
            DoCalibrationStep();
            usleep(10);
            StartDataTaking();
        }
    }





}

double CalibrationModule::SortVectors( vector<double>& sortedMin, vector<double>& sortedMax){
    stringstream sx;
    std::sort(sortedMin.begin(), sortedMin.end());
    std::sort(sortedMax.begin(), sortedMax.end());
    int last = sortedMax.size()-1;
    int state = 0;
    int check = 0;
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
    // Function to calculate the best common ADC or TDC value for all channels
    vector<double> sorted_0mV;
    vector<double> sorted_MAXmV;
    double val =  0;
    if(!IsCalibration() || !CheckModes())
    {
        return;
    }
    if(m_modeIndex < 3 || m_modeIndex > 5)
    {
        return;
    }

    for(int vmm=0; vmm<m_vmmActs.size(); vmm++){
        int fec = GetFEC(vmm);
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);

        for(int ch=0; ch<64; ch++)
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
    }


    if(sorted_0mV.size()>32 &&sorted_MAXmV.size()>32)
    {

        val =  SortVectors(sorted_0mV, sorted_MAXmV);
        for(unsigned int vmm=0; vmm<m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);

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



void CalibrationModule::SaveCorrections(){
    if(!IsCalibration())
    {
        return;
    }

    QString name = "";

    if(m_modeIndex == 1 || m_modeIndex == 2)
    {

        m_isCalibrated[m_modeIndex] = false;
        if(m_modeIndex == 1)
        {
            name = "adc_offset_slope";
        }
        else if(m_modeIndex == 2)
        {
            name = "time_offset_slope";
        }


        QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");

        for(int vmm=0; vmm<m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            //int fecId = m_mainWindow->
            name += "_FEC" + QString::number(fec);
            name += "_VMM" + QString::number(hdmi*2+chip);

        }
        //GetRegVal(const char *reg)
        //         unsigned short Hybrid::GetReg(std::string feature){
        //name += "_VMM" + QString::number(hdmi*2+chip);
        name += ("_" + theDate + ".json");
        QJsonDocument doc(*m_jsonObject);
        QFile jsonFile(name);
        jsonFile.open(QFile::WriteOnly);
        jsonFile.write(doc.toJson(QJsonDocument::JsonFormat::Compact));
        delete m_jsonObject;

    }
    else
    {
        m_mainWindow->m_daqWindow->LoadConfig("Calib_config");
        sleep(1);
        for(int vmm=0; vmm<m_vmmActs.size(); vmm++){
            for(int ch=0; ch<64; ch++)
            {
                int fec = GetFEC(vmm);
                int hdmi = GetHDMI(vmm);
                int chip = GetVMM(vmm);
                if(m_modeIndex == 3) {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "sd", m_bitVal[fec][hdmi][0][chip][ch]  , ch );
                }
                else if(m_modeIndex == 4)
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_10", m_bitVal[fec][hdmi][0][chip][ch]  , ch );
                }
                else if(m_modeIndex == 5) {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_8", m_bitVal[fec][hdmi][0][chip][ch]  , ch );
                }
            }
        }
        //m_mainWindow->m_daqs[0].SendAll();
        m_mainWindow->m_daqWindow->ui->line_configFile->setText("Calib_config");
        m_mainWindow->m_daqWindow->on_Button_save_clicked();usleep(1000);
        m_mainWindow->m_daqWindow->ui->line_configFile->setText("");
        sleep(1);
        m_mainWindow->m_daqWindow->LoadConfig("Calib_config");

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
        datagram.resize(m_udpSocket->pendingDatagramSize());
        //m_udpSocket->readDatagram(datagram.data(), datagram.size(), &vmmip);
        int size = m_udpSocket->pendingDatagramSize();
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
    int pos = text.find("-");
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
        m_number_bits = m_number_bits_threshold;
    }
    else if(m_modeIndex == 4){
        m_number_bits = m_number_bits_adc;
    }
    else if(m_modeIndex == 5){
        m_number_bits = m_number_bits_tdc;
    }
    else if(m_modeIndex == 10){
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

    for (unsigned int n=0; n < 1000; n++){
        m_old_sdp2[n] = 0;
        m_old_TP_skew[n] = 0;
        m_old_sdt[n] = 0;
    }
    for (unsigned int ch=0; ch < 64; ch++){
        m_x.push_back(ch);
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
                        case 0: period_ns = 6.25;
                            break;
                        case 1: period_ns = 6.25;
                            break;
                        case 2: period_ns = 12.5;
                            break;
                        case 3: period_ns = 25.0;
                            break;
                        case 4: period_ns = 50.0;
                            break;
                        case 5: period_ns = 100.0;
                            break;
                        case 6: period_ns = 200.0;
                            break;
                        case 7: period_ns = 500.0;
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


                        }
                        m_mean[bit][fec][hdmi][hybrid][vmm].clear();
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




void CalibrationModule::MeasurePedestal()
{
    stringstream sx;

    for(unsigned int vmm=0; vmm<m_vmmActs.size(); vmm++){
        int fec = GetFEC(vmm);
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);

        for(unsigned int ch = 0; ch<64; ch++){
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("monitoring",std::to_string(ch));
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
            //usleep(1000);
            for(int bit=0; bit<m_number_bits;bit++)
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("smx",0,ch);
                m_mainWindow->m_daqs[0].SendAll();
                int pedestal =  m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hdmi, 0,vmm, 2);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("smx",1,ch);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sd",bit*15,ch);
                m_mainWindow->m_daqs[0].SendAll();
                int threshold =  m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hdmi, 0,vmm, 2);
                m_mean[bit][fec][hdmi][0][chip].push_back(pedestal);
                m_mean[m_number_bits + bit][fec][hdmi][0][chip].push_back(threshold);

            }
        }


    }
    m_dataAvailable = true;

    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
    //CalculateCorrections();
    PlotData();
}

// ------------------------------------------------------------------------ //
void CalibrationModule::Receive(const char* buffer, int size, QString ip)
{
    stringstream sx;
    m_lastUdpTimeStamp = m_commonData.m_udpTimeStamp;
    m_numHits=0;
    if(mapIPFirmware[ip] == "0002")
    {
        m_numHits = Receive_VMM3(buffer, size, mapIPFecId[ip]);

    }
    else
    {
        m_numHits = Receive_VMM2(buffer, size, mapIPFecId[ip]);
    }
    std::cout << ip.toStdString() << " " << m_bitCount << " " <<  m_numHits << std::endl;
    AccumulateData();
}

int CalibrationModule::GetFEC(int vmmId) {
    return m_vmmActs[vmmId]/(HDMIS_PER_FEC*HYBRIDS_PER_HDMI*VMMS_PER_HYBRID);
}

int CalibrationModule::GetHDMI(int vmmId) {
    int hdmi =  m_vmmActs[vmmId]%(HDMIS_PER_FEC*HYBRIDS_PER_HDMI*VMMS_PER_HYBRID);
    return hdmi/2;
}

int CalibrationModule::GetVMM(int vmmId) {
    int hdmi =  m_vmmActs[vmmId]%(HDMIS_PER_FEC*HYBRIDS_PER_HDMI*VMMS_PER_HYBRID);
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
        uint16_t bcid = Gray2bin32(data1 & 0xFFF);
        if(IsDbgActive()) {
            sx.str("");
            sx << "SRS Data fecId " << (int)fecId << ", vmmId " << (int)vmmid << ", chNo: " <<  (int)chNo
               << ", bcid: " <<  (int)bcid
               << ", tdc: " <<  (int)tdc
               << ", adc: " <<  (int)adc
               << ", overThreshold: " <<  (int)overThreshold
               << ", triggerOffset: " << (int) triggerOffset
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
            double theTdc = (int)(tdc/8)*8 + 4.0;

            double theTime =  ((bcid+1)*bc_period_ns - theTdc*tac_slope_ns/255);
            //m_outFile << "rawdata," << hdmi << "," << chip << "," << (int)chNo << "," << m_bitCount << "," << theTime << "," << bcid << "," << (int)tdc << "," << (int)adc << std::endl;


            if(m_modeIndex == 1 || m_modeIndex == 4  || m_modeIndex == 7)
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(adc);
            }
            else if(m_modeIndex == 5|| m_modeIndex == 8)
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(tdc);
            }
            else if(m_modeIndex == 9)
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(bcid);
            }
            else if(m_modeIndex == 3)
            {
                if(m_bitCount == 0)
                {
                    m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(m_maxThreshold-m_threshold);
                }
                else {
                    m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(m_minThreshold+m_threshold);
                }
            }
            else if(m_modeIndex == 6)
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(1);
            }
            else if(m_modeIndex == 2 )
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(theTime);
            }

        }
        return 1;
    } else {
        /// Marker
        uint8_t vmmid = (data2 >> 10) & 0x1F;
        uint32_t timestamp_lower_10bit = data2 & 0x03FF;
        uint32_t timestamp_upper_32bit = data1;
        uint64_t timestamp_42bit = (timestamp_upper_32bit << 10) + timestamp_lower_10bit;
        if(IsDbgActive()) {
            sx.str("");
            sx << "SRS Marker fecId " << (int)m_commonData.m_fecId << ", vmmId " << vmmid << ", timestamp lower 10bit: " <<  timestamp_lower_10bit
               << ", timestamp upper 32bit: " <<  timestamp_upper_32bit
               << ", timestamp 42bit: " <<  timestamp_42bit << "\n";
            GetMessageHandler()(sx,"calibration_module::Parse_VMM3"); sx.str("");
        }

        return 0;
    }
}

int CalibrationModule::Receive_VMM3(const char *buffer, int size, int fecId) {
    int numHits = 0;
    stringstream sx;
    if (size < 4) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "Undersize data: " << size << " words!";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }

    struct SRSHeader_VMM3 *hdr = (struct SRSHeader_VMM3 *) buffer;

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
        int64_t fcDiff = m_commonData.m_frameCounter - m_commonData.m_lastFrameCounter;
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
        uint32_t data1 = htonl(*(uint32_t *) &buffer[data1Offset]);
        uint16_t data2 = htons(*(uint16_t *) &buffer[data2Offset]);

        int res = Parse_VMM3(data1, data2, fecId );
        if (res == 1) { // This was data
            numHits++;
        }
        readoutIndex++;
        dataLength -= m_hitAndMarkerSize_VMM3;
        if (numHits == m_maxHits_VMM3 && dataLength > 0) {
            if(IsDbgActive()) {
                sx.str("");
                sx << "Data overflow, skipping " << dataLength << " bytes\n";
                GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
            }
            break;
        }
    }
    return numHits;
}



int CalibrationModule::Parse_VMM2(uint32_t data1, uint32_t data2, uint32_t vmmid, int fecId) {
    data1 = Reversebits32(data1);
    data2 = Reversebits32(data2);

    uint8_t tdc = ((data1 >> 18) & 0x3f) + (((data1 >> 8) & 0x03) << 6);
    uint16_t adc = ((data1 >> 24) & 0xff) + (((data1 >> 16) & 0x03) << 8);
    uint16_t bcid = Gray2bin32(((data1 >> 10) & 0x3f) + (((data1 >> 0) & 0x3f) << 6));
    uint8_t chNo = (data2 >> 2) & 0x3f;
    //uint8_t overThreshold = (data2 >> 1) & 0x01;

    auto it = find (m_BCID.begin(), m_BCID.end(), bcid);
    if ((m_BCID.empty() || it != m_BCID.end()))
    {
        int hdmi = vmmid/2;
        int chip = vmmid%2;
        double tac_slope_ns = m_tac_slope[fecId][hdmi][0][chip];
        double bc_period_ns = m_bc_period[fecId][hdmi][0];
        double theTdc = (int)(tdc/8)*8 + 4.0;
        double theTime =  ((bcid+1)*bc_period_ns - theTdc*tac_slope_ns/255);
        if(m_modeIndex == 1 || m_modeIndex == 4  || m_modeIndex == 7)
        {
            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(adc);
        }
        else if(m_modeIndex == 5|| m_modeIndex == 8)
        {
            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(tdc);
        }
        else if(m_modeIndex == 9)
        {
            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(bcid);
        }
        else if(m_modeIndex == 3)
        {
            if(m_bitCount == 1)
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(m_maxThreshold-m_threshold);
            }
            else {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(m_minThreshold+m_threshold);
            }
        }
        else if(m_modeIndex == 6)
        {
            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(1);
        }
        else if(m_modeIndex == 2 )
        {
            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(theTime);
        }
    }

    return 1;
}

int CalibrationModule::Receive_VMM2(const char *buffer, int size, int fecId) {
    stringstream sx;
    int numHits = 0;

    struct SRSHeader_VMM2 *hdr = (struct SRSHeader_VMM2 *)buffer;
    if (ntohl(hdr->m_frameCounter) == 0xfafafafa) {
        if(IsDbgActive()) {
            sx << "End of frame\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM2"); sx.str("");
        }
        return 0;
    }
    if (size < 20) {
        if(IsDbgActive()) {
            sx << "Undersize data\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM2"); sx.str("");
        }
        return 0;
    }
    m_commonData.m_dataId = ntohl(hdr->m_dataId);


    if (m_commonData.m_dataId == 0x56413300 || m_commonData.m_dataId == 0x564d3300) {
        if(IsDbgActive()) {
            sx << "No data\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM2"); sx.str("");
        }
        return 0;
    }


    if ((m_commonData.m_dataId & 0xffffff00) != 0x564d3300) {
        if(IsDbgActive()) {
            sx << "Unknown data\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM2"); sx.str("");
        }
        return 0;
    }

    auto dataLength = size - m_SRSHeaderSize_VMM2;
    if ((dataLength & 0xfff8) != dataLength) {
        if(IsDbgActive()) {
            sx << "Invalid data length " << dataLength << "\n";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM2"); sx.str("");
        }
        return 0;
    }
    m_commonData.m_udpTimeStamp = ntohl(hdr->m_udpTimeStamp);

    int vmmId = m_commonData.m_dataId & 0x0f;
    if(IsDbgActive()) {
        sx << "VMM2 data, vmmID " << vmmId << "\n";
        GetMessageHandler()(sx,"calibration_module::Receive_VMM2"); sx.str("");
    }

    m_commonData.m_fecId = (((m_commonData.m_dataId & 0xF0) >> 4)-1);
    if(IsDbgActive()) {
        sx << "VMM2 data, fecId " << (int)m_commonData.m_fecId  << "\n";
        GetMessageHandler()(sx,"calibration_module::Receive_VMM2"); sx.str("");
    }

    int index = 0;
    while (dataLength >= m_hitSize_VMM2) {

        uint32_t data1 = htonl(*(uint32_t *)&buffer[m_SRSHeaderSize_VMM2 + m_hitSize_VMM2 * index]);
        uint32_t data2 = htonl(*(uint32_t *)&buffer[m_SRSHeaderSize_VMM2 + m_data1Size + m_hitSize_VMM2 * index]);
        int res = Parse_VMM2(data1, data2, vmmId, fecId );
        if (res == 1) {
            numHits++;
            index++;
        }
        dataLength -= m_hitSize_VMM2;
        if (numHits == m_maxHits_VMM2 && dataLength >= m_hitSize_VMM2) {
            if(IsDbgActive()) {
                sx << "Data overflow, skipping " << dataLength << " bytes\n";
                GetMessageHandler()(sx,"calibration_module::Receive_VMM2"); sx.str("");
            }
            break;
        }
    }
    return numHits;
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
