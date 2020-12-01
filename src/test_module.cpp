#include "test_module.h"
#include "currentmonitor.h"
#include <set>
#include <QThread>
#include <QTimer>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QSerialPort>
#include <QSerialPortInfo>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <PolynomialRegression.h>

/*Constructor of TestModule: A new MessageHandler is used and connected to the Test Log Window on the UI
 The calibration module of the mainWindow gets loaded, the Message Handler of this test module gets passed
 to it.

*/
TestModule::TestModule(MainWindow *top, QObject *parent) :
    QObject(parent), m_mainWindow{top}, m_dbg(true)
{
    m_msg = new MessageHandler();
    m_msg->SetMessageSize(75);
    m_msg->SetGUI(true);
    LoadCalibrationModule(*m_mainWindow->m_calib);
    m_calibmod->LoadMessageHandler(*m_msg);
    connect(m_msg, SIGNAL(on_log_ready()), this, SLOT(on_readTestLog()));
    connect(m_mainWindow->m_daqWindow->ui->onACQ, SIGNAL(clicked()), this, SLOT(acqON()));
    connect(m_mainWindow->m_daqWindow->ui->offACQ, SIGNAL(clicked()), this, SLOT(acqOFF()));
    //connect(m_currentMonitor,SIGNAL(finished()),this,SLOT(handleCurrents()));
    m_mainWindow->m_daqWindow->ui->checkBox_readcurrent->setChecked(true);
    plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlotVMM1);

    //plotVector.push_back(m_mainWindow->m_daqWindow->ui->customPlotVMM2);
    for(int i=0; i< plotVector.size();i++)
    {
        QCustomPlot *plot = plotVector[i];
        plot->clearItems();
        plot->clearPlottables();
        plot->clearGraphs();
        plot->yAxis->setLabel("");
        plot->xAxis->setLabel("");
        plot->legend->setVisible(false);
        plot->xAxis->setRange(0,1024);
        plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
        plot->replot();
    }

    m_db = QSqlDatabase::addDatabase("QPSQL");
    readSettingFile();
}

void TestModule::LoadCalibrationModule(CalibrationModule& calibmod){
    m_calibmod = &calibmod;
}

void TestModule::ResetDataStructures(){
    for(int i=0;i<2;i++){
        for(int j=0; j<2;j++){
            m_hResults.h_nwchannels[i][j]=64;
        }
    }
    //m_hResults.h_plotsdata.clear();
    //m_hResults.h_plotslabel.clear();
    //m_hResults.h_logs.clear();
    //m_currents.clear();
    //m_temps[0].clear();
    //m_temps[1].clear();
}

double TestModule::evaluateADCCalibration(QVector<double> dataset, QString caller){
    stringstream sx;
    int size=dataset.size();
    double high = 1000;
    if(QString::compare(caller,"internal",Qt::CaseInsensitive)==0){
        high = 375;
    }
    else if(QString::compare(caller,"external",Qt::CaseInsensitive)==0){
        high = 550;
    }

    double avgmid = 0;
    double avgside = 0;
    int mid = size/2 -1;
    int n_s=0;
    int n_m = 1;
    int i=0;
    while(n_s<=10&&i<31){
        if(dataset[i]!=0 && dataset[i]<high){
            n_s++;
            avgside += dataset[i];
        }
        if(dataset[size-i-1]!=0 && dataset[size-i-1]<high){
            n_s++;
            avgside += dataset[size-i-1];
        }
        i++;
    }
    i=1;
    if(dataset[mid]!=0 && dataset[mid]<high){
        avgmid += dataset[mid];
        n_m++;
    }
    while(n_m<=10&&i<31){
        if(dataset[mid+i]!=0 && dataset[mid+i]<high){
            n_m++;
            avgmid+= dataset[mid+i];
            }
        if(dataset[mid-i]!=0 && dataset[mid-i]<high){
            n_m++;
            avgmid += dataset[mid-i];
            }
        i++;
        }
    if(n_m==0){n_m=1;}
    if(n_s==0){n_s=1;}
    avgmid/=n_m;
    avgside/=n_s;
    double diff = avgmid-avgside;
    sx << "side: " <<avgside << " mid: " << avgmid <<" diff: " << diff <<endl;
    return diff;


    GetMessageHandler()(sx,"TestModule::evaluateADCCalibration");
    //QString name ="averageaverage"+caller;
    //PlotData(chans,averaged,"Channel","averaged average adc",name);
}

QVector<double> TestModule::evaluateADCCalibrationFit(QVector<double> datax, QVector<double> datay, QString caller, int chip, QVector<double> &params)
{
    //remove Datapoints with y=0 to not mess up to fit
    QString chanmarkstring = "ADCCalibration"+caller;
    QVector<double> xclean;
    QVector<double> yclean;
    double curveideal;
    double curvestd;
    double centerideal;
    double centerstd;
    double maxheightideal;
    double maxheightstd;
    double posrangeup;
    double posrangelow;
    QString resultinsert1;
    QString resultinsert2;
    QLabel* labels[6];
    bool failedMeasurement =false;
    double good_dev = m_mainWindow->m_daqWindow->ui->adcrange_g_l->text().toDouble();
    double ok_dev = m_mainWindow->m_daqWindow->ui->adcrange_o_l->text().toDouble();
    if(QString::compare(caller,"Internal",Qt::CaseInsensitive)==0){
        curveideal = -0.0604396074478;
        curvestd = 0.00896593791387;
        centerideal = 31.3400625589;
        centerstd = 1.55508392126;
        maxheightideal = 313.702862948;
        maxheightstd = 16.2023985974;
        getCurveSettings("Internal",curveideal,curvestd);
        cout << curveideal <<"," << curvestd <<","<< centerideal<<","<<centerstd<<endl;
        resultinsert1 = "adcintcurvature";
        resultinsert2 = "adcintvertex";
        labels[0]=m_mainWindow->m_daqWindow->ui->adccurvestatus1label;
        labels[1]=m_mainWindow->m_daqWindow->ui->adccurvestatus2label;
        labels[2]=m_mainWindow->m_daqWindow->ui->adcvertexstatus1label;
        labels[3]=m_mainWindow->m_daqWindow->ui->adcvertexstatus2label;
        labels[4]=m_mainWindow->m_daqWindow->ui->adccal_bads1;
        labels[5]=m_mainWindow->m_daqWindow->ui->adccal_bads2;
        posrangelow = m_mainWindow->m_daqWindow->ui->adcvertex_g_i->text().toDouble();;
        posrangeup = m_mainWindow->m_daqWindow->ui->adcvertex_o_i->text().toDouble();
    }
    else if(QString::compare(caller,"External",Qt::CaseInsensitive)==0){
        curveideal = -0.070142162122;
        curvestd = 0.00628598041229;
        centerideal = 31.6371319652;
        centerstd = 2.17699310923;
        maxheightideal = 426.371640739;
        maxheightstd = 18.5010979906;
        getCurveSettings("External",curveideal,curvestd);
        cout << curveideal <<"," << curvestd <<","<< centerideal<<","<<centerstd<<endl;
        resultinsert1 = "adcextcurvature";
        resultinsert2 = "adcextvertex";
        labels[0]=m_mainWindow->m_daqWindow->ui->adccurvestatus3label;
        labels[1]=m_mainWindow->m_daqWindow->ui->adccurvestatus4label;
        labels[2]=m_mainWindow->m_daqWindow->ui->adcvertexstatus3label;
        labels[3]=m_mainWindow->m_daqWindow->ui->adcvertexstatus4label;
        labels[4]=m_mainWindow->m_daqWindow->ui->adccal_bads1;
        labels[5]=m_mainWindow->m_daqWindow->ui->adccal_bads2;
        posrangelow = m_mainWindow->m_daqWindow->ui->adcvertex_g_e->text().toDouble();
        posrangeup = m_mainWindow->m_daqWindow->ui->adcvertex_o_e->text().toDouble();
    }
    for(int i=0;i<6;i++){
        m_labelstoclear.push_back(labels[i]);
    }
    for(int i=0;i<datax.size();i++){
        if(datay[i]!=0 && datay[i]<800){
            xclean.push_back(datax[i]);
            yclean.push_back(datay[i]);
        }
    }
    if(xclean.size()==0){
        for(int i=0;i<datax.size();i++){
            xclean.push_back(datax[i]);
            yclean.push_back(datay[i]);
        }
        failedMeasurement=true;
    }
    const int order = 2;
    std::vector<double> results;
    PolynomialRegression<double> *fitter = new PolynomialRegression<double>();
    fitter->fitIt(xclean.toStdVector(),yclean.toStdVector(),order,results);
    for(int i=0;i<results.size();i++){
        cout << results[i]<<endl;
        params.push_back(results[i]);
    }
    QVector<double> yexpected;
    int size = datax.size();
    QVector<double> ploty [2];
    double peakpos = results[1]/(-2*results[2]);
    double curvature = results[2];
    double peakheight = results[0]-pow(results[1],2)/(4*results[2]);
    if(failedMeasurement){
        peakpos =0;
        curvature = 0;
        peakheight = 0;
    }
    cout << curvature <<"," << peakpos <<"," << peakheight <<endl;
    bool markedcurve = false;
    bool markedpos = false;
    if(fabs(curvature)>=fabs(curveideal)){
        if(fabs(curvature-curveideal)>=1.5*curvestd){
            m_hResults.h_VMMResults[chip].insert(resultinsert1,2);
            labels[chip]->setStyleSheet("background-color: red");
            markedcurve =true;
        }
    }
    else{
        if(fabs(curvature-curveideal)>=2.5*curvestd){
            m_hResults.h_VMMResults[chip].insert(resultinsert1,2);
            labels[chip]->setStyleSheet("background-color: red");
            markedcurve =true;
        }
    }
    if(!(peakpos>=posrangelow&&peakpos<=posrangeup)){
        m_hResults.h_VMMResults[chip].insert(resultinsert2,2);
        labels[chip+2]->setStyleSheet("background-color: red");
        markedpos =true;
    }
    if(fabs(peakheight-maxheightideal)>3*maxheightstd){
        m_hResults.h_VMMResults[chip].insert(resultinsert2,2);
        labels[chip+2]->setStyleSheet("background-color: red");
        markedpos =true;
    }

    m_hResults.h_DBExportStrings[chip].insert(resultinsert1,QString::number(curvature));
    m_hResults.h_DBExportStrings[chip].insert(resultinsert2,QString::number(peakpos));
    if(markedcurve == false && markedpos==false ){
        yexpected.clear();
        for(int i=0;i<datax.size();i++){
            double x = datax[i];
            double yexp = results[2]*pow(x,2)+results[1]*x+results[0];
            yexpected.push_back(yexp);
        }
    }
    else if(markedcurve == true || markedpos==true){
        yexpected.clear();
        for(int i=0;i<datax.size();i++){
            double x = datax[i];
            double yexp = curveideal*pow((x-centerideal),2)+maxheightideal;
            yexpected.push_back(yexp);
        }
    }
    if(markedcurve == false){
        labels[chip]->setStyleSheet("background-color: lightgreen");
    }
    if(markedpos == false){
        labels[chip+2]->setStyleSheet("background-color: lightgreen");
    }
    labels[chip]->setText(QString::number(curvature,'g',4)+"mV/Ch²");
    labels[chip+2]->setText("(Ch"+QString::number(peakpos,'g',3)+","+QString::number(peakheight,'g',4)+"mV)");
    double diffs = yexpected[size/2-1]-((yexpected[0]+yexpected[size-1])/2);
    cout <<diffs<<endl;
    ploty[0]=datay;
    ploty[1]=yexpected;
    QString badchans = "";
    int n_bad =0;
    for(int i=0;i<64;i++){
        if(datay[i]==0){
            MarkChannel(chanmarkstring,chip,i,"bad");
            badchans += QString::number(i)+", ";
            n_bad++;
        }
        else if(fabs(datay[i]-yexpected[i])<good_dev){
            MarkChannel(chanmarkstring,chip,i,"good");
        }
        else if(fabs(datay[i]-yexpected[i])<ok_dev){
            MarkChannel(chanmarkstring,chip,i,"ok");
        }
        else{
            MarkChannel(chanmarkstring,chip,i,"bad");
            badchans += QString::number(i)+", ";
            n_bad++;
        }
    }
    badchans.chop(2);
    labels[chip+4]->setText(badchans);
    labels[chip+4]->setToolTip(badchans);
    if(n_bad>0){
        labels[chip+4]->setStyleSheet("background-color: red");
    }
    else{
        labels[chip+4]->setStyleSheet("background-color: lightgreen");
    }
    //PlotData(datax,ploty,"Channel","ADC","ADC Fit","",2);
    return ploty[1];

}


bool TestModule::MaskNoisyChannels(){
    handleTemperatures();
    stringstream sx;
    VMM* vmm[2];
    vmm[0] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0];
    vmm[1] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1];
    int runmode = m_mainWindow->m_daqWindow->ui->comboBoxRunMode->findText("user settings");
    m_mainWindow->m_daqWindow->ui->comboBoxRunMode->setCurrentIndex(runmode);
    int calibtype = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->findText("Channels");
    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(calibtype);
    m_mainWindow->m_daqWindow->ui->Runs->setValue(100);
    m_calibmod->StartCalibration();
    QVector<double> chans;
    while(!(m_calibmod->m_dataAvailable)){
        QCoreApplication::processEvents();
    }
    QVector<double> hits[VMMS_PER_HYBRID] = {QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][0]),QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][1])};
    for(int j=0;j<2;j++){
        for(int i =0;i<64;i++){
            if(hits[j][i]>100){
                vmm[j]->SetRegi("sm",1,i);
                sx <<"Masked Channel " << i << " on VMM " << j<<endl;
            }
        }
    }
    for(int i =0;i<64;i++){
        chans.push_back(i);
    }
    PlotData(chans,hits,"Channel","Hits","maskingtest");
    GetMessageHandler()(sx,"TestModule::MaskNoisyChannels");
    handleTemperatures();
    return true;
}

void TestModule::exportConfig(QString filename)
{
    QString DBFilesPath = readSettingsDB();
    QSqlDatabase db =m_db;
    //QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName("/home/fjaekel/VMM-software-RD51/build/testdb.db");
    db.open();
    //QSqlQuery creationquery;
    //creationquery.exec("CREATE TABLE [IF NOT EXISTS] Hybrid_Overview(Hybrid_ID text,Measurement_ID text,Hybrid_Class text,VMM int,VMM_Class text,BrokenChannels int[],Location text,Date date,MonitorADCSlope real,MonitorADCIntercept real,Baseline real,BaseNoise real,Threshold real,WorkingChannelsExt int,WorkingChannelsInt int,AvgCurr1_9V real,AvgCurr2_9V real,ADCExtCurvature real,ADCIntCurvature real,ADCExtVertex real, ADCIntVertex real,AVGTDC real,PRIMARY KEY(Measurement_ID, VMM));CREATE TABLE [IF NOT EXISTS] Hybrid_Data(Measurement_ID text PRIMARY KEY,Hybrid_ID text,Date date,Measurement_type text,labels text []);CREATE TABLE [IF NOT EXISTS] configs(Measurement_ID text,Date date,Filename text,Filecontent text,PRIMARY KEY(Measurement_ID,Filename));CREATE VIEW [IF NOT EXISTS] latest_measurements as SELECT * from hybrid_overview WHERE measurement_id IN (SELECT max FROM(SELECT hybrid_id,MAX(measurement_id) FROM hybrid_overview GROUP BY hybrid_id) as foo);");
    QSqlQuery query;
    QString path1 = m_mainWindow->GetApplicationPath();
    QDir dir(path1);
    QDir dbdir(DBFilesPath);
    QString measid = filename;
    QFile DBentry(dbdir.filePath(measid+".txt"));
    DBentry.open(QIODevice::Append|QIODevice::Text);
    QString queryforfile;
    stringstream sx;
    std::string fname = filename.toStdString();
    m_mainWindow->m_vmmConfigHandler->WriteAllVMMConf(fname);
    m_mainWindow->m_hybridConfigHandler->WriteAllHybridConf(fname);
    m_mainWindow->m_fecConfigHandler->WriteAllFECConf(fname);
    fname+=".txt";
    m_mainWindow->m_daqConfigHandler->WriteDAQConf(fname.c_str());

    QString path = m_mainWindow->GetApplicationPath();
    path+="/../";
    path+=CONFIG_DIR;
    path+="/";
    QStringList filters;
    filters << filename<<"*";
    QDir filesToExport(path);
    filesToExport.setNameFilters(filters);
    QFileInfoList list = filesToExport.entryInfoList();
    for(int i=0;i<list.size();++i){
        QFileInfo fileInfo = list.at(i);
        QString name = fileInfo.fileName();
        if(name.contains(measid)){
            sx << fileInfo.fileName().toStdString()<< endl;
            QFile file(filesToExport.filePath(fileInfo.filePath()));
            QString filecontents ="";
            if(file.open(QIODevice::ReadOnly|QIODevice::Text)){
                while(!file.atEnd()){
                    QString line = file.readLine();
                    filecontents +=line;
                }
            }
            QString querystr = "INSERT INTO configs(Measurement_ID,date,filename,filecontent) VALUES(?,?,?,?);";
            queryforfile+="INSERT INTO configs(Measurement_ID,date,filename,filecontent) VALUES('";
            queryforfile += measid;
            queryforfile += "','" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
            queryforfile += "','" + name;
            queryforfile += "','" +filecontents+"');\n";
            query.prepare(querystr);
            query.bindValue(0,measid);
            query.bindValue(1,"now()");
            query.bindValue(2,name);
            query.bindValue(3,filecontents);
            query.exec();
            filesToExport.remove(name);
        }
    }
    QTextStream out(&DBentry);
    out << queryforfile;
    return;

}

void TestModule::FitLinear(QVector<double> x, QVector<double> y, double &slope, double &intercept)
{
    double xavg = getAverage(x);
    double yavg = getAverage(y);
    double xysum=0;
    double xxsum=0;
    for(int i=0;i<x.size();i++){
        xysum += (x[i]-xavg)*(y[i]-yavg);
        xxsum += pow((x[i]-xavg),2);
    }
    slope = xysum/xxsum;
    intercept = yavg - slope*xavg;
}

