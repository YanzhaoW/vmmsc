#include "daq_window.h"

DAQWindow::DAQWindow(MainWindow *top, QWidget *parent) :
    QMainWindow(parent),
    m_mainWindow{top},
    ui(new Ui::daq_window),
    m_msg(0)
{

    ui->setupUi(this);
    ui->Send->setEnabled(false);
    LoadMessageHandler(m_mainWindow->m_daqs[0].GetMessageHandler());
    connect(m_msg, SIGNAL(on_log_ready()), this, SLOT(on_readLog()));
    ui->openConnection->setToolTip("Open communication");

    //    this->setStyleSheet("QMainWindow {background: 'lightgray';}");

    //connect(ui->selectDir, SIGNAL(clicked()),
    //        this, SLOT(on_selectDir_clicked()));
    ui->selectDir->setToolTip("Opens file browser to select config file");
    ui->Button_load->setToolTip("Loading config file");
    ui->Button_save->setToolTip("Saving settings into config file");
    ui->onACQ->setEnabled(false);
    ui->offACQ->setEnabled(false);

    ui->first_trigger_starts_acq->setToolTip("If checked, the first trigger signal that occurs at the NIM trigger input of the FEC will start the acquisition.\nUseful to synchronize the start of acquisition in case multiple FECs are used.");

    QString correctedFileName = m_mainWindow->GetApplicationPath() +  "/../configs/default.txt";

    if(FileExists(correctedFileName.toStdString().c_str())){
        LoadConfig("default");
    }

}
bool DAQWindow::FileExists(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}


DAQWindow::~DAQWindow()
{
    delete ui;
}
// ------------------------------------------------------------------------ //
void DAQWindow::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ------------------------------------------------------------------------- //
void DAQWindow::on_readLog()
{
    string buff = GetMessageHandler().GetBuffer();
    ui->loggingScreen->append(QString::fromStdString(buff));
    //ui->loggingScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    GetMessageHandler().ClearBuffer();
}
// ------------------------------------------------------------------------- //
void DAQWindow::SetConnectionMessage(QString warning, QString bkgcol ){

    ui->connectionLabel->setWordWrap(true);
    ui->connectionLabel->setText( warning );
    ui->connectionLabel->setStyleSheet("background-color: "+bkgcol);
}
// ------------------------------------------------------------------------- //
void DAQWindow::SetWarningMessage(QString warning, QString bkgcol ){
    ui->connectionWarning->setWordWrap(true);
    ui->connectionWarning->setText( warning );
    ui->connectionWarning->setStyleSheet("background-color: "+bkgcol);
}
// ------------------------------------------------------------------------- //



void DAQWindow::on_Box_fec1_clicked()
{
    if (ui->Box_fec1->isChecked()){
        fecBoxLogic(true,0);
    }
    else {fecBoxLogic(false,0);}
}
void DAQWindow::on_Box_fec2_clicked()
{
    if (ui->Box_fec2->isChecked()){
        fecBoxLogic(true,1);
    }
    else {fecBoxLogic(false,1);}
}
void DAQWindow::on_Box_fec3_clicked()
{
    if (ui->Box_fec3->isChecked()){fecBoxLogic(true,2);}
    else {fecBoxLogic(false,2);}
}
void DAQWindow::on_Box_fec4_clicked()
{
    if (ui->Box_fec4->isChecked()){fecBoxLogic(true,3);}
    else {fecBoxLogic(false,3);}
}
void DAQWindow::on_Box_fec5_clicked()
{
    if (ui->Box_fec5->isChecked()){fecBoxLogic(true,4);}
    else {fecBoxLogic(false,4);}
}
void DAQWindow::on_Box_fec6_clicked()
{
    if (ui->Box_fec6->isChecked()){fecBoxLogic(true,5);}
    else {fecBoxLogic(false,5);}
}
void DAQWindow::on_Box_fec7_clicked()
{
    if (ui->Box_fec7->isChecked()){fecBoxLogic(true,6);}
    else {fecBoxLogic(false,6);}
}
void DAQWindow::on_Box_fec8_clicked()
{
    if (ui->Box_fec8->isChecked()){fecBoxLogic(true,7);}
    else {fecBoxLogic(false,7);}
}
void DAQWindow::fecBoxLogic(bool checked, unsigned short fec){
    if(checked) {
        ui->Send->setEnabled(false);
        ui->onACQ->setEnabled(false);
        ui->offACQ->setEnabled(false);
    }

    QList<QCheckBox*> a = ui->Fec_group_box->findChildren<QCheckBox*>();
    std::sort(a.begin(), a.end(),
              [](const QCheckBox* x, const QCheckBox* y) -> bool { return x->text() <  y->text();
    });

    unsigned short ActiveBefore = 0;
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<fec && a.at(i)->isChecked()) ActiveBefore++;
    }
    if (checked){
        ui->tabWidget->insertTab(ActiveBefore, new FECWindow(this,fec), QString(" FEC %0").arg(fec+1));
        ui->tabWidget->setCurrentIndex(ActiveBefore);
        m_mainWindow->m_daqs[0].SetFEC(fec,true);
    }
    else {
        ui->tabWidget->removeTab(ActiveBefore);
        m_mainWindow->m_daqs[0].SetFEC(fec,false);
    }

}


