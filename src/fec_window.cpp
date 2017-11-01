#include "fec_window.h"
#include "hdmi_window.h" // has NOT to be included here. If included in header file: compiler error in mainwindow.h:61:9: error: ‘daq_window’ does not name a type


fec_window::fec_window(daq_window *top, unsigned short fec, QWidget *parent) :
    QWidget(parent),
    root_daq{top},
    fec_index{fec},
    ui(new Ui::fec_window)
{
    ui->setupUi(this);
    updateWindow();
    SetToolTips();
    LoadSettings();
//    this->setStyleSheet("QWidget {background: 'white';}");
    ui->linkPB->setEnabled(false);
    ui->resetLinks->setEnabled(false);
    ui->fec_WarmInit->setEnabled(false);
    ui->fec_reset->setEnabled(false);
    ui->trgPulser->setEnabled(false);
    ui->trgExternal->setEnabled(false);
    ui->onACQ->setEnabled(false);
    ui->offACQ->setEnabled(false);

    ui->debugScreen->setReadOnly(true);



//    connect(ui->setEvbld, SIGNAL(pressed()),
//                                            this, SLOT(updateSettings()));
    connect(ui->evbld_mode, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->evbld_infodata, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->timeStampResCheckBox, SIGNAL(stateChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->trgPeriod, SIGNAL(textChanged(QString)),
                                    this, SLOT(updateSettings()));
    connect(ui->pulserDelay, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->bcid_reset, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->acqSync, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->acqWindow, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));

    //L0
    connect(ui->L0BCoffset, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->L0_offset, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->L0_rollover, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->L0_window, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->L0_truncate, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->L0_nskip, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->sL0enaV, SIGNAL(pressed()),
                                    this, SLOT(updateSettings()));
    connect(ui->sL0ena, SIGNAL(pressed()),
                                    this, SLOT(updateSettings()));
    connect(ui->sL0cktest, SIGNAL(pressed()),
                                    this, SLOT(updateSettings()));

    connect(ui->linkPB, SIGNAL(clicked()),
                                    this, SLOT(checkLinkStatus()));
    connect(root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod, SIGNAL(checkLinks()),
                                    this, SLOT(writeFECStatus()));

    connect(root_daq->ui->openConnection_2, SIGNAL(clicked()),
                                    this, SLOT(updateSettings()));
    connect(ui->resetLinks, SIGNAL(clicked()),
                                    this, SLOT(updateSettings()));
    connect(ui->fec_WarmInit, SIGNAL(clicked()),
                                    this, SLOT( resetFEC() ));
    connect(ui->fec_reset, SIGNAL(clicked()),
                                    this, SLOT( resetFEC() ));
    connect(ui->trgPulser, SIGNAL(clicked()),
                                    this, SLOT( updateSettings() ));
    connect(ui->trgExternal, SIGNAL(clicked()),
                                    this, SLOT( updateSettings() ));
    connect(ui->onACQ, SIGNAL(clicked()),
                                    this, SLOT( updateSettings() ));
    connect(ui->offACQ, SIGNAL(clicked()),
                                    this, SLOT( updateSettings() ));

    connect(root_daq, SIGNAL(ChangeState()),
                                    this, SLOT( ACQhandler() ));

}

fec_window::~fec_window()
{
    delete ui;
}

void fec_window::ACQhandler(){
    if(root_daq->sendstate == "GlobalACQon" ){
          emit ui->offACQ->clicked();
          ui->trgPulser->setChecked(false);
          ui->trgExternal->setChecked(false);
          ui->onACQ->setChecked(false);
          ui->offACQ->setChecked(false);

          ui->trgPulser->setEnabled(false);
          ui->trgExternal->setEnabled(false);
          ui->onACQ->setEnabled(false);
          ui->offACQ->setEnabled(false);
    }
    else if(root_daq->sendstate == "GlobalACQoff" ){
          ui->trgPulser->setEnabled(true);
          ui->trgExternal->setEnabled(true);
          ui->onACQ->setEnabled(true);
          ui->offACQ->setEnabled(true);
    }
    else if(root_daq->sendstate == "trigPulser" ){
        SetTrigMode(1);
    }
    else if(root_daq->sendstate == "trigExternal" ){
        SetTrigMode(0);
    }

}


