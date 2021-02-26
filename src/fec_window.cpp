#include "fec_window.h"
#include "hybrid_window.h"


FECWindow::FECWindow(DAQWindow *top, unsigned short fec, QWidget *parent) :
    QWidget(parent),
    m_daqWindow{top},
    m_fecIndex{fec},
    m_ui(new Ui::fec_window)
{
    internalClockPeriod = 25;
    m_ui->setupUi(this);
    m_ui->trgout_time->clear();
    m_ui->trgout_time->addItem("trg in");
    for(int i=0; i<4096; i++) {
        m_ui->trgout_time->addItem( QString::number(i));
    }
    UpdateWindow();
    LoadSettings();
    //    this->setStyleSheet("QWidget {background: 'white';}");
    m_ui->linkPB->setEnabled(false);
    m_ui->readSystemParams->setEnabled(false);

    m_ui->fec_WarmInit->setEnabled(false);
    m_ui->onACQ->setEnabled(false);
    m_ui->offACQ->setEnabled(false);

    m_ui->debugScreen->setReadOnly(true);

    SetToolTips();

    connect(m_ui->latency_reset, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->latency_data_max, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->latency_data_error, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->tp_latency, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->tp_offset_first, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->tp_offset, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->tp_number, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->debug_data_format, SIGNAL(stateChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->linkPB, SIGNAL(clicked()),
            this, SLOT(onCheckLinkStatus()));

    connect(m_ui->trgin_invert, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->trgout_invert, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->trgout_time, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));

    connect(m_daqWindow->ui->openConnection, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->fec_WarmInit, SIGNAL(clicked()),
            this, SLOT( onResetFEC() ));
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

// ------------------------------------------------------------------------- //
void FECWindow::SetToolTips()
{
    //////////////////////////////////////////////////////////////////////////
    // -------------------------------------------------------------------- //
    // Creates all the ToolTips for Push bottons etc.
    // -------------------------------------------------------------------- //
    //////////////////////////////////////////////////////////////////////////



    m_ui->latency_reset->setToolTip("At the start of the acquisition, the VMM BCID is reset.\nAdjust the reset latency so that the BCID of a VMM hit is identical to the trigger counter on the FEC (enable debug data format, pulse only one channel)");
    m_ui->label_latency_reset->setToolTip("At the start of the acquisition, the VMM BCID is reset.\nAdjust the reset latency so that the BCID of a VMM hit is identical to the trigger counter on the FEC (enable debug data format, pulse only one channel)");
    m_ui->latency_data_max->setToolTip("Maximum latency of VMM data to arrive at FEC.\nData that arrives on FEC after the maximum latency period will be marked as error data (offset 16)");
    m_ui->label_latency_data_max->setToolTip("Maximum latency of VMM data to arrive at FEC.\nData that arrives on FEC after the maximum latency period will be marked as error data (offset 16)");
    m_ui->latency_data_error->setToolTip("Maximum allowed fluctuation of arrival time of VMM data on FEC in 40 mHz clock cycles.\nData that arrives on FEC earlier than the latency error will be marked as error data (offset 16)");
    m_ui->label_latency_data_error->setToolTip("Maximum allowed fluctuation of arrival time of VMM data on FEC in 40 mHz clock cycles.\nData that arrives on FEC earlier than the latency error will be marked as error data (offset 16)");
    m_ui->tp_number->setToolTip("Number of test pulses from VMM internal pulser within a BCID cycle (0-4095)");
    m_ui->label_tp_number->setToolTip("Number of test pulses from VMM internal pulser within a BCID cycle (0-4095)");
    m_ui->tp_offset_first->setToolTip("Desired BCID of the first test pulse of the VMM internal pulser");
    m_ui->label_tp_offset_first->setToolTip("Desired BCID of the first test pulse of the VMM internal pulser");
    m_ui->tp_offset->setToolTip("BCID offset for subsequent pulses from VMM internal pulser");
    m_ui->label_tp_offset->setToolTip("BCID offset for subsequent pulses from VMM internal pulser");
    m_ui->tp_latency->setToolTip("Latency in 40 MHz clock cycles for the VMM pulser.\nAdjust the value so that the BCID of the VMM hits are identical to the offset of the first test pulse.");
    m_ui->label_tp_latency->setToolTip("Latency in 40 MHz clock cycles for the VMM pulser.\nAdjust the value so that the BCID of the VMM hits are identical to the offset of the first test pulse.");
    m_ui->debug_data_format->setToolTip("Enable the debug data format.\nThe debug data format shows the trigger counter (FEC counter counting 40 MHz clock cycles) at which the hits from the VMM arrive.");
    m_ui->trgin_invert->setToolTip("Setting (off/normal/inverted) for the NIM trigger input of the FEC. Inverted means that the signal will be low when the input is high.\nIf the setting normal or inverted, the trigger timestamp is appearing in the data as marker of VMM 31.");
    m_ui->trgout_invert->setToolTip("Setting (off/normal/inverted) for the NIM trigger output of the FEC. Inverted only works if the trigger input is chosen as signal.\nIf the setting normal or inverted, the trigger timestamp is appearing in the data as marker of VMM 31.");
    m_ui->trgout_time->setToolTip("Trigger output can either give out the signal from the NIM trigger input, or occur for 25 ns at the chosen BCID.");


}

void FECWindow::onACQHandler(){
    if(m_daqWindow->m_sendstate == "GlobalACQon" ){
        emit m_ui->offACQ->clicked();
        m_ui->onACQ->setChecked(false);
        m_ui->offACQ->setChecked(false);

        m_ui->onACQ->setEnabled(false);
        m_ui->offACQ->setEnabled(false);
    }
    else if(m_daqWindow->m_sendstate == "GlobalACQoff" ){

        m_ui->onACQ->setEnabled(true);
        m_ui->offACQ->setEnabled(true);
    }
}


void FECWindow::onUpdateSettings(){
   if(QObject::sender() == m_ui->tp_offset_first){
       if(m_ui->tp_offset_first->value() + (m_ui->tp_number->value()-1)*m_ui->tp_offset->value() <= 4095) {
            SetFec("tp_offset_first",  m_ui->tp_offset_first->value() );
        }
       else {
            m_ui->tp_offset_first->setValue(GetFec("tp_offset_first"));
       }
    }
    else if(QObject::sender() == m_ui->tp_offset){
        if(m_ui->tp_offset_first->value() + (m_ui->tp_number->value()-1)*m_ui->tp_offset->value() <= 4095) {
            SetFec("tp_offset",  m_ui->tp_offset->value() );
        }
        else {
             m_ui->tp_offset->setValue(GetFec("tp_offset"));
        }
    }
    else if(QObject::sender() == m_ui->tp_number){
       if(m_ui->tp_offset_first->value() + (m_ui->tp_number->value()-1)*m_ui->tp_offset->value() <= 4095) {
            SetFec("tp_number",  m_ui->tp_number->value() );
            if(GetFec( "tp_number" ) == 1) {
                m_ui->tp_offset->setEnabled(false);
            }
            else {
               m_ui->tp_offset->setEnabled(true);
            }
       }
       else {
            m_ui->tp_number->setValue(GetFec("tp_number"));
       }

    }
    else if(QObject::sender() == m_ui->tp_latency){
         SetFec("tp_latency",  m_ui->tp_latency->value() );
     }
    else if(QObject::sender() == m_ui->latency_reset){
        SetFec("latency_reset",  m_ui->latency_reset->value() );
    }
    else if(QObject::sender() == m_ui->latency_data_max){
         SetFec("latency_data_max",  m_ui->latency_data_max->value() );
    }
    else if(QObject::sender() == m_ui->latency_data_error){
         SetFec("latency_data_error",  m_ui->latency_data_error->value() );
    }
    else if(QObject::sender() == m_ui->debug_data_format){
        SetFec("debug_data_format",  m_ui->debug_data_format->isChecked() );
    }

   else if(QObject::sender() == m_ui->trgin_invert){
        SetFec("trgin_invert",  m_ui->trgin_invert->currentIndex() );
        if(GetFec( "trgin_invert" ) == 0) {
            SetFec("register_trigger_timestamp", 0);
        }
        else {
            SetFec("register_trigger_timestamp", 1);
        }

    }
   else if(QObject::sender() == m_ui->trgout_invert){
       SetFec("trgout_invert",  m_ui->trgout_invert->currentIndex() );
       if(GetFec( "trgout_invert" ) == 0) {
           m_ui->trgout_time->setEnabled(false);
       }
       else {
          m_ui->trgout_time->setEnabled(true);
       }
   }
   else if(QObject::sender() == m_ui->trgout_time){
        SetFec("trgout_time",  m_ui->trgout_time->currentIndex() );
   }
   else if(QObject::sender() == m_daqWindow->ui->openConnection){
        if(m_daqWindow->ui->connectionLabel->text()==QString("all alive")){
            m_ui->linkPB->setEnabled(true);
            m_ui->readSystemParams->setEnabled(true);
            m_ui->fec_WarmInit->setEnabled(true);
            m_ui->onACQ->setEnabled(true);
            m_ui->offACQ->setEnabled(true);
        }
        else{
            //m_ui->linkPB->setEnabled(false);
            m_ui->fec_WarmInit->setEnabled(false);
            m_ui->readSystemParams->setEnabled(false);
            m_ui->onACQ->setEnabled(false);
            m_ui->offACQ->setEnabled(false);
        }
    }

    else if(QObject::sender() == m_ui->onACQ){
        m_ui->onACQ->setCheckable(true);
        m_ui->onACQ->setChecked(true);
        m_ui->offACQ->setChecked(false);
        m_daqWindow->ui->Send->setEnabled(false);
        m_daqWindow->m_daq.SendAll(true);
        m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQon();
    }
    else if(QObject::sender() == m_ui->offACQ){
        m_ui->offACQ->setCheckable(true);
        m_ui->offACQ->setChecked(true);
        m_ui->onACQ->setChecked(false);
        m_daqWindow->ui->Send->setEnabled(true);
        m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
    }




}


void FECWindow::LoadSettings(){
    QHostAddress ipAddress;
    ipAddress.setAddress(GetFec( "ip_fec" ));
    m_ui->ip_fec->setText( ipAddress.toString());
    ipAddress.setAddress(GetFec( "ip_daq" ));
    m_ui->ip_daq->setText( ipAddress.toString());


    if(GetFec( "tp_offset_first" ) + (GetFec( "tp_number" )-1)*GetFec( "tp_offset" )  > 4095) {
         SetFec("tp_number", 1);
         SetFec("tp_offset_first", 100);
         SetFec("tp_offset", 1000);
    }
    m_ui->tp_number->setValue( GetFec( "tp_number" ) );
    m_ui->tp_offset->setValue( GetFec( "tp_offset" ) );
    m_ui->tp_offset_first->setValue( GetFec( "tp_offset_first" ) );

    m_ui->tp_latency->setValue( GetFec( "tp_latency" ) );
    m_ui->latency_reset->setValue( GetFec( "latency_reset" ) );
    m_ui->latency_data_max->setValue( GetFec( "latency_data_max" ) );
    m_ui->latency_data_error->setValue( GetFec( "latency_data_error" ) );

    m_ui->debug_data_format->setChecked( GetFec( "debug_data_format" ) );
    if(GetFec( "tp_number" ) <= 1) {
        m_ui->tp_offset->setEnabled(false);
    }
    else {
       m_ui->tp_offset->setEnabled(true);
    }

    m_ui->trgin_invert->setCurrentIndex( GetFec( "trgin_invert" ) );
    if(GetFec( "trgin_invert" ) == 0) {
        SetFec("register_trigger_timestamp", 0);
    }
    else {
        SetFec("register_trigger_timestamp", 1);
    }

    m_ui->trgout_invert->setCurrentIndex( GetFec( "trgout_invert" ) );
    if(GetFec( "trgout_invert" ) == 0) {
        m_ui->trgout_time->setEnabled(false);
    }
    else {
        m_ui->trgout_time->setEnabled(true);
    }
    m_ui->trgout_time->setCurrentIndex( GetFec( "trgout_time" ) );

}

bool FECWindow::SetFec(const char *feature, unsigned long val){
    return m_daqWindow->m_daq.m_fecs[m_fecIndex].SetReg(feature,  (unsigned long) val );
}
unsigned long FECWindow::GetFec(const char *feature){
    return m_daqWindow->m_daq.m_fecs[m_fecIndex].GetRegVal(feature);
}

void FECWindow::on_Box_hybrid1_clicked()
{
    if (m_ui->Box_hybrid1->isChecked()){hybridBoxLogic(true,0);}
    else {hybridBoxLogic(false,0);}
}
void FECWindow::on_Box_hybrid2_clicked()
{
    if (m_ui->Box_hybrid2->isChecked()){hybridBoxLogic(true,1);}
    else {hybridBoxLogic(false,1);}
}
void FECWindow::on_Box_hybrid3_clicked()
{
    if (m_ui->Box_hybrid3->isChecked()){hybridBoxLogic(true,2);}
    else {hybridBoxLogic(false,2);}
}
void FECWindow::on_Box_hybrid4_clicked()
{
    if (m_ui->Box_hybrid4->isChecked()){hybridBoxLogic(true,3);}
    else {hybridBoxLogic(false,3);}
}
void FECWindow::on_Box_hybrid5_clicked()
{
    if (m_ui->Box_hybrid5->isChecked()){hybridBoxLogic(true,4);}
    else {hybridBoxLogic(false,4);}
}
void FECWindow::on_Box_hybrid6_clicked()
{
    if (m_ui->Box_hybrid6->isChecked()){hybridBoxLogic(true,5);}
    else {hybridBoxLogic(false,5);}
}
void FECWindow::on_Box_hybrid7_clicked()
{
    if (m_ui->Box_hybrid7->isChecked()){hybridBoxLogic(true,6);}
    else {hybridBoxLogic(false,6);}
}
void FECWindow::on_Box_hybrid8_clicked()
{
    if (m_ui->Box_hybrid8->isChecked()){hybridBoxLogic(true,7);}
    else {hybridBoxLogic(false,7);}
}
void FECWindow::hybridBoxLogic(bool checked, unsigned short hybrid){

    QList<QCheckBox*> a = m_ui->groupBox->findChildren<QCheckBox*>();
    std::sort(a.begin(), a.end(),
              [](const QCheckBox* x, const QCheckBox* y) -> bool { return x->text() <  y->text();
    });

    unsigned short ActiveBefore = 0;
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<hybrid && a.at(i)->isChecked()) ActiveBefore++;
    }
    if (checked){
        m_ui->tabWidget->insertTab(ActiveBefore, new HybridWindow(this,m_fecIndex,hybrid), QString("Hybrid %0").arg(hybrid+1));
        m_ui->tabWidget->setCurrentIndex(ActiveBefore);
        m_daqWindow->m_daq.m_fecs[m_fecIndex].SetHybrid(hybrid, true);
    }
    else {
        m_ui->tabWidget->removeTab(ActiveBefore);
        m_daqWindow->m_daq.m_fecs[m_fecIndex].SetHybrid(hybrid, false);
    }
}