void DAQWindow::LoadConfig(QString text){
    std::string fname = text.toStdString();
    std::string filename = fname;
    if (fname == "") {
        std::cout << "No file name specified" << std::endl;
        ui->line_configFile->insert("ERROR: no file name given");
    }
    else {
        if(!text.contains(".txt")) {
            fname+=".txt";
        }
        bool found = m_mainWindow->m_daqConfigHandler->LoadDAQConf(fname.c_str());
        if (!found){
            std::cout << "File not found" << std::endl;
            ui->line_configFile->insert("ERROR: not found");
        }

        else {
            m_mainWindow->m_vmmConfigHandler->LoadAllVMMConf(filename);
            m_mainWindow->m_hybridConfigHandler->LoadAllHybridConf(filename);
            m_mainWindow->m_fecConfigHandler->LoadAllFECConf(filename);
            for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                if (j==0) {ui->Box_fec1->setChecked(false);on_Box_fec1_clicked();}
                if (j==1 ){ui->Box_fec2->setChecked(false);on_Box_fec2_clicked();}
                if (j==2 ){ui->Box_fec3->setChecked(false);on_Box_fec3_clicked();}
                if (j==3 ){ui->Box_fec4->setChecked(false);on_Box_fec4_clicked();}
                if (j==4 ){ui->Box_fec5->setChecked(false);on_Box_fec5_clicked();}
                if (j==5 ){ui->Box_fec6->setChecked(false);on_Box_fec6_clicked();}
                if (j==6 ){ui->Box_fec7->setChecked(false);on_Box_fec7_clicked();}
                if (j==7 ){ui->Box_fec8->setChecked(false);on_Box_fec8_clicked();}
            }
            m_mainWindow->m_daqConfigHandler->LoadDAQConf(fname.c_str());
            for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
                if (m_mainWindow->m_daq_act[i]){
                    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                        if (m_mainWindow->m_daqs[i].GetFEC(j)){
                            m_mainWindow->m_daqs[i].m_fecs[j].SetReg("first_trigger_starts_acq", ui->first_trigger_starts_acq->isChecked());
                            if (j==0 && !ui->Box_fec1->isChecked()){ui->Box_fec1->setChecked(true);on_Box_fec1_clicked();}
                            if (j==1 && !ui->Box_fec2->isChecked()){ui->Box_fec2->setChecked(true);on_Box_fec2_clicked();}
                            if (j==2 && !ui->Box_fec3->isChecked()){ui->Box_fec3->setChecked(true);on_Box_fec3_clicked();}
                            if (j==3 && !ui->Box_fec4->isChecked()){ui->Box_fec4->setChecked(true);on_Box_fec4_clicked();}
                            if (j==4 && !ui->Box_fec5->isChecked()){ui->Box_fec5->setChecked(true);on_Box_fec5_clicked();}
                            if (j==5 && !ui->Box_fec6->isChecked()){ui->Box_fec6->setChecked(true);on_Box_fec6_clicked();}
                            if (j==6 && !ui->Box_fec7->isChecked()){ui->Box_fec7->setChecked(true);on_Box_fec7_clicked();}
                            if (j==7 && !ui->Box_fec8->isChecked()){ui->Box_fec8->setChecked(true);on_Box_fec8_clicked();}
                             for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                                if(m_mainWindow->m_daqs[i].m_fecs[j].GetHDMI(k)){
                                    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                                        if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].GetHybrid(l)){
                                            for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                                                if (m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetVMM(m)){
                                                    std::cout << "vmm " << m << " on hybrid " << l << "(pos " << m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("position")<< ", " <<m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetReg("axis") << ") on hmdi "<< k << " on fec " << j << " on daq " << i << " is active" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else{
                            if (j==0 && ui->Box_fec1->isChecked()){ui->Box_fec1->setChecked(false);on_Box_fec1_clicked();}
                            if (j==1 && ui->Box_fec2->isChecked()){ui->Box_fec2->setChecked(false);on_Box_fec2_clicked();}
                            if (j==2 && ui->Box_fec3->isChecked()){ui->Box_fec3->setChecked(false);on_Box_fec3_clicked();}
                            if (j==3 && ui->Box_fec4->isChecked()){ui->Box_fec4->setChecked(false);on_Box_fec4_clicked();}
                            if (j==4 && ui->Box_fec5->isChecked()){ui->Box_fec5->setChecked(false);on_Box_fec5_clicked();}
                            if (j==5 && ui->Box_fec6->isChecked()){ui->Box_fec6->setChecked(false);on_Box_fec6_clicked();}
                            if (j==6 && ui->Box_fec7->isChecked()){ui->Box_fec7->setChecked(false);on_Box_fec7_clicked();}
                            if (j==7 && ui->Box_fec8->isChecked()){ui->Box_fec8->setChecked(false);on_Box_fec8_clicked();}
                        }
                    }
                }
            }
        } //else file found
    } //end else not ""
}

