#include <arpa/inet.h>
#include <cmath>
#include <QJsonDocument>
#include <QJsonArray>
#include "calibration_module.h"




CalibrationModule::CalibrationModule(MainWindow *top, QObject *parent) :
    QObject(parent),
    m_mainWindow{top},
    m_dbg(true),
    m_udpSocket(nullptr),
    m_isCalibratedADC(false),
    m_isCalibratedTDC(false),
    m_isCalibratedTime(false),
    m_isCalibratedThreshold(false),
    m_msg(new MessageHandler),
    m_ignore16(false)
{
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


    connect( m_mainWindow->m_daqWindow->ui->comboBoxFec, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updatePlot()));

    connect( m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType, SIGNAL(currentIndexChanged(int)),
             this, SLOT(updatePlot()));

}



// ------------------------------------------------------------------------ //
void CalibrationModule::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
    // remove monitoring m_daqMonitor->LoadMessageHandler(msg());
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

    if(m_runMode == "Calibration" && m_calibMode == "ADC" && !m_isCalibratedADC)
    {
        return false;
    }
    if(m_runMode == "Calibration" && m_calibMode == "TDC" && !m_isCalibratedTDC)
    {
        return false;
    }
    if(m_runMode == "Calibration" && m_calibMode == "Threshold" && !m_isCalibratedThreshold)
    {
        return false;
    }
    if(m_runMode == "Calibration" && m_calibMode == "Time" && !m_isCalibratedTime)
    {
        return false;
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

void CalibrationModule::FitTimeData()
{

    m_jsonObject = new QJsonObject();


    QJsonArray calibrationArray;

    for(int vmm =0; vmm < m_vmmActs.size(); vmm++){
        int fec = GetFEC(vmm);
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);
        QJsonObject calibrationObject;
        QJsonArray offsetArray;
        QJsonArray slopeArray;
        for(unsigned int ch = 0; ch<64; ch++){
            std::vector<double> x;
            std::vector<double> y;
            double slope=1, offset=0;
            double sumX=0, sumY=0, sumXY=0, sumX2=0;
            for(int bit =0; bit<m_number_bits; bit++){
                double theMean = m_mean[bit][fec][hdmi][0][chip][ch];
                double theTime = bit*3.125;
                //std::cout << ch << " " << bit << " " << theTime <<  " " << theMean << std::endl;
                x.push_back(theTime);
                y.push_back(theMean);
                sumX += theTime;
                sumY += theMean;
                sumXY += theTime * theMean;
                sumX2 += theTime * theTime;
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
                //std::cout << ch << " " << slope << " " << offset << std::endl;
                if(slope > 0)
                {
                    slope = 1/slope;
                }
                else
                {
                    slope = 1;
                    offset = 0;
                }
            }
            else
            {
                slope = 1;
                offset = 0;
            }
            m_slope_time[fec][hdmi][0][chip].push_back(slope);
            m_offset_time[fec][hdmi][0][chip].push_back(offset);
            slope = std::round(1000*slope)/1000;
            offset = std::round(1000*offset)/1000;
            slopeArray.push_back(slope);
            offsetArray.push_back(offset);

        }
        calibrationObject.insert("fecID",fec);
        calibrationObject.insert("vmmID",hdmi*2+chip);
        calibrationObject.insert("offsets",offsetArray);
        calibrationObject.insert("slopes",slopeArray);
        calibrationArray.push_back(calibrationObject);
    }
    m_jsonObject->insert("vmm_calibration",calibrationArray);



}