void TestModule::readSettingFile()
{
    QString path = m_mainWindow->GetApplicationPath()+"/../testing/testsettings.txt";
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        return;
    }
    else{
        while(!file.atEnd()){
            QString line = QString(file.readLine());
            line.remove('\n');
            QStringList splitted = line.split(QRegExp("\\s+")); //split whitespace
            if(splitted[0]=="MonitoringADCSlope"){
                if(splitted.count()<5){
                    cout << "incomplete configuration"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->monitoringADC_g_l->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->monitoringADC_g_u->setText(splitted[2]);
                    m_mainWindow->m_daqWindow->ui->monitoringADC_o_l->setText(splitted[3]);
                    m_mainWindow->m_daqWindow->ui->monitoringADC_o_u->setText(splitted[4]);
                }
            }
            if(splitted[0]=="MonitoringADCIntercept"){
                if(splitted.count()<5){
                    cout << "incomplete configuration"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->monitoringADC_g_l_2->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->monitoringADC_g_u_2->setText(splitted[2]);
                    m_mainWindow->m_daqWindow->ui->monitoringADC_o_l_2->setText(splitted[3]);
                    m_mainWindow->m_daqWindow->ui->monitoringADC_o_u_2->setText(splitted[4]);
                }
            }
            if(splitted[0]=="Pedestal"){
                if(splitted.count()<5){
                    cout << "incomplete configuration"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->pedestal_g_l->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->pedestal_g_u->setText(splitted[2]);
                    m_mainWindow->m_daqWindow->ui->pedestal_o_l->setText(splitted[3]);
                    m_mainWindow->m_daqWindow->ui->pedestal_o_u->setText(splitted[4]);
                }
            }
            if(splitted[0]=="AverageTDCRange"){
                if(splitted.count()<5){
                    cout << "incomplete configuration"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->tdc_g_l->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->tdc_g_u->setText(splitted[2]);
                    m_mainWindow->m_daqWindow->ui->tdc_o_l->setText(splitted[3]);
                    m_mainWindow->m_daqWindow->ui->tdc_o_u->setText(splitted[4]);
                }
            }
            if(splitted[0]=="BaselineNoise"){
                if(splitted.count()<3){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->noise_g_l->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->noise_o_l->setText(splitted[2]);
                }
            }
            if(splitted[0]=="Threshold"){
                if(splitted.count()<3){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->threshold_g_l->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->threshold_o_l->setText(splitted[2]);
                }
            }
            if(splitted[0]=="ADCCalibrationChannelDeviation"){
                if(splitted.count()<3){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->adcrange_g_l->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->adcrange_o_l->setText(splitted[2]);
                }
            }
            if(splitted[0]=="WorkingChannelsExternal"){
                if(splitted.count()<3){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->external_g_l->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->external_o_l->setText(splitted[2]);
                }
            }
            if(splitted[0]=="WorkingChannelsInternal"){
                if(splitted.count()<3){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->internal_g_l->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->internal_o_l->setText(splitted[2]);
                }
            }
            if(splitted[0]=="ADCCalibrationVertexPosExtern"){
                if(splitted.count()<3){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->adcvertex_g_e->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->adcvertex_o_e->setText(splitted[2]);
                }
            }
            if(splitted[0]=="ADCCalibrationVertexPosIntern"){
                if(splitted.count()<3){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->adcvertex_g_i->setText(splitted[1]);
                    m_mainWindow->m_daqWindow->ui->adcvertex_o_i->setText(splitted[2]);
                }
            }
            if(splitted[0]=="Location"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->lineEdit_location->setText(splitted[1]);
                }
            }
            if(splitted[0]=="FlashFirmware"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    bool setb = QVariant(splitted[1]).toBool();
                    m_mainWindow->m_daqWindow->ui->checkBox_firmware->setChecked(setb);
                }
            }
            if(splitted[0]=="ForceFlash"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    bool setb = QVariant(splitted[1]).toBool();
                    m_mainWindow->m_daqWindow->ui->checkBox_forcefirmware->setChecked(setb);
                }
            }
            if(splitted[0]=="AutoreadCurrent"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    bool setb = QVariant(splitted[1]).toBool();
                    m_mainWindow->m_daqWindow->ui->checkBox_readcurrent->setChecked(setb);
                }
            }
            if(splitted[0]=="TestExternal"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    bool setb = QVariant(splitted[1]).toBool();
                    m_mainWindow->m_daqWindow->ui->checkBox_external->setChecked(setb);
                }
            }
            if(splitted[0]=="AutomaticTemperatureStop"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    bool setb = QVariant(splitted[1]).toBool();
                    m_mainWindow->m_daqWindow->ui->checkBox_temperature->setChecked(setb);
                }
            }
            if(splitted[0]=="NeighboringTest"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    bool setb = QVariant(splitted[1]).toBool();
                    m_mainWindow->m_daqWindow->ui->checkBox_neighbor->setChecked(setb);
                }
            }
            if(splitted[0]=="EnableSbip"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    bool setb = QVariant(splitted[1]).toBool();
                    m_mainWindow->m_daqWindow->ui->checkBox_sbip->setChecked(setb);
                }
            }
            if(splitted[0]=="WarningTemp"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_mainWindow->m_daqWindow->ui->lineEdit_warningtemp->setText(splitted[1]);
                }
            }
        }
    }
}

QString TestModule::readSettingsDB()
{
    QString path = m_mainWindow->GetApplicationPath()+"/../testing/testsettings.txt";
    QFile file(path);
    QString DBFilesPath;
    bool dbpathset = false;
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        return "";
    }
    else{
        while(!file.atEnd()){
            QString line = QString(file.readLine());
            line.remove('\n');
            QStringList splitted = line.split(QRegExp("\\s+"));
            if(splitted[0]=="DatabaseIP"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_db.setHostName(splitted[1]);
                }
            }
            if(splitted[0]=="DatabaseName"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_db.setDatabaseName(splitted[1]);
                }
            }
            if(splitted[0]=="DatabasePort"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_db.setPort(QVariant(splitted[1]).toInt());
                }
            }
            if(splitted[0]=="DatabaseUser"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_db.setUserName(splitted[1]);
                }
            }
            if(splitted[0]=="DatabasePassword"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    m_db.setPassword(splitted[1]);
                }
            }
            if(splitted[0]=="DatabaseFileFolder"){
                if(splitted.count()<2){
                    cout << "incomplete configuration,"<<endl;
                }
                else{
                    DBFilesPath = splitted[1];
                    dbpathset = true;
                }
            }
        }
    }
    if(dbpathset==false){
        DBFilesPath = path;
    }
    return DBFilesPath;
}

void TestModule::getCurveSettings(QString caller, double &ideal, double &range)
{
    QString sIdeal = "ADCCalibrationCurvature";
    QString sRange = "ADCCalibrationCurvature";
    if(QString::compare(caller,"Internal",Qt::CaseInsensitive)==0){
        sIdeal += "InternIdeal";
        sRange += "InternStddev";
    }
    if(QString::compare(caller,"External",Qt::CaseInsensitive)==0){
        sIdeal += "ExternIdeal";
        sRange += "ExternStddev";
    }

    QString path = m_mainWindow->GetApplicationPath()+"/../testing/testsettings.txt";
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        return;
    }
    else{
        while(!file.atEnd()){
            QString line = QString(file.readLine());
            line.remove('\n');
            QStringList splitted = line.split(QRegExp("\\s+"));
            if(splitted.count()<2){
                cout << "incomplete configuration"<<endl;
            }
            else{
                if(splitted[0]==sIdeal){
                    ideal = QVariant(splitted[1]).toDouble();
                    cout << ideal<<endl;
                }
                if(splitted[0]==sRange){
                    range = QVariant(splitted[1]).toDouble();
                    cout << range<<endl;
                }
            }
        }
    }
}

void TestModule::StartTest(){
    stringstream sx;
    sx << "Starting Test... this may take some time" << std::endl;
    ResetDataStructures();
    if(IsDbg()){
        GetMessageHandler()(sx,"TetsModule::StartTest");
        sx.clear();
        sx.str("");
    }
    QCoreApplication::processEvents();
    GetOptionalSettings();
    //Setup Current monitoring... afterwards needs also termination of current monitoring!!
    if(m_currmon){
        if(!m_currentMonitor){
            m_currentMonitor = new CurrentMonitor(this);
            m_currentMonitor->LoadMessageHandler(*m_msg);
            m_currentMonitor->loadWarningcurrs(2.0,0.3);
            //m_currentMonitor->startMonitoring();
            m_currentMonitor->startMonitoringProcess();
        }
        else{
            //m_currentMonitor->startMonitoring();
            m_currentMonitor->startMonitoringProcess();
        }
        //connect(m_currentMonitor,SIGNAL(ShortCircuit()),this,SLOT(SetTermination()));
    }
    QThread::sleep(2);
/*
    if(m_currmon){
        if(TestShortCircuit()){
            m_currentMonitor->finishMonitor();
            GetMessageHandler()("Current too large, short circuit detected","TestModule::StartTest");
            ResetHybrid();
            return;
        }
    }
*/
    connect(&m_temptimer, SIGNAL(timeout()),this,SLOT(handleTemperatures()));
    //QCoreApplication::processEvents();
    m_connected = ConnectToSRS(); //Establish connection to SRS:
    QCoreApplication::processEvents(); //update GUI
    if(!m_connected){
//        m_currentMonitor->finishMonitor();
        m_currentMonitor->finishMonitoringProcess();
        return;
    }

    if(m_firmwareflash){
        if(m_forceflash){
            loadHybridFirmware();
        }
    }

    //Connection to SRS/FEC established.
    //Proceed to test if Hybrid is connected.

    bool foundHybrid = CheckAndProcessLinkStatus();  //Check link status of all Fecs. Looks for at least one 4 in status message.
    QCoreApplication::processEvents(); //update UI

    if(IsDbg()){
        PrintActiveHDMIonFEC();
    }

    //If at least one connected and ready Hybrid is found, one can continue.
    if(foundHybrid){
        sx << "Found 1 or more connected Hybrids"<< std::endl;
    }
    else{//If no Hybrid is found, try to WarnInitFec and reboot it. If still nothin is found, exit testing.
        sx << "No HDMI found. Try WarnInitFEC and reboot FEC." << std::endl;
        for(int i = 0; i<DAQS_PER_GUIWINDOW; i++){
            for(int j = 0; j< FECS_PER_DAQ; j++){
                if(m_mainWindow->m_daqs[i].GetFEC(j)){
                    m_mainWindow->m_daqs[i].m_fecs[j].m_fecConfigModule->ResetFEC();
                    m_mainWindow->m_daqs[i].m_fecs[j].m_fecConfigModule->ResetFEC();
                }
            }
        }
        bool foundHybrid2 = CheckAndProcessLinkStatus();
        if(foundHybrid2){
            sx << "Found 1 or more connected Hybrids"<< std::endl;
        }
        else{
            //if after Reset nothing is found try to flash firmware onto hybrid, if then nothing is found try reset again, if still unsucessful abort test.
            sx << "After FEC reset still no Hybrid found"<<std::endl;
            if(m_firmwareflash){
                sx << "Trying to load Firmware onto Hybrid" <<std::endl;
                loadHybridFirmware();
                bool foundHybrid3 = CheckAndProcessLinkStatus();
                if(foundHybrid3){
                    sx<< "Found Hybrid after Firmware upgrade, proceed testing" << std::endl;
                }
                else{
                    sx<< "After Firmware upgrade no Hybrid found, try Reset again"<< std::endl;
                    for(int i = 0; i<DAQS_PER_GUIWINDOW; i++){
                        for(int j = 0; j< FECS_PER_DAQ; j++){
                            if(m_mainWindow->m_daqs[i].GetFEC(j)){
                                m_mainWindow->m_daqs[i].m_fecs[j].m_fecConfigModule->ResetFEC();
                                m_mainWindow->m_daqs[i].m_fecs[j].m_fecConfigModule->ResetFEC();
                            }
                        }
                    }
                    bool foundHybrid4 = CheckAndProcessLinkStatus();
                    if(foundHybrid4){
                        sx << "Found Hybrid after Firmware upgrade and FEC Reset, proceed testing"<< std::endl;
                    }
                    else{
                        sx << "Critical Error: Connection to Hybrid could not be found. Is the HDMI cable connected to the FEC and the VMM powered?" << std::endl;
                        GetMessageHandler()(sx,"TestModule::StartTest");
                        if(m_currmon){
                            //m_currentMonitor->finishMonitor();
                            m_currentMonitor->finishMonitoringProcess();
                        }
                        return;
                    }

                }
            }

            else{
                sx << "Critical Error: Connection to Hybrid could not be found. Please check HDMI connection or manually check the Firmware on the Hybrid. Exit Testing" << std::endl;
                GetMessageHandler()(sx,"TestModule::StartTest");
                if(m_currmon){
                    //m_currentMonitor->finishMonitor();
                    m_currentMonitor->finishMonitoringProcess();
                }
                return;
            }
        }
    }
    m_temptimer.start(2000);
    //cout << "Started temperature timer clock" << endl;
    if(IsDbg()){
        GetMessageHandler()(sx,"TestModule::StartTest");
        sx.clear();
        sx.str("");
    }

    QCoreApplication::processEvents(); //update UI
    SetHDMItoTest(); //set private Member Variables of FEC and HDMI ID that need to be tested.
    if(m_HDMI_test!=-1|| m_FEC_test!=-1){
        sx << "Proceeding to test on HDMI with Index " << m_HDMI_test << " ("<< m_HDMI_test+1 << ") on FEC " << m_FEC_test << std::endl;
    }

    bool foundFW = CheckFirmware();
    if(foundFW){
        m_mainWindow->m_daqWindow->ui->firmwarestatuslabel->setText("Found on Hybrid");
        m_mainWindow->m_daqWindow->ui->firmwarestatuslabel->setStyleSheet("background-color: lightgreen");

    }
    else{
        m_mainWindow->m_daqWindow->ui->firmwarestatuslabel->setText("No Firmware on Hybrid");
        m_mainWindow->m_daqWindow->ui->firmwarestatuslabel->setStyleSheet("background-color: red");
    }
    m_labelstoclear.push_back(m_mainWindow->m_daqWindow->ui->firmwarestatuslabel);
    GetMessageHandler()(sx, "TestModule::StartTest");
    sx.clear();
    sx.str("");
    QCoreApplication::processEvents(); //update UI

    m_hResults.h_globalHybridID = GetHybridID();
    m_hResults.h_measurementID = m_hResults.h_globalHybridID + QDateTime::currentDateTime().toString("yyyyMMddhhmm");

    int mode = GetTestMode();
    std::string TestResults = "";
    enableSbip(m_sbip);
    m_temptimer.stop();
    if(mode == 0 || mode == -1){ // 0: ---Select Mode ---  -1: Combobox not properly initialized
        /*if(m_currentMonitor->pid()>0){//upon ending/aborting the test the current monitor needs to be finished, but only if it actually exists.
            m_currentMonitor->close();
            m_currentMonitor = new QProcess(this);
        }*/
        GetMessageHandler()("Please Select a Test mode before starting the test","TestModule::StartTest");
        if(m_currmon){
            //m_currentMonitor->finishMonitor();
            m_currentMonitor->finishMonitoringProcess();
        }
        return;
    }
    else if(mode == 1){
        if(!m_exportblock){
            exportConfig(m_hResults.h_measurementID);
        }
        GetMessageHandler()("TestMode Test All, starting Test...","TestModule::StartTest");
        TestResults += TestMonitoringADC();
        QCoreApplication::processEvents(); //update UI
        bool mon[2] = {m_hResults.h_monitoringADC[0],m_hResults.h_monitoringADC[1]};
        //TestResults += TestPedestal();
        //QCoreApplication::processEvents(); //update UI
        if(mon[0]==true || mon[1]==true){
            TestResults += TestBaselineWidth(10);
            QCoreApplication::processEvents();
            TestResults += TestThreshold();
            QCoreApplication::processEvents();
            TestResults += TestThrTrimmability();
            QCoreApplication::processEvents();
        }
        if(m_extpulse){
            TestResults+=TestChannelsExternal(0);
            QCoreApplication::processEvents();
        }
        TestResults+=TestChannelsInternal();
        QCoreApplication::processEvents();
        if(m_neighbor){
            TestResults+=TestNeighbouring();
            QCoreApplication::processEvents();
        }
    }
    else if(mode == 2){
        if(!m_exportblock){
            exportConfig(m_hResults.h_measurementID);
        }
        TestResults += TestMonitoringADC();
        QCoreApplication::processEvents(); //update UI
        //TestResults += TestPedestal();
        //QCoreApplication::processEvents(); //update UI
        TestResults += TestBaselineWidth(10);
        QCoreApplication::processEvents();
        TestResults += TestThreshold();
        QCoreApplication::processEvents();
        TestResults += TestThrTrimmability();
        QCoreApplication::processEvents();
        GetMessageHandler()("TestMode ADC, starting Tests: Monitoring ADC, Pedestal, BaselineWidth, Threshold","TestModule::StartTest");
    }
    else if(mode == 3){
        if(!m_exportblock){
            exportConfig(m_hResults.h_measurementID);
        }
        if(m_extpulse){
            TestResults+=TestChannelsExternal(0);
            QCoreApplication::processEvents();
        }
        TestResults+=TestChannelsInternal();
        QCoreApplication::processEvents();
        GetMessageHandler()("TestMode Data Acquisition, starting Tests External Pulses, Internal Pulses","TestModule::StartTest");
    }
    else if(mode == 4){
        if(!m_exportblock){
            exportConfig(m_hResults.h_measurementID);
        }
        if(m_extpulse){
            TestResults+=TestChannelsExternal(0);
            QCoreApplication::processEvents();
        }
        GetMessageHandler()("TestMode External Pulses, starting Test...","TestModule::StartTest");
    }
    else if(mode == 5){
        if(!m_exportblock){
            exportConfig(m_hResults.h_measurementID);
        }
        TestResults+=TestChannelsInternal();
        QCoreApplication::processEvents();
        GetMessageHandler()("TestMode Internal Pulses, starting Test...","TestModule::StartTest");
    }
    else if(mode == 6){
        if(!m_exportblock){
            exportConfig(m_hResults.h_measurementID);
        }
        //TestResults+=TestPedestal();
        //QCoreApplication::processEvents();
        TestResults+=TestBaselineWidth(10);
        QCoreApplication::processEvents();
        GetMessageHandler()("TestMode Pedestal, starting Tests Pedestal and BaselineWidth","TestModule::StartTest");
    }
    else if(mode == 7){
        if(!m_exportblock){
            exportConfig(m_hResults.h_measurementID);
        }
        TestResults+=TestThreshold();
        QCoreApplication::processEvents();
        TestResults+=TestThrTrimmability();
        QCoreApplication::processEvents();
        GetMessageHandler()("TestMode Threshold, starting Test...","TestModule::StartTest");
    }
    else if(mode == 8){
        if(!m_exportblock){
            exportConfig(m_hResults.h_measurementID);
        }
        TestResults+=TestNeighbouring();
        QCoreApplication::processEvents();
        GetMessageHandler()("TestMode Neighbouring, starting Test...","TestModule::StartTest");
    }
    else{
        GetMessageHandler()("Unknown Test Mode, abort Testing","TestModule::StartTest");
    }

    handleTemperatures(true);
    if(m_currmon){
        m_currentMonitor->finishMonitoringProcess();
        handleCurrents();
        //m_currentMonitor->finishMonitor();
    }
    cout << "trying to evaluate" <<endl;
    if(!m_exportblock){
        evaluateResults(TestResults);
        cout << "trying to export" <<endl;
        exportResults();
    }
    m_exportblock = false;
    m_terminator = false;
    return;
}

int TestModule::GetTestMode(){
    return m_mainWindow->m_daqWindow->ui->comboBoxTestMode->currentIndex();
}

bool TestModule::ConnectToSRS(){
    stringstream sx;
    emit m_mainWindow->m_daqWindow->ui->openConnection->clicked();
    QThread::usleep(1000);
    m_labelstoclear.push_back(m_mainWindow->m_daqWindow->ui->test_connection_status_label);
    if(! (m_mainWindow->m_daqWindow->ui->connectionLabel->text()==QString("all alive"))) {
        sx << "Critical Error: Communication to SRS could not be established! Exit testing!" << std::endl;
        if(IsDbg()){
            GetMessageHandler()(sx, "TestModule::ConnectToSRS");
        }
        m_mainWindow->m_daqWindow->ui->test_connection_status_label->setText("Failed");
        m_mainWindow->m_daqWindow->ui->test_connection_status_label->setStyleSheet("background-color: red");
        return false;
    }
    else{
        sx << "Connection to SRS successful!" << std::endl;
        m_mainWindow->m_daqWindow->ui->test_connection_status_label->setText("OK");
        m_mainWindow->m_daqWindow->ui->test_connection_status_label->setStyleSheet("background-color: lightgreen");
        if(IsDbg()){
            GetMessageHandler()(sx, "TestModule::ConnectToSRS");
        }
        return true;
    }
}