void DAQWindow::on_Button_load_clicked()
{
    QString text = ui->line_configFile->displayText();
    LoadConfig(text);

}

void DAQWindow::on_Button_save_clicked()
{
    QString text = ui->line_configFile->displayText();
    std::string fname = text.toStdString();
    if (fname == "") {
        std::cout << "No file name specified" << std::endl;
        ui->line_configFile->insert("ERROR: no file name given");
    }
    else {
        m_mainWindow->m_vmmConfigHandler->WriteAllVMMConf(fname);
        m_mainWindow->m_hybridConfigHandler->WriteAllHybridConf(fname);
        m_mainWindow->m_fecConfigHandler->WriteAllFECConf(fname);
        fname+=".txt";
        m_mainWindow->m_daqConfigHandler->WriteDAQConf(fname.c_str());
        std::cout << "writing to file " << fname << std::endl;
    }
}

void DAQWindow::on_openConnection_clicked()
{
    if(!ui->onACQ->isDown())
    {
        SetConnectionMessage("ping failed", "red");
        ui->Send->setEnabled(false);
        ui->onACQ->setEnabled(false);
        ui->offACQ->setEnabled(false);
        for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
            if (m_mainWindow->m_daq_act[i]){
                for (unsigned short j=FECS_PER_DAQ-1; j >0; j--){
                    if (m_mainWindow->m_daqs[i].GetFEC(j)){
                        QHostAddress ipAddress;
                        ipAddress.setAddress(m_mainWindow->m_daqs[i].m_fecs[j].GetReg("ip_fec"));
                        long theIP = ipAddress.toIPv4Address();
                        int res = m_mainWindow->m_daqs[i].CheckIP_FEC(theIP, j);
                        if(res > -1) {
                            int ret = QMessageBox::warning(this, tr("FEC IPv4 address"),
                                                           "The last octet of the IP address is the FEC ID, and has to be hence unique.\nLast octet of FEC "
                                                           + QString::number(j+1) + " IP address already in use in FEC " + QString::number(res+1) + "\n\n"
                                                           +"To change the IP address of a FEC in case you use multiple FECs, connect the FECs one by one and change the IP.\n"
                                                           +"The default IP address for FECs is 10.0.0.2.\n",
                                                           QMessageBox::Ok);
                            return;
                        }
                        res = m_mainWindow->m_daqs[0].CheckIP_DAQ(theIP);
                        if(res > -1) {
                            int ret = QMessageBox::warning(this, tr("FEC IPv4 address"),
                                                           "FEC " + QString::number(j+1) + " IP address already used as DAQ IP in FEC " + QString::number(res+1),
                                                           QMessageBox::Ok);
                            return;
                        }
                        ipAddress.setAddress(m_mainWindow->m_daqs[i].m_fecs[j].GetReg("ip_daq"));
                        theIP = ipAddress.toIPv4Address();
                        res = m_mainWindow->m_daqs[0].CheckIP_DAQ(theIP);
                        if(res == -1) {
                            int ret = QMessageBox::warning(this, tr("DAQ IPv4 address"),
                                                           "All FECs have to use the same DAQ IP address (IP of the computer where the slow control runs)!",
                                                           QMessageBox::Ok);
                            return;
                        }
                    }
                }
            }
        }
        for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
            if (m_mainWindow->m_daq_act[i]){
                for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                    if (m_mainWindow->m_daqs[i].GetFEC(j)){

                        if(m_mainWindow->m_daqs[i].m_fecs[j].m_fecConfigModule->Connect()==1){
                            SetConnectionMessage("all alive","green");
                            ui->Send->setEnabled(true);
                            ui->onACQ->setEnabled(true);
                            ui->offACQ->setEnabled(true);
                        }
                        else{
                            SetConnectionMessage("ping failed", "red");
                            ui->Send->setEnabled(false);
                            ui->onACQ->setEnabled(false);
                            ui->offACQ->setEnabled(false);
                            return;
                        }
                    }
                }
            }
        }
    }
}


