#include "daq_window.h"

daq_window::daq_window(MainWindow *top, QWidget *parent) :
    QMainWindow(parent),
    root_main{top},
    m_msg(0),
    ui(new Ui::daq_window)
{
    ui->setupUi(this);
    ui->Send->setEnabled(false);
    LoadMessageHandler(root_main->daq[0].msg());
    connect(m_msg, SIGNAL(logReady()), this, SLOT(readLog()));
    ui->openConnection_2->setToolTip("Open communication");

    ui->trgPulser->setEnabled(false);
    ui->trgExternal->setEnabled(false);
    ui->onACQ->setEnabled(false);
    ui->offACQ->setEnabled(false);
    ui->checkBox->setEnabled(false);

}

daq_window::~daq_window()
{
    delete ui;
}
// ------------------------------------------------------------------------ //
void daq_window::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ------------------------------------------------------------------------- //
void daq_window::readLog()
{
    string buff = msg().buffer();
    ui->loggingScreen->append(QString::fromStdString(buff));
    //ui->loggingScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    msg().clear();
}
// ------------------------------------------------------------------------- //
void daq_window::SetWarning(QString warning, QString bkgcol ){

    ui->connectionLabel_2->setWordWrap(true);
    ui->connectionLabel_2->setText( warning );
    ui->connectionLabel_2->setStyleSheet("background-color: "+bkgcol);
}
// ------------------------------------------------------------------------- //
void daq_window::SetWarning2(QString warning, QString bkgcol ){
    ui->connectionLabel_3->setWordWrap(true);
    ui->connectionLabel_3->setText( warning );
    ui->connectionLabel_3->setStyleSheet("background-color: "+bkgcol);
}
// ------------------------------------------------------------------------- //

void daq_window::on_Box_fec1_clicked()
{
    if (ui->Box_fec1->isChecked()){
        fecBoxLogic(true,0);
    }
    else {fecBoxLogic(false,0);}
}
void daq_window::on_Box_fec2_clicked()
{
    if (ui->Box_fec2->isChecked()){
        fecBoxLogic(true,1);
    }
    else {fecBoxLogic(false,1);}
}
void daq_window::on_Box_fec3_clicked()
{
    if (ui->Box_fec3->isChecked()){fecBoxLogic(true,2);}
    else {fecBoxLogic(false,2);}
}
void daq_window::on_Box_fec4_clicked()
{
    if (ui->Box_fec4->isChecked()){fecBoxLogic(true,3);}
    else {fecBoxLogic(false,3);}
}
void daq_window::on_Box_fec5_clicked()
{
    if (ui->Box_fec5->isChecked()){fecBoxLogic(true,4);}
    else {fecBoxLogic(false,4);}
}
void daq_window::on_Box_fec6_clicked()
{
    if (ui->Box_fec6->isChecked()){fecBoxLogic(true,5);}
    else {fecBoxLogic(false,5);}
}
void daq_window::on_Box_fec7_clicked()
{
    if (ui->Box_fec7->isChecked()){fecBoxLogic(true,6);}
    else {fecBoxLogic(false,6);}
}
void daq_window::on_Box_fec8_clicked()
{
    if (ui->Box_fec8->isChecked()){fecBoxLogic(true,7);}
    else {fecBoxLogic(false,7);}
}
void daq_window::fecBoxLogic(bool checked, unsigned short fec){
    if(checked) {
        ui->Send->setEnabled(false);
        ui->checkBox->setChecked(false);
        ui->checkBox->setEnabled(false);
        ui->trgPulser->setEnabled(false);
        ui->trgExternal->setEnabled(false);
        ui->onACQ->setEnabled(false);
        ui->offACQ->setEnabled(false);
    }
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<fec && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        ui->tabWidget->insertTab(fec-NotActiveBefore, new fec_window(this,fec), QString(" FEC %0").arg(fec+1));
        ui->tabWidget->setCurrentIndex(fec-NotActiveBefore);
        root_main->daq[0].SetFEC(fec,true);
    }
    else {
        ui->tabWidget->removeTab(fec-NotActiveBefore);
        root_main->daq[0].SetFEC(fec,false);
//        delete fec_window(this,fec);
    }
}