bool TestModule::CheckAndProcessLinkStatus(){
    std::vector<std::pair<int, QString>> stati; //Create Vector containing FEC_Number and relevant part of LinkStatus
    stringstream sx;
    sx << "Checking Link Status of FECs" << std::endl;
    for(uint fec=0; fec < FECS_PER_DAQ; fec++){
        bool read_ok = false;
        QString link_message = "";
        if(m_mainWindow->m_daqs[0].GetFEC(fec)){
            m_mainWindow->m_daqs[0].m_fecs[fec].m_fecConfigModule->CheckLinkStatus(read_ok,link_message);
            if(read_ok){
                QString sub = "Data, 5:";
                QString linkstatus = link_message.mid(link_message.indexOf("Data, 5:")+sub.length()+1,8);
                linkstatus = linkstatus.trimmed();
                stati.push_back(make_pair(fec,linkstatus));
                sx << "Linkstatus of FEC " << fec << " : " << linkstatus.toStdString() << std::endl;
            }
            else{
                stati.push_back(make_pair(fec,"00000000"));
            }
        }
        else{
            stati.push_back(make_pair(fec,"00000000"));
        }
    }
    m_activeHDMIonFEC.clear();
    int n_active_hdmi = 0;
    for(auto &it : stati){
        std::vector<bool> activeHDMIonFEC;
        QString status = it.second;
        std::string stdstatus = status.toStdString();
        if(stdstatus.length()<8){
            int len = stdstatus.length();
            cout << len;
            std::string statusnew = std::string(8-len, '0')+stdstatus;
            stdstatus = statusnew;
        }
        for(int i = 0; i<stdstatus.length();i++){
            QString stat = QString::fromStdString(stdstatus);
            QChar ls = stat.at(i);
            int lsi = ls.digitValue();
            if(lsi != 4){
                activeHDMIonFEC.push_back(0);
            }
            else{
                activeHDMIonFEC.push_back(1);
                n_active_hdmi++;
            }
        }
        m_activeHDMIonFEC.push_back(make_pair(it.first,activeHDMIonFEC));
    }
    if(IsDbg()){
        GetMessageHandler()(sx,"TestModule::CheckAndProcessLinkStatus");
    }
    m_labelstoclear.push_back(m_mainWindow->m_daqWindow->ui->linkstatuslabelstatus);
    if(n_active_hdmi>0){
        m_mainWindow->m_daqWindow->ui->linkstatuslabelstatus->setText("Found active HDMI");
        m_mainWindow->m_daqWindow->ui->linkstatuslabelstatus->setStyleSheet("background-color: lightgreen");
        return true;
    }
    else{
        m_mainWindow->m_daqWindow->ui->linkstatuslabelstatus->setText("No active HDMI found");
        m_mainWindow->m_daqWindow->ui->linkstatuslabelstatus->setStyleSheet("background-color: red");
        return false;
    }
}

void TestModule::SetHDMItoTest(){//Sets member variables of testModule which HDMI on which FEC is to be tested.
                                    //Also updates the GUI.
    m_HDMI_test = -1;
    m_FEC_test = -1;
    stringstream sx;
    QList<QCheckBox*> a = m_mainWindow->m_daqWindow->ui->Fec_group_box->findChildren<QCheckBox*>();
    std::sort(a.begin(), a.end(),
          [](const QCheckBox* x, const QCheckBox* y) -> bool { return x->text() <  y->text();
    });
    //For GUI update: First deactivate all FEC-Checkboxes
    for(int k= 0; k<FECS_PER_DAQ; k++){
        m_mainWindow->m_daqs[0].SetFEC(k,false);;
        a[k]->setChecked(false);
        emit a[k]->clicked();
    }
    for(auto it : m_activeHDMIonFEC){
        int i=0;
        for(std::vector<bool>::reverse_iterator boolit = it.second.rbegin(); boolit != it.second.rend(); boolit++ ){
            bool HDMIact = *boolit;
            if(HDMIact==1){
                m_HDMI_test = i;
                m_FEC_test = it.first;
                sx << "HDMI to test: " << m_HDMI_test << " On FEC: " << m_FEC_test << std::endl;
                cout << "HDMI to test: " << m_HDMI_test << " On FEC: " << m_FEC_test << std::endl;
                //GetMessageHandler()(sx, "TestModule::SetHDMItoTest");
                for(int k = 0; k< FECS_PER_DAQ; k++){
                    if(k==m_FEC_test){
                        m_mainWindow->m_daqs[0].SetFEC(k, true);
                        a[k]->setChecked(true);//GUI Update: Check FEC-Box
                    }
                    else{
                        m_mainWindow->m_daqs[0].SetFEC(k, false);
                        a[k]->setChecked(false); //GUI Update: uncheck FEC-Box
                    }
                    for(int h = 0; h<HDMIS_PER_FEC; h++){
                        if(k==m_FEC_test && h==m_HDMI_test){
                            m_mainWindow->m_daqs[0].m_fecs[k].SetHDMI(h,true);
                            m_mainWindow->m_daqs[0].m_fecs[k].m_hdmis[h].SetHybrid(0,true);
                            m_mainWindow->m_daqs[0].m_fecs[k].m_hdmis[h].m_hybrids[0].SetVMM(0,true);
                            m_mainWindow->m_daqs[0].m_fecs[k].m_hdmis[h].m_hybrids[0].SetVMM(1,true);
                        }
                        else{
                            m_mainWindow->m_daqs[0].m_fecs[k].SetHDMI(h,false);
                        }
                    }
                    emit a[k]->clicked();
                }

                return;
            }
            i++;
        }
    }
    sx << "HDMI to test: " << m_HDMI_test << " On FEC: " << m_FEC_test << std::endl;
    if(IsDbg()){
        GetMessageHandler()(sx, "TestModule::SetHDMItoTest");
    }
    return;
}

double TestModule::ReadTemperature(int vmmnr)
{
    VMM* vmm = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[vmmnr];
    FECConfigModule* fcm = m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_fecConfigModule;
    vmm->SetRegi("monitoring","Temperature_sensor");
    m_mainWindow->m_daqs[0].SendAll();
    QThread::usleep(1000);
    int adc_result = fcm->ReadADC(m_HDMI_test,vmmnr,2);
    double temperature = (725-adc_result)/1.85;
    return temperature;
}

QString TestModule::GetHybridID()
{   QString id = m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_fecConfigModule->ReadI2C(m_HDMI_test,0);
    if(id!="ffffffff"){
        m_mainWindow->m_daqWindow->ui->hybridIDlabel->setStyleSheet("background-color: lightgreen");
    }
    else{
        bool ok;
        id = QInputDialog::getText(0,tr("QInputDialog::getText()"),tr("Hybrid ID invalid, please enter ID"),QLineEdit::Normal,QString(),&ok);
        if(ok==true){
            m_mainWindow->m_daqWindow->ui->hybridIDlabel->setStyleSheet("background-color: yellow");
            m_mainWindow->m_daqWindow->ui->hybridIDlabel->setToolTip("<font color=\"#000000\"> Unable to automatically read out Hybrid ID. Might be a hint, that I2C is broken");
        }
        else{
            SetTermination();
            m_exportblock = true;
        }
    }
    m_mainWindow->m_daqWindow->ui->hybridIDlabel->setText(id);
    m_labelstoclear.push_back(m_mainWindow->m_daqWindow->ui->hybridIDlabel);
    return id;
}

void TestModule::PrintActiveHDMIonFEC(){
    stringstream sx = stringstream();
    for(auto it : m_activeHDMIonFEC){
        sx << "On FEC " << it.first << ": Following HDMI are active: HDMI ";
        for(std::vector<bool>::reverse_iterator boolit = it.second.rbegin(); boolit != it.second.rend(); boolit++ ){
            bool HDMIact = *boolit;
            sx << HDMIact << " ";
        }
        sx << std::endl;
    }
    GetMessageHandler()(sx,"TestModule::PrintActiveHDMIonFEC");
    sx.clear();
}

bool TestModule::loadHybridFirmware()
{
    //with impact not working correctly: use xc3sprog, requires usbreset
    /*
    std::array<char,200> buffer;
    std::string line;
    std::unique_ptr<FILE,decltype(&pclose)> pipe(popen("lsusb","r"),pclose);
    if(!pipe){
        GetMessageHandler()("Could not execute lsusb command, Firmware loading aborted", "TestModule::loadHybridFirmware");
        return false;
    }
//    int relevantbus, relevantdevice;
    int productid, vendorid;
    //int busnr, devicenr;
    char devicename[200];
    char busnr[3], devicenr[3], relevantbus[3], relevantdevice[3];
    while(fgets(buffer.data(),buffer.size(), pipe.get())!=nullptr){
        line = buffer.data();
        cout << "Read in line: " << line;
        sscanf(line.c_str(),"Bus %c%c%c Device %c%c%c: ID %x:%x %s\n",&busnr[0],&busnr[1],&busnr[2],&devicenr[0],&devicenr[1],&devicenr[2],&vendorid,&productid,&devicename);
        //lines>>"Bus ">>busnr>>" Device ">>devicenr>> ": ID ">>vendorid>>":">>productid>>" ">> devicename;
        if(vendorid==0x03fd && productid == 0x0008){//<-Xilinx Platform Cable II
            for(int i=0; i<3;i++){
                relevantbus[i]=busnr[i];
                relevantdevice[i]=devicenr[i];
            }
            cout << "Relevant bus and device are:"<< relevantbus << "/" << relevantdevice << std::endl;
        }
//        cout << busname << std::endl;
        cout <<"Bus " << busnr << std::endl;
//        cout << device << std::endl;
        cout << "Device " << devicenr << std::endl;
//        cout << colon1 << std::endl;
  //      cout << id << std::endl;
        cout << "VendorID " <<vendorid << std::endl;
    //    cout << colon2 << std::endl;
        cout << "ProductID " <<productid << std::endl;
        cout << "Devicename" << devicename << std::endl;

        //line.clear();
    }
    cout << "Relevant bus and device are:"<< relevantbus << "/" << relevantdevice << std::endl;
    cout << "while loop finished"<< std::endl;
    //std::stringstream cmd;
    //cmd<< "usbreset /dev/bus/usb/" << relevantbus <<"/"<<relevantdevice;
    //cout << cmd.str();
    //system(cmd.str().c_str());
    //system("xc3sprog -c xpc -v -p 0 /home/fjaekel/FPGAFirmwares/vmm3h_v1_1.bit");
*/

    //with impact working properly
    if(IsDbg()){
        GetMessageHandler()("Starting to flash Hybrid Firmware... This may take some time","TestModule::loadHybridFirmware");
    }
    QString program = "bash";
    QStringList arguments;
    arguments << "../testing/setupfirmwareflashing.sh";
    QProcess* flash = new QProcess(this);
    flash->start(program,arguments);
    flash->setProcessChannelMode(QProcess::ForwardedChannels);
    flash->waitForFinished(120000);
    QByteArray errors = flash->readAllStandardError();
    cout << "ERRORS OF FIRMWAREFLASHING: " << errors.toStdString() << std::endl;

    /*                      TODO:
        Check errors if firmwareflashing was successfull
                                                                            */

    stringstream st;
    if(!errors.isEmpty()){
        st << "There were Errors while Flashing the Firmware: " <<endl;
        st << errors.toStdString()<<endl;
    }
    GetMessageHandler()(st,"TestModule::loadHybridFirmware");

    QMessageBox msgBox;
    msgBox.setText("Firmware flashing finished. Please disconnect the programmer.");
    msgBox.exec();

    return true;
}

bool TestModule::CheckFirmware()
{
    //Function to check if Firmware exists on Hybrid. As at the moment it is not possible to actually check for Firmware
    //one assumes if communication is possible, there is firmware on it
    if(m_HDMI_test != -1 && m_FEC_test != -1){
        m_hResults.h_firmware = 1;
        return true;
    }
    else{
        m_hResults.h_firmware = 0;
        return false;
    }
}

void TestModule::on_readTestLog(){
    string buff = GetMessageHandler().GetBuffer();
    m_mainWindow->m_daqWindow->ui->TestLogScreen->append(QString::fromStdString(buff));
    GetMessageHandler().ClearBuffer();
}

std::string TestModule::TestPedestal()
{
    if(m_terminator){
        return "Test aborted";
    }
    handleTemperatures();
    stringstream sx;
    bool mon[2] = {m_hResults.h_monitoringADC[0],m_hResults.h_monitoringADC[1]};
    int runmode = m_mainWindow->m_daqWindow->ui->comboBoxRunMode->findText("automatic calibration");
    m_mainWindow->m_daqWindow->ui->comboBoxRunMode->setCurrentIndex(runmode);
    int calibtype = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->findText("Pedestal");
    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(calibtype);
    m_calibmod->StartCalibration();
    /*while(!m_acqon){
        cout << "wait";
    }*/
    //Acquire Data
    while(!(m_calibmod->m_dataAvailable)){
        QCoreApplication::processEvents();
    }

    QVector<double> pedestals[VMMS_PER_HYBRID] = {QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][0]),QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][1])};
    QVector<double> xvals;
    for(int i=0;i<64;i++){
        xvals.push_back(i);
    }

    //Find dead Channels and calculate mean value of pedestal without the dead Channels
    double qGood[2] = {m_mainWindow->m_daqWindow->ui->pedestal_g_l->text().toDouble(),m_mainWindow->m_daqWindow->ui->pedestal_g_u->text().toDouble()};
    double qOk[2] = {m_mainWindow->m_daqWindow->ui->pedestal_o_l->text().toDouble(),m_mainWindow->m_daqWindow->ui->pedestal_o_u->text().toDouble()};
    std::set<int> outliers[VMMS_PER_HYBRID];
    outliers[0] = findOutliers(pedestals[0],100);
    outliers[1] = findOutliers(pedestals[1],100);
    QVector<double> nooutliers[2];
    for(int i = 0; i<VMMS_PER_HYBRID;i++){
        //pedestals[i]=QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][i]);
        int n=0;
        for(int j = 0;j<64; j++){
            double ped = pedestals[i][j];
            cout << ped;
            if(ped>=qGood[0] && ped<=qGood[1]){
                cout <<"good" <<endl;
                MarkChannel("Pedestal",i,j,"good");
            }
            else if(ped>=qOk[0] && ped<=qOk[1]){
                cout <<"ok" <<endl;
                MarkChannel("Pedestal",i,j,"ok");
            }
            else if(ped<qOk[0] || ped>qOk[1]){
                cout <<"bad" <<endl;
                MarkChannel("Pedestal",i,j,"bad");
            }
            else{
                cout <<"unknown" <<endl;
                MarkChannel("Pedestal",i,j,"unknown");
            }
        }
    }
    for(int i=0;i<2;i++){
        for(int j=0;j<64;j++){
            if(outliers[i].find(j)==outliers[i].end()){
                    nooutliers[i].push_back(pedestals[i][j]);
            }
        }
    }
    double avg[2] = {getAverage(nooutliers[0]),getAverage(nooutliers[1])};
    double stdds[2] = {getStdDev(nooutliers[0]),getStdDev(nooutliers[1])};


    QLabel* labelvec[2] = {m_mainWindow->m_daqWindow->ui->pedestalstatuslabel1,m_mainWindow->m_daqWindow->ui->pedestalstatuslabel2};
    QString exports[2] = {"",""};
    //Evaluate found Data
    for(int i = 0; i<VMMS_PER_HYBRID;i++){
        if(mon[i]){
            labelvec[i]->setWordWrap(true);
            QString labeltext = QString(QString::number(avg[i],'f',1)); //+ " ± " +QString(QString::number(stdds[i],'f',1))+" mV";
            labelvec[i]->setText(labeltext);
            if(avg[i]>qGood[0] && avg[i]<qGood[1]){
                labelvec[i]->setStyleSheet("background-color: lightgreen");
                m_hResults.h_VMMResults[i].insert("Pedestal",0);
           }
            else if(avg[i]>qOk[0] && avg[i]<qOk[1] && (avg[i]<qGood[0] || avg[i]>qGood[1])){
                labelvec[i]->setStyleSheet("background-color: yellow");
                m_hResults.h_VMMResults[i].insert("Pedestal",1);
            }
            else{
                labelvec[i]->setStyleSheet("background-color: red");
                m_hResults.h_VMMResults[i].insert("Pedestal",2);
            }
            exports[i] += labeltext;
            m_labelstoclear.push_back(labelvec[i]);
            m_hResults.h_DBExportStrings[i].insert("Baseline",exports[i]);
        }
    }
    //m_hResults.h_DBExportStrings.insert("Baseline_VMM0",exports[0]);
    //m_hResults.h_DBExportStrings.insert("Baseline_VMM1",exports[1]);
    PlotData(xvals,pedestals,"Channel Nr.", "Pedestal","Pedestal");


    sx<< std::endl;
    handleTemperatures();
    return sx.str();
}