void DAQWindow::on_reset_warnings_clicked()
{
    SetWarningMessage("","light");
}

void DAQWindow::on_Send_clicked()
{
    if(!ui->onACQ->isChecked())
    {
        for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
            if (m_mainWindow->m_daq_act[i]){
                m_mainWindow->m_daqs[i].SendAll();
            }
        }
    }
}


void DAQWindow::on_onACQ_clicked()
{
    ui->onACQ->setCheckable(true);
    ui->onACQ->setChecked(true);
    ui->offACQ->setChecked(false);
    ui->Send->setEnabled(false);

    m_mainWindow->m_daqs[0].SendAll();
    m_mainWindow->m_daqs[0].ACQHandler(true);
}

void DAQWindow::on_offACQ_clicked()
{
    ui->offACQ->setCheckable(true);
    ui->offACQ->setChecked(true);
    ui->onACQ->setChecked(false);
    ui->Send->setEnabled(true);
    m_mainWindow->m_daqs[0].ACQHandler(false);
}



void DAQWindow::on_Debug_pressed()
{
    bool dbg = false;
    if(!ui->Debug->isChecked()){
        dbg =true;
        m_mainWindow->GetMessageHandler()("Debug enabled", "DEBUG");
    }
    else m_mainWindow->GetMessageHandler()("Debug disabled", "DEBUG");
    m_mainWindow->GetSocketHandler().m_dbg=dbg;

    for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
        if (m_mainWindow->m_daq_act[i]){
            for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                if (m_mainWindow->m_daqs[i].GetFEC(j)){
                    m_mainWindow->m_daqs[0].m_fecs[j].m_fecConfigModule->SetDebugMode(dbg);
                }
            }
        }
    }
}