void CalibrationModule::PlotData(){

    std::cout << "PLOT" << std::endl;
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

        if(fec == theFec && hdmi >= selectedRangeStart && hdmi <= selectedRangeStop)
        {
            QCustomPlot *plot = plotVector[hdmi*2+chip];

            plot->clearGraphs();
            // give the axes some labels:
            plot->xAxis->setLabel("channel");
            // set axes ranges, so we see all data:
            plot->xAxis->setRange(0, 66);
            QPen pen(Qt::red,3,Qt::SolidLine);

            QString title = "VMM " + QString::number(hdmi*2+chip);
            double colorFactor = 0;
            if(m_calibMode == "ADC")
            {
                title+= ": Mean ADC";
                colorFactor = 5;
            }
            else if(m_calibMode == "TDC") {
                title+= ": Mean TDC";
                colorFactor = 10;
            }
            else if(m_calibMode == "Channels")
            {
                title+= " Counts";
            }
            else if(m_calibMode == "BCID") {
                title+= ": Mean BCID";
            }
            else if(m_calibMode == "Time") {
                title+= ": Mean time";
                colorFactor = 10;
            }
            else if(m_calibMode == "Threshold") {
                title+= ": Threshold";
                colorFactor = 5;
            }
            else if(m_calibMode == "Pedestal") {
                title+= ": Pedestal";
            }
            if(m_runMode == "User" )
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
                if(m_calibMode != "Time")
                {


                    for(int bit=0; bit<m_number_bits;bit++){
                        plot->addGraph();
                        plot->graph(bit)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                        plot->graph(bit)->setPen(QPen(QColor(bit*colorFactor)));
                        plot->legend->removeItem(plot->legend->itemCount()-1);
                    }
                    int n_graph =plot->graphCount();
                    plot->addGraph();
                    plot->graph(n_graph)->setPen(QPen(Qt::red,4,Qt::SolidLine));
                    plot->graph(n_graph)->setName("Best common value");
                    plot->graph(n_graph)->setData( QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_y));
                    plot->addGraph();
                    plot->graph(n_graph+1)->setData(QVector<double>::fromStdVector(m_x), QVector<double>::fromStdVector(m_calVal[fec][hdmi][0][chip] ));
                    plot->graph(n_graph+1)->setPen(QPen(Qt::green,4,Qt::SolidLine));
                    plot->graph(n_graph+1)->setName("Calibrated curve");
                }
                else
                {
                    int colorFactor=10;
                    for(int bit=0; bit<m_number_bits;bit++){
                        plot->addGraph();
                        plot->graph(2*bit)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(m_mean[bit][fec][hdmi][0][chip]));
                        plot->graph(2*bit)->setPen(QPen(QColor(bit*colorFactor)));
                        plot->legend->removeItem(plot->legend->itemCount()-1);
                        std::vector<double> y_time;
                        for(int ch=0; ch<64; ch++)
                        {
                            y_time.push_back((m_mean[bit][fec][hdmi][0][chip][ch] - m_offset_time[fec][hdmi][0][chip][ch])*  m_slope_time[fec][hdmi][0][chip][ch]);
                            //std::cout << ch << " " << m_mean[bit][fec][hdmi][0][chip][ch] << std::endl;
                        }
                        plot->addGraph();
                        plot->graph(bit*2+1)->setData(
                                    QVector<double>::fromStdVector(m_x),
                                    QVector<double>::fromStdVector(y_time));

                        plot->graph(bit*2+1)->setPen(QPen(Qt::red,1,Qt::SolidLine));
                        plot->legend->removeItem(plot->legend->itemCount()-1);
                    }

                }
                plot->yAxis->setLabel(title);
                plot->legend->setVisible(true);
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

void CalibrationModule::TakeData(){
    m_eventcount = 0 ;
    m_errorcount=0;
    m_numHits = 0;
    m_bitCount=0;
    m_lastUdpTimeStamp=0;
    m_isCalibratedADC = false;
    m_isCalibratedTDC = false;
    m_isCalibratedThreshold = false;
    m_isCalibratedTime = false;
    m_dataAvailable = false;
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

    DoCalibrationStep();



}

// ------------------------------------------------------------------------ //