string TestModule::TestBaselineWidth(int nruns)
{
    if(m_terminator){
        return "Test aborted";
    }
    handleTemperatures();
    stringstream sx;
    QVector<double> channels;
    QVector<double> channels1;
    QVector<double> blines[2][64];
    QVector<double> baselines[2];
    VMM* vmm[2];
    bool mon[2] = {m_hResults.h_monitoringADC[0],m_hResults.h_monitoringADC[1]};
    vmm[0] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0];
    vmm[1] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1];
    FECConfigModule* fcm = m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_fecConfigModule;
    int runmode = m_mainWindow->m_daqWindow->ui->comboBoxRunMode->findText("automatic calibration");
    m_mainWindow->m_daqWindow->ui->comboBoxRunMode->setCurrentIndex(runmode);
    int calibtype = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->findText("Pedestal");
    m_mainWindow->m_daqWindow->ui->Runs->setValue(1000);
    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(calibtype);
    int oldgains[2] = {vmm[0]->GetRegister("gain"),vmm[1]->GetRegister("gain")};
    cout <<oldgains[0]<<"\t" << oldgains[1] <<endl;
    vmm[0]->SetRegi("gain",7);
    vmm[1]->SetRegi("gain",7);
    for(int i=0; i<64;i++){
        if(i%8!=1 || i%8!=6 || i%8!=0 || i%8!=7){
            /*vmm[0]->SetRegi("st",0,i);
            vmm[1]->SetRegi("st",0,i);
            vmm[0]->SetRegi("smx",0,i);
            vmm[1]->SetRegi("smx",0,i);*/
            vmm[0]->SetRegi("monitoring",i);
            vmm[1]->SetRegi("monitoring",i);
            m_mainWindow->m_daqs[0].SendAll();
            QThread::usleep(1000);
            for(int j=0; j< nruns; j++){
                blines[0][i].resize(nruns);
                blines[1][i].resize(nruns);
                int bline1 = fcm->ReadADC(m_HDMI_test,0,2);
                int bline2 = fcm->ReadADC(m_HDMI_test,1,2);
                baselines[0].push_back(bline1);
                baselines[1].push_back(bline2);
                blines[0][i][j] = bline1;
                blines[1][i][j] = bline2;
                channels.push_back(i);
            }
        channels1.push_back(i);
        }
    }
    if(m_extpulse){
        QString program ="python3";
        QStringList arguments;
        arguments << "../testing/advance_channels.py";
        QProcess* advance = new QProcess(this);
        advance->startDetached(program,arguments);
        advance->waitForFinished();
    }
    QThread::usleep(100);
    for(int i=0; i<64;i++){
        if(i%8==1 || i%8==6 || i%8==0 || i%8==7 && i!=0){
            /*vmm[0]->SetRegi("st",0,i);
            vmm[1]->SetRegi("st",0,i);
            vmm[0]->SetRegi("smx",0,i);
            vmm[1]->SetRegi("smx",0,i);*/
            vmm[0]->SetRegi("monitoring",i);
            vmm[1]->SetRegi("monitoring",i);
            m_mainWindow->m_daqs[0].SendAll();
            QThread:: usleep(1000);
            for(int j=0; j< nruns; j++){
                blines[0][i].resize(nruns);
                blines[1][i].resize(nruns);
                int bline1 = fcm->ReadADC(m_HDMI_test,0,2);
                int bline2 = fcm->ReadADC(m_HDMI_test,1,2);
                baselines[0].push_back(bline1);
                baselines[1].push_back(bline2);
                blines[0][i][j] = bline1;
                blines[1][i][j] = bline2;
                channels.push_back(i);
            }
        channels1.push_back(i);
        }
    }
    if(m_extpulse){
        QString program ="python3";
        QStringList arguments;
        arguments << "../testing/advance_channels.py";
        QProcess* advance = new QProcess(this);
        advance->startDetached(program,arguments);
        advance->waitForFinished();
    }
    vmm[0]->SetRegi("monitoring",0);
    vmm[1]->SetRegi("monitoring",0);
    m_mainWindow->m_daqs[0].SendAll();
    QThread::usleep(1000);
    for(int j=0; j< nruns; j++){
        blines[0][0].resize(nruns);
        blines[1][0].resize(nruns);
        int bline1 = fcm->ReadADC(m_HDMI_test,0,2);
        int bline2 = fcm->ReadADC(m_HDMI_test,1,2);
        baselines[0].push_back(bline1);
        baselines[1].push_back(bline2);
        blines[0][0][j] = bline1;
        blines[1][0][j] = bline2;
        channels.push_back(0);
    }
    channels1.push_back(0);

    /*
    for(int i=0;i<nruns;i++){
        m_calibmod->StartCalibration();
        while(!(m_calibmod->m_dataAvailable)){
            QCoreApplication::processEvents();
        }
        QVector<double> peds[2] = {QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][0]),QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][1])};
        for(int h=0; h<2;h++){
            for(int j=0; j<peds[h].size();j++){
                blines[h][j].push_back(peds[h][j]);
            }
        }
        cout<< "Ready with run " << i <<endl;
    }*/
    cout << "acquired all data"<<endl;
    QVector<double> pedestals[2];
    for(int j=0;j<2;j++){
        for(int i=0;i<64;i++){
            pedestals[j].push_back(getAverage(blines[j][i]));
        }
    }
    cout << "Got pedestals"<<endl;
    QVector<double> xvals;
    for(int i=0;i<64;i++){
        xvals.push_back(i);
    }

    //Find dead Channels and calculate mean value of pedestal without the dead Channels
    double qGood[2] = {m_mainWindow->m_daqWindow->ui->pedestal_g_l->text().toDouble(),m_mainWindow->m_daqWindow->ui->pedestal_g_u->text().toDouble()};
    double qOk[2] = {m_mainWindow->m_daqWindow->ui->pedestal_o_l->text().toDouble(),m_mainWindow->m_daqWindow->ui->pedestal_o_u->text().toDouble()};
    std::set<int> outliers[VMMS_PER_HYBRID];
    outliers[0] = findOutliers(pedestals[0],100);
    outliers[1] = findOutliers(pedestals[1],100);
    QVector<double> nooutliers[2];
    for(int i = 0; i<VMMS_PER_HYBRID;i++){
        //pedestals[i]=QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][i]);
        for(int j = 0;j<64; j++){
            double ped = pedestals[i][j];
            cout << ped;
            if(ped>=qGood[0] && ped<=qGood[1]){
                cout <<"good" <<endl;
                MarkChannel("Pedestal",i,j,"good");
            }
            else if(ped>=qOk[0] && ped<=qOk[1]){
                cout <<"ok" <<endl;
                MarkChannel("Pedestal",i,j,"ok");
            }
            else if(ped<qOk[0] || ped>qOk[1]){
                cout <<"bad" <<endl;
                MarkChannel("Pedestal",i,j,"bad");
            }
            else{
                cout <<"unknown" <<endl;
                MarkChannel("Pedestal",i,j,"unknown");
            }
        }
    }
    for(int i=0;i<2;i++){
        for(int j=0;j<64;j++){
            if(outliers[i].find(j)==outliers[i].end()){
                    nooutliers[i].push_back(pedestals[i][j]);
            }
        }
    }
    double avg[2] = {getAverage(nooutliers[0]),getAverage(nooutliers[1])};
    double stdds[2] = {getStdDev(nooutliers[0]),getStdDev(nooutliers[1])};


    QLabel* labelvec[2] = {m_mainWindow->m_daqWindow->ui->pedestalstatuslabel1,m_mainWindow->m_daqWindow->ui->pedestalstatuslabel2};
    QString exports[2] = {"",""};
    //Evaluate found Data
    for(int i = 0; i<VMMS_PER_HYBRID;i++){
        if(mon[i]){
            labelvec[i]->setWordWrap(true);
            QString labeltext = QString(QString::number(avg[i],'f',1)); //+ " ± " +QString(QString::number(stdds[i],'f',1))+" mV";
            labelvec[i]->setText(labeltext);
            if(avg[i]>qGood[0] && avg[i]<qGood[1]){
                labelvec[i]->setStyleSheet("background-color: lightgreen");
                m_hResults.h_VMMResults[i].insert("Pedestal",0);
           }
            else if(avg[i]>qOk[0] && avg[i]<qOk[1] && (avg[i]<qGood[0] || avg[i]>qGood[1])){
                labelvec[i]->setStyleSheet("background-color: yellow");
                m_hResults.h_VMMResults[i].insert("Pedestal",1);
            }
            else{
                labelvec[i]->setStyleSheet("background-color: red");
                m_hResults.h_VMMResults[i].insert("Pedestal",2);
            }
            m_labelstoclear.push_back(labelvec[i]);
            exports[i] += labeltext;
            m_hResults.h_DBExportStrings[i].insert("Baseline",exports[i]);
        }
    }
    //m_hResults.h_DBExportStrings.insert("Baseline_VMM0",exports[0]);
    //m_hResults.h_DBExportStrings.insert("Baseline_VMM1",exports[1]);

    PlotData(xvals,pedestals,"Channel Nr.", "Pedestal","Pedestal");


    vmm[0]->SetRegi("gain",oldgains[0]);
    vmm[1]->SetRegi("gain",oldgains[1]);
    m_mainWindow->m_daqs[0].SendAll();

    /*
     *                      BASLINE WIDTH CALCULATIONS
     **/

    //PlotData(channels,baselines,"Channel","measured baselines","Baseline Width");
    //Analysis
    QVector<double> stdd[2];
    double qGoodD = m_mainWindow->m_daqWindow->ui->noise_g_l->text().toDouble();
    double qOkD = m_mainWindow->m_daqWindow->ui->noise_o_l->text().toDouble();
    for(int i=0; i<2;i++){
        for(int j=0; j<64;j++){
            stdd[i].push_back(getStdDev(blines[i][j]));
            if(stdd[i][j]<=qGoodD){
                MarkChannel("Baseline Noise",i,j,"good");
            }
            else if(stdd[i][j]>qGoodD && stdd[i][j]<=qOkD){
                MarkChannel("Baseline Noise",i,j,"ok");
            }
            else{
                MarkChannel("Baseline Noise",i,j,"bad");
            }
        }
    }

    PlotData(xvals,stdd,"Channel","RMS Noise [mV]","Baseline Noise");
    double avgnoise[2] = {getAverage(stdd[0]),getAverage(stdd[1])};
    double noisestd[2] = {getStdDev(stdd[0]),getStdDev(stdd[1])};
    sx << "Average Noise on VMM0 :" << avgnoise[0] << "±" << noisestd[0] <<"mV" << endl;
    QLabel* labelvec2[2] = {m_mainWindow->m_daqWindow->ui->noisestatus1label,m_mainWindow->m_daqWindow->ui->noisestatus2label};
    QString exports2[2] = {"",""};
    for(int i = 0; i<VMMS_PER_HYBRID;i++){
        if(mon[i]){
            labelvec2[i]->setWordWrap(true);
            QString labeltext = QString(QString::number(avgnoise[i],'f',3)); //+ " ± " +QString(QString::number(stdds[i],'f',1))+" mV";
            labelvec2[i]->setText(labeltext);
            if(avgnoise[i]<=qGoodD){
                labelvec2[i]->setStyleSheet("background-color: lightgreen");
                m_hResults.h_VMMResults[i].insert("Baseline Noise",0);
           }
            else if(avgnoise[i]>qGoodD&& avgnoise[i]<=qOkD){
                labelvec2[i]->setStyleSheet("background-color: yellow");
                m_hResults.h_VMMResults[i].insert("Baseline Noise",1);
            }
            else{
                labelvec2[i]->setStyleSheet("background-color: red");
                m_hResults.h_VMMResults[i].insert("Baseline Noise",2);
            }
            m_labelstoclear.push_back(labelvec2[i]);
            exports2[i] += labeltext;
            m_hResults.h_DBExportStrings[i].insert("Basenoise",exports2[i]);
        }
    }
    sx << "Average Noise on VMM1 :" << avgnoise[1] << "±" << noisestd[1] <<"mV" << endl;
    handleTemperatures();
    return sx.str();
}

double TestModule::getAverage(QVector<double> data){
    double avg = 0;
    int size = data.size();
    if(size==0){
        return 0;
    }
    for(int i =0; i<size;i++){
        avg +=data[i];
    }
    avg /=size;
    return avg;
}

double TestModule::getStdDev(QVector<double> data){
    double avg = getAverage(data);
    int size = data.size();
    double stdd=0;
    if(size==0){
        return 0;
    }
    for(int i=0; i< size; i++){
        stdd += pow((avg-data[i]),2);
    }
    stdd /=size;
    stdd = sqrt(stdd);
    if(stdd<1e-16){
        stdd = 0;
    }
    return stdd;
}

double TestModule::getMedian(QVector<double> data)
{
    std::vector<double> copy = data.toStdVector();
    std::sort(copy.begin(),copy.end());
    double median;
    if(copy.size()%2==0){
        median = (copy[copy.size()/2]+copy[copy.size()/2-1])/2;
    }
    else{
        median = copy[copy.size()/2];
    }
    return median;
}

string TestModule::TestThreshold()
{
    if(m_terminator){
        return "Test aborted";
    }
    handleTemperatures();
    stringstream sx;
    QVector<double> channels;
    QVector<double> thresholds[2];
    VMM* vmm[2];
    vmm[0] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0];
    vmm[1] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1];
    FECConfigModule* fcm = m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_fecConfigModule;
    bool mon[2] = {m_hResults.h_monitoringADC[0],m_hResults.h_monitoringADC[1]};

    for(int i=0; i<64;i++){
        vmm[0]->SetRegi("smx",1,i);
        vmm[1]->SetRegi("smx",1,i);
        vmm[0]->SetRegi("monitoring",i);
        vmm[1]->SetRegi("monitoring",i);
        m_mainWindow->m_daqs[0].SendAll();
        int thrval1 = fcm->ReadADC(m_HDMI_test,0,2);
        int thrval2 = fcm->ReadADC(m_HDMI_test,1,2);
        thresholds[0].push_back(thrval1);
        thresholds[1].push_back(thrval2);
        channels.push_back(i);
        vmm[0]->SetRegi("smx",0,i);
        vmm[1]->SetRegi("smx",0,i);
    }
    double avg[2] = {getAverage(thresholds[0]),getAverage(thresholds[1])};
    double stdds[2] = {getStdDev(thresholds[0]),getStdDev(thresholds[1])};
    QLabel* labelvec[2] = {m_mainWindow->m_daqWindow->ui->thresholdstatuslabel1,m_mainWindow->m_daqWindow->ui->thresholdstatuslabel2};
    QString exports[2] = {"",""};
    double slopes[2] = {m_hResults.h_monitoringADCcal[0][0],m_hResults.h_monitoringADCcal[1][0]};
    double intercepts[2] = {m_hResults.h_monitoringADCcal[0][1],m_hResults.h_monitoringADCcal[1][1]};
    double setthreshold[2] = {m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0].GetRegister("sdt")*slopes[0]+intercepts[0],m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1].GetRegister("sdt")*slopes[1]+intercepts[1]};
    for(int i=0;i<2;i++){
        if(!mon[i]){
            setthreshold[i] = 0.801*m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[i].GetRegister("sdt")+25.182;
        }
    }
    double qGood = m_mainWindow->m_daqWindow->ui->threshold_g_l->text().toDouble();
    double qOk = m_mainWindow->m_daqWindow->ui->threshold_o_l->text().toDouble();
    for(int i=0;i<2;i++){
        for(int j=0;j<64;j++){
            if(fabs(thresholds[i][j]-setthreshold[i])<qGood){
                MarkChannel("Threshold",i,j,"good");
            }
            else if(fabs(thresholds[i][j]-setthreshold[i])<qOk){
                MarkChannel("Threshold",i,j,"ok");
            }
            else{
                MarkChannel("Threshold",i,j,"bad");
            }
        }
    }

    for(int i = 0; i<VMMS_PER_HYBRID;i++){
        if(mon[i]){
            double min = *std::min_element(thresholds[i].begin(),thresholds[i].end());
            double max = *std::max_element(thresholds[i].begin(),thresholds[i].end());
            double diff = max-min;
            if(avg[i]==4095){
                diff=4095;
            }
            QString labeltext = QString::number(diff);//+"mV";
            labelvec[i]->setWordWrap(true);
            labelvec[i]->setText(labeltext);
            if(diff<=qGood){
                labelvec[i]->setStyleSheet("background-color: lightgreen");
                m_hResults.h_VMMResults[i].insert("Threshold",0);
            }
            else if(diff>qGood && diff<=qOk){
                labelvec[i]->setStyleSheet("background-color: yellow");
                m_hResults.h_VMMResults[i].insert("Threshold",1);
            }
            else{
                labelvec[i]->setStyleSheet("background-color: red");
                m_hResults.h_VMMResults[i].insert("Threshold",2);
            }
            m_labelstoclear.push_back(labelvec[i]);
            exports[i]+= QString::number(diff);
            m_hResults.h_DBExportStrings[i].insert("Threshold",exports[i]);
        }
    }

    PlotData(channels,thresholds,"Channel", "Channel Threshold", "Threshold Scan");
    handleTemperatures();
    return "Finished";
}

std::string TestModule::TestMonitoringADC()
{
    if(m_terminator){
        return "Test aborted";
    }
    handleTemperatures();
    stringstream sx;
    QVector<double> thrDACvals;
    QVector<double> readADC[2];
    VMM* vmm[2];
    vmm[0] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0];
    vmm[1] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1];
    FECConfigModule* fcm = m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_fecConfigModule;

    vmm[0]->SetRegi("monitoring","Threshold_DAC");
    vmm[1]->SetRegi("monitoring","Threshold_DAC");
    m_mainWindow->m_daqs[0].SendAll();
    QThread::usleep(1000);

    //Acquisition of Data

    for(unsigned short i= 0; i<1024;i+=16){
        vmm[0]->SetRegi("sdt",i);
        vmm[1]->SetRegi("sdt",i);
        m_mainWindow->m_daqs[0].SendAll();
        int ADCval1 = fcm->ReadADC(m_HDMI_test, 0, 2);
        int ADCval2 = fcm->ReadADC(m_HDMI_test, 1, 2);
        thrDACvals.push_back(i);
        readADC[0].push_back(ADCval1);
        readADC[1].push_back(ADCval2);
    }
    vmm[0]->SetRegi("sdt",300);
    vmm[1]->SetRegi("sdt",300);
    m_mainWindow->m_daqs[0].SendAll();

    //Evaluation of Data:

    double slopeapprox[2];
    double intercept[2];
//    slopeapprox[0] = (readADC[0][0]-readADC[0][63])/(thrDACvals[0]-thrDACvals[63]);
//    slopeapprox[1] = (readADC[1][0]-readADC[1][63])/(thrDACvals[0]-thrDACvals[63]);
//    cout << slopeapprox[0] << "   " << slopeapprox[1] << std::endl;
    for(int i=0;i<2;i++){
        FitLinear(thrDACvals,readADC[i],slopeapprox[i],intercept[i]);
    }
    /*
       Define in following loop (in if clause) Criteria if ADC is okay or not.
       For now just check the slope.
                                                                                 */
    m_hResults.h_monitoringADCcal[0][0]=slopeapprox[0];
    m_hResults.h_monitoringADCcal[1][0]=slopeapprox[1];
    m_hResults.h_monitoringADCcal[0][1]=intercept[0];
    m_hResults.h_monitoringADCcal[1][1]=intercept[1];

    QLabel* labelvec[2] = {m_mainWindow->m_daqWindow->ui->adcstatuslabel1,m_mainWindow->m_daqWindow->ui->adcstatuslabel2};
    double qGoodSlopes[2] = {m_mainWindow->m_daqWindow->ui->monitoringADC_g_l->text().toDouble(),m_mainWindow->m_daqWindow->ui->monitoringADC_g_u->text().toDouble()};
    double qOkSlopes[2] = {m_mainWindow->m_daqWindow->ui->monitoringADC_o_l->text().toDouble(),m_mainWindow->m_daqWindow->ui->monitoringADC_o_u->text().toDouble()};
    double qGoodIntercept[2] = {m_mainWindow->m_daqWindow->ui->monitoringADC_g_l_2->text().toDouble(),m_mainWindow->m_daqWindow->ui->monitoringADC_g_u_2->text().toDouble()};
    double qOkIntercept[2] = {m_mainWindow->m_daqWindow->ui->monitoringADC_o_l_2->text().toDouble(),m_mainWindow->m_daqWindow->ui->monitoringADC_o_u_2->text().toDouble()};
    sx << "Testing Monitoring ADC:";
    for(int i=0;i<2;i++){
        sx << "     VMM " << i << " : Status ";
        if(slopeapprox[i]>=qGoodSlopes[0] && slopeapprox[i]<= qGoodSlopes[1] && intercept[i]>=qGoodIntercept[0] && intercept[i]<=qGoodIntercept[1]){
            labelvec[i]->setText("Monitoring ADC functional");
            labelvec[i]->setStyleSheet("background-color: lightgreen");
            m_hResults.h_monitoringADC[i]=true;
            m_hResults.h_VMMResults[i].insert("MonitoringADC",0);
            sx << 0;
        }
        else if(((slopeapprox[i]<qGoodSlopes[0] || slopeapprox[i]>qGoodSlopes[1] || intercept[i]<qGoodIntercept[0] || intercept[i]>qGoodIntercept[1])) &&
                 slopeapprox[i]>=qOkSlopes[0] && slopeapprox[i]<= qOkSlopes[1] && intercept[i]>=qOkIntercept[0] &&intercept[i]<=qOkIntercept[1]){
            labelvec[i]->setText("Monitoring ADC OK");
            labelvec[i]->setStyleSheet("background-color: yellow");
            m_hResults.h_monitoringADC[i]=true;
            m_hResults.h_VMMResults[i].insert("MonitoringADC",1);
            sx << 1;
        }
        else{
            labelvec[i]->setText("Monitoring ADC bad");
            labelvec[i]->setStyleSheet("background-color: red");
            m_hResults.h_monitoringADC[i]=false;
            m_hResults.h_VMMResults[i].insert("MonitoringADC",2);
            sx << 2;
        }
        m_labelstoclear.push_back(labelvec[i]);
    }
    sx<< std::endl;
    m_hResults.h_DBExportStrings[0].insert("monitoradcslope",QString::number(slopeapprox[0]));
    m_hResults.h_DBExportStrings[1].insert("monitoradcslope",QString::number(slopeapprox[1]));
    m_hResults.h_DBExportStrings[0].insert("monitoradcintercept",QString::number(intercept[0]));
    m_hResults.h_DBExportStrings[1].insert("monitoradcintercept",QString::number(intercept[1]));

    //Plotting of Data on Screen:
    PlotData(thrDACvals,readADC,"Threshold DAC", "ADC Value", "Monitoring ADC");
    handleTemperatures();
    return sx.str();
}