void fec_window::updateSettings(){

//    if(QObject::sender() == ui->setEvbld){
//        root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->testing();
//    }
    if(QObject::sender() == ui->evbld_mode){
        Fec_Set("evbld_mode",  ui->evbld_mode->currentIndex() );
    }
    else if(QObject::sender() == ui->evbld_infodata){
        Fec_Set("evbld_infodata",  ui->evbld_infodata->currentIndex() );
    }
    else if(QObject::sender() == ui->timeStampResCheckBox){
        Fec_Set("highres",  ui->timeStampResCheckBox->isChecked() );
    }
    else if(QObject::sender() == ui->trgPeriod){
            QString val_trg = ui->trgPeriod->text();
            bool ok;
            int value = val_trg.toInt(&ok,16);
        Fec_Set("trigger_period", value );
    }
    else if(QObject::sender() == ui->pulserDelay){
        Fec_Set("tp_delay",  ui->pulserDelay->value() );
    }
    else if(QObject::sender() == ui->bcid_reset){
        Fec_Set("bcid_reset",  ui->bcid_reset->value() );
    }
    else if(QObject::sender() == ui->acqSync){
        Fec_Set("acq_sync",  ui->acqSync->value() );
    }
    else if(QObject::sender() == ui->acqWindow){
        Fec_Set("acq_window",  ui->acqWindow->value() );
    }

    //L0
    else if(QObject::sender() == ui->L0BCoffset){
        Fec_Set("l0offset",  ui->L0BCoffset->value() );
    }
    else if(QObject::sender() == ui->L0_offset){
        Fec_Set("offset",  ui->L0_offset->value() );
    }
    else if(QObject::sender() == ui->L0_rollover){
        Fec_Set("rollover",  ui->L0_rollover->value() );
    }
    else if(QObject::sender() == ui->L0_window){
        Fec_Set("window",  ui->L0_window->value() );
    }
    else if(QObject::sender() == ui->L0_truncate){
        Fec_Set("truncate",  ui->L0_truncate->value() );
    }
    else if(QObject::sender() == ui->L0_nskip){
        Fec_Set("nskip",  ui->L0_nskip->value() );
    }
    else if(QObject::sender() == ui->sL0enaV){
        Fec_Set("sL0enaV", !ui->sL0enaV->isChecked());
    }
    else if(QObject::sender() == ui->sL0ena){
        Fec_Set("sL0ena", !ui->sL0ena->isChecked());
    }
    else if(QObject::sender() == ui->sL0cktest){
        Fec_Set("sL0cktest", !ui->sL0cktest->isChecked());
    }
    else if(QObject::sender() == root_daq->ui->openConnection_2){
        if(root_daq->ui->connectionLabel_2->text()==QString("all alive")){
            ui->linkPB->setEnabled(true);
//            ui->resetLinks->setEnabled(true);
            if(!root_daq->ui->checkBox->isChecked()){
            ui->fec_WarmInit->setEnabled(true);
            ui->fec_reset->setEnabled(true);
            ui->trgPulser->setEnabled(true);
            ui->trgExternal->setEnabled(true);
            ui->onACQ->setEnabled(true);
            ui->offACQ->setEnabled(true);
            }
        }
        else{
            ui->linkPB->setEnabled(false);
            ui->resetLinks->setEnabled(false);
            ui->fec_WarmInit->setEnabled(false);
            ui->fec_reset->setEnabled(false);
            ui->trgPulser->setEnabled(false);
            ui->trgExternal->setEnabled(false);
            ui->onACQ->setEnabled(false);
            ui->offACQ->setEnabled(false);
            }
    }
    else if(QObject::sender() == ui->resetLinks){
        root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->resetLinks();
    }


    else if(QObject::sender() == ui->trgPulser){
        ui->trgPulser->setCheckable(true);
        ui->trgPulser->setChecked(true);
        ui->trgExternal->setChecked(false);
        SetTrigMode(1);
    }
    else if(QObject::sender() == ui->trgExternal){
        ui->trgExternal->setCheckable(true);
        ui->trgExternal->setChecked(true);
        ui->trgPulser->setChecked(false);
        SetTrigMode(0);
    }
    else if(QObject::sender() == ui->onACQ){
        ui->onACQ->setCheckable(true);
        if(ui->trgExternal->isChecked()){
            emit ui->trgExternal->clicked();
        }
        else if(ui->trgPulser->isChecked()){
            emit ui->trgPulser->clicked();
        }
        else{
            root_daq->SetWarning2("Select Trigger Mode","red");
            ui->onACQ->setChecked(false);
            return;
        }

        ui->onACQ->setChecked(true);
        ui->offACQ->setChecked(false);
        root_daq->ui->Send->setEnabled(false);
        root_daq->root_main->daq[0].SendAll();
        root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->ACQon();
    }
    else if(QObject::sender() == ui->offACQ){
        ui->offACQ->setCheckable(true);
        ui->offACQ->setChecked(true);
        ui->onACQ->setChecked(false);
        root_daq->ui->Send->setEnabled(true);
         root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->ACQoff();
    }




}