void CalibrationModule::DoCalibrationStep(){
    emit m_mainWindow->m_daqWindow->ui->openConnection_2->clicked();usleep(1000);
    if(! (m_mainWindow->m_daqWindow->ui->connectionLabel_2->text()==QString("all alive"))) {
        std::cout<<"Communication couldn't be established! \n exit calibration"<<std::endl;
        m_mainWindow->m_daqWindow->ui->InfoScreen->setTextColor(Qt::red);
        m_mainWindow->m_daqWindow->ui->InfoScreen->append(QString("Communication couldn't be established! \n exit calibration"));
        m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(true);
        return;
    }

    //Calibration
    if(IsCalibration())
    {
        std::map<std::string, unsigned short> m_high;

        if(m_calibMode == "ADC")
        {
            m_high.emplace("st", 1);
            m_high.emplace("ADC0_10", m_bitCount);
        }
        else if(m_calibMode == "TDC")
        {
            m_high.emplace("st", 1);
            m_high.emplace("ADC0_8", m_bitCount);
        }

        for(unsigned int vmm=0; vmm<m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            if(m_calibMode == "Time")
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].SetReg("TP_skew", m_bitCount+2);
                m_high.emplace("st", 1);
                usleep(1000);
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].LoadDefault(true, m_high );
            }

            else if(m_calibMode == "ADC" || m_calibMode == "TDC")
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].LoadDefault(true, m_high );
            }

        }
    }
    if(m_calibMode == "Threshold")
    {
        CalibrateThreshold(true);
        return;
    }
    if(m_calibMode == "Pedestal")
    {
        CalibrateThreshold(false);
        return;
    }
    m_mainWindow->m_daqWindow->ui->checkBoxGlobalDAQ->setChecked(true);
    emit m_mainWindow->m_daqWindow->ui->checkBoxGlobalDAQ->stateChanged(true);
    emit m_mainWindow->m_daqWindow->ui->trgPulser->clicked();usleep(1000);
    emit m_mainWindow->m_daqWindow->ui->onACQ->clicked();usleep(1000);
    ConnectDAQSocket();


}

// ------------------------------------------------------------------------ //
void CalibrationModule::AccumulateData(){
    stringstream sx;
    if(m_eventcount < m_mainWindow->m_daqWindow->ui->Runs->value()){
        if(m_lastUdpTimeStamp != 0 && m_lastUdpTimeStamp != m_commonData.m_udpTimeStamp)
        {
            m_eventcount++;
        }
    }
    else{
        std::cout << "Finished calibration acquisition!" << std::endl;
        m_eventcount=0;
        m_numHits=0;
        emit m_mainWindow->m_daqWindow->ui->offACQ->clicked();usleep(1000);
        CloseDAQSocket();
        m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
        m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
        if(m_bitCount == m_number_bits-1){
            m_dataAvailable = true;
            if(m_calibMode == "ADC")
            {
                m_isCalibratedADC= true;
            }
            else if(m_calibMode == "TDC")
            {
                m_isCalibratedTDC= true;
            }
            else if(m_calibMode == "Time")
            {
                m_isCalibratedTime= true;
            }
            for(int bit =0; bit<m_number_bits; bit++){

                for(int vmm =0; vmm < m_vmmActs.size(); vmm++){
                    int fec = GetFEC(vmm);
                    int hdmi = GetHDMI(vmm);
                    int chip = GetVMM(vmm);

                    for(unsigned int ch = 0; ch<64; ch++){

                        int size = m_data[bit][fec][hdmi][0][chip][ch].size();
                        if(m_calibMode == "Channels")
                        {
                            size = 1;
                        }
                        double mean = -1;
                        if(size>0)
                        {
                            mean = std::accumulate(m_data[bit][fec][hdmi][0][chip][ch].begin(),m_data[bit][fec][hdmi][0][chip][ch].end(),0.0)/size;
                        }
                        m_mean[bit][fec][hdmi][0][chip].push_back(mean);

                    }

                }

            }
            if(m_runMode == "Calibration")
            {
                if(m_calibMode == "Time")
                {
                    FitTimeData();
                }
                else
                {
                    CalculateCorrections();
                }
            }
            PlotData();

        }
        else{
            m_bitCount+=1;
            usleep(1000);DoCalibrationStep();usleep(1000);
        }


    }


}