void DAQWindow::on_selectDir_clicked()
{

    stringstream sx;

    QFileDialog getdir;
    //    getdir.setProxyModel();
    QString dirStr = QFileDialog::getOpenFileName(this,
                                                  tr("Select config file"), m_mainWindow->GetApplicationPath() + "/../configs",
                                                  tr("Text (*.txt)") );
    if(dirStr=="") return;
    if(!dirStr.contains("/configs/")){
        qDebug()<< "Config file not located in config folder/subfolder  -- Abort";
        ui->line_configFile->setText("ERROR: config file not in config folder");
        return;
    }
    QString fname = dirStr.split("/").last();
    if(fname.contains("_daq0_")){
        fname = fname.split("_daq0_").first();
    }


    ui->line_configFile->setText(fname);
    emit ui->Button_load->clicked();


}

void DAQWindow::on_pushButtonTakeData_pressed()
{
    if(ui->connectionLabel->text()==QString("all alive")){
        if(!ui->pushButtonTakeData->isChecked()){
            ui->pushButtonTakeData->setCheckable(true);
            m_mainWindow->m_calib->StartCalibration();
        }
        /*
        else{
            m_mainWindow->m_calib->CloseDAQSocket();
            emit ui->offACQ->clicked();
            ui->comboBoxFec->clear();
        }
        */
    }
    else
    {
        ui->pushButtonTakeData->setChecked(false);
        ui->pushButtonTakeData->setCheckable(false);
    }
}




void DAQWindow::on_pushButtonStoreCorrections_pressed()
{
    m_mainWindow->m_calib->SaveCorrections();

}


void DAQWindow::on_pushButtonCSV_pressed()
{
    m_mainWindow->m_calib->SavePlotsAsCSV();
}


void DAQWindow::on_pushButtonSavePDF_pressed()
{
    m_mainWindow->m_calib->SavePlotsAsPDF();
}

void DAQWindow::on_pushButtonAbort_pressed()
{
    m_mainWindow->m_calib->StopDataTaking();
    on_offACQ_clicked();
    m_mainWindow->m_daqWindow->ui->pushButtonTakeData->setChecked(false);
}

void DAQWindow::on_pushButtonStartTest_pressed()
{
    m_mainWindow->m_test->StartTest();
}


void DAQWindow::on_pushButtonClearTestLog_pressed()
{
    m_mainWindow->m_daqWindow->ui->TestLogScreen->clear();
}


void DAQWindow::on_pushButtonSavePlotL_clicked()
{
    QCustomPlot* plot = m_mainWindow->m_daqWindow->ui->customPlotVMM1;
    QString fileName = QFileDialog::getSaveFileName(this,tr("Save Plot as PDF"), "",tr("PDF File (*.pdf);;All Files (*)"));
    fileName.remove(".pdf");
    fileName.remove(".png");
    if(fileName.isEmpty()){
        return;
    }
    else{
        plot->savePdf(fileName+".pdf");
        return;
    }
}