std::string TestModule::TestNeighbouring()
{
    if(m_terminator){
        return "Test aborted";
    }
    handleTemperatures();
    stringstream sx;
    VMM* vmm[2];
    vmm[0] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0];
    vmm[1] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1];
    FECConfigModule* fcm = m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_fecConfigModule;
    m_mainWindow->m_daqs[0].SendAll();
    QThread::usleep(1000);

    int runmode = m_mainWindow->m_daqWindow->ui->comboBoxRunMode->findText("user settings");
    m_mainWindow->m_daqWindow->ui->comboBoxRunMode->setCurrentIndex(runmode);
    int calibtype = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->findText("Channels");
    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(calibtype);
    m_mainWindow->m_daqWindow->ui->Runs->setValue(10);
    vmm[0]->SetRegi("sng",1);
    vmm[1]->SetRegi("sng",1);
    QVector<double> allhits[2];
    for(int j=0;j<2;j++){
        for(int i=0;i<64;i++){
            cout << "Testing Channel "<< i << " on VMM " << j << endl <<endl <<endl <<endl <<endl;
            vmm[j]->SetRegi("st",1,i);
            m_mainWindow->m_daqs[0].SendAll();
            m_calibmod->StartCalibration();
            while(!(m_calibmod->m_dataAvailable)){
                QCoreApplication::processEvents();
            }
            vmm[j]->SetRegi("st",0,i);
            handleTemperatures();
            QVector<double> hits[2] = {QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][0]),QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][1])};
            for(int h=0; h<hits[0].size();h++){
                allhits[0].resize(hits[0].size());
                allhits[1].resize(hits[0].size());
                allhits[0][h]+=hits[0][h];
                allhits[1][h]+=hits[1][h];
            }
            if(i==63){
                if(j==0){
                    if(m_hResults.h_ChannelResults[j][i].value("Internal")!=2){
                        if(fabs(hits[j][i-1]-hits[j][i])<10 && fabs(hits[1][0]-hits[j][i])<10){
                            sx << "Neighbouring worked for Channel " << i << " on VMM " << j << endl;
                        }
                    }
                }
                if(j==1){
                    if(m_hResults.h_ChannelResults[j][i].value("Internal")!=2){
                        if(fabs(hits[j][i-1]-hits[j][i])<10){
                            sx << "Neighbouring worked for Channel " << i << " on VMM " << j << endl;
                        }
                    }
                }
            }
            else if(i==0){
                if(j==0){
                    if(m_hResults.h_ChannelResults[j][i].value("Internal")!=2){
                        if(fabs(hits[j][i+1]-hits[j][i])<10){
                            sx << "Neighbouring worked for Channel " << i << " on VMM " << j << endl;
                        }
                    }
                }
                else if(j==1){
                    if(m_hResults.h_ChannelResults[j][i].value("Internal")!=2){
                        if(fabs(hits[0][63]-hits[j][i])<10 && fabs(hits[j][i+1]-hits[j][i])<10){
                            sx << "Neighbouring worked for Channel " << i << " on VMM " << j << endl;
                        }
                    }
                }
            }
            else{
                if(m_hResults.h_ChannelResults[j][i].value("Internal")!=2){
                    if(fabs(hits[j][i-1]-hits[j][i])<10 && fabs(hits[j][i+1]-hits[j][i])<10){
                        sx << "Neighbouring worked for Channel " << i << " on VMM " << j << endl;
                    }
                }
            }
        }
    }

    QVector<double> channels;
    for(int i=0; i< allhits[0].size();i++){
        channels.append(i);
    }
    int norm = 384;
    for(int i=0;i<2;i++){
        for(int j=0;j<64;j++){
            if(j!=0 && j!=63){
                double ch = allhits[i][j];
                double rneigh = allhits[i][j+1];
                double lneigh = allhits[i][j-1];
                if(ch < norm){
                    if(lneigh<norm){
                        if(rneigh<norm){
                            MarkChannel("Neighbor",i,j,"bad");
                        }
                    }
                }
                else{
                    MarkChannel("Neighbor",i,j,"good");
                }
            }
            else if(j==0){
                if(i==0){
                    double ch = allhits[i][j];
                    double rneigh = allhits[i][j+1];
                    if(rneigh<norm){
                        if(ch<=norm*1/3.){
                            MarkChannel("Neighbor",i,j,"bad");
                        }
                    }
                    else{
                        MarkChannel("Neighbor",i,j,"good");
                    }
                }
                if(i==1){
                    double ch = allhits[i][j];
                    double rneigh = allhits[i][j+1];
                    double lneigh = allhits[0][63];
                    if(ch<norm){
                        if(lneigh<norm){
                            if(rneigh<norm){
                                MarkChannel("Neighbor",i,j,"bad");
                            }
                        }
                    }
                    else{
                        MarkChannel("Neighbor",i,j,"good");
                    }
                }
            }
            else if(j==63){
                if(i==1){
                    double ch = allhits[i][j];
                    double lneigh = allhits[i][j-1];
                    if(lneigh<norm){
                        if(ch<=norm*1/3.){
                            MarkChannel("Neighbor",i,j,"bad");
                        }
                    }
                    else{
                        MarkChannel("Neighbor",i,j,"good");
                    }
                }
                if(i==0){
                    double ch = allhits[i][j];
                    double rneigh = allhits[1][0];
                    double lneigh = allhits[i][j-1];
                    if(ch<norm){
                        if(lneigh<norm){
                            if(rneigh<norm){
                                MarkChannel("Neighbor",i,j,"bad");
                            }
                        }
                    }
                    else{
                        MarkChannel("Neighbor",i,j,"good");
                    }
                }
            }
        }
    }

    PlotData(channels,allhits,"Channelnr","hits","Neighbouring");
    vmm[0]->SetRegi("sng",0);
    vmm[1]->SetRegi("sng",0);
    handleTemperatures();
    return sx.str();
}

//Template to construct own tests:
/*std::string TestModule::TestSomething()
{
    stringstream sx;
    int runmode = m_mainWindow->m_daqWindow->ui->comboBoxRunMode->findText("automatic calibration"); //or "user settings"
    m_mainWindow->m_daqWindow->ui->comboBoxRunMode->setCurrentIndex(runmode);
    int calibtype = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->findText("Offline ADC"); //"Channels"/"ADC"/"TDC"/"BCID"
    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(calibtype);
    m_calibmod->StartCalibration();

    //Acquire Data
    while(!(m_calibmod->m_dataAvailable)){
        QCoreApplication::processEvents();
    }

    QVector<double> x[VMMS_PER_HYBRID] = {QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][0]),QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][1])};
}
*/


std::string TestModule::TestThrTrimmability()
{
    if(m_terminator){
        return "Test aborted";
    }
    handleTemperatures();
    stringstream sx;
    QVector<double> trims;
    QVector<double> thrs[2][64];
    QVector<double> ranges[2];
    QVector<double> chans;
    double thresholdrange [2] = {m_mainWindow->m_daqWindow->ui->thresholdstatuslabel1->text().remove("mV").toDouble(),m_mainWindow->m_daqWindow->ui->thresholdstatuslabel2->text().remove("mV").toDouble()};
    QThread::usleep(1000000);
    cout << "Thresholdranges" <<  thresholdrange[0] << thresholdrange[1]<<endl;
    QThread::usleep(1000000);
    VMM* vmm[2];
    vmm[0] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0];
    vmm[1] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1];
    FECConfigModule* fcm = m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_fecConfigModule;
    bool mon[2] = {m_hResults.h_monitoringADC[0],m_hResults.h_monitoringADC[1]};

    for(int j=0;j<32;){
        for(int i=0;i<64;i++){
            vmm[0]->SetRegi("sd",j,i);
            vmm[1]->SetRegi("sd",j,i);
            vmm[0]->SetRegi("monitoring",i);
            vmm[1]->SetRegi("monitoring",i);
            vmm[0]->SetRegi("smx",1,i);
            vmm[1]->SetRegi("smx",1,i);
            m_mainWindow->m_daqs[0].SendAll();
            int thr0 = fcm->ReadADC(m_HDMI_test,0,2);
            int thr1 = fcm->ReadADC(m_HDMI_test,1,2);
            thrs[0][i].push_back(thr0);
            thrs[1][i].push_back(thr1);
            vmm[0]->SetRegi("smx",0,i);
            vmm[1]->SetRegi("smx",0,i);
        }
        trims.push_back(j);
        j+=2;
    }
    for(int i=0; i< 64;i++){
        vmm[0]->SetRegi("sd",0,i);
        vmm[1]->SetRegi("sd",0,i);
    }
    for(int i=0;i<64;i++){
        chans.push_back(i);
    }

    for(int j=0;j<2;j++){
        for(int i=0;i<64;i++){
            bool bad = false;
            double minval = *std::min_element(thrs[j][i].begin(),thrs[j][i].end());
            double maxval = *std::max_element(thrs[j][i].begin(),thrs[j][i].end());
            double range = maxval-minval;
            cout << "min " << minval << " max " << maxval << " for " << j << "," << i << ",";
            ranges[j].push_back(range);
            for(int h=1;h<thrs[j][i].size();h++){
                if(thrs[j][i][h]>thrs[j][i][h-1]){
                    bad = true;
                }
            }
            double required = thresholdrange[j];
            if(range<required){
                bad = true;
            }
            if(bad==true){
                MarkChannel("Threshold Trimability",j,i,"bad");
                cout<< "Marking Channel as bad (Thresholdtrinmmabiity"<<endl;
            }
            else{
                MarkChannel("Threshold Trimability",j,i,"good");
                cout<< "Marking Channel as good (Thresholdtrinmmabiity"<<endl;

            }

        }
    }
    cout << "finished thrtrimability measurement" <<endl;
    for(int i=0;i<ranges[0].size();i++){
        cout << ranges[0][i]<<endl;
    }

    PlotData(chans,ranges,"Channel", "Trimmable Range","Threshold trimability range");
    PlotData(trims,thrs[0],"thresholdtrim","measured threshold","Threshold Trimability VMM0","Ch",64);
    PlotData(trims,thrs[1],"thresholdtrim","measured threshold","Threshold Trimability VMM1","Ch",64);
    handleTemperatures();
    return "finished";
}

bool TestModule::TestShortCircuit()
{
    return m_currentMonitor->TestForShortCircuit(0.3,2.0);
}

std::string TestModule::TestChannelsExternal(int tries,int restarts){
    if(m_terminator){
        return "Test aborted";
    }
    handleTemperatures();
    stringstream sx;
    stringstream log;
    bool forcecontinue =false;
    m_hResults.h_nwchannels[0][0]=64;
    m_hResults.h_nwchannels[0][1]=64;
    int runmode = m_mainWindow->m_daqWindow->ui->comboBoxRunMode->findText("user settings");
    m_mainWindow->m_daqWindow->ui->comboBoxRunMode->setCurrentIndex(runmode);
    int calibtype = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->findText("Channels");
    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(calibtype);
    //MaskNoisyChannels();
    m_mainWindow->m_daqWindow->ui->Runs->setValue(22000);
    QString program ="python3";
    QStringList arguments2;
    arguments2 << "../testing/siggen.py";
    QProcess* signalgenerator = new QProcess(this);
    m_calibmod->StartCalibration();
    signalgenerator->setProgram(program);
    signalgenerator->setArguments(arguments2);
    if(!signalgenerator->startDetached()){
        return TestChannelsExternal(tries,restarts+1);
    }
    std::chrono::high_resolution_clock::time_point teststart = std::chrono::high_resolution_clock::now();
    int i= 0;
    while(!(m_calibmod->m_dataAvailable)){
        if(forcecontinue){
            break;
        }
        QCoreApplication::processEvents();
        std::chrono::high_resolution_clock::time_point time_elapsed = std::chrono::high_resolution_clock::now();
        auto test_duration = std::chrono::duration_cast<std::chrono::milliseconds>( time_elapsed-teststart ).count();
        if(test_duration>25000){
            GetMessageHandler()("No hits received for 25 seconds, trying again.", "TestModule::TestChannelsExternal");
            m_calibmod->StopDataTaking();
            m_calibmod->Reset();
            if(restarts<2){
                log << "Tries: " << tries << " Restarts: " << restarts <<endl;
                GetMessageHandler()(log,"TestModule::TestChannelsExternal");
                return TestChannelsExternal(tries,restarts+1);
            }
            else{
                m_calibmod->StopDataTaking();
                forcecontinue=true;
            }
        }
    }

    if(forcecontinue){
        m_mainWindow->m_daqWindow->ui->Runs->setValue(100);
        m_calibmod->StartCalibration();
    }

    QVector<QVector<double>> allhits [2] = {m_calibmod->m_allhitdata[0][m_FEC_test][m_HDMI_test][0][0],m_calibmod->m_allhitdata[0][m_FEC_test][m_HDMI_test][0][1]};
    QVector<double> hits[VMMS_PER_HYBRID] = {QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][0]),QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][1])};
    QVector<double> channels;
    QVector<double> hitsfromall[2];
    QVector<double> adcs[2];
    QVector<double> channeladcs[2][64];
    QVector<double> channeltdcs[2][64];
    QVector<double> avgadc[2];
    QVector<double> avgtdc[2];
    QVector<double> tdcs[2];
    QVector<double> bcids[2];
    for(int i=0; i< 64;i++){
        channels.append(i);
    }
    for(int j=0;j<2;j++){
        if(hits[j].size()==0){
            hits[j].resize(64);
            for(int i=0;i<64;i++){
                hits[j][i]=0;
            }
        }
    }
    PlotData(channels,hits,QString("Channel#"),QString("Hits"),QString("Externalpulses"), QString("VMM"));
    //Evaluate Data: Mark Channels as broken (status 1), if they recieve no hits, as working (0)
    //for received hits between 9000 and 9200, as "suspicious" if the amount of hits are between 0 and 9000
    // and undefined (3) if more hits (should not happen)
    sx <<"Channels (external test) on : " ;
    QString test = "External";
    for(int j=0; j<2; j++){
        sx << "VMM"<<j <<": ";
        for(int i=0; i<64;i++){
            if(hits[j][i]<=200){
                MarkChannel(test,j,i,"bad");
                m_hResults.h_nwchannels[0][j]--;
                sx << i << " broken, ";
            }
            else if(hits[j][i]>=9000 && hits[j][i]<9200){
                MarkChannel(test,j,i,"good");
            }
            else if(hits[j][i]<9000 && hits[j][i]>200){
                MarkChannel(test,j,i,"ok");
                sx << i << " suspicious, ";
            }
            else{
                MarkChannel(test,j,i,"ok");
                sx << i << " suspicious, ";
            }
        }
    }

    //test to filter
    for(int i=0; i<64; i++){
        hitsfromall[0].push_back(0);
        hitsfromall[1].push_back(0);
    }
    for(int j=0;j<2;j++){
        if(allhits[j].size()==0){
            for(int k =0;k<64;k++){
                channeladcs[j][k].push_back(0);
                channeltdcs[j][k].push_back(0);
            }
        }
        else{
            for(int i=0; i<allhits[j].size();i++){
                QVector<double> hit = allhits[j][i];
                double chNo =hit[0];
                double bcid = hit[1];
                double tdc = hit[2];
                double adc = hit[3];
                int chs= int(chNo);
                bcids[j].push_back(bcid);
                tdcs[j].push_back(tdc);
                adcs[j].push_back(adc);
                channeladcs[j][chs].push_back(adc);
                channeltdcs[j][chs].push_back(tdc);
                hitsfromall[j][chs]++;
            }
        }
    }
    for(int i=0; i<2;i++){
        for(int j=0; j<64;j++){
            avgadc[i].push_back(getAverage(channeladcs[i][j]));
            avgtdc[i].push_back(getAverage(channeltdcs[i][j]));
        }
    }

//    sx << evaluateADCCalibration(avgadc[0],"External") << evaluateADCCalibration(avgadc[1],"External");
    QVector<double> popt;
    QVector<double> fits[2];
    fits[0] = evaluateADCCalibrationFit(channels,avgadc[0],"External",0,popt);
    fits[1] = evaluateADCCalibrationFit(channels,avgadc[1],"External",1,popt);
/*
    double adcdiff[2] = {evaluateADCCalibration(avgadc[0],"external"),evaluateADCCalibration(avgadc[1],"external")};
    double qGood2 = m_mainWindow->m_daqWindow->ui->adcrange_g_l->text().toDouble();
    double qOk2 = m_mainWindow->m_daqWindow->ui->adcrange_o_l->text().toDouble();
    QLabel* labels[2] = {m_mainWindow->m_daqWindow->ui->adccurvestatus3label, m_mainWindow->m_daqWindow->ui->adccurvestatus4label};
    for(int i=0;i<2;i++){
        if(adcdiff[i]<0.000000000000001){
            sx << "ADC Difference 0 or smaller" << adcdiff[i]<<endl;
            adcdiff[i] = 4095;
        }
        QString labeltext = QString::number(adcdiff[i])+"mV";
        if(adcdiff[i]<=qGood2){
            labels[i]->setStyleSheet("background-color: lightgreen");
            m_hResults.h_VMMResults[i].insert("ADCCurveExt",0);
        }
        else if(adcdiff[i]>qGood2 && adcdiff[i]<=qOk2){
            labels[i]->setStyleSheet("background-color: yellow");
            m_hResults.h_VMMResults[i].insert("ADCCurveExt",1);
        }
        else{
            labels[i]->setStyleSheet("background-color: red");
            m_hResults.h_VMMResults[i].insert("ADCCurveExt",2);
        }
        m_labelstoclear.push_back(labels[i]);
        labels[i]->setText(labeltext);
        for(int j=0;j<64;j++){
            if(avgadc[i][j]==0){
                MarkChannel("ADCCalibrationexternal",i,j,"bad");
            }
            else{
                MarkChannel("ADCCalibrationexternal",i,j,"good");
            }
        }

    }
    m_hResults.h_DBExportStrings[0].insert("ADCCurveExt",QString::number(adcdiff[0]));
    m_hResults.h_DBExportStrings[1].insert("ADCCurveExt",QString::number(adcdiff[1]));*/
    PlotData(channels,avgadc,QString("Channel"),QString("Average ADC"),QString("ADCCalibrationExternal"));
    AddFitToPlot(channels,fits);
    //PlotData(channels,avgtdc,QString("Channel"),QString("Average TDC"),QString("TDCCalibrationExternal"));
    PlotHistogram(adcs,0,1024,1024,QString("ADC"),QString("n"),QString("ADCExternalPulses"), QString("ADC Histogram"));
    PlotHistogram(tdcs,0,256,256,QString("TDC"),QString("n"),QString("TDCExternalPulses"), QString("TDC Histogram"));
    PlotHistogram(bcids,0,4096,4096,QString("BCID"),QString("n"),QString("BCIDExternalPulses"), QString("BCID Histogram"));
    int n2000 = bcids[0].count(2000)+bcids[1].count(2000);
    if(!forcecontinue){
        if(n2000>100){
            if(tries<=3){
                GetMessageHandler()("Something went wrong with the Measurement, restarting","TestModule::TestChannelsExternal");
                return TestChannelsExternal(tries+1,0);
            }
            else{
                GetMessageHandler()("External Test Pulse measurement could not be executed without errors, maybe need to check manually","TestModule::TestChannelsExternal");
            }
        }
    }

    sx << endl;
    QLabel* labelvec[2] = {m_mainWindow->m_daqWindow->ui->extchannels1label,m_mainWindow->m_daqWindow->ui->extchannels2label};
    QString exports[2] = {"",""};
    double qGood = m_mainWindow->m_daqWindow->ui->external_g_l->text().toDouble();
    double qOk = m_mainWindow->m_daqWindow->ui->external_o_l->text().toDouble();
    //Classify to good, ok, or bad depending on number of working channels
    for(int i = 0; i<VMMS_PER_HYBRID;i++){
        int working = m_hResults.h_nwchannels[0][i];
        labelvec[i]->setWordWrap(true);
        QString labeltext = QString::number(working);
        labelvec[i]->setText(labeltext);
        exports[i] += QString::number(working);
        if(working>=qGood){
            labelvec[i]->setStyleSheet("background-color: lightgreen");
            m_hResults.h_VMMResults[i].insert("External",0);
        }
        else if(working<qGood && working>=qOk){
            labelvec[i]->setStyleSheet("background-color: yellow");
            m_hResults.h_VMMResults[i].insert("External",1);
        }
        else{
            labelvec[i]->setStyleSheet("background-color: red");
            m_hResults.h_VMMResults[i].insert("External",2);
        }
        m_labelstoclear.push_back(labelvec[i]);

    }/*
    m_hResults.h_DBExportStrings.insert("WorkingChannels_VMM0_ext",exports[0]);
    m_hResults.h_DBExportStrings.insert("WorkingChannels_VMM1_ext",exports[1]);
    m_hResults.h_DBExportStrings.insert("BrokenChannels_VMM0", brokenstring[0]);
    m_hResults.h_DBExportStrings.insert("BrokenChannels_VMM1", brokenstring[1]);*/
    m_hResults.h_DBExportStrings[0].insert("WorkingChannelsExt",exports[0]);
    m_hResults.h_DBExportStrings[1].insert("WorkingChannelsExt",exports[1]);
    handleTemperatures();
    return sx.str();
}