void CalibrationModule::CalculateCorrections(){
    std::cout << "CalculateCorrections" << std::endl;
    stringstream sx;
    // Function to calculate the best common ADC or TDC value for all channels
    vector<double> sorted_0mV;
    vector<double> sorted_MAXmV;
    if(!IsCalibration() || !CheckModes())
    {
        return;
    }
    if(m_calibMode != "ADC" && m_calibMode != "TDC" && m_calibMode != "Threshold")
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
        std::sort(sorted_0mV.begin(), sorted_0mV.end());
        std::sort(sorted_MAXmV.begin(), sorted_MAXmV.end());
        int last = sorted_MAXmV.size()-1;
        int state = 0;
        int check = 0;
        while(sorted_0mV[check-state] < 0 || (sorted_0mV[check-state]-sorted_MAXmV[last-check]) < 0 ){
            if(state == 0){
                state =1;
                check+=1;
            }
            else if(state == 1){
                state =0;
            }

        }
        if(IsDbgActive()) {
            sx <<"Best common value "<<(sorted_0mV[check-state]+sorted_MAXmV[last-check])/2<<" value of high: "<< sorted_MAXmV[check-state]<<", value of low "<<sorted_0mV[last-check]<<std::endl;
            GetMessageHandler()(sx,"calibration_module::AccumulateData"); sx.str("");
        }

        double val =  (sorted_0mV[check-state]+sorted_MAXmV[last-check])/2;

        for(unsigned int ch =0; ch<64; ch++){
            m_y.push_back(val);
        }
        for(unsigned int vmm=0; vmm<m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            for(unsigned int ch =0; ch<64; ch++){
                double smallestDifference = 9999.;
                int bin_number = 0;
                double mean = 0;
                for(int bit = 0; bit<m_number_bits; bit++){
                    if(m_mean[bit][fec][hdmi][0][chip][ch]>=0)
                    {
                        double difference = pow(pow( m_y[ch] - m_mean[bit][fec][hdmi][0][chip][ch],2),0.5);
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



void CalibrationModule::SetCorrections(){
    if(!IsCalibration())
    {
        return;
    }
    if(m_calibMode == "Time")
    {
        QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
        QString name = "offset_slope";
        for(int vmm=0; vmm<m_vmmActs.size(); vmm++){
            int fec = GetFEC(vmm);
            int hdmi = GetHDMI(vmm);
            int chip = GetVMM(vmm);
            name += "_FEC" + QString::number(fec);
            name += "_VMM" + QString::number(hdmi*2+chip);

        }
        name += ("_" + theDate + ".json");
        QJsonDocument doc(*m_jsonObject);
        QFile jsonFile(name);
        jsonFile.open(QFile::WriteOnly);
        jsonFile.write(doc.toJson(QJsonDocument::JsonFormat::Compact));

    }
    else
    {
        m_mainWindow->m_daqWindow->LoadConfig("Calib_config");
        for(int vmm=0; vmm<m_vmmActs.size(); vmm++){
            for(int ch=0; ch<64; ch++)
            {
                int fec = GetFEC(vmm);
                int hdmi = GetHDMI(vmm);
                int chip = GetVMM(vmm);
                if(m_calibMode == "ADC")
                {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_10", m_bitVal[fec][hdmi][0][chip][ch]  , ch );

                }
                else if(m_calibMode == "TDC") {

                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "ADC0_8", m_bitVal[fec][hdmi][0][chip][ch]  , ch );
                }
                else if(m_calibMode == "Threshold") {
                    m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi( "sd", m_bitVal[fec][hdmi][0][chip][ch]  , ch );

                }
            }

        }
        m_mainWindow->m_daqWindow->ui->line_configFile->setText("Calib_config");
        emit m_mainWindow->m_daqWindow->ui->Button_save->clicked();usleep(1000);
        m_mainWindow->m_daqWindow->ui->line_configFile->setText("");
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

    if(m_udpSocket->state() == QAbstractSocket::UnconnectedState) {
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

    if(m_runMode == "User")
    {

        if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 0){
            m_calibMode = "ADC";
            m_number_bits = 1;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 1){
            m_calibMode = "TDC";
            m_number_bits = 1;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 2){
            m_calibMode = "BCID";
            m_number_bits = 1;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 3){
            m_calibMode = "Time";
            m_number_bits = 1;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 4)
        {
            m_calibMode = "Channels";
            m_number_bits = 1;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 5){
            m_calibMode = "Threshold";
            m_number_bits = 1;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 6){
            m_calibMode = "Pedestal";
            m_number_bits = 1;
        }
    }
    else
    {
        if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 0){
            m_calibMode = "ADC";
            m_number_bits = m_number_bits_adc;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 1){
            m_calibMode = "TDC";
            m_number_bits = m_number_bits_tdc;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 2){
            m_calibMode = "Time";
            m_number_bits = m_number_bits_time-3;
        }
        else if(m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->currentIndex() == 3){
            m_calibMode = "Threshold";
            m_number_bits = m_number_bits_threshold;
        }

    }
}


void CalibrationModule::InitializeDataStructures()
{
    mapIPFecId.clear();
    mapIPFirmware.clear();
    m_x.clear();
    m_y.clear();

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


                            m_offset_time[fec][hdmi][hybrid][vmm].clear();
                            m_slope_time[fec][hdmi][hybrid][vmm].clear();
                            m_calVal[fec][hdmi][hybrid][vmm].clear();
                            m_bitVal[fec][hdmi][hybrid][vmm].clear();

                        }
                        m_mean[bit][fec][hdmi][hybrid][vmm].clear();
                        for (unsigned int ch=0; ch < 64; ch++){
                            m_data[bit][fec][hdmi][hybrid][vmm][ch].clear();

                        }
                    }
                }
            }
        }
    }

}