void DAQWindow::on_comboBox_selectPlotL_currentIndexChanged(const QString &arg1)
{
    QHash<QString, QString> labelmap = m_mainWindow->m_test->m_hResults.h_plotslabel.value(arg1);
    QString type = labelmap.value("type");
    if(type == "histo"){
        QHash<QString, QVector<double>> datamap = m_mainWindow->m_test->m_hResults.h_histodata.value(arg1);
        QVector<double> x = datamap.value("x");
        QVector<double> y1 = datamap.value("y1");
        QVector<double> y2 = datamap.value("y2");
        QVector<double> lims = m_mainWindow->m_test->m_hResults.h_histodata.value(arg1).value("lims");
        QString xlabel = labelmap.value("x");
        QString ylabel = labelmap.value("y");
        QString graphlabel = labelmap.value("graph");
        if(datamap.empty()){
            return;
        }
        QVector<double> y[2] = {y1,y2};
        double low = lims[0];
        double high = lims[1];
        int bins = int(lims[2]);
        cout << low <<","<< high<<","<<bins;
        m_mainWindow->m_test->PlotHistogram(y,0,4096,4096,xlabel,ylabel,arg1,graphlabel);
    }
    else if(type == "channels"){
        QHash<QString, QVector<double>> datamap = m_mainWindow->m_test->m_hResults.h_plotsdata.value(arg1);
        QVector<double> x = datamap.value("x");
        QVector<double> y[64];
        for(int i=0;i<64;i++){
            QString key = "y"+QString::number(i+1);
            QVector<double> yi = datamap.value(key);
            y[i] = yi;
        }
        QString xlabel = labelmap.value("x");
        QString ylabel = labelmap.value("y");
        QString graphlabel = labelmap.value("graph");
        if(datamap.empty()){
            return;
        }
        m_mainWindow->m_test->PlotData(x,y,xlabel,ylabel,arg1,graphlabel,64);
    }
    else{
        QHash<QString, QVector<double>> datamap = m_mainWindow->m_test->m_hResults.h_plotsdata.value(arg1);
        QVector<double> x = datamap.value("x");
        QVector<double> y1 = datamap.value("y1");
        QVector<double> y2 = datamap.value("y2");
        QVector<double> lims = m_mainWindow->m_test->m_hResults.h_plotsdata.value(arg1).value("lims");
        QString xlabel = labelmap.value("x");
        QString ylabel = labelmap.value("y");
        QString graphlabel = labelmap.value("graph");
        QVector<double> y[2] = {y1,y2};
        if(datamap.empty()){
            return;
        }
        m_mainWindow->m_test->PlotData(x,y,xlabel,ylabel,arg1,graphlabel);
        if(QString::compare(arg1,"ADCCalibrationInternal",Qt::CaseInsensitive)==0){
            QVector<double>pars;
            QVector<double> fits [2] = {m_mainWindow->m_test->evaluateADCCalibrationFit(x,y[0],"internal",0,pars),m_mainWindow->m_test->evaluateADCCalibrationFit(x,y[1],"internal",1,pars)};
            m_mainWindow->m_test->AddFitToPlot(x,fits);
        }
        if(QString::compare(arg1,"ADCCalibrationExternal",Qt::CaseInsensitive)==0){
            QVector<double>pars;
            QVector<double> fits [2] = {m_mainWindow->m_test->evaluateADCCalibrationFit(x,y[0],"external",0,pars),m_mainWindow->m_test->evaluateADCCalibrationFit(x,y[1],"external",1,pars)};
            m_mainWindow->m_test->AddFitToPlot(x,fits);
        }

    }
}

void DAQWindow::on_checkBox_readcurrent_stateChanged(int arg1)
{
    m_mainWindow->m_daqWindow->ui->lineEdit_1_9V->setReadOnly(arg1);
    m_mainWindow->m_daqWindow->ui->lineEdit_2_9V->setReadOnly(arg1);
}

void DAQWindow::on_lineEdit_1_9V_textChanged(const QString &arg1)
{
    m_mainWindow->m_test->m_hResults.h_current_1_9V = arg1.toDouble();
}

void DAQWindow::on_lineEdit_2_9V_textChanged(const QString &arg1)
{
    m_mainWindow->m_test->m_hResults.h_current_2_9V = arg1.toDouble();
}

void DAQWindow::on_pushButtonDeleteLast_clicked()
{
    m_mainWindow->m_test->DeleteLastMeasurement();
}

void DAQWindow::on_pushButtonNewHybrid_clicked()
{
    m_mainWindow->m_test->ResetHybrid();
}

void DAQWindow::on_first_trigger_starts_acq_stateChanged(int arg1)
{
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if (m_mainWindow->m_daqs[0].GetFEC(j)){
            m_mainWindow->m_daqs[0].m_fecs[j].SetReg("first_trigger_starts_acq", ui->first_trigger_starts_acq->isChecked());
        }
    }

}