string TestModule::TestChannelsInternal()
{
    if(m_terminator){
        return "Test aborted";
    }
    cout << "trying internal test pulses"<<endl;
    //handleTemperatures();
    stringstream sx;
    bool forcecontinue = false;
    m_hResults.h_nwchannels[1][0]=64;
    m_hResults.h_nwchannels[1][1]=64;
    //MaskNoisyChannels();
    VMM* vmm[2];
    vmm[0] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0];
    vmm[1] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1];
    FECConfigModule* fcm = m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_fecConfigModule;
    int runmode = m_mainWindow->m_daqWindow->ui->comboBoxRunMode->findText("user settings");
    m_mainWindow->m_daqWindow->ui->comboBoxRunMode->setCurrentIndex(runmode);
    int calibtype = m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->findText("Channels");
    m_mainWindow->m_daqWindow->ui->comboBoxCalibrationType->setCurrentIndex(calibtype);
    //turn on all test capacitors
    for(int i=0; i<64;i++){
        vmm[0]->SetRegi("st",1,i);
        vmm[1]->SetRegi("st",1,i);
    }
    //Set up for data acquisition
    m_mainWindow->m_daqs[0].SendAll();
    m_mainWindow->m_daqWindow->ui->Runs->setValue(10000);
    if(m_currmon){
        //m_currentMonitor->forceRead();
    }
    m_calibmod->StartCalibration();
    //wait for acquisition to finish
    std::chrono::high_resolution_clock::time_point teststart = std::chrono::high_resolution_clock::now();
    while(!(m_calibmod->m_dataAvailable)){
        QCoreApplication::processEvents();
        std::chrono::high_resolution_clock::time_point time_elapsed = std::chrono::high_resolution_clock::now();
        auto test_duration = std::chrono::duration_cast<std::chrono::milliseconds>( time_elapsed-teststart ).count();
        if(test_duration>15000){
            m_calibmod->StopDataTaking();
            forcecontinue = true;
            break;
        }
    }
    if(forcecontinue){
        m_mainWindow->m_daqWindow->ui->Runs->setValue(100);
        m_calibmod->StartCalibration();
    }

    //evaluate data
    QVector<double> hits[VMMS_PER_HYBRID] = {QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][0]),QVector<double>::fromStdVector(m_calibmod->m_mean[0][m_FEC_test][m_HDMI_test][0][1])};
    QVector<double> channels;
    for(int i=0; i< 64;i++){
        channels.append(i);
    }
    for(int j=0;j<2;j++){
        if(hits[j].size()==0){
            hits[j].resize(64);
            for(int i=0;i<64;i++){
                hits[j][i]=0;
            }
        }
    }
    double avg[2] = {getMedian(hits[0]),getMedian(hits[1])};
    for(int i=0;i<2;i++){
        if(avg[i]<1000){
            double* max = std::max_element(hits[i].begin(),hits[i].end());
            if(*max<1000){
                avg[i]=1000;
            }
            else{
                avg[i]=*max;
            }
        }
    }
    PlotData(channels,hits,QString("Channel#"),QString("Hits"),QString("Internalpulses"), QString("VMM"));
    sx <<"Channels (internal test) on : " ;
    QString test = "Internal";
    for(int j=0; j<2; j++){
        sx << "VMM"<<j <<": ";
        for(int i=0; i<64;i++){
            if(hits[j][i]<=300){
                MarkChannel(test,j,i,"bad");
                m_hResults.h_nwchannels[1][j]--;
                sx << i << " broken, ";
            }
            else if(fabs(hits[j][i]-avg[j])<1000){
                MarkChannel(test,j,i,"good");
            }
            else{
                MarkChannel(test,j,i,"unknown");
                sx << i << " suspicious, ";
            }
        }
    }
    sx << endl;

    for(int i=0; i<64;i++){
        vmm[0]->SetRegi("st",0,i);
        vmm[1]->SetRegi("st",0,i);
    }
    m_mainWindow->m_daqs[0].SendAll();

    QLabel* labelvec[2] = {m_mainWindow->m_daqWindow->ui->intchannels1label,m_mainWindow->m_daqWindow->ui->intchannels2label};
    QString exports[2] = {"",""};
    double qGood = m_mainWindow->m_daqWindow->ui->internal_g_l->text().toDouble();
    double qOk = m_mainWindow->m_daqWindow->ui->internal_o_l->text().toDouble();
    //Classify to good, ok, or bad depending on number of working channels
    for(int i = 0; i<VMMS_PER_HYBRID;i++){
        int working = m_hResults.h_nwchannels[1][i];
        labelvec[i]->setWordWrap(true);
        QString labeltext = QString::number(working);
        labelvec[i]->setText(labeltext);
        exports[i] += QString::number(working);
        if(working>=qGood){
            labelvec[i]->setStyleSheet("background-color: lightgreen");
            m_hResults.h_VMMResults[i].insert("Internal",0);
        }
        else if(working<qGood && working>=qOk){
            labelvec[i]->setStyleSheet("background-color: yellow");
            m_hResults.h_VMMResults[i].insert("Internal",1);
        }
        else{
            labelvec[i]->setStyleSheet("background-color: red");
            m_hResults.h_VMMResults[i].insert("Internal",2);
        }
        m_labelstoclear.push_back(labelvec[i]);
    }
    //m_hResults.h_DBExportStrings.insert("WorkingChannels_VMM0_int",exports[0]);
    //m_hResults.h_DBExportStrings.insert("WorkingChannels_VMM1_int",exports[1]);
    m_hResults.h_DBExportStrings[0].insert("WorkingChannelsInt",exports[0]);
    m_hResults.h_DBExportStrings[1].insert("WorkingChannelsInt",exports[1]);


    QVector<QVector<double>> allhits [2] = {m_calibmod->m_allhitdata[0][m_FEC_test][m_HDMI_test][0][0],m_calibmod->m_allhitdata[0][m_FEC_test][m_HDMI_test][0][1]};
    QVector<double> hitsfromall[2];
    QVector<double> adcs[2];
    QVector<double> channeladcs[2][64];
    QVector<double> channeltdcs[2][64];
    QVector<double> avgadc[2];
    QVector<double> avgtdc[2];
    QVector<double> tdcs[2];
    QVector<double> bcids[2];

    for(int i=0; i<64; i++){
        hitsfromall[0].push_back(0);
        hitsfromall[1].push_back(0);
    }
    for(int j=0;j<2;j++){
        if(allhits[j].size()==0){
            for(int k =0;k<64;k++){
                channeladcs[j][k].push_back(0);
                channeltdcs[j][k].push_back(0);
            }
        }
        else{
            for(int i=0; i<allhits[j].size();i++){
                QVector<double> hit = allhits[j][i];
                double chNo =hit[0];
                double bcid = hit[1];
                double tdc = hit[2];
                double adc = hit[3];
                int chs= int(chNo);
                bcids[j].push_back(bcid);
                tdcs[j].push_back(tdc);
                adcs[j].push_back(adc);
                channeladcs[j][chs].push_back(adc);
                channeltdcs[j][chs].push_back(tdc);
                hitsfromall[j][chs]++;
            }
        }
    }
    for(int i=0; i<2;i++){
        for(int j=0; j<64;j++){
            avgadc[i].push_back(getAverage(channeladcs[i][j]));
            avgtdc[i].push_back(getAverage(channeltdcs[i][j]));
        }
    }

    sx << evaluateADCCalibration(avgadc[0],"Internal")<< evaluateADCCalibration(avgadc[1],"Internal");
    QVector<double> popt;
    QVector<double> fits[2];
    fits[0] = evaluateADCCalibrationFit(channels,avgadc[0],"Internal",0,popt);
    fits[1] = evaluateADCCalibrationFit(channels,avgadc[1],"Internal",1,popt);
    //double adcdiff[2] = {evaluateADCCalibration(avgadc[0],"internal"),evaluateADCCalibration(avgadc[1],"internal")};
    //double qGood2 = m_mainWindow->m_daqWindow->ui->adcrange_g_l->text().toDouble()*5/7.;
    //double qOk2 = m_mainWindow->m_daqWindow->ui->adcrange_o_l->text().toDouble()*5/7.;
    //QLabel* labels[2] = {m_mainWindow->m_daqWindow->ui->adccurvestatus1label, m_mainWindow->m_daqWindow->ui->adccurvestatus2label};
    /*for(int i=0;i<2;i++){
        if(adcdiff[i]<0.000000000000001){
            sx << "ADC Difference 0 or smaller" << adcdiff[i]<<endl;
            adcdiff[i] = 4095;
        }
        QString labeltext = QString::number(adcdiff[i])+"mV";
        if(adcdiff[i]<=qGood2){
            labels[i]->setStyleSheet("background-color: lightgreen");
            m_hResults.h_VMMResults[i].insert("ADCCurveInt",0);
        }
        else if(adcdiff[i]>qGood2 && adcdiff[i]<=qOk2){
            labels[i]->setStyleSheet("background-color: yellow");
            m_hResults.h_VMMResults[i].insert("ADCCurveInt",1);
        }
        else{
            labels[i]->setStyleSheet("background-color: red");
            m_hResults.h_VMMResults[i].insert("ADCCurveInt",2);
        }
        m_labelstoclear.push_back(labels[i]);
        labels[i]->setText(labeltext);
        for(int j=0;j<64;j++){
            if(avgadc[i][j]==0){
                MarkChannel("ADCCalibrationinternal",i,j,"bad");
            }
            else{
                MarkChannel("ADCCalibrationinternal",i,j,"good");
            }
        }

    }
    m_hResults.h_DBExportStrings[0].insert("ADCCurveInt",QString::number(adcdiff[0]));
    m_hResults.h_DBExportStrings[1].insert("ADCCurveInt",QString::number(adcdiff[1]));
    */
    double qGood3[2] = {m_mainWindow->m_daqWindow->ui->tdc_g_l->text().toDouble(),m_mainWindow->m_daqWindow->ui->tdc_g_u->text().toDouble()};
    double qOk3[2] = {m_mainWindow->m_daqWindow->ui->tdc_o_l->text().toDouble(),m_mainWindow->m_daqWindow->ui->tdc_o_u->text().toDouble()};
    double avgavgtdc[2] = {getAverage(avgtdc[0]),getAverage(avgtdc[1])};
    QLabel* labels2[2] = {m_mainWindow->m_daqWindow->ui->tdcstatuslabel1, m_mainWindow->m_daqWindow->ui->tdcstatuslabel2};
    for(int i=0;i<2;i++){
        QString labeltext = QString::number(avgavgtdc[i])+"mV";
        for(int j=0; j<64;j++){
            if(avgtdc[i][j]>qGood3[0]&&avgtdc[i][j]<qGood3[1]){
                MarkChannel("TDCCalibrationInternal",i,j,"good");
            }
            else if(avgtdc[i][j]>qOk3[0]&&avgtdc[i][j]<qOk3[1]){
                MarkChannel("TDCCalibrationInternal",i,j,"ok");
            }
            else{
                MarkChannel("TDCCalibrationInternal",i,j,"bad");
            }
        }

        if(avgavgtdc[i]>=qGood3[0]&&avgavgtdc[i]<=qGood3[1]){
            labels2[i]->setStyleSheet("background-color: lightgreen");
            m_hResults.h_VMMResults[i].insert("avgTDC",0);
        }
        else if(avgavgtdc[i]>=qOk3[0]&&avgavgtdc[i]<=qOk3[1]){
            labels2[i]->setStyleSheet("background-color: yellow");
            m_hResults.h_VMMResults[i].insert("avgTDC",1);
        }
        else{
            labels2[i]->setStyleSheet("background-color: red");
            m_hResults.h_VMMResults[i].insert("avgTDC",2);
        }
        labels2[i]->setText(QString::number(avgavgtdc[i]));
        m_hResults.h_DBExportStrings[i].insert("avgTDC",QString::number(avgavgtdc[i]));
        m_labelstoclear.push_back(labels2[i]);
    }


    PlotData(channels,avgadc,QString("Channel"),QString("Average ADC"),QString("ADCCalibrationInternal"));
    PlotData(channels,avgtdc,QString("Channel"),QString("Average TDC"),QString("TDCCalibrationInternal"));
    AddFitToPlot(channels,fits);
    //PlotData(channels, hitsfromall,QString("Channel#"),QString("Hits"),QString("Internalpulses"), QString("VMM"));
    PlotHistogram(adcs,0,1024,1024,QString("ADC"),QString("n"),QString("ADCInternalPulses"), QString("ADC Histogram"));
    PlotHistogram(tdcs,0,256,256,QString("TDC"),QString("n"),QString("TDCInternalPulses"), QString("TDC Histogram"));
    PlotHistogram(bcids,0,4096,4096,QString("BCID"),QString("n"),QString("BCIDInternalPulses"), QString("BCID Histogram"));
    //handleTemperatures();
    return sx.str();
}

void TestModule::MarkChannel(QString whichtest, int vmm, int chno, QString status)
{
    if(QString::compare(status,"good",Qt::CaseInsensitive)==0){
        MarkChannel(whichtest,vmm,chno,0);
        cout << vmm << " " << chno << " good" <<endl;
        return;
    }
    else if(QString::compare(status,"ok",Qt::CaseInsensitive)==0){
        MarkChannel(whichtest,vmm,chno,1);
        cout << vmm << " " << chno << " ok" <<endl;
        return;
    }
    else if(QString::compare(status,"bad",Qt::CaseInsensitive)==0){
        MarkChannel(whichtest,vmm,chno,2);
        cout << vmm << " " << chno << " bad" <<endl;
        return;
    }
    else{
        MarkChannel(whichtest,vmm,chno,3);
        cout << vmm << " " << chno << " unknown" <<endl;
        return;
    }
}

void TestModule::MarkChannel(QString whichtest, int vmm, int chno, int status)
{
        m_hResults.h_ChannelResults[vmm][chno].insert(whichtest,status);
}

void TestModule::handleCurrents()
{
    //m_currentMonitor->finishMonitor();
    m_currentMonitor->finishMonitoringProcess();
    m_currentMonitor->loadCurrents();
    QVector<QPair<QDateTime,double>> curr19 = m_currentMonitor->get_currents_1_9V();
    QVector<QPair<QDateTime,double>> curr29 = m_currentMonitor->get_currents_2_9V();
    QVector<double> currs[2];
    QVector<QDateTime> times;
    int samesize;
    double avg19=0;
    double avg29=0;
    double med19=0;
    double med29=0;
    if(curr19.size()==0 || curr29.size()==0){
        GetMessageHandler()("Error while obtaining currents. Exiting Function", "TestModule::handleCurrents");
        return;
    }
    else if(curr19.size()==curr29.size()){
        samesize = curr19.size();
    }
    else if(curr19.size()>curr29.size()){
        samesize = curr29.size();
    }
    else if(curr19.size()<curr29.size()){
        samesize = curr29.size();
    }
    for(int i=0; i< samesize; i++){
        QDateTime theTime = curr19[i].first;
        times.append(theTime);
        double theCurr = curr19[i].second;
        currs[0].append(theCurr);
        avg19+=theCurr;
        //cout << theTime.toString().toStdString() << "\t" << theCurr << "A" << std::endl;
    }
    for(int i=0; i< samesize; i++){
        QDateTime theTime = curr29[i].first;
        times.append(theTime);
        double theCurr = curr29[i].second;
        currs[1].append(theCurr);
        avg29+=theCurr;
        //cout << theTime.toString().toStdString() << "\t" << theCurr << "A" << std::endl;
    }
    QVector<double> secondssincestart;
    const QDateTime start = times[0];
    for(int i=0; i< samesize; i++){
        secondssincestart.append(start.msecsTo(times[i])/1000.);
    }
    PlotData(secondssincestart,currs,"Time since first start [s]", "Current [A]", "Current over time", "Current @CH");
    QString exports[2] = {"",""};

    //HIER BITTE WIEDER EINFÜGEN!
    //m_currentMonitor->setupCurrentMonitor();


    //m_mainWindow->m_daqWindow->ui->lineEdit_1_9V->setText(QString::number(avg19/samesize));
    med19 = getMedian(currs[0]);
    med29 = getMedian(currs[1]);
    m_mainWindow->m_daqWindow->ui->lineEdit_1_9V->setText(QString::number(med19));
    //m_hResults.h_current_1_9V=avg19/samesize;
    //exports[0] = QString::number(avg19/samesize);
    m_hResults.h_current_1_9V=med19;
    exports[0] = QString::number(med19);
    //m_mainWindow->m_daqWindow->ui->lineEdit_2_9V->setText(QString::number(avg29/samesize));
    m_mainWindow->m_daqWindow->ui->lineEdit_2_9V->setText(QString::number(med29));

    //m_hResults.h_current_2_9V=avg29/samesize;
    //exports[1] = QString::number(avg29/samesize);
    m_hResults.h_current_2_9V=med29;
    exports[1] = QString::number(med29);
    m_hResults.h_DBExportStrings[0].insert("AvgCurr1_9V", exports[0]);
    m_hResults.h_DBExportStrings[0].insert("AvgCurr2_9V", exports[1]);
    m_hResults.h_DBExportStrings[1].insert("AvgCurr1_9V", exports[0]);
    m_hResults.h_DBExportStrings[1].insert("AvgCurr2_9V", exports[1]);
    GetMessageHandler()("Successfully handled currents", "TestModule::handleCurrents");
}

std::set<int> TestModule::findOutliers(QVector<double> input, double thr){
    std::set<int> indices;
    indices.clear();
    std::vector<double> copy = input.toStdVector();
    std::sort(copy.begin(),copy.end());
    double median;
    if(copy.size()%2==0){
        median = (copy[copy.size()/2]+copy[copy.size()/2-1])/2;
    }
    else{
        median = copy[copy.size()/2];
    }
    int chno = 0;
    for(int i=0; i< input.size();i++){
        double val = input[i];
        if(fabs(val-median)>thr){
            indices.insert(chno);
        }
        chno++;
    }
    return indices;
}