void daq_window::on_Button_load_clicked()
{

    QString text = ui->line_configFile->displayText();
    std::string fname = text.toStdString();
    std::string filename = fname;
    if (fname == "") {
        std::cout << "No file name specified" << std::endl;
        ui->line_configFile->insert("ERROR: no file name given");
    }
    else {

        fname+=".txt";
        bool found = root_main->daqconfhandl->LoadDAQConf(fname.c_str());
        if (!found){
            std::cout << "File not found" << std::endl;
            ui->line_configFile->insert("ERROR: not found");
        }

        else {
            root_main->vmmconfhandl->LoadAllVMMConf(filename);
            root_main->hybridconfhandl->LoadAllHybridConf(filename);
            root_main->fecconfhandl->LoadAllFECConf(filename);
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
            root_main->daqconfhandl->LoadDAQConf(fname.c_str());
            std::cout << "loading file " << fname << std::endl;
            for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
                if (root_main->daq_act[i]){
                    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                        if (root_main->daq[i].GetFEC(j)){
                            if (j==0 && !ui->Box_fec1->isChecked()){ui->Box_fec1->setChecked(true);on_Box_fec1_clicked();}
                            if (j==1 && !ui->Box_fec2->isChecked()){ui->Box_fec2->setChecked(true);on_Box_fec2_clicked();}
                            if (j==2 && !ui->Box_fec3->isChecked()){ui->Box_fec3->setChecked(true);on_Box_fec3_clicked();}
                            if (j==3 && !ui->Box_fec4->isChecked()){ui->Box_fec4->setChecked(true);on_Box_fec4_clicked();}
                            if (j==4 && !ui->Box_fec5->isChecked()){ui->Box_fec5->setChecked(true);on_Box_fec5_clicked();}
                            if (j==5 && !ui->Box_fec6->isChecked()){ui->Box_fec6->setChecked(true);on_Box_fec6_clicked();}
                            if (j==6 && !ui->Box_fec7->isChecked()){ui->Box_fec7->setChecked(true);on_Box_fec7_clicked();}
                            if (j==7 && !ui->Box_fec8->isChecked()){ui->Box_fec8->setChecked(true);on_Box_fec8_clicked();}
                            for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                                if(root_main->daq[i].fec[j].GetHDMI(k)){
                                    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                                        if (root_main->daq[i].fec[j].hdmi[k].GetHybrid(l)){
                                            for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                                                if (root_main->daq[i].fec[j].hdmi[k].hybrid[l].GetVMM(m)){
                                                    std::cout << "vmm " << m << " on hybrid " << l << "(pos " << root_main->daq[i].fec[j].hdmi[k].hybrid[l].GetPosNo()<< ", " <<root_main->daq[i].fec[j].hdmi[k].hybrid[l].GetPosX() << ") on hmdi "<< k << " on fec " << j << " on daq " << i << " is active" << std::endl;
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
//root_main->vmmconfhandl->LoadAllVMMConf(filename);
}

void daq_window::on_Button_save_clicked()
{
    QString text = ui->line_configFile->displayText();
    std::string fname = text.toStdString();
    if (fname == "") {
        std::cout << "No file name specified" << std::endl;
        ui->line_configFile->insert("ERROR: no file name given");
    }
    else {
        root_main->vmmconfhandl->WriteAllVMMConf(fname);
        root_main->hybridconfhandl->WriteAllHybridConf(fname);
        root_main->fecconfhandl->WriteAllFECConf(fname);
        fname+=".txt";
        root_main->daqconfhandl->WriteDAQConf(fname.c_str());
        std::cout << "loading file " << fname << std::endl;
    }
}

void daq_window::on_openConnection_2_clicked()
{
    for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
        if (root_main->daq_act[i]){
            for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                if (root_main->daq[i].GetFEC(j)){

                    if(root_main->daq[i].fec[j].fec_conf_mod->Connect()==1){
                        SetWarning("all alive","green");
                         ui->Send->setEnabled(true);
                         ui->checkBox->setEnabled(true);
                    }
                    else{

                        SetWarning("ping failed", "red");
                         ui->Send->setEnabled(false);
                         ui->checkBox->setChecked(false);
                         ui->checkBox->setEnabled(false);
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

void daq_window::on_reset_warnings_clicked()
{
    SetWarning2("","light");
}

void daq_window::on_Send_clicked()
{
    for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
        if (root_main->daq_act[i]){
            root_main->daq[i].SendAll();
        }
    }
}

void daq_window::on_checkBox_stateChanged(int arg1)
{
    if(ui->checkBox->isChecked()){
        ui->trgPulser->setEnabled(true);
        ui->trgExternal->setEnabled(true);
        ui->onACQ->setEnabled(true);
        ui->offACQ->setEnabled(true);
        sendstate = "GlobalACQon";
        emit ChangeState();
    }
    else if(!ui->checkBox->isChecked()){
        ui->trgPulser->setEnabled(false);
        ui->trgExternal->setEnabled(false);
        ui->onACQ->setEnabled(false);
        ui->offACQ->setEnabled(false);
        sendstate = "GlobalACQoff";
        emit ChangeState();
    }
}
void daq_window::on_trgPulser_clicked()
{
    ui->trgPulser->setCheckable(true);
    ui->trgPulser->setChecked(true);
    ui->trgExternal->setChecked(false);
    sendstate = "trigPulser";
    emit ChangeState();
}

void daq_window::on_trgExternal_clicked()
{
    ui->trgExternal->setCheckable(true);
    ui->trgExternal->setChecked(true);
    ui->trgPulser->setChecked(false);
    sendstate = "trigExternal";
    emit ChangeState();
}
void daq_window::on_onACQ_clicked()
{
    ui->onACQ->setCheckable(true);
    ui->onACQ->setChecked(true);
    ui->offACQ->setChecked(false);
    ui->Send->setEnabled(false);
    root_main->daq[0].SendAll();
    root_main->daq[0].ACQHandler(true);
}

void daq_window::on_offACQ_clicked()
{
    ui->offACQ->setCheckable(true);
    ui->offACQ->setChecked(true);
    ui->onACQ->setChecked(false);
    ui->Send->setEnabled(true);
    root_main->daq[0].ACQHandler(false);
}