void FECWindow::UpdateWindow(){
    for (unsigned short k=0; k < HYBRIDS_PER_FEC; k++){
        if(m_daqWindow->m_daq.m_fecs[m_fecIndex].GetHybrid(k)){
            if (k == 0 && !m_ui->Box_hybrid1->isChecked()){m_ui->Box_hybrid1->setChecked(true); on_Box_hybrid1_clicked();}
            if (k == 1 && !m_ui->Box_hybrid2->isChecked()){m_ui->Box_hybrid2->setChecked(true); on_Box_hybrid2_clicked();}
            if (k == 2 && !m_ui->Box_hybrid3->isChecked()){m_ui->Box_hybrid3->setChecked(true); on_Box_hybrid3_clicked();}
            if (k == 3 && !m_ui->Box_hybrid4->isChecked()){m_ui->Box_hybrid4->setChecked(true); on_Box_hybrid4_clicked();}
            if (k == 4 && !m_ui->Box_hybrid5->isChecked()){m_ui->Box_hybrid5->setChecked(true); on_Box_hybrid5_clicked();}
            if (k == 5 && !m_ui->Box_hybrid6->isChecked()){m_ui->Box_hybrid6->setChecked(true); on_Box_hybrid6_clicked();}
            if (k == 6 && !m_ui->Box_hybrid7->isChecked()){m_ui->Box_hybrid7->setChecked(true); on_Box_hybrid7_clicked();}
            if (k == 7 && !m_ui->Box_hybrid8->isChecked()){m_ui->Box_hybrid8->setChecked(true); on_Box_hybrid8_clicked();}
        }
        else{
            if (k == 0 && m_ui->Box_hybrid1->isChecked()){m_ui->Box_hybrid1->setChecked(false); on_Box_hybrid1_clicked();}
            if (k == 1 && m_ui->Box_hybrid2->isChecked()){m_ui->Box_hybrid2->setChecked(false); on_Box_hybrid2_clicked();}
            if (k == 2 && m_ui->Box_hybrid3->isChecked()){m_ui->Box_hybrid3->setChecked(false); on_Box_hybrid3_clicked();}
            if (k == 3 && m_ui->Box_hybrid4->isChecked()){m_ui->Box_hybrid4->setChecked(false); on_Box_hybrid4_clicked();}
            if (k == 4 && m_ui->Box_hybrid5->isChecked()){m_ui->Box_hybrid5->setChecked(false); on_Box_hybrid5_clicked();}
            if (k == 5 && m_ui->Box_hybrid6->isChecked()){m_ui->Box_hybrid6->setChecked(false); on_Box_hybrid6_clicked();}
            if (k == 6 && m_ui->Box_hybrid7->isChecked()){m_ui->Box_hybrid7->setChecked(false); on_Box_hybrid7_clicked();}
            if (k == 7 && m_ui->Box_hybrid8->isChecked()){m_ui->Box_hybrid8->setChecked(false); on_Box_hybrid8_clicked();}
        }
    }
}


