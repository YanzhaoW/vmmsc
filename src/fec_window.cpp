#include "fec_window.h"
#include "hdmi_window.h" // has NOT to be included here. If included in header file: compiler error in mainwindow.h:61:9: error: ‘daq_window’ does not name a type


FECWindow::FECWindow(DAQWindow *top, unsigned short fec, QWidget *parent) :
    QWidget(parent),
    m_daqWindow{top},
    m_fecIndex{fec},
    m_ui(new Ui::fec_window)
{
    internalClockPeriod = 25;
    m_ui->setupUi(this);
    UpdateWindow();
    LoadSettings();
    //    this->setStyleSheet("QWidget {background: 'white';}");
    m_ui->linkPB->setEnabled(false);
    m_ui->readSystemParams->setEnabled(false);
    m_ui->fec_WarmInit->setEnabled(false);
    //m_ui->fec_reset->setEnabled(false);
    m_ui->trgPulser->setEnabled(false);
    m_ui->trgExternal->setEnabled(false);
    m_ui->onACQ->setEnabled(false);
    m_ui->offACQ->setEnabled(false);

    m_ui->debugScreen->setReadOnly(true);
    m_ui->OpenWrFifo->setChecked(false); SetFec("open_fec_wr_fifo_outside_acq_win",  m_ui->OpenWrFifo->isChecked() );



    //    connect(ui->setEvbld, SIGNAL(pressed()),
    //                                            this, SLOT(updateSettings()));
//    connect(m_ui->evbld_mode, SIGNAL(currentIndexChanged(int)),
//            this, SLOT(onUpdateSettings()));
//    connect(m_ui->evbld_infodata, SIGNAL(currentIndexChanged(int)),
//            this, SLOT(onUpdateSettings()));
//    connect(m_ui->timeStampResCheckBox, SIGNAL(stateChanged(int)),
//            this, SLOT(onUpdateSettings()));
    connect(m_ui->readoutCycle, SIGNAL(editingFinished()),
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
    connect(m_ui->AccWin, SIGNAL(stateChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->OpenWrFifo, SIGNAL(stateChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->Stamp_ext_trg, SIGNAL(stateChanged(int)),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->linkPB, SIGNAL(clicked()),
            this, SLOT(onCheckLinkStatus()));

    // connect(m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule, SIGNAL(CheckLinks()),
    //        this, SLOT(onWriteFECStatus()));

    connect(m_daqWindow->ui->openConnection, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->fec_WarmInit, SIGNAL(clicked()),
            this, SLOT( onResetFEC() ));
    //connect(m_ui->fec_reset, SIGNAL(clicked()),
    //        this, SLOT( onResetFEC() ));
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
     if(QObject::sender() == m_ui->pulserDelay){
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
    else if(QObject::sender() == m_ui->AccWin){
        SetFec("acceptance_window",  m_ui->AccWin->isChecked() );
    }
    else if(QObject::sender() == m_ui->OpenWrFifo){
        SetFec("open_fec_wr_fifo_outside_acq_win",  m_ui->OpenWrFifo->isChecked() );
    }
    else if(QObject::sender() == m_ui->Stamp_ext_trg){
        SetFec("ts_ext_trg",  m_ui->Stamp_ext_trg->isChecked() );
    }

    else if(QObject::sender() == m_daqWindow->ui->openConnection){
        if(m_daqWindow->ui->connectionLabel->text()==QString("all alive")){
            m_ui->linkPB->setEnabled(true);
            m_ui->readSystemParams->setEnabled(true);
            m_ui->fec_WarmInit->setEnabled(true);
            if(!m_daqWindow->ui->checkBoxGlobalDAQ->isChecked()){
                //m_ui->fec_reset->setEnabled(true);
                m_ui->trgPulser->setEnabled(true);
                m_ui->trgExternal->setEnabled(true);
                m_ui->onACQ->setEnabled(true);
                m_ui->offACQ->setEnabled(true);
            }
        }
        else{
            m_ui->linkPB->setEnabled(false);
            m_ui->fec_WarmInit->setEnabled(false);
            m_ui->readSystemParams->setEnabled(false);
            m_ui->trgPulser->setEnabled(false);
            m_ui->trgExternal->setEnabled(false);
            m_ui->onACQ->setEnabled(false);
            m_ui->offACQ->setEnabled(false);
        }
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
            m_daqWindow->SetWarningMessage("Select Trigger Mode","red");
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
    QHostAddress ipAddress;
    ipAddress.setAddress(GetFec( "ip_fec" ));
    m_ui->ip_fec->setText( ipAddress.toString());
    ipAddress.setAddress(GetFec( "ip_daq" ));
    m_ui->ip_daq->setText( ipAddress.toString());

    m_ui->pulserDelay->setValue( GetFec( "tp_delay" ) );
    m_ui->readoutCycle->setText( QString::number( GetFec( "readout_cycle" ), 16 ) );

    m_ui->bcid_reset->setValue( GetFec( "bcid_reset" ) );
    m_ui->acqSync->setValue( GetFec( "acq_sync" ) );
    m_ui->acqWindow->setValue( GetFec( "acq_window" ) );
    m_ui->ClearS6FIFI->setChecked( GetFec( "clear_S6_fifo" ) );
    m_ui->AccWin->setChecked( GetFec( "acceptance_window" ) );
    m_ui->OpenWrFifo->setChecked( GetFec( "open_fec_wr_fifo_outside_acq_win" ) );

    m_ui->Stamp_ext_trg->setChecked( GetFec( "ts_ext_trg" ) );

}

bool FECWindow::SetFec(const char *feature, unsigned long val){
    return m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].SetReg(feature,  (unsigned long) val );
}
unsigned long FECWindow::GetFec(const char *feature){
    return m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetRegVal(feature);
}

void FECWindow::on_Box_hdmi1_clicked()
{
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
    //bool do_reset = (m_ui->fec_reset == QObject::sender() ? true : false);
    m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->ResetFEC();
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
    long theIP = 0x0a000002;
    bool ok;
    theIP = GetFec("ip_fec");
    QHostAddress ipAddress;
    ipAddress.setAddress(theIP);
    QString result = QInputDialog::getText(this, tr("FEC IPv4 address"), tr("New IPv4 address:"), QLineEdit::Normal, ipAddress.toString(), &ok);

    if (ok && !result.isEmpty())
    {
        if (!ipAddress.setAddress(result)){
            int ret = QMessageBox::warning(this, tr("FEC IPv4 address setting"),
                                           tr("Invalid IPv4 address!"),
                                           QMessageBox::Ok);
        }
        else {
            theIP = ipAddress.toIPv4Address();
            int res = m_daqWindow->m_mainWindow->m_daqs[0].CheckIP_FEC(theIP, m_fecIndex);
            if(res > -1) {
                int ret = QMessageBox::warning(this, tr("FEC IPv4 address setting"),
                                               "The last octet of the IP address is the FEC ID, and has to be hence unique.\n"
                                               "Last octet of FEC IP address already in use in FEC " + QString::number(res+1),
                                               QMessageBox::Ok);
                return;
            }
            res = m_daqWindow->m_mainWindow->m_daqs[0].CheckIP_DAQ(theIP);
            if(res > -1) {
                int ret = QMessageBox::warning(this, tr("FEC IPv4 address setting"),
                                               "FEC IP address already in use as DAQ IP in FEC " + QString::number(res+1),
                                               QMessageBox::Ok);
                return;
            }
            QMessageBox::StandardButton reply;

            reply = QMessageBox::question(this, "FEC IPv4 address setting", "Do you want to also program the FEC with this IP address?", QMessageBox::Yes | QMessageBox::No );
            if(reply == QMessageBox::Yes) {
               m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->writeFECip(theIP);
            }
            QThread::usleep(1000);
            SetFec("ip_fec", theIP);
            m_ui->ip_fec->setText(result);
            QThread::usleep(1000);
        }
     }
}


void FECWindow::on_pushButtonDAQIP_pressed()
{
    long theIP = 0x0a000003;
    bool ok;
    theIP = GetFec("ip_daq");
    QHostAddress ipAddress;
    ipAddress.setAddress(theIP);

    QString result = QInputDialog::getText(this, tr("DAQ IPv4 address"), tr("New IPv4 address:"), QLineEdit::Normal,  ipAddress.toString(), &ok);

    if (ok && !result.isEmpty())
    {
        if (!ipAddress.setAddress(result)){
            int ret = QMessageBox::warning(this, tr("DAQ IPv4 address"),
                                           tr("Invalid IPv4 address!"),
                                           QMessageBox::Ok);
        }
        else {
            theIP = ipAddress.toIPv4Address();
            int res = m_daqWindow->m_mainWindow->m_daqs[0].CheckIP_FEC(theIP, -1);
            if(res > -1) {
                int ret = QMessageBox::warning(this, tr("DAQ IPv4 address"),
                                               "DAQ IP address already in use as FEC IP in FEC " + QString::number(res+1),
                                               QMessageBox::Ok);
                return;
            }

            QMessageBox::StandardButton reply;

            reply = QMessageBox::question(this, "DAQ IPv4 address setting", "Do you want to also program the FEC with this DAQ IP address?", QMessageBox::Yes | QMessageBox::No );
            if(reply == QMessageBox::Yes) {
                m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->writeDAQip(theIP);
            }
            QThread::usleep(1000);
            SetFec("ip_daq", theIP);
            m_ui->ip_daq->setText(result);
            QThread::usleep(1000);
        }
     }
}

