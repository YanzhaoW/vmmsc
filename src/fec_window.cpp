#include "fec_window.h"
#include "hdmi_window.h" // has NOT to be included here. If included in header file: compiler error in mainwindow.h:61:9: error: ‘daq_window’ does not name a type


FECWindow::FECWindow(DAQWindow *top, unsigned short fec, QWidget *parent) :
    QWidget(parent),
    m_daqWindow{top},
    m_fecIndex{fec},
    m_ui(new Ui::fec_window)
{
    m_ui->setupUi(this);
    UpdateWindow();
    SetToolTips();
    LoadSettings();
    //    this->setStyleSheet("QWidget {background: 'white';}");
    m_ui->linkPB->setEnabled(false);
    m_ui->resetLinks->setEnabled(false);
    m_ui->fec_WarmInit->setEnabled(false);
    m_ui->fec_reset->setEnabled(false);
    m_ui->trgPulser->setEnabled(false);
    m_ui->trgExternal->setEnabled(false);
    m_ui->onACQ->setEnabled(false);
    m_ui->offACQ->setEnabled(false);

    m_ui->debugScreen->setReadOnly(true);



    //    connect(ui->setEvbld, SIGNAL(pressed()),
    //                                            this, SLOT(updateSettings()));
    connect(m_ui->evbld_mode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->evbld_infodata, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->timeStampResCheckBox, SIGNAL(stateChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->trgPeriod, SIGNAL(textChanged(QString)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->pulserDelay, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->bcid_reset, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->acqSync, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->acqWindow, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->ClearS6FIFI, SIGNAL(stateChanged(int)),
            this, SLOT(onUpdateSettings()));

    //L0
    connect(m_ui->L0BCoffset, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->L0_offset, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->L0_rollover, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->L0_window, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->L0_truncate, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->L0_nskip, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0enaV, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0ena, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0cktest, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->linkPB, SIGNAL(clicked()),
            this, SLOT(onCheckLinkStatus()));

    // connect(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule, SIGNAL(CheckLinks()),
    //        this, SLOT(onWriteFECStatus()));

    connect(m_daqWindow->ui->openConnection_2, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->resetLinks, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->fec_WarmInit, SIGNAL(clicked()),
            this, SLOT( onResetFEC() ));
    connect(m_ui->fec_reset, SIGNAL(clicked()),
            this, SLOT( onResetFEC() ));
    connect(m_ui->trgPulser, SIGNAL(clicked()),
            this, SLOT( onUpdateSettings() ));
    connect(m_ui->trgExternal, SIGNAL(clicked()),
            this, SLOT( onUpdateSettings() ));
    connect(m_ui->onACQ, SIGNAL(clicked()),
            this, SLOT( onUpdateSettings() ));
    connect(m_ui->offACQ, SIGNAL(clicked()),
            this, SLOT( onUpdateSettings() ));

    connect(m_daqWindow, SIGNAL(ChangeState()),
            this, SLOT( onACQHandler() ));

}

FECWindow::~FECWindow()
{
    delete m_ui;
}

void FECWindow::onACQHandler(){
    if(m_daqWindow->m_sendstate == "GlobalACQon" ){
        emit m_ui->offACQ->clicked();
        m_ui->trgPulser->setChecked(false);
        m_ui->trgExternal->setChecked(false);
        m_ui->onACQ->setChecked(false);
        m_ui->offACQ->setChecked(false);

        m_ui->trgPulser->setEnabled(false);
        m_ui->trgExternal->setEnabled(false);
        m_ui->onACQ->setEnabled(false);
        m_ui->offACQ->setEnabled(false);
    }
    else if(m_daqWindow->m_sendstate == "GlobalACQoff" ){
        m_ui->trgPulser->setEnabled(true);
        m_ui->trgExternal->setEnabled(true);
        m_ui->onACQ->setEnabled(true);
        m_ui->offACQ->setEnabled(true);
    }
    else if(m_daqWindow->m_sendstate == "trigPulser" ){
        onSetReadoutMode(1);
    }
    else if(m_daqWindow->m_sendstate == "trigExternal" ){
        onSetReadoutMode(0);
    }

}


void FECWindow::onUpdateSettings(){

    //    if(QObject::sender() == ui->setEvbld){
    //        root_daq->root_main->daq[0].fec[fec_index].fec_conf_mod->testing();
    //    }
    if(QObject::sender() == m_ui->evbld_mode){
        SetFec("evbld_mode",  m_ui->evbld_mode->currentIndex() );
    }
    else if(QObject::sender() == m_ui->evbld_infodata){
        SetFec("evbld_infodata",  m_ui->evbld_infodata->currentIndex() );
    }
    else if(QObject::sender() == m_ui->timeStampResCheckBox){
        SetFec("highres",  m_ui->timeStampResCheckBox->isChecked() );
    }
    else if(QObject::sender() == m_ui->trgPeriod){
        QString val_trg = m_ui->trgPeriod->text();
        bool ok;
        int value = val_trg.toInt(&ok,16);
        SetFec("trigger_period", value );
    }
    else if(QObject::sender() == m_ui->pulserDelay){
        SetFec("tp_delay",  m_ui->pulserDelay->value() );
    }
    else if(QObject::sender() == m_ui->bcid_reset){
        SetFec("bcid_reset",  m_ui->bcid_reset->value() );
    }
    else if(QObject::sender() == m_ui->acqSync){
        SetFec("acq_sync",  m_ui->acqSync->value() );
    }
    else if(QObject::sender() == m_ui->acqWindow){
        SetFec("acq_window",  m_ui->acqWindow->value() );
    }
    else if(QObject::sender() == m_ui->ClearS6FIFI){
        SetFec("clear_S6_fifo",  m_ui->ClearS6FIFI->isChecked() );
    }

    //L0
    else if(QObject::sender() == m_ui->L0BCoffset){
        SetFec("l0offset",  m_ui->L0BCoffset->value() );
    }
    else if(QObject::sender() == m_ui->L0_offset){
        SetFec("offset",  m_ui->L0_offset->value() );
    }
    else if(QObject::sender() == m_ui->L0_rollover){
        SetFec("rollover",  m_ui->L0_rollover->value() );
    }
    else if(QObject::sender() == m_ui->L0_window){
        SetFec("window",  m_ui->L0_window->value() );
    }
    else if(QObject::sender() == m_ui->L0_truncate){
        SetFec("truncate",  m_ui->L0_truncate->value() );
    }
    else if(QObject::sender() == m_ui->L0_nskip){
        SetFec("nskip",  m_ui->L0_nskip->value() );
    }
    else if(QObject::sender() == m_ui->sL0enaV){
        SetFec("sL0enaV", !m_ui->sL0enaV->isChecked());
    }
    else if(QObject::sender() == m_ui->sL0ena){
        SetFec("sL0ena", !m_ui->sL0ena->isChecked());
    }
    else if(QObject::sender() == m_ui->sL0cktest){
        SetFec("sL0cktest", !m_ui->sL0cktest->isChecked());
    }
    else if(QObject::sender() == m_daqWindow->ui->openConnection_2){
        if(m_daqWindow->ui->connectionLabel_2->text()==QString("all alive")){
            m_ui->linkPB->setEnabled(true);
            //            ui->resetLinks->setEnabled(true);
            if(!m_daqWindow->ui->checkBoxGlobalDAQ->isChecked()){
                m_ui->fec_WarmInit->setEnabled(true);
                m_ui->fec_reset->setEnabled(true);
                m_ui->trgPulser->setEnabled(true);
                m_ui->trgExternal->setEnabled(true);
                m_ui->onACQ->setEnabled(true);
                m_ui->offACQ->setEnabled(true);
            }
        }
        else{
            m_ui->linkPB->setEnabled(false);
            m_ui->resetLinks->setEnabled(false);
            m_ui->fec_WarmInit->setEnabled(false);
            m_ui->fec_reset->setEnabled(false);
            m_ui->trgPulser->setEnabled(false);
            m_ui->trgExternal->setEnabled(false);
            m_ui->onACQ->setEnabled(false);
            m_ui->offACQ->setEnabled(false);
        }
    }
    else if(QObject::sender() == m_ui->resetLinks){
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->ResetLinks();
    }


    else if(QObject::sender() == m_ui->trgPulser){
        m_ui->trgPulser->setCheckable(true);
        m_ui->trgPulser->setChecked(true);
        m_ui->trgExternal->setChecked(false);
        onSetReadoutMode(1);
    }
    else if(QObject::sender() == m_ui->trgExternal){
        m_ui->trgExternal->setCheckable(true);
        m_ui->trgExternal->setChecked(true);
        m_ui->trgPulser->setChecked(false);
        onSetReadoutMode(0);
    }
    else if(QObject::sender() == m_ui->onACQ){
        m_ui->onACQ->setCheckable(true);
        if(m_ui->trgExternal->isChecked()){
            emit m_ui->trgExternal->clicked();
        }
        else if(m_ui->trgPulser->isChecked()){
            emit m_ui->trgPulser->clicked();
        }
        else{
            m_daqWindow->SetWarning2("Select Trigger Mode","red");
            m_ui->onACQ->setChecked(false);
            return;
        }

        m_ui->onACQ->setChecked(true);
        m_ui->offACQ->setChecked(false);
        m_daqWindow->ui->Send->setEnabled(false);
        m_daqWindow->m_mainWindow->m_daqs[0].SendAll();
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->ACQon();
    }
    else if(QObject::sender() == m_ui->offACQ){
        m_ui->offACQ->setCheckable(true);
        m_ui->offACQ->setChecked(true);
        m_ui->onACQ->setChecked(false);
        m_daqWindow->ui->Send->setEnabled(true);
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
    }




}

void FECWindow::onSetReadoutMode(int mode){
    SetFec("triggermode", mode);
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->SetReadoutMode();
}



void FECWindow::LoadSettings(){
    //Test only, set default value
    m_ui->lineEdit_triggerOffset->setText("2048000");
    m_ui->lineEdit_triggerWindow->setText("204750");
    m_ui->lineEdit_triggerPulseDelay->setText("0");

    QString ip =  QString("%1.%2.%3.%4").arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip1" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip2" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip3" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip4" ));

    while( !m_daqWindow->m_mainWindow->m_daqs[0].CheckIP( ip, m_fecIndex ) ){
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg( "ip4",(unsigned long) m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetRegVal( "ip4" )+1 );
        ip =  QString("%1.%2.%3.%4").arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip1" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip2" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip3" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip4" ));
    }

    m_ui->ip1_2->setText( m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip1" ) );
    m_ui->ip2_2->setText( m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip2" ) );
    m_ui->ip3_2->setText( m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip3" ) );
    m_ui->ip4_2->setText( m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip4" ) );

    m_ui->pulserDelay->setValue( GetFec( "tp_delay" ) );
    m_ui->trgPeriod->setText( QString::number( GetFec( "trigger_period" ), 16 ) );



    unsigned int triggerPeriod = GetFec( "trigger_period" );

    BC_period = 25;

    if(triggerPeriod > 32000)
    {
        BC_period = 200;
    }
    else if(triggerPeriod > 16000)
    {
        BC_period = 100;
    }
    else if(triggerPeriod > 8000)
    {
        BC_period = 50;
    }


    m_ui->bcid_reset->setValue( GetFec( "bcid_reset" ) );
    m_ui->acqSync->setValue( GetFec( "acq_sync" ) );
    m_ui->acqWindow->setValue( GetFec( "acq_window" ) );
    m_ui->ClearS6FIFI->setChecked( GetFec( "clear_S6_fifo" ) );

    m_ui->evbld_mode->setCurrentIndex( GetFec( "evbld_mode" ) );
    m_ui->evbld_infodata->setCurrentIndex( GetFec( "evbld_infodata" ) );
    m_ui->timeStampResCheckBox->setChecked( GetFec( "highres" ) );

    //L0
    m_ui->L0BCoffset->setValue( GetFec( "l0offset" ) );
    m_ui->L0_offset->setValue( GetFec( "offset" ) );
    m_ui->L0_rollover->setValue( GetFec( "rollover" ) );
    m_ui->L0_window->setValue( GetFec( "window" ) );
    m_ui->L0_truncate->setValue( GetFec( "truncate" ) );
    m_ui->L0_nskip->setValue( GetFec( "nskip" ) );
    m_ui->sL0enaV->setChecked( GetFec( "sL0enaV" ) );
    m_ui->sL0ena->setChecked( GetFec( "sL0ena" ) );
    m_ui->sL0cktest->setChecked( GetFec( "sL0cktest" ) );


}

void FECWindow::SetToolTips(){
    m_ui->sL0enaV->setToolTip("disable mixed signal functions when L0 enabled");
    m_ui->sL0ena->setToolTip("enable L0 core / reset core & gate clk if 0");
    m_ui->sL0cktest->setToolTip("enable clocks when L0 core disabled (test)");

}
bool FECWindow::SetFec(const char *feature, unsigned long val){
    return m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg(feature,  (unsigned long) val );
}
unsigned long FECWindow::GetFec(const char *feature){
    return m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetRegVal(feature);
}

void FECWindow::on_Box_hdmi1_clicked()
{
    std::cout << " Box_hdmi1 clicked" << std::endl;
    if (m_ui->Box_hdmi1->isChecked()){HDMIBoxLogic(true,0);}
    else {HDMIBoxLogic(false,0);}
}
void FECWindow::on_Box_hdmi2_clicked()
{
    if (m_ui->Box_hdmi2->isChecked()){HDMIBoxLogic(true,1);}
    else {HDMIBoxLogic(false,1);}
}
void FECWindow::on_Box_hdmi3_clicked()
{
    if (m_ui->Box_hdmi3->isChecked()){HDMIBoxLogic(true,2);}
    else {HDMIBoxLogic(false,2);}
}
void FECWindow::on_Box_hdmi4_clicked()
{
    if (m_ui->Box_hdmi4->isChecked()){HDMIBoxLogic(true,3);}
    else {HDMIBoxLogic(false,3);}
}
void FECWindow::on_Box_hdmi5_clicked()
{
    if (m_ui->Box_hdmi5->isChecked()){HDMIBoxLogic(true,4);}
    else {HDMIBoxLogic(false,4);}
}
void FECWindow::on_Box_hdmi6_clicked()
{
    if (m_ui->Box_hdmi6->isChecked()){HDMIBoxLogic(true,5);}
    else {HDMIBoxLogic(false,5);}
}
void FECWindow::on_Box_hdmi7_clicked()
{
    if (m_ui->Box_hdmi7->isChecked()){HDMIBoxLogic(true,6);}
    else {HDMIBoxLogic(false,6);}
}
void FECWindow::on_Box_hdmi8_clicked()
{
    if (m_ui->Box_hdmi8->isChecked()){HDMIBoxLogic(true,7);}
    else {HDMIBoxLogic(false,7);}
}
void FECWindow::HDMIBoxLogic(bool checked, unsigned short hdmi){
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = m_ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<hdmi && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        m_ui->tabWidget->insertTab(hdmi-NotActiveBefore, new HDMIWindow(this,m_fecIndex,hdmi), QString(" HDMI %0").arg(hdmi+1));
        m_ui->tabWidget->setCurrentIndex(hdmi-NotActiveBefore);
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetHDMI(hdmi, true);

    }
    else {
        m_ui->tabWidget->removeTab(hdmi-NotActiveBefore);
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetHDMI(hdmi, false);
    }
}


void FECWindow::UpdateWindow(){
    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
        if(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetHDMI(k)){
            if (k == 0 && !m_ui->Box_hdmi1->isChecked()){m_ui->Box_hdmi1->setChecked(true); on_Box_hdmi1_clicked();}
            if (k == 1 && !m_ui->Box_hdmi2->isChecked()){m_ui->Box_hdmi2->setChecked(true); on_Box_hdmi2_clicked();}
            if (k == 2 && !m_ui->Box_hdmi3->isChecked()){m_ui->Box_hdmi3->setChecked(true); on_Box_hdmi3_clicked();}
            if (k == 3 && !m_ui->Box_hdmi4->isChecked()){m_ui->Box_hdmi4->setChecked(true); on_Box_hdmi4_clicked();}
            if (k == 4 && !m_ui->Box_hdmi5->isChecked()){m_ui->Box_hdmi5->setChecked(true); on_Box_hdmi5_clicked();}
            if (k == 5 && !m_ui->Box_hdmi6->isChecked()){m_ui->Box_hdmi6->setChecked(true); on_Box_hdmi6_clicked();}
            if (k == 6 && !m_ui->Box_hdmi7->isChecked()){m_ui->Box_hdmi7->setChecked(true); on_Box_hdmi7_clicked();}
            if (k == 7 && !m_ui->Box_hdmi8->isChecked()){m_ui->Box_hdmi8->setChecked(true); on_Box_hdmi8_clicked();}
        }
        else{
            if (k == 0 && m_ui->Box_hdmi1->isChecked()){m_ui->Box_hdmi1->setChecked(false); on_Box_hdmi1_clicked();}
            if (k == 1 && m_ui->Box_hdmi2->isChecked()){m_ui->Box_hdmi2->setChecked(false); on_Box_hdmi2_clicked();}
            if (k == 2 && m_ui->Box_hdmi3->isChecked()){m_ui->Box_hdmi3->setChecked(false); on_Box_hdmi3_clicked();}
            if (k == 3 && m_ui->Box_hdmi4->isChecked()){m_ui->Box_hdmi4->setChecked(false); on_Box_hdmi4_clicked();}
            if (k == 4 && m_ui->Box_hdmi5->isChecked()){m_ui->Box_hdmi5->setChecked(false); on_Box_hdmi5_clicked();}
            if (k == 5 && m_ui->Box_hdmi6->isChecked()){m_ui->Box_hdmi6->setChecked(false); on_Box_hdmi6_clicked();}
            if (k == 6 && m_ui->Box_hdmi7->isChecked()){m_ui->Box_hdmi7->setChecked(false); on_Box_hdmi7_clicked();}
            if (k == 7 && m_ui->Box_hdmi8->isChecked()){m_ui->Box_hdmi8->setChecked(false); on_Box_hdmi8_clicked();}
        }
    }
}

void FECWindow::on_ip4_2_textChanged()
{
    QString ip =  QString("%1.%2.%3.%4").arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip1" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip2" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip3" )).arg(m_ui->ip4_2->text().toInt());
    if(m_daqWindow->m_mainWindow->m_daqs[0].CheckIP( ip, m_fecIndex )) m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("ip4", (unsigned long)m_ui->ip4_2->text().toInt());
    else {
        m_ui->ip4_2->setText(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip4" ));
        m_daqWindow->SetWarning2("IP already exists- resetted!", "orange");
    }
}

void FECWindow::on_ip3_2_textChanged()
{
    QString ip =  QString("%1.%2.%3.%4").arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip1" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip2" )).arg(m_ui->ip3_2->text().toInt()).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip4" ));
    if(m_daqWindow->m_mainWindow->m_daqs[0].CheckIP( ip, m_fecIndex ))m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("ip3", (unsigned long)m_ui->ip3_2->text().toInt());
    else {
        m_ui->ip3_2->setText(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip3" ));
        m_daqWindow->SetWarning2("IP already exists- resetted!", "orange");
    }
}

void FECWindow::on_ip2_2_textChanged()
{
    QString ip =  QString("%1.%2.%3.%4").arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip1" )).arg(m_ui->ip2_2->text().toInt()).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip3" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip4" ));
    if(m_daqWindow->m_mainWindow->m_daqs[0].CheckIP( ip, m_fecIndex ))m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("ip2", (unsigned long)m_ui->ip2_2->text().toInt());
    else {
        m_ui->ip2_2->setText(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip2" ));
        m_daqWindow->SetWarning2("IP already exists- resetted!", "orange");
    }
}

void FECWindow::on_ip1_2_textChanged()
{
    QString ip =  QString("%1.%2.%3.%4").arg(m_ui->ip3_2->text().toInt()).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip2" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip3" )).arg(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip4" ));
    if(m_daqWindow->m_mainWindow->m_daqs[0].CheckIP( ip, m_fecIndex ))m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("ip1", (unsigned long)m_ui->ip1_2->text().toInt());
    else {
        m_ui->ip1_2->setText(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetReg( "ip1" ));
        m_daqWindow->SetWarning2("IP already exists- resetted!", "orange");
    }
}

void FECWindow::onCheckLinkStatus(){
    QString message;
    bool readOK= false;
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->CheckLinkStatus(readOK, message);
    if(readOK)
    {
        m_ui->debugScreen->insertPlainText(message);
        m_ui->debugScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        std::cout<< message.toStdString()<<std::endl;
    }

}


// ------------------------------------------------------------------------- //
void FECWindow::onResetFEC()
{
    bool do_reset = (m_ui->fec_reset == QObject::sender() ? true : false);
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->ResetFEC(do_reset);
    m_ui->trgExternal->setChecked(false);
    m_ui->trgPulser->setChecked(false);
    m_ui->onACQ->setChecked(false);
    m_ui->offACQ->setChecked(false);
}
// ------------------------------------------------------------------------- //


void FECWindow::on_clearDebugScreen_clicked()
{
    m_ui->debugScreen->clear();
}



void FECWindow::on_readSystemParams_pressed()
{
    QMap<QString, QString> registers;
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->ReadSystemRegisters(registers);
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetFirmwareVersion(registers["FirmwareVers"]);

    stringstream sx;
    sx.str("");
    sx << "**********************\n"
       << " Firmware version:\n" << registers["FirmwareVers"].toStdString() << "\n\n"
       << " FEC IP:\n" << registers["FECip"].toStdString() <<  "\n\n"
       << " DAQ destination IP:\n" << registers["DAQip"].toStdString()  <<  "\n"
       << "**********************";
    cout << sx.str() << endl;
    QString message = QString::fromStdString(sx.str());
    m_ui->debugScreen->insertPlainText(message);
    m_ui->debugScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
}


void FECWindow::on_pushButtonFECIP_pressed()
{
    int FECip = 0x0a000000;
    bool ok;
    QString result = QString::number(QInputDialog::getInt(this,"Set FEC IP Adress","10.0.0.X",1,1,4,1,&ok));

    if (ok && !result.isEmpty())
    {
        FECip = FECip + result.toInt();

        int FECip = 0x0a000002;
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->writeFECip(FECip);
        usleep(1000);
        m_ui->ip4_2->setText(result);
    }
}



void FECWindow::on_lineEdit_triggerOffset_editingFinished()
{

    long val = m_ui->lineEdit_triggerOffset->text().toInt();
    if(val < 0)
    {
        val = -val;
    }
    if(val > 31*4096*BC_period)
    {
        val = 31*4096*BC_period;
    }

    long div = val/BC_period;
    long mod = val%BC_period;
    if(mod !=0)
    {
        val = abs(div*BC_period);

    }
    m_ui->lineEdit_triggerOffset->setText( QString::number( val, 10 ) );

    long offset = val/(4096*BC_period);
    long bcid = val%(4096*BC_period);
    bcid = bcid/BC_period;
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("time_offset_triggerperiod",(unsigned long)offset);
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("time_offset_BCID",(unsigned long)bcid);

}

void FECWindow::on_lineEdit_triggerWindow_editingFinished()
{
    long val = m_ui->lineEdit_triggerWindow->text().toLong();
    if(val < 0)
    {
        val = -val;
    }
    if(val > 4095*BC_period)
    {
        val = 4095*BC_period;
    }

    long div = val/BC_period;
    long mod = val%BC_period;
    if(mod !=0)
    {
        val = div*BC_period;

    }
    m_ui->lineEdit_triggerWindow->setText( QString::number( val, 10 ) );
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("time_window_BCID",(unsigned long)div);
}

void FECWindow::on_checkBox_TriggeredMode_clicked()
{
    if(m_ui->checkBox_TriggeredMode->isChecked())
    {
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("triggered_mode",(unsigned long)1);
    }
    else
    {
        m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("triggered_mode",(unsigned long)0);
    }
}




void FECWindow::on_pushButtonDAQIP_pressed()
{
    int DAQip = 0xC0A80003;
    //DAQip = DAQip + result.toInt();
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->writeDAQip(DAQip);
    usleep(1000);
    //m_ui->ip4_2->setText(result);

}


void FECWindow::on_lineEdit_triggerPulseDelay_editingFinished()
{
    const int internalClockPeriod = 25;
    long val = m_ui->lineEdit_triggerPulseDelay->text().toLong();
    if(val < 0)
    {
        val = -val;
    }
    if(val > 255*internalClockPeriod)
    {
        val = 255*internalClockPeriod;
    }

    long div = val/internalClockPeriod;
    long mod = val%internalClockPeriod;
    if(mod !=0)
    {
        val = div*internalClockPeriod;

    }
    m_ui->lineEdit_triggerPulseDelay->setText( QString::number( val, 10 ) );
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg("trigger_pulse_delay",(unsigned long)div);
}