bool TestModule::exportResults()
{
    QString DBFilesPath = readSettingsDB();
    QSqlDatabase db = m_db;
    //QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //db.setDatabaseName("/home/fjaekel/VMM-software-RD51/build/testdb.db");
    db.open();
    //QSqlQuery creationquery;
    //creationquery.exec("CREATE TABLE [IF NOT EXISTS] Hybrid_Overview(Hybrid_ID text,Measurement_ID text,Hybrid_Class text,VMM int,VMM_Class text,BrokenChannels int[],Location text,Date date,MonitorADCSlope real,MonitorADCIntercept real,Baseline real,BaseNoise real,Threshold real,WorkingChannelsExt int,WorkingChannelsInt int,AvgCurr1_9V real,AvgCurr2_9V real,ADCExtCurvature real,ADCIntCurvature real,ADCExtVertex real, ADCIntVertex real,AVGTDC real,PRIMARY KEY(Measurement_ID, VMM));CREATE TABLE [IF NOT EXISTS] Hybrid_Data(Measurement_ID text PRIMARY KEY,Hybrid_ID text,Date date,Measurement_type text,labels text []);CREATE TABLE [IF NOT EXISTS] configs(Measurement_ID text,Date date,Filename text,Filecontent text,PRIMARY KEY(Measurement_ID,Filename));CREATE VIEW [IF NOT EXISTS] latest_measurements as SELECT * from hybrid_overview WHERE measurement_id IN (SELECT max FROM(SELECT hybrid_id,MAX(measurement_id) FROM hybrid_overview GROUP BY hybrid_id) as foo);");
    QSqlQuery query;
    QString measid = m_hResults.h_measurementID;
    QString path = m_mainWindow->GetApplicationPath();
    QDir dir(path);
    QDir dbdir(DBFilesPath);
    QFile DBentry(dbdir.filePath(measid+".txt"));
    QString queryforfile="";
    int firmware = m_hResults.h_firmware;
    QList<QString> measurements = m_hResults.h_plotsdata.keys();
    QString location = m_mainWindow->m_daqWindow->ui->lineEdit_location->text();
    for(int j=0; j<2;j++){
        QList<QString> stati = m_hResults.h_DBExportStrings[j].keys();
        QString querytext = "INSERT INTO hybrid_overview (Measurement_ID, Hybrid_ID, VMM,location";
        queryforfile += "INSERT INTO hybrid_overview (Measurement_ID, Hybrid_ID, VMM,location";
        for(int i=0; i<stati.size();i++){
            QString whichColumn = stati[i];
            querytext += ", " +whichColumn ;
            queryforfile += ", " +whichColumn ;
        }
        querytext +=",Date) VALUES(?,?,?,?";
        queryforfile += ",Date) VALUES('"+ measid+"','"+m_hResults.h_globalHybridID + "'," + QString::number(j) + ",'" + location;
        for(int i= 0; i<stati.size();i++){
            querytext += ",?";
        }
        querytext += ",?);";
        query.prepare(querytext);
        query.bindValue(0,measid);
        cout << measid.toStdString()<<",";
        query.bindValue(1,m_hResults.h_globalHybridID);
        cout<< m_hResults.h_globalHybridID.toStdString()<<",";
        cout << j << ",";
        query.bindValue(2,j);
        query.bindValue(3,location);
        for(int i=0; i< stati.size();i++){
            QString whichColumn = stati[i];
            QString enterToDB = m_hResults.h_DBExportStrings[j].value(whichColumn);
            cout << enterToDB.toStdString()<< ",";
            query.bindValue(i+4,enterToDB);
            queryforfile += "', '"+ enterToDB;
        }
        queryforfile +="','" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
        queryforfile +="');\n";
        cout << "Text that would get written into a file: "<< queryforfile.toStdString()<<endl;
        query.bindValue(stati.size()+4,"now()");
        cout << querytext.toStdString()<<endl;
        query.exec();
    }
    for(int i=0; i< measurements.size(); i++){
        QHash<QString,QString> labels = m_hResults.h_plotslabel.value(measurements[i]);
        QString type = labels.value("type");
        QString labelstring = labelsToSqlArray(labels);
        QHash<QString, QVector<double>> measmap = m_hResults.h_plotsdata.value(measurements[i]);
        queryforfile += "INSERT INTO hybrid_data (Measurement_ID, Hybrid_ID, Date, Measurement_type, labels) VALUES('";
        queryforfile += measid;
        queryforfile +="','"+m_hResults.h_globalHybridID;
        queryforfile += "','" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
        queryforfile += "','" +measurements[i].remove(" ");
        queryforfile += "','" + labelstring;
        queryforfile += "');\n";
        if(type == "points"){
            QVector<double> x = measmap.value("x");
            QVector<double> y1 = measmap.value("y1");
            QVector<double> y2 = measmap.value("y2");
            QString xstring = dataToSqlArray(x);
            QString y1string = dataToSqlArray(y1);
            QString y2string = dataToSqlArray(y2);
            QString query1 = "CREATE TABLE IF NOT EXISTS "+measurements[i].remove(" ") + "(Measurement_ID text, Hybrid_ID text, Date date, Measurement_type text, labels text[], x_ real[], y1_ real[], y2_ real[]);";
            queryforfile += query1;
            query.exec(query1);
            QString query2 = "INSERT INTO "+measurements[i].remove(" ") + "(Measurement_ID, Hybrid_ID, Date, Measurement_type, labels, x_, y1_, y2_) VALUES(?,?,?,?,?,?,?,?);";
            queryforfile += "INSERT INTO "+measurements[i].remove(" ") + "(Measurement_ID, Hybrid_ID, Date, Measurement_type, labels, x_, y1_, y2_) VALUES('";
            queryforfile += measid;
            queryforfile +="','"+m_hResults.h_globalHybridID;
            queryforfile += "','" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
            queryforfile += "','" +measurements[i].remove(" ");
            queryforfile += "','" + labelstring;
            queryforfile += "','" + xstring;
            queryforfile += "','" + y1string;
            queryforfile += "','" + y2string;
            queryforfile += "');\n";
            query.prepare(query2);
            query.bindValue(0,measid);
            query.bindValue(1,m_hResults.h_globalHybridID);
            query.bindValue(2,"now()");
            query.bindValue(3,measurements[i].remove(" "));
            query.bindValue(4,labelstring);
            query.bindValue(5,xstring);
            query.bindValue(6,y1string);
            query.bindValue(7,y2string);
            query.exec();
        }
        else if(type == "channels"){
            QVector<double> x = measmap.value("x");
            QString xstring = dataToSqlArray(x);
            QVector<QString> ystrings;
            QString query1 = "CREATE TABLE IF NOT EXISTS "+measurements[i].remove(" ") + "(x_ real[] ";
            queryforfile += "CREATE TABLE IF NOT exists "+measurements[i].remove(" ") + "(Measurement_ID text, Hybrid_ID text, Date date, Measurement_type text, labels text[], x_ real[] ";
            for(int i=0;i<64;i++){
                QString key = "y"+QString::number(i+1);
                QVector<double> yi = measmap.value(key);
                QString ystr = dataToSqlArray(yi);
                ystrings.push_back(ystr);
                QString tablekey = key+"_";
                query1+= ", " + tablekey + " real[]";
                queryforfile += ", " + tablekey + " real[]";
            }
            query1 += ") INHERITS(Hybrid_Data);";
            queryforfile += ");\n";
            query.exec(query1);
            //QString query2 = "INSERT INTO "+measurements[i].remove(" ") + "(Measurement_ID, Hybrid_ID, Date, Measurement_type, x_, y1_, y2_) VALUES(?,?,?,?,?,?,?);";
            QString query2 = "INSERT INTO "+measurements[i].remove(" ") +"(Measurement_ID, Hybrid_ID, Date, Measurement_type, labels, x_";
            queryforfile += "INSERT INTO "+measurements[i].remove(" ") + "(Measurement_ID, Hybrid_ID, Date, Measurement_type, labels, x_";
            for(int j=0;j<64;j++){
                QString row = ", y"+QString::number(j+1)+"_";
                query2+=row;
                queryforfile += row;
            }
            query2+=") VALUES(?,?,?,?,?,?";
            queryforfile +=") VALUES('";
            for(int j=0;j<64;j++){
                query2+=",?";
            }
            query2+=");";
            queryforfile += measid;
            queryforfile +="','"+m_hResults.h_globalHybridID;
            queryforfile += "','" + QDateTime::currentDateTime().toString("yyyy-MM-dd");
            queryforfile += "','" +measurements[i].remove(" ");
            queryforfile += "','" + labelstring;
            queryforfile += "','" + xstring;
            query.prepare(query2);
            query.bindValue(0,measid);
            query.bindValue(1,m_hResults.h_globalHybridID);
            query.bindValue(2,"now()");
            query.bindValue(3,measurements[i].remove(" "));
            query.bindValue(4,labelstring);
            query.bindValue(5,xstring);
            for(int j=0;j<64;j++)
            {
                QString valtobind = ystrings.at(j);
                query.bindValue(j+6,valtobind);
                queryforfile += "','"+valtobind;
            }
            queryforfile +="');\n";
            query.exec();
        }
    }
    DBentry.open(QIODevice::Append| QIODevice::Text);
    QTextStream out(&DBentry);
    out << queryforfile;
    db.close();
    QProcess* git = new QProcess(this);
    QStringList args;
    args << "pull";
    git->start("git",args);
    git->waitForFinished();
    args.clear();
    git->setWorkingDirectory(DBFilesPath);
    args << "add" << QString(DBFilesPath+"/"+measid+".txt");;
    git->start("git",args);
    git->waitForFinished();
    args.clear();
    args << "commit" << "-m" <<  QString(measid);
    git->start("git",args);
    git->waitForFinished();
    args.clear();
    args << "push";
    git->start("git",args);
    git->waitForFinished();
    GetMessageHandler()("Succesfully exported Results","TestModule::exportResults");
    return true;
}

QString TestModule::dataToSqlArray(QVector<double> vec, int len){
    QString outstring = "{";
    outstring.append(QString::number(vec[0]));
    for(int j=1; j< vec.size();j++){
        outstring.append(",");
        QString mini = QString::number(vec[j]);
        outstring.append(mini);
    }
    outstring.append("}");
    return outstring;
}

QString TestModule::dataToSqlArray(QVector<double> vec[], int len){
    QString outstring = "{{";
    outstring.append(QString::number(vec[0][0]));
    for(int i=0;i<len;i++){
        for(int j=1; j< vec[i].size();j++){
            outstring.append(",");
            QString mini = QString::number(vec[i][j]);
            outstring.append(mini);
        }
        outstring.append("},{");
    }
    outstring.append("}");
    return outstring;
}

QString TestModule::labelsToSqlArray(QHash<QString, QString> labels)
{
    QString out = "{";
    QString xlabel = labels["x"];
    QString ylabel = labels["y"];
    QString graphlabel = labels["graph"];
    QString type = labels["type"];
    out += type+",";
    out += xlabel+",";
    out += ylabel+",";
    out += graphlabel+"}";
    return out;
}

QString TestModule::dataToSqlArray(QVector<int> vec, int len){
    QString outstring = "{";
    if(vec.size()>0){
        outstring.append(QString::number(vec[0]));
    }
    for(int j=1; j< vec.size();j++){
        outstring.append(",");
        QString mini = QString::number(vec[j]);
        outstring.append(mini);
    }
    outstring.append("}");
    return outstring;
}

void TestModule::PlotData(QVector<double> x, QVector<double> y[], QString xlabel, QString ylabel, QString name, QString graphlabel, int datalen){
    QCustomPlot* plot = plotVector[0];
    plot->clearItems();
    plot->clearPlottables();
    plot->clearGraphs();
    plot->plotLayout()->removeAt(1);
    plot->legend = new QCPLegend();
    plot->axisRect()->insetLayout()->addElement(plot->legend, Qt::AlignRight|Qt::AlignTop);
    plot->axisRect()->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    plot->legend->setLayer("legend");
    plot->yAxis->setLabel(ylabel);
    plot->xAxis->setLabel(xlabel);
    plot->legend->setVisible(true);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    QCPLayoutGrid* subLayout = new QCPLayoutGrid();
    plot->plotLayout()->addElement(1, 0, subLayout);
    subLayout->setMargins(QMargins(5, 5, 5, 5));
    subLayout->addElement(0,0,plot->legend);
    plot->legend->setFillOrder(QCPLegend::foColumnsFirst);
    plot->plotLayout()->setRowStretchFactor(1, 0.001);
    plot->xAxis->rescale();
    plot->yAxis->rescale();
    plot->replot();
    QColor* colors = new QColor[datalen];
    if(datalen==2){
        colors[0] = Qt::blue;
        colors[1] = Qt::red;
    }
    else{
        for(int i =0;i<datalen;i++){
            double h = 0.618033988749895* 360./datalen * i;
            colors[i] = QColor::fromHsv(int(h),245,245,255);
        }
    }
    for(int i = 0; i<datalen; i++){
        QString graphname = graphlabel;
        graphname.append(QString::fromStdString(std::to_string(i)));
        plot->addGraph();
        plot->graph(i)->setName(graphname);
        plot->graph(i)->setLineStyle(QCPGraph::lsLine);
        plot->graph(i)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
        plot->graph(i)->setPen(QPen(QBrush(colors[i]),1));
        plot->graph(i)->setData(x,y[i]);
        plot->yAxis->rescale();
        plot->xAxis->rescale();
    }
    plot->yAxis->scaleRange(1.1);
    plot->replot(QCustomPlot::RefreshPriority(QCustomPlot::rpImmediateRefresh));
    //store plot data in HybridResult struct of TestModule
    if(name!=""){
        if(!m_hResults.h_plotsdata.keys().contains(name)){
            QHash<QString,QString> labels;
            QHash<QString,QVector<double>> data;
            data["x"] = x;
            for(int i=0;i<datalen;i++){
                QString key = "y"+QString::number(i+1);
                data[key]=y[i];
            }
//            data["y1"] = y[0];
//            data["y2"] = y[1];
            labels["x"] = xlabel;
            labels["y"] = ylabel;
            labels["graph"] = graphlabel;
            if(datalen>2){
                labels["type"]=QString("channels");
            }
            else{
                labels["type"] = QString("points");
            }
            m_hResults.h_plotslabel[name] = labels;
            m_hResults.h_plotsdata[name] = data;
            //plot->savePdf(name+".pdf");
        }
        if(m_hResults.h_plotsdata.keys().contains(name)){
            QHash<QString,QVector<double>> data;
            data["x"] = x;
            for(int i=0;i<datalen;i++){
                QString key = "y"+QString::number(i+1);
                data[key]=y[i];
            }
            m_hResults.h_plotsdata[name] = data;
        }

        if(m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->findText(name)==-1){
            m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->addItem(name);
            m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->setCurrentIndex(m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->findText(name));
        }
    }
    delete[] colors;
}

void TestModule::PlotHistogram(QVector<double> y[2], double xlow, double xhigh,int nbins, QString xlabel, QString ylabel, QString name, QString graphlabel){
    QVector<double> bins;
    QVector<double> ydatas;
    QVector<double> limits;
    limits.resize(3);
    limits[0] = xlow;
    limits[1] = xhigh;
    limits[2] = double(nbins);
    ydatas.resize(nbins);
    ydatas.fill(0);
    double binwidth = (xhigh-xlow)/nbins;
    for(int i=0; i<nbins; i++){
        double bincenter = xlow+i*binwidth-0.5*binwidth;
        bins.push_back(bincenter);
    }
    cout << xlow << xhigh << nbins<< endl;
    for(int j=0;j<2;j++){
        QVector<double> values =y[j];
        for(int i=0; i<values.size();i++){
            double val = values[i];
            int bin = int ((val-xlow)/binwidth);
            ydatas[bin]++;
        }
    }
    QCustomPlot* plot = plotVector[0];
    plot->clearItems();
    plot->clearPlottables();
    plot->clearGraphs();
    plot->plotLayout()->removeAt(1);
    plot->legend = new QCPLegend();
    plot->axisRect()->insetLayout()->addElement(plot->legend, Qt::AlignRight|Qt::AlignTop);
    plot->axisRect()->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    plot->legend->setLayer("legend");
    plot->yAxis->setLabel(ylabel);
    plot->xAxis->setLabel(xlabel);
    plot->legend->setVisible(true);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes);
    QCPLayoutGrid* subLayout = new QCPLayoutGrid();
    plot->plotLayout()->addElement(1, 0, subLayout);
    subLayout->setMargins(QMargins(5, 5, 5, 5));
    subLayout->addElement(0,0,plot->legend);
    plot->legend->setFillOrder(QCPLegend::foColumnsFirst);
    plot->plotLayout()->setRowStretchFactor(1, 0.001);
    QCPBars* histo = new QCPBars(plot->xAxis, plot->yAxis);
    histo->setData(bins,ydatas);
    histo->setName(name);
    histo->setPen(QPen(QColor(111, 9, 176).lighter(170)));
    histo->setBrush(QColor(111, 9, 176));
    plot->xAxis->rescale();
    plot->yAxis->rescale();
    plot->yAxis->scaleRange(1.1);
    plot->replot();
    if(name!=""){
        if(!m_hResults.h_histodata.keys().contains(name)){
            QHash<QString,QString> labels;
            QHash<QString,QVector<double>> data;
            cout <<limits[0] <<"," <<limits[1]<< "," <<limits[2]<<endl;
            data["x"] = bins;
            data["y1"] = y[0];
            data["y2"] = y[1];
            data["lims"] = limits;
            labels["x"] = xlabel;
            labels["y"] = ylabel;
            labels["graph"] = graphlabel;
            labels["type"] = QString("histo");

            m_hResults.h_plotslabel[name] = labels;
            m_hResults.h_histodata[name] = data;
            QVector<double> test = m_hResults.h_histodata.value(name).value("lims");
            cout <<test[0] <<"," <<test[1]<< "," <<test[2]<<endl;
        }
        else if(m_hResults.h_histodata.keys().contains(name)){
            QHash<QString,QVector<double>> data;
            data["x"] = bins;
            data["y1"] = y[0];
            data["y2"] = y[1];
            m_hResults.h_histodata[name] = data;
        }
        if(m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->findText(name)==-1){
            m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->addItem(name);
            m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->setCurrentIndex(m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->findText(name));
        }
    }
    return;
}

void TestModule::AddFitToPlot(QVector<double> x, QVector<double> y[], QString graphlabel, int datalen)
{
    QCustomPlot* plot = plotVector[0];
    QColor* colors = new QColor[datalen];
    if(datalen==2){
        colors[0] = Qt::blue;
        colors[1] = Qt::red;
    }
    for(int i = 0; i<datalen; i++){
        QString graphname = graphlabel;
        graphname.append(QString::fromStdString(std::to_string(i)));
        plot->addGraph();
        plot->graph(i+2)->setName(graphname);
        plot->graph(i+2)->setLineStyle(QCPGraph::lsLine);
        //plot->graph(i+2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
        plot->graph(i+2)->setPen(QPen(QBrush(colors[i]),1,Qt::DashLine));
        plot->graph(i+2)->setData(x,y[i]);
        plot->yAxis->rescale();
        plot->xAxis->rescale();
    }
    plot->yAxis->scaleRange(1.1);
    plot->replot(QCustomPlot::RefreshPriority(QCustomPlot::rpImmediateRefresh));
    delete[] colors;
}

bool TestModule::DeleteLastMeasurement()
{
    readSettingsDB();
    QString lastmeasure = m_hResults.h_measurementID;
    QSqlDatabase db = m_db;
    QSqlQuery query;
    /*QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setPort(5432);
    db.setUserName("fjaekel");
    db.setDatabaseName("testdb");
    db.setPassword("1234");*/
    db.open();
    QString prepare = "DELETE FROM hybrid_overview WHERE measurement_id = ?;";
    query.prepare(prepare);
    query.bindValue(0,lastmeasure);
    query.exec();
    QString prepare2 = "DELETE FROM hybrid_Data WHERE measurement_id = ?;";
    query.prepare(prepare2);
    query.bindValue(0,lastmeasure);
    query.exec();
    QString prepare3 = "DELETE FROM configs WHERE measurement_id = ?;";
    query.prepare(prepare3);
    query.bindValue(0,lastmeasure);
    query.exec();
    return true;
}