void FECWindow::onCheckLinkStatus(){
    QString message;
    bool readOK= false;
    m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->CheckLinkStatus(readOK, message);
    if(readOK)
    {
        m_ui->debugScreen->insertPlainText(message);
        m_ui->debugScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    }

}


// ------------------------------------------------------------------------- //
void FECWindow::onResetFEC()
{
    //bool do_reset = (m_ui->fec_reset == QObject::sender() ? true : false);
    m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ResetFEC();
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
    m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ReadSystemRegisters(registers);
    m_daqWindow->m_daq.m_fecs[m_fecIndex].SetFirmwareVersion(registers["FirmwareVers"]);

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
            int res = m_daqWindow->m_daq.CheckIP_FEC(theIP, m_fecIndex);
            if(res > -1) {
                int ret = QMessageBox::warning(this, tr("FEC IPv4 address setting"),
                                               "The last octet of the IP address is the FEC ID, and has to be hence unique.\n"
                                               "Last octet of FEC IP address already in use in FEC " + QString::number(res+1),
                                               QMessageBox::Ok);
                return;
            }
            res = m_daqWindow->m_daq.CheckIP_DAQ(theIP);
            if(res > -1) {
                int ret = QMessageBox::warning(this, tr("FEC IPv4 address setting"),
                                               "FEC IP address already in use as DAQ IP in FEC " + QString::number(res+1),
                                               QMessageBox::Ok);
                return;
            }
            QMessageBox::StandardButton reply;

            reply = QMessageBox::question(this, "FEC IPv4 address setting", "The SRS FEC is connected via ethernet cable to a network card on the slow control PC. The FEC IP address is the address to which the PC sends its commands. The FEC stores its own IP address on the FEC EEPROM.\nATTENTION: Do you only want to change the FEC IP address in the slow control GUI (press NO), or re-program the FEC IP address in the FEC EEPROM (press YES)?", QMessageBox::Yes | QMessageBox::No );
            if(reply == QMessageBox::Yes) {
               m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->writeFECip(theIP);
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
            int res = m_daqWindow->m_daq.CheckIP_FEC(theIP, -1);
            if(res > -1) {
                int ret = QMessageBox::warning(this, tr("DAQ IPv4 address"),
                                               "DAQ IP address already in use as FEC IP in FEC " + QString::number(res+1),
                                               QMessageBox::Ok);
                return;
            }

            QMessageBox::StandardButton reply;

            reply = QMessageBox::question(this, "DAQ IPv4 address setting", "The SRS FEC is connected via ethernet cable to a network card on the slow control PC. The DAQ IP address is the address of this network card, and the FEC card has to know it to send data to the PC.\nATTENTION: Do you only want to change the DAQ IP address in the slow control GUI (press NO), or re-program the DAQ IP address in the FEC EEPROM (press YES)?", QMessageBox::Yes | QMessageBox::No );
            if(reply == QMessageBox::Yes) {
                m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->writeDAQip(theIP);
            }
            QThread::usleep(1000);
            SetFec("ip_daq", theIP);
            m_ui->ip_daq->setText(result);
            QThread::usleep(1000);
        }
     }
}