void CalibrationModule::CalibrateThreshold(bool modeThreshold)
{
    stringstream sx;
    for(unsigned int vmm=0; vmm<m_vmmActs.size(); vmm++){
        int fec = GetFEC(vmm);
        int hdmi = GetHDMI(vmm);
        int chip = GetVMM(vmm);

        for(unsigned int ch = 0; ch<64; ch++){
            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("st",0,ch);
            if(modeThreshold)
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("smx",1,ch);
            }
            else
            {
                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("smx",0,ch);
            }

            m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("monitoring",std::to_string(ch));
            for(int bit=0;bit<m_number_bits;bit++)
            {

                m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[0].m_vmms[chip].SetRegi("sd",bit,ch);
                m_mainWindow->m_daqs[0].SendAll();
                usleep(1000);
                for(int n=0; n<1;n++)
                {
                    int threshold =  m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->ReadADC(hdmi, 0,vmm, 2);
                    usleep(100);
                    if(!IsDbgActive() && hdmi==5 && chip==1 && ch == 0 ) {
                        if(modeThreshold)
                        {
                            sx << fec << " " << hdmi << " " << vmm << " " << ch << " " << bit << ": Threshold = " << threshold<< std::endl;
                        }
                        else
                        {
                            sx << fec << " " << hdmi << " " << vmm << " " << ch << " " << bit << ": Pedestal = " << threshold<< std::endl;
                        }
                        GetMessageHandler()(sx,"calibration_module::CalibrateThreshold"); sx.str("");
                    }

                    m_data[bit][fec][hdmi][0][chip][ch].push_back(threshold);


                }
                double mean = -1;
                int size = m_data[bit][fec][hdmi][0][chip][ch].size();
                if(size>0)
                {
                    mean = std::accumulate(m_data[bit][fec][hdmi][0][chip][ch].begin(),m_data[bit][fec][hdmi][0][chip][ch].end(),0.0)/size;
                }
                m_mean[bit][fec][hdmi][0][chip].push_back(mean);


            }
        }
    }
    m_dataAvailable = true;
    if(modeThreshold)
    {
        m_isCalibratedThreshold= true;
    }


    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
    CalculateCorrections();
    PlotData();
}