bool TestModule::ResetHybrid()
{
    QLabel* vmmlabels[2] = {m_mainWindow->m_daqWindow->ui->vmm1label,m_mainWindow->m_daqWindow->ui->vmm2label};
    for(int i=0;i<2;i++){
        for(int j=0; j<2;j++){
            m_hResults.h_nwchannels[i][j]=64;
            m_hResults.h_monitoringADCcal[i][j]=0;
        }
        m_hResults.h_monitoringADC[i]=false;
        m_hResults.h_DBExportStrings[i].clear();
        m_hResults.h_VMMResults[i].clear();
        for(int h=0;h<64;h++){
            m_hResults.h_ChannelResults[i][h].clear();
        }
        m_temps[i].clear();
        vmmlabels[i]->setStyleSheet("background-color: ");
    }
    m_hResults.h_plotsdata.clear();
    m_hResults.h_plotslabel.clear();
    m_hResults.h_histodata.clear();
    if(m_currmon){
        m_currentMonitor->clearCurrents();
    }
    m_currents.clear();
    QVector<QLabel*> labels = m_labelstoclear;
    for(int i=0;i<labels.size();i++){
        labels[i]->clear();
        labels[i]->setStyleSheet("background-color: ");
        labels[i]->toolTip().clear();
    }
    QGridLayout* grids [2] = {m_mainWindow->m_daqWindow->ui->ChannelsVMM0_grid,m_mainWindow->m_daqWindow->ui->ChannelsVMM1_grid};
    for(int i=0;i<2;i++){
        QLayoutItem *child;
        while((child=grids[i]->takeAt(0))!=nullptr){
            delete child->widget();
            delete child;
        }
    }
    m_mainWindow->m_daqWindow->ui->temp1label->clear();
    m_mainWindow->m_daqWindow->ui->temp2label->clear();
    m_mainWindow->m_daqWindow->ui->temp1label->setStyleSheet("background-color: ");
    m_mainWindow->m_daqWindow->ui->temp2label->setStyleSheet("background-color: ");
    m_mainWindow->m_daqWindow->ui->lineEdit_1_9V->setText("");
    m_mainWindow->m_daqWindow->ui->lineEdit_2_9V->setText("");
    m_mainWindow->m_daqWindow->ui->comboBox_selectPlotL->clear();
    m_mainWindow->m_daqWindow->ui->customPlotVMM1->clearItems();
    m_mainWindow->m_daqWindow->ui->customPlotVMM1->clearPlottables();
    m_mainWindow->m_daqWindow->ui->customPlotVMM1->clearGraphs();
    m_mainWindow->m_daqWindow->ui->customPlotVMM1->plotLayout()->removeAt(1);
    return true;
}

void TestModule::enableSbip(bool ena)
{
    VMM* vmm[2];
    vmm[0] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[0];
    vmm[1] = &m_mainWindow->m_daqs[0].m_fecs[m_FEC_test].m_hdmis[m_HDMI_test].m_hybrids[0].m_vmms[1];
    for(int i=0;i<2;i++){
        if(ena==true){
            vmm[i]->SetRegi("sbip",1);
            GetMessageHandler()("Enabled sbip","TestModule::enableSbip");
        }
        else{
            vmm[i]->SetRegi("sbip",0);
            GetMessageHandler()("Disabled sbip","TestModule::enableSbip");
        }
    }
}

bool TestModule::SetTermination()
{
    m_terminator = true;
    return true;
}

void TestModule::GetOptionalSettings()
{
    stringstream sx;
    m_currmon = m_mainWindow->m_daqWindow->ui->checkBox_readcurrent->isChecked();
    m_firmwareflash = m_mainWindow->m_daqWindow->ui->checkBox_firmware->isChecked();
    m_forceflash = m_mainWindow->m_daqWindow->ui->checkBox_forcefirmware->isChecked();
    m_extpulse = m_mainWindow->m_daqWindow->ui->checkBox_external->isChecked();
    m_tempoff = m_mainWindow->m_daqWindow->ui->checkBox_temperature->isChecked();
    m_neighbor = m_mainWindow->m_daqWindow->ui->checkBox_neighbor->isChecked();
    m_sbip = m_mainWindow->m_daqWindow->ui->checkBox_sbip->isChecked();
    sx << "Optional Settings: Firmware flashing: ";
    if(m_firmwareflash){
        if(m_forceflash){
            sx <<"FORCE";
        }
        else{
            sx << "ON";
        }
    }
    else{sx<<"OFF";}
    sx << " Current Monitoring: ";
    if(m_currmon){sx << "ON";}
    else{sx<<"OFF";}
    sx<<endl;
    sx << " External test pulses: ";
    if(m_extpulse){sx << "ON";}
    else{sx<<"OFF";}
    sx << " Temperature shutoff: ";
    if(m_tempoff){sx << "ON";}
    else{sx<<"OFF";}
    sx<<endl;
    sx << " Neighboring logic test: ";
    if(m_neighbor){sx << "ON";}
    else{sx<<"OFF";}
    sx << " SBip Flag: ";
    if(m_sbip){sx << "ON";}
    else{sx<<"OFF";}
    sx<<endl;
    GetMessageHandler()(sx.str(), "TestModule::GetOptionalSettings");
    return;
}

void TestModule::evaluateResults(std::string results){
    QVector<QLabel*> labels1;
    QGridLayout* grids [2] = {m_mainWindow->m_daqWindow->ui->ChannelsVMM0_grid,m_mainWindow->m_daqWindow->ui->ChannelsVMM1_grid};
    QLabel* vmmlabels[2] = {m_mainWindow->m_daqWindow->ui->vmm1label,m_mainWindow->m_daqWindow->ui->vmm2label};
    QString vmmstati[2] ={"",""};
    QString hybridclass = "";
    QVector<int> brokenChans[2];
    for(int j=0;j<2;j++){
        int n_perfect =0;
        int n_ok =0;
        //Channel evaluation Start

        for(int i=0; i<64;i++){
            QLabel* label = new QLabel();
            QHash<QString,int> channelinfo = m_hResults.h_ChannelResults[j][i];
            QList<QString> keys = channelinfo.keys();
            double total =0;
            int ntests =0;
            QString tTip = "<font color=\"#000000\">Channel " +QString::number(i);
            bool fixable = false;
            if(channelinfo.value("External")==2){
                //if External test says channel is broken check if also internally broken!
                if(channelinfo.value("Internal")==2){
                    //Also internally broken
                    if(channelinfo.value("Pedestal")==2){
                        //baseline too high
                    }
                    else{
                        //unknown reason
                    }
                }
                else{
                    //Channel not connected to detector (fixable)
                    fixable==true;
                    stringstream sx;
                    sx << "Channel " << i <<"on VMM " << j << "has internally working channels, but does not receie external pulses. Might be fixable!";
                    GetMessageHandler()(sx,"TestModule::evaluateResult");
                }
            }
            QStringList badtests;
            QStringList oktests;
            for(int h =0; h< keys.size();h++){
                total += channelinfo.value(keys[h]);
                //cout << keys[h].toStdString()<<endl;
                if(channelinfo.value(keys[h])!=0){
                    tTip += ", " + QString(keys[h]);
                }
                if(channelinfo.value(keys[h])==1){
                    oktests << keys[h];
                }
                if(channelinfo.value(keys[h])==2){
                    badtests << keys[h];
                }
                ntests++;
            }

            QList<int> values = channelinfo.values();
            int ngood = values.count(0);
            int nok = values.count(1);
            int nbad = values.count(2);
            int unknown = values.count(3);
            QString color = "";
            if(nbad>=1){
                bool crit = false;
                foreach(const QString &str,badtests){
                    if(m_criticaltests.contains(str)){
                        crit=true;
                    }
                }
                if(crit==true){
                    color = "background-color: #ff0000";
                    brokenChans[j].push_back(i);
                }
                else{
                    color = "background-color: rgb(255,255,0)";
                    n_ok++;
                }
                label->setStyleSheet(color);
            }
            else if(nok>=1 && nbad==0){
                int ntotal = nok;
                double avg = ntotal/ntests;
                color = "background-color: #ff";
                int colval=255*(1-avg);
                QString add = QString::number(colval,16);
                if(add.size()%2!=0){
                    add = "0"+add;
                }
                color += add;
                color += "00";
                label->setStyleSheet(color);
                n_ok++;
            }
            else if(nbad==0 && nok==0){
                color = "background-color: #00ff00";
                n_perfect++;
                label->setStyleSheet(color);
            }
            else{
                color = "background-color: #0000ff";
            }
            label->setStyleSheet(color);
            /*
            double avg= total/ntests;
            if(avg==0){
                label->setStyleSheet("background-color: #00ff00");
            }
            else if(avg>0 && avg<=1){
                QString color ="background-color: #";
                int colval=255*avg;
                QString add = QString::number(colval,16);
                if(add.size()%2!=0){
                    add = "0"+add;
                }
                color +=add;
                color += "ff00";
                label->setStyleSheet(color);
            }
            else if(avg>1 && avg<=2){
                QString color ="background-color: #ff";
                int colval=255*(1-avg/2);
                QString add = QString::number(colval,16);
                if(add.size()%2!=0){
                    add = "0"+add;
                }
                color += add;
                color += "00";
                label->setStyleSheet(color);
            }
            else{
                label->setStyleSheet("background-color: blue");
            }
            */
            label->setText(QString::number(total));
            label->setToolTip(tTip);
            label->setAlignment(Qt::AlignCenter);
            labels1.push_back(label);
            grids[j]->addWidget(label,i/16,i%16);
        }
        m_hResults.h_DBExportStrings[j].insert("brokenchannels",dataToSqlArray(brokenChans[j]));
        QHash<QString,int> vmmresults = m_hResults.h_VMMResults[j];
        QList<QString> keys2 = vmmresults.keys();
        int ntestsmade=0;
        int total2 =0;
        for(int h =0; h< keys2.size();h++){
            total2 += vmmresults.value(keys2[h]);
            ntestsmade++;
        }
        double avgresult = total2/ntestsmade;
        if(keys2.contains("MonitoringADC")==true){
            if(m_hResults.h_VMMResults[j].value("MonitoringADC")==2){
            vmmstati[j] = "E";
            vmmlabels[j]->setStyleSheet("background-color: red");
            vmmlabels[j]->setText("VMM"+QString::number(j)+": E");
            }
            else if(m_hResults.h_VMMResults[j].value("MonitoringADC")<2){
                if (n_perfect+n_ok>=m_mainWindow->m_daqWindow->ui->external_g_l->text().toDouble()){
                    vmmstati[j] = "A";
                    vmmlabels[j]->setStyleSheet("background-color: lightgreen");
                    vmmlabels[j]->setText("VMM"+QString::number(j)+": A");
                }
                else if(n_perfect+n_ok >=m_mainWindow->m_daqWindow->ui->external_g_l->text().toDouble()-1){
                    vmmstati[j] = "B";
                    vmmlabels[j]->setStyleSheet("background-color: rgb(255,133,255)");
                    vmmlabels[j]->setText("VMM"+QString::number(j)+": B");
                }
                else if(n_perfect+n_ok>=m_mainWindow->m_daqWindow->ui->external_o_l->text().toDouble()){
                    vmmstati[j] = "C";
                    vmmlabels[j]->setStyleSheet("background-color: yellow");
                    vmmlabels[j]->setText("VMM"+QString::number(j)+": C");
                }
                else if(n_perfect+n_ok>1){
                    vmmstati[j] = "D";
                    vmmlabels[j]->setStyleSheet("background-color: orange");
                    vmmlabels[j]->setText("VMM"+QString::number(j)+": D");
                }
                else{
                    vmmstati[j] = "E";
                    vmmlabels[j]->setStyleSheet("background-color: red");
                    vmmlabels[j]->setText("VMM"+QString::number(j)+": E");
                }
            }
        }
        else{
            if (n_perfect+n_ok>=m_mainWindow->m_daqWindow->ui->external_g_l->text().toDouble()){
                vmmstati[j] = "A";
                vmmlabels[j]->setStyleSheet("background-color: lightgreen");
                vmmlabels[j]->setText("VMM"+QString::number(j)+": A");
            }
            else if(n_perfect+n_ok >=m_mainWindow->m_daqWindow->ui->external_g_l->text().toDouble()-1){
                vmmstati[j] = "B";
                vmmlabels[j]->setStyleSheet("background-color: rgb(255,133,255)");
                vmmlabels[j]->setText("VMM"+QString::number(j)+": B");
            }
            else if(n_perfect+n_ok>=m_mainWindow->m_daqWindow->ui->external_o_l->text().toDouble()){
                vmmstati[j] = "C";
                vmmlabels[j]->setStyleSheet("background-color: yellow");
                vmmlabels[j]->setText("VMM"+QString::number(j)+": C");
            }
            else if(n_perfect+n_ok>1){
                vmmstati[j] = "D";
                vmmlabels[j]->setStyleSheet("background-color: orange");
                vmmlabels[j]->setText("VMM"+QString::number(j)+": D");
            }
            else{
                vmmstati[j] = "E";
                vmmlabels[j]->setStyleSheet("background-color: red");
                vmmlabels[j]->setText("VMM"+QString::number(j)+": E");
            }
        }
    m_hResults.h_DBExportStrings[j].insert("vmm_class",vmmstati[j]);
    }
    if(vmmstati[0]=="A"){
        if(vmmstati[1]=="A"){
            hybridclass="a";
        }
        else if(vmmstati[1] == "B"||vmmstati[1] == "C"){
            hybridclass = "b";
        }
        else if(vmmstati[1] == "D" || vmmstati[1] == "E"){
            hybridclass = "c";
        }
    }
    else if(vmmstati[0]=="B"){
        if(vmmstati[1]=="A"){
            hybridclass="b";
        }
        else if(vmmstati[1] == "B"){
            hybridclass = "b";
        }
        else if(vmmstati[1] == "C"){
            int ntot = m_hResults.h_nwchannels[1][0] + m_hResults.h_nwchannels[1][1];
            if(ntot >=125){
                hybridclass = "b";
            }
            else{
                hybridclass = "c";
            }
        }
        else if(vmmstati[1] == "D" || vmmstati[1] == "E"){
            hybridclass = "c";
        }
    }
    else if(vmmstati[0]=="C"){
        if(vmmstati[1]=="A"){
            hybridclass="b";
        }
        else if(vmmstati[1] == "B" || vmmstati[1]=="C"){
            int ntot = m_hResults.h_nwchannels[1][0] + m_hResults.h_nwchannels[1][1];
            if(ntot >=125){
                hybridclass = "b";
            }
            else{
                hybridclass = "c";
            }
        }
        else if(vmmstati[1] == "D" || vmmstati[1] == "E"){
            hybridclass = "c";
        }
    }
    else if(vmmstati[0]=="D"){
        hybridclass = "c";
    }
    else if(vmmstati[0]=="E"){
        if(vmmstati[1]=="E"){
            hybridclass = "d";
        }
        else{
            hybridclass = "c";
        }
    }
    else{
        hybridclass = "unknown";
    }

    m_mainWindow->m_daqWindow->ui->hybrid_classlabel->setText(hybridclass);
    if(hybridclass=="a"){
        m_mainWindow->m_daqWindow->ui->hybrid_classlabel->setStyleSheet("background-color: lightgreen");
    }
    else if(hybridclass=="b"){
        m_mainWindow->m_daqWindow->ui->hybrid_classlabel->setStyleSheet("background-color: yellow");
    }
    else if(hybridclass=="c"){
        m_mainWindow->m_daqWindow->ui->hybrid_classlabel->setStyleSheet("background-color: red");
    }
    else if(hybridclass=="d"){
        m_mainWindow->m_daqWindow->ui->hybrid_classlabel->setStyleSheet("background-color: black");
        m_mainWindow->m_daqWindow->ui->hybrid_classlabel->setStyleSheet("color: white");
    }
    m_labelstoclear.push_back(m_mainWindow->m_daqWindow->ui->hybrid_classlabel);
    m_hResults.h_DBExportStrings[0].insert("hybrid_class",hybridclass);
    m_hResults.h_DBExportStrings[1].insert("hybrid_class",hybridclass);
    results += "Evaluated Results";
    GetMessageHandler()(results, "TestModule::evaluateResults");
}

void TestModule::PrintMData(){
    std::vector<double> ym;
    std::vector<double> yo;
    std::vector<double> ys;
    cout << "Going to Print m_data" << std::endl;
    for(int i=0; i<32;i++){
        for(int j = 0; j<FECS_PER_DAQ; j++){
            for(int k = 0; k<HDMIS_PER_FEC;k++){
                for(int l = 0; l<HYBRIDS_PER_HDMI;l++){
                    for(int m = 0; m<VMMS_PER_HYBRID; m++){
                        ym = m_calibmod->m_mean[i][j][k][l][m];
                        yo = m_calibmod->m_offset[j][k][l][m];
                        ys = m_calibmod->m_slope[j][k][l][m];
                        if(ym.size()>0){
                            cout<< "       " << i << j << k << l << m << ":      ";
                        }
                        for(uint o=0; o< ym.size();o++){
                            cout << ym[o] << " ";
                        }

                    }
                }
            }
        }
    }
    cout << std::endl;
    QVector<QDateTime> times;
    QVector<double> currents;
    for(int i=0; i<m_currents.size();i++){
        times.push_back(m_currents[i].first);
        currents.push_back(m_currents[i].second);
        cout << times[i].toString().toStdString() << "\t" << currents[i] << std::endl;
    }
}

void TestModule::acqON(){
    bool checked = m_mainWindow->m_daqWindow->ui->onACQ->isChecked();
    m_acqon = checked;
}

void TestModule::acqOFF(){
    bool checked = m_mainWindow->m_daqWindow->ui->offACQ->isChecked();
    m_acqon = !checked;
}

void TestModule::handleTemperatures(bool end)
{
    //cout << "entered temperature handling"<< endl;
    double t1 = ReadTemperature(0);
    double t2 = ReadTemperature(1);
    QDateTime theTime = QDateTime::currentDateTime();
    if(t1 >= -1820 && t2 >= -1820){
        m_temps[0].push_back(qMakePair(theTime,t1));
        m_temps[1].push_back(qMakePair(theTime,t2));
    }
    m_mainWindow->m_daqWindow->ui->temp1label->setText(QString::number(t1));
    m_mainWindow->m_daqWindow->ui->temp2label->setText(QString::number(t2));
    if(t1>= -1820 && t2 >=-1820){
        m_mainWindow->m_daqWindow->ui->temp1label->setStyleSheet("background-color: lightgreen");
        m_mainWindow->m_daqWindow->ui->temp2label->setStyleSheet("background-color: lightgreen");
    }
    if(t1 < -1820){
        m_mainWindow->m_daqWindow->ui->temp1label->setStyleSheet("background-color: yellow");
        m_mainWindow->m_daqWindow->ui->temp1label->setText("Currently not working");
    }
    if(t2 < -1820){
        m_mainWindow->m_daqWindow->ui->temp2label->setStyleSheet("background-color: yellow");
        m_mainWindow->m_daqWindow->ui->temp2label->setText("Currently not working");
    }
    if(t1>m_mainWindow->m_daqWindow->ui->lineEdit_warningtemp->text().toDouble()||t2>m_mainWindow->m_daqWindow->ui->lineEdit_warningtemp->text().toDouble()){
        m_mainWindow->m_daqWindow->ui->temp1label->setStyleSheet("background-color: red");
        m_mainWindow->m_daqWindow->ui->temp2label->setStyleSheet("background-color: red");
        if(m_tempoff){
            if(m_currmon){
                //m_currentMonitor->emergencyStop();
            }
            SetTermination();
        }
    }
    if(end)
    {
        m_temptimer.stop();
        QVector<double> temps[2];
        QVector<QDateTime> times;
        int size = m_temps[0].size();
        if(size==0){
            return;
        }
        for(int i=0; i< size; i++){
            QDateTime theTime = m_temps[0][i].first;
            times.append(theTime);
            double temp1 = m_temps[0][i].second;
            double temp2 = m_temps[1][i].second;
            temps[0].append(temp1);
            temps[1].append(temp2);
        }
        QVector<double> secondssincestart;
        const QDateTime start = times[0];
        for(int i=0; i< size; i++){
            secondssincestart.append(start.msecsTo(times[i])/1000.);
        }
        PlotData(secondssincestart,temps,"Time since first start [s]", "Temperature [°C]","Temperature","VMM");
        GetMessageHandler()("Handled Temperatures","TestModule::handleTemperatures");
    }
}