void fec_window::SetTrigMode(int mode){
    Fec_Set("triggermode", mode);
    root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->setTriggerMode();
}



void fec_window::LoadSettings(){

    QString ip =  QString("%1.%2.%3.%4").arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip1" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip2" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip3" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip4" ));

    while( !root_daq->root_main->daq[0].CheckIP( ip, fec_index ) ){
        root_daq->root_main->daq[0].fec[fec_index].SetReg( "ip4",(unsigned long) root_daq->root_main->daq[0].fec[fec_index].GetRegVal( "ip4" )+1 );
        ip =  QString("%1.%2.%3.%4").arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip1" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip2" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip3" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip4" ));
    }

    ui->ip1_2->setText( root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip1" ) );
    ui->ip2_2->setText( root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip2" ) );
    ui->ip3_2->setText( root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip3" ) );
    ui->ip4_2->setText( root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip4" ) );

    ui->pulserDelay->setValue( Fec_Get( "tp_delay" ) );
    ui->trgPeriod->setText( QString::number( Fec_Get( "trigger_period" ), 16 ) );
    ui->bcid_reset->setValue( Fec_Get( "bcid_reset" ) );
    ui->acqSync->setValue( Fec_Get( "acq_sync" ) );
    ui->acqWindow->setValue( Fec_Get( "acq_window" ) );

    ui->evbld_mode->setCurrentIndex( Fec_Get( "evbld_mode" ) );
    ui->evbld_infodata->setCurrentIndex( Fec_Get( "evbld_infodata" ) );
    ui->timeStampResCheckBox->setChecked( Fec_Get( "highres" ) );

    //L0
    ui->L0BCoffset->setValue( Fec_Get( "l0offset" ) );
    ui->L0_offset->setValue( Fec_Get( "offset" ) );
    ui->L0_rollover->setValue( Fec_Get( "rollover" ) );
    ui->L0_window->setValue( Fec_Get( "window" ) );
    ui->L0_truncate->setValue( Fec_Get( "truncate" ) );
    ui->L0_nskip->setValue( Fec_Get( "nskip" ) );
    ui->sL0enaV->setChecked( Fec_Get( "sL0enaV" ) );
    ui->sL0ena->setChecked( Fec_Get( "sL0ena" ) );
    ui->sL0cktest->setChecked( Fec_Get( "sL0cktest" ) );


}

void fec_window::SetToolTips(){
    ui->sL0enaV->setToolTip("disable mixed signal functions when L0 enabled");
    ui->sL0ena->setToolTip("enable L0 core / reset core & gate clk if 0");
    ui->sL0cktest->setToolTip("enable clocks when L0 core disabled (test)");

}
bool fec_window::Fec_Set(const char *feature, unsigned long val){
    return root_daq->root_main->daq[0].fec[fec_index].SetReg(feature,  (unsigned long) val );
}
unsigned long fec_window::Fec_Get(const char *feature){
    return root_daq->root_main->daq[0].fec[fec_index].GetRegVal(feature);
}

void fec_window::on_Box_hdmi1_clicked()
{
    std::cout << " Box_hdmi1 clicked" << std::endl;
    if (ui->Box_hdmi1->isChecked()){hdmiBoxLogic(true,0);}
    else {hdmiBoxLogic(false,0);}
}
void fec_window::on_Box_hdmi2_clicked()
{
    if (ui->Box_hdmi2->isChecked()){hdmiBoxLogic(true,1);}
    else {hdmiBoxLogic(false,1);}
}
void fec_window::on_Box_hdmi3_clicked()
{
    if (ui->Box_hdmi3->isChecked()){hdmiBoxLogic(true,2);}
    else {hdmiBoxLogic(false,2);}
}
void fec_window::on_Box_hdmi4_clicked()
{
    if (ui->Box_hdmi4->isChecked()){hdmiBoxLogic(true,3);}
    else {hdmiBoxLogic(false,3);}
}
void fec_window::on_Box_hdmi5_clicked()
{
    if (ui->Box_hdmi5->isChecked()){hdmiBoxLogic(true,4);}
    else {hdmiBoxLogic(false,4);}
}
void fec_window::on_Box_hdmi6_clicked()
{
    if (ui->Box_hdmi6->isChecked()){hdmiBoxLogic(true,5);}
    else {hdmiBoxLogic(false,5);}
}
void fec_window::on_Box_hdmi7_clicked()
{
    if (ui->Box_hdmi7->isChecked()){hdmiBoxLogic(true,6);}
    else {hdmiBoxLogic(false,6);}
}
void fec_window::on_Box_hdmi8_clicked()
{
    if (ui->Box_hdmi8->isChecked()){hdmiBoxLogic(true,7);}
    else {hdmiBoxLogic(false,7);}
}
void fec_window::hdmiBoxLogic(bool checked, unsigned short hdmi){
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<hdmi && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        ui->tabWidget->insertTab(hdmi-NotActiveBefore, new hdmi_window(this,fec_index,hdmi), QString(" HDMI %0").arg(hdmi+1));
        ui->tabWidget->setCurrentIndex(hdmi-NotActiveBefore);
        root_daq->root_main->daq[0].fec[fec_index].SetHDMI(hdmi, true);

    }
    else {
        ui->tabWidget->removeTab(hdmi-NotActiveBefore);
        root_daq->root_main->daq[0].fec[fec_index].SetHDMI(hdmi, false);
    }
}


void fec_window::updateWindow(){
    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
        if(root_daq->root_main->daq[0].fec[fec_index].GetHDMI(k)){
            if (k == 0 && !ui->Box_hdmi1->isChecked()){ui->Box_hdmi1->setChecked(true); on_Box_hdmi1_clicked();}
            if (k == 1 && !ui->Box_hdmi2->isChecked()){ui->Box_hdmi2->setChecked(true); on_Box_hdmi2_clicked();}
            if (k == 2 && !ui->Box_hdmi3->isChecked()){ui->Box_hdmi3->setChecked(true); on_Box_hdmi3_clicked();}
            if (k == 3 && !ui->Box_hdmi4->isChecked()){ui->Box_hdmi4->setChecked(true); on_Box_hdmi4_clicked();}
            if (k == 4 && !ui->Box_hdmi5->isChecked()){ui->Box_hdmi5->setChecked(true); on_Box_hdmi5_clicked();}
            if (k == 5 && !ui->Box_hdmi6->isChecked()){ui->Box_hdmi6->setChecked(true); on_Box_hdmi6_clicked();}
            if (k == 6 && !ui->Box_hdmi7->isChecked()){ui->Box_hdmi7->setChecked(true); on_Box_hdmi7_clicked();}
            if (k == 7 && !ui->Box_hdmi8->isChecked()){ui->Box_hdmi8->setChecked(true); on_Box_hdmi8_clicked();}
        }
        else{
            if (k == 0 && ui->Box_hdmi1->isChecked()){ui->Box_hdmi1->setChecked(false); on_Box_hdmi1_clicked();}
            if (k == 1 && ui->Box_hdmi2->isChecked()){ui->Box_hdmi2->setChecked(false); on_Box_hdmi2_clicked();}
            if (k == 2 && ui->Box_hdmi3->isChecked()){ui->Box_hdmi3->setChecked(false); on_Box_hdmi3_clicked();}
            if (k == 3 && ui->Box_hdmi4->isChecked()){ui->Box_hdmi4->setChecked(false); on_Box_hdmi4_clicked();}
            if (k == 4 && ui->Box_hdmi5->isChecked()){ui->Box_hdmi5->setChecked(false); on_Box_hdmi5_clicked();}
            if (k == 5 && ui->Box_hdmi6->isChecked()){ui->Box_hdmi6->setChecked(false); on_Box_hdmi6_clicked();}
            if (k == 6 && ui->Box_hdmi7->isChecked()){ui->Box_hdmi7->setChecked(false); on_Box_hdmi7_clicked();}
            if (k == 7 && ui->Box_hdmi8->isChecked()){ui->Box_hdmi8->setChecked(false); on_Box_hdmi8_clicked();}
        }
    }
}

void fec_window::on_ip4_2_textChanged(const QString &arg1)
{
    QString ip =  QString("%1.%2.%3.%4").arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip1" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip2" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip3" )).arg(ui->ip4_2->text().toInt());
    if(root_daq->root_main->daq[0].CheckIP( ip, fec_index )) root_daq->root_main->daq[0].fec[fec_index].SetReg("ip4", (unsigned long)ui->ip4_2->text().toInt());
    else {
        ui->ip4_2->setText(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip4" ));
        root_daq->SetWarning2("IP already exists- resetted!", "orange");
    }
}

void fec_window::on_ip3_2_textChanged(const QString &arg1)
{
    QString ip =  QString("%1.%2.%3.%4").arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip1" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip2" )).arg(ui->ip3_2->text().toInt()).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip4" ));
   if(root_daq->root_main->daq[0].CheckIP( ip, fec_index ))root_daq->root_main->daq[0].fec[fec_index].SetReg("ip3", (unsigned long)ui->ip3_2->text().toInt());
   else {
       ui->ip3_2->setText(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip3" ));
       root_daq->SetWarning2("IP already exists- resetted!", "orange");
   }
}

void fec_window::on_ip2_2_textChanged(const QString &arg1)
{
    QString ip =  QString("%1.%2.%3.%4").arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip1" )).arg(ui->ip2_2->text().toInt()).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip3" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip4" ));
   if(root_daq->root_main->daq[0].CheckIP( ip, fec_index ))root_daq->root_main->daq[0].fec[fec_index].SetReg("ip2", (unsigned long)ui->ip2_2->text().toInt());
   else {
       ui->ip2_2->setText(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip2" ));
       root_daq->SetWarning2("IP already exists- resetted!", "orange");
   }
}

void fec_window::on_ip1_2_textChanged(const QString &arg1)
{
    QString ip =  QString("%1.%2.%3.%4").arg(ui->ip3_2->text().toInt()).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip2" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip3" )).arg(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip4" ));
  if(root_daq->root_main->daq[0].CheckIP( ip, fec_index ))root_daq->root_main->daq[0].fec[fec_index].SetReg("ip1", (unsigned long)ui->ip1_2->text().toInt());
  else {
      ui->ip1_2->setText(root_daq->root_main->daq[0].fec[fec_index].GetReg( "ip1" ));
      root_daq->SetWarning2("IP already exists- resetted!", "orange");
  }
}

void fec_window::checkLinkStatus(){
    root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->checkLinkStatus();
}

void fec_window::writeFECStatus()
{
    QByteArray buff;
    buff.clear();
    buff.resize(root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->socket().fecSocket().pendingDatagramSize() );
    root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->socket().fecSocket().readDatagram(buff.data(), buff.size());
    if(buff.size()==0) return;

    bool ok;
    QString sizeOfPackageReceived, datagramCheck;
    datagramCheck = buff.mid(0,4).toHex();
    quint32 check = datagramCheck.toUInt(&ok,16);

    sizeOfPackageReceived = sizeOfPackageReceived.number(buff.size(),10);

    if(check<1000000) {
        stringstream ss;
        ss << " ****** NEW PACKET RECEIVED ****** " << endl;
        ss << " Data received size: " << sizeOfPackageReceived.toStdString()
           << " bytes" << endl;
        QString bin, hex;
        for(int i = 0; i < buff.size()/4; i++) {
            hex = buff.mid(i*4, 4).toHex();
            quint32 tmp32 = hex.toUInt(&ok,16);
            if(i==0) ss << " Rec'd ID: " << bin.number(tmp32,10).toStdString() << endl;
            else {
                ss << " Data, " << i << ": " << bin.number(tmp32,16).toStdString() << endl;
            }
        } // i
        ui->debugScreen->append(QString::fromStdString(ss.str()));
        ui->debugScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    }
}

void fec_window::on_clearDebugScreenPB_clicked()
{
    ui->debugScreen->clear();
}

// ------------------------------------------------------------------------- //
void fec_window::resetFEC()
{
    bool do_reset = (ui->fec_reset == QObject::sender() ? true : false);
    root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->resetFEC(do_reset);
    //    runModule().resetFEC(do_reset);
//    ui->fecRB->setChecked(1);

    ui->trgExternal->setChecked(false);
    ui->trgPulser->setChecked(false);
    ui->onACQ->setChecked(false);
    ui->offACQ->setChecked(false);
//    ui->setTrgAcqConst->setChecked(false);

//    SetInitialState();
//    m_commOK = true;
//    m_configOK = false;
//    m_tdaqOK = false;
//    m_runModeOK = false;
//    m_acqMode = "";
//    emit checkFSM();
}
// ------------------------------------------------------------------------- //