// ------------------------------------------------------------------------ //
void CalibrationModule::Receive(const char* buffer, int size, QString ip)
{
    int oldNumHits = m_numHits;
    stringstream sx;
    m_lastUdpTimeStamp = m_commonData.m_udpTimeStamp;
    if(mapIPFirmware[ip] == "0002")
    {
        m_numHits += Receive_VMM3(buffer, size, mapIPFecId[ip]);

    }
    else
    {
        m_numHits += Receive_VMM2(buffer, size, mapIPFecId[ip]);
    }

    if(m_numHits-oldNumHits > 0)
    {
        m_errorcount = 0;
        if(IsDbgActive()) {
            sx << "Acquired " << m_numHits-oldNumHits << " hits" << "\n";
            GetMessageHandler()(sx,"calibration_module::Receive"); sx.str("");
        }
        AccumulateData();
    }
    else
    {
        m_errorcount++;
    }
    if(m_errorcount>100000)
    {
        std::cout << "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX Too many errors!" << std::endl;
        m_errorcount = 0;
        emit m_mainWindow->m_daqWindow->ui->offACQ->clicked();usleep(1000);
        CloseDAQSocket();
        m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
        m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setCheckable(false);
    }
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
            sx << "SRS Data fecId " << (int)m_commonData.m_fecId << ", vmmId " << vmmid << ", chNo: " <<  chNo
               << ", bcid: " <<  bcid
               << ", tdc: " <<  tdc
               << ", adc: " <<  adc
               << ", overThreshold: " <<  overThreshold
               << ", triggerOffset: " <<  triggerOffset
               << "\n";
            GetMessageHandler()(sx,"calibration_module::Parse_VMM3"); sx.str("");
        }
        auto it = find (m_BCID.begin(), m_BCID.end(), bcid);
        if (m_BCID.empty() || it != m_BCID.end())
        {
            int hdmi = vmmid/2;
            int chip = vmmid%2;
            double tac_slope_ns = m_tac_slope[fecId][hdmi][0][chip];
            double bc_period_ns = m_bc_period[fecId][hdmi][0];
            int theBcid = bcid;
            if(!m_BCID.empty())
            {
                theBcid = bcid-m_BCID[0];
            }
            double theTime =  ((theBcid+1)*bc_period_ns - tdc*tac_slope_ns/255);

            if(m_calibMode == "ADC")
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(adc);
            }
            if(m_calibMode == "TDC")
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(tdc);
            }
            if(m_calibMode == "BCID")
            {
                m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(bcid);
            }
            if(m_calibMode == "Channels")
            {
                m_data[0][fecId][hdmi][0][chip][chNo].push_back(1);
            }
            if(m_calibMode == "Time")
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

    if (ntohl(hdr->m_frameCounter) == 0xfafafafa) {
        if(IsDbgActive()) {
            sx.str("");
            sx << "End of frame!";
            GetMessageHandler()(sx,"calibration_module::Receive_VMM3"); sx.str("");
        }
        return 0;
    }
    m_commonData.m_lastFrameCounter = m_commonData.m_frameCounter;
    m_commonData.m_frameCounter = ntohl(hdr->m_frameCounter);

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
    if (m_BCID.empty() || it != m_BCID.end())
    {
        int hdmi = vmmid/2;
        int chip = vmmid%2;
        double tac_slope_ns = m_tac_slope[fecId][hdmi][0][chip];
        double bc_period_ns = m_bc_period[fecId][hdmi][0];
        int theBcid = bcid;
        if(!m_BCID.empty())
        {
            theBcid = bcid-m_BCID[0];
        }


        if(m_calibMode == "ADC")
        {
            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(adc);
        }
        if(m_calibMode == "TDC")
        {
            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(tdc);
        }
        if(m_calibMode == "BCID")
        {
            m_data[m_bitCount][fecId][hdmi][0][chip][chNo].push_back(bcid);
        }
        if(m_calibMode == "Channels")
        {
            m_data[0][fecId][hdmi][0][chip][chNo].push_back(1);
        }
        if(m_calibMode == "Time")
        {
            double theTime =  ((theBcid+1)*bc_period_ns - tdc*tac_slope_ns/255);
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
