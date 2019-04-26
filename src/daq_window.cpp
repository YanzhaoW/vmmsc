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
    ui->openConnection_2->setToolTip("Open communication");

    //    this->setStyleSheet("QMainWindow {background: 'lightgray';}");

   //connect(ui->selectDir, SIGNAL(clicked()),
   //        this, SLOT(on_selectDir_clicked()));
    ui->selectDir->setToolTip("Opens file browser to select config file");
    ui->Button_load->setToolTip("Loading config file");
    ui->Button_save->setToolTip("Saving settings into config file");
    ui->trgPulser->setEnabled(false);
    ui->trgExternal->setEnabled(false);
    ui->onACQ->setEnabled(false);
    ui->offACQ->setEnabled(false);
    ui->checkBoxGlobalDAQ->setEnabled(false);

    QString correctedFileName = m_mainWindow->GetApplicationPath() +  "/../configs/default.txt";
    std::cout << correctedFileName.toStdString() << std::endl;
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
void DAQWindow::SetWarning(QString warning, QString bkgcol ){

    ui->connectionLabel_2->setWordWrap(true);
    ui->connectionLabel_2->setText( warning );
    ui->connectionLabel_2->setStyleSheet("background-color: "+bkgcol);
}
// ------------------------------------------------------------------------- //
void DAQWindow::SetWarning2(QString warning, QString bkgcol ){
    ui->connectionLabel_3->setWordWrap(true);
    ui->connectionLabel_3->setText( warning );
    ui->connectionLabel_3->setStyleSheet("background-color: "+bkgcol);
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
        ui->checkBoxGlobalDAQ->setChecked(false);
        ui->checkBoxGlobalDAQ->setEnabled(false);
        ui->trgPulser->setEnabled(false);
        ui->trgExternal->setEnabled(false);
        ui->onACQ->setEnabled(false);
        ui->offACQ->setEnabled(false);
    }
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = ui->Fec_group_box->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<fec && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        ui->tabWidget->insertTab(fec-NotActiveBefore, new FECWindow(this,fec), QString(" FEC %0").arg(fec+1));
        ui->tabWidget->setCurrentIndex(fec-NotActiveBefore);
        m_mainWindow->m_daqs[0].SetFEC(fec,true);
    }
    else {
        ui->tabWidget->removeTab(fec-NotActiveBefore);
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
        fname+=".txt";
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
            std::cout << "loading file " << fname << std::endl;
            for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
                if (m_mainWindow->m_daq_act[i]){
                    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                        if (m_mainWindow->m_daqs[i].GetFEC(j)){
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
                                                    std::cout << "vmm " << m << " on hybrid " << l << "(pos " << m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetPosNo()<< ", " <<m_mainWindow->m_daqs[i].m_fecs[j].m_hdmis[k].m_hybrids[l].GetPosX() << ") on hmdi "<< k << " on fec " << j << " on daq " << i << " is active" << std::endl;
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

void DAQWindow::on_openConnection_2_clicked()
{
    if(!ui->onACQ->isDown())
    {
        m_ip_fec.clear();
        for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
            if (m_mainWindow->m_daq_act[i]){
                for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                    if (m_mainWindow->m_daqs[i].GetFEC(j)){

                        if(m_mainWindow->m_daqs[i].m_fecs[j].m_fecConfigModule->Connect()==1){
                            int id =  m_mainWindow->m_daqs[i].m_fecs[j].GetIP_id();
                            m_ip_fec.insert(std::make_pair(j,id));
                            SetWarning("all alive","green");
                            ui->Send->setEnabled(true);
                            ui->checkBoxGlobalDAQ->setEnabled(true);
                        }
                        else{

                            SetWarning("ping failed", "red");
                            ui->Send->setEnabled(false);
                            ui->checkBoxGlobalDAQ->setChecked(false);
                            ui->checkBoxGlobalDAQ->setEnabled(false);
                            ui->trgPulser->setEnabled(false);
                            ui->trgExternal->setEnabled(false);
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


int DAQWindow::GetFecIP(int id)
{
    auto search = m_ip_fec.find(id);
    if(search != m_ip_fec.end())
    {
        return m_ip_fec[id];
    }
    return -1;
}



void DAQWindow::on_reset_warnings_clicked()
{
    SetWarning2("","light");
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

void DAQWindow::on_checkBoxGlobalDAQ_stateChanged()
{
    if(ui->checkBoxGlobalDAQ->isChecked()){
        ui->trgPulser->setEnabled(true);
        ui->trgExternal->setEnabled(true);
        ui->onACQ->setEnabled(true);
        ui->offACQ->setEnabled(true);
        m_sendstate = "GlobalACQon";
        emit ChangeState();
    }
    else if(!ui->checkBoxGlobalDAQ->isChecked()){
        emit ui->offACQ->clicked();
        ui->trgPulser->setChecked(false);
        ui->trgExternal->setChecked(false);
        ui->onACQ->setChecked(false);
        ui->offACQ->setChecked(false);

        ui->trgPulser->setEnabled(false);
        ui->trgExternal->setEnabled(false);
        ui->onACQ->setEnabled(false);
        ui->offACQ->setEnabled(false);
        m_sendstate = "GlobalACQoff";
        emit ChangeState();
    }
}
void DAQWindow::on_trgPulser_clicked()
{
    ui->trgPulser->setCheckable(true);
    ui->trgPulser->setChecked(true);
    ui->trgExternal->setChecked(false);
    m_sendstate = "trigPulser";
    emit ChangeState();
}

void DAQWindow::on_trgExternal_clicked()
{
    ui->trgExternal->setCheckable(true);
    ui->trgExternal->setChecked(true);
    ui->trgPulser->setChecked(false);
    m_sendstate = "trigExternal";
    emit ChangeState();
}

void DAQWindow::on_onACQ_clicked()
{
    ui->onACQ->setCheckable(true);
    if(ui->trgExternal->isChecked()){
        emit ui->trgExternal->clicked();
    }
    else if(ui->trgPulser->isChecked()){
        emit ui->trgPulser->clicked();
    }
    else{
        SetWarning2("Select Trigger Mode","red");
        ui->onACQ->setChecked(false);
        return;
    }
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
    if(ui->connectionLabel_2->text()==QString("all alive")){
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




void DAQWindow::on_comboBoxRunMode_currentIndexChanged(int index)
{
    m_mainWindow->m_calib->m_dataAvailable = false;
    ui->comboBoxCalibrationType->setCurrentIndex(-1);
    ui->comboBoxCalibrationType->clear();
    if(index == 0)
    {
        ui->comboBoxCalibrationType->addItem("Offline ADC");
        ui->comboBoxCalibrationType->addItem("Offline Time (BCID/TDC)");
        ui->comboBoxCalibrationType->addItem("Threshold");
        ui->comboBoxCalibrationType->addItem("Pedestal");
        ui->comboBoxCalibrationType->addItem("S-curve");
        ui->comboBoxCalibrationType->addItem("ADC");
        ui->comboBoxCalibrationType->addItem("TDC");
    }
    if(index == 1)
    {
        ui->comboBoxCalibrationType->addItem("Channels");
        ui->comboBoxCalibrationType->addItem("ADC");
        ui->comboBoxCalibrationType->addItem("TDC");
        ui->comboBoxCalibrationType->addItem("BCID");
    }
}

void DAQWindow::on_comboBoxCalibrationType_currentIndexChanged(int index)
{
    m_mainWindow->m_calib->m_dataAvailable = false;
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

