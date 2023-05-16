#include "daq_window.h"
//#include "globparameter.h"
// std/stl
#include <iostream>
#include <sstream>



using namespace std;

std::vector<int> g_hybrid_i2c;
int g_clock_source = CLOCK_SOURCE;
bool g_use_config_check = true;


#if CLOCK_SOURCE == 1
double g_clock_period = 22.5;
std::string g_card_name = "FEN";
//Slow control assister
int g_slow_control = 1;
#elif CLOCK_SOURCE == 2
double g_clock_period = 22.5;
std::string g_card_name = "FEC";
//Slow control SRS
int g_slow_control = 2;
#elif CLOCK_SOURCE == 3
double g_clock_period = 25;
std::string g_card_name = "FEC";
//Slow control SRS
int g_slow_control = 2;
#else
double g_clock_period = 22.7137;
std::string g_card_name = "FEN";
int g_slow_control = SLOW_CONTROL;
#endif


double g_time_factor = g_clock_period/8.0;

bool g_connection_ok = false;

DAQWindow::DAQWindow(QMainWindow *parent) :
    QMainWindow(parent),
    m_ui(new Ui::daq_window)
{
    m_ui->setupUi(this);
    this->setWindowTitle("VMM3a slow control");
    m_ui->comboBoxSlowControl->setCurrentIndex(g_slow_control);
    //Master (0) has only fec IP field
    if(g_slow_control == 0) {
        m_ui->pushButtonDAQIP->setText("Board-ID");
    }
    else {
        m_ui->pushButtonDAQIP->setText("DAQ IP");
    }
    if(g_slow_control == 2) {
        m_ui->pushButtonFECIP->setText("FEC IP");
    }
    else if(g_slow_control == 1) {
        m_ui->pushButtonFECIP->setText("FEN IP");
    }
    else if(g_slow_control == 0) {
        m_ui->pushButtonFECIP->setText("Master");
    }


    m_ui->openConnection->setStyleSheet("QPushButton { color: black; }");
    m_ui->openConnection->setToolTip("Open or check communication");
    m_ui->selectDir->setToolTip("Opens file browser to select config file");
    m_ui->Button_load->setToolTip("Loading config file");
    m_ui->Button_save->setToolTip("Saving settings into config file");
    m_ui->treeWidgetSystem->clear();
    m_ui->treeWidgetSystem->setColumnCount(2);
    m_ui->treeWidgetSystem->setHeaderLabels(QStringList() << "Item" << "Description");
    m_ui->treeWidgetSystem->sortByColumn(0, Qt::AscendingOrder);
    m_ui->treeWidgetSystem->setSortingEnabled(true);

    QString card = QString::fromStdString(g_card_name)+"s";
    m_ui->Fec_group_box->setTitle(card);
    m_ui->groupBox_acq->setTitle("ACQ (all " + card +")");
    m_ui->comboBoxClockSource->setCurrentIndex(g_clock_source);
    if(g_use_config_check == true) {
        m_ui->checkBoxConfigCheck->setChecked(true);
    }
    else {
        m_ui->checkBoxConfigCheck->setChecked(false);
    }
    m_daqConfig = new DAQConfig(this);

    m_ui->tableWidget_LinkStatus->setRowCount(HYBRIDS_PER_FEC);
    m_ui->tableWidget_LinkStatus->setColumnCount(FECS_PER_DAQ);
    m_ui->tableWidget_LinkStatus->setVerticalHeaderLabels({"H0","H1","H2","H3", "H4", "H5", "H6", "H7"});
    m_ui->tableWidget_LinkStatus->setHorizontalHeaderLabels({"0","1","2","3","4","5","6","7"});
    for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
        m_ui->tableWidget_LinkStatus->setRowHeight(hyb,20);
        for(int fec=0; fec<FECS_PER_DAQ; fec++) {
            m_ui->tableWidget_LinkStatus->setItem(hyb, fec, new QTableWidgetItem);
            m_ui->tableWidget_LinkStatus->setColumnWidth(fec,33);
        }
    }


    m_ui->tableWidget_Temperature->setRowCount(HYBRIDS_PER_FEC*2);
    m_ui->tableWidget_Temperature->setColumnCount(FECS_PER_DAQ);
    m_ui->tableWidget_Temperature->setVerticalHeaderLabels({"V0","V1","V2","V3","V4","V5","V6","V7","V8","V9","V10","V11","V12","V13","V14","V15"});
    m_ui->tableWidget_Temperature->setHorizontalHeaderLabels({"0","1","2","3","4","5","6","7"});
    for(int vmm=0; vmm<HYBRIDS_PER_FEC*2; vmm++) {
        m_ui->tableWidget_Temperature->setRowHeight(vmm,20);
        for(int fec=0; fec<FECS_PER_DAQ; fec++) {
            m_ui->tableWidget_Temperature->setItem(vmm, fec, new QTableWidgetItem);
            m_ui->tableWidget_Temperature->setColumnWidth(fec,33);
        }
    }
    m_ui->trgout_time->clear();
    m_ui->trgout_time->addItem("trg in");
    for(int i=0; i<4096; i++) {
        m_ui->trgout_time->addItem( QString::number(i));
    }
    if(g_clock_source < 2) {
        m_ui->trgin_invert->setVisible(false);
        m_ui->trgout_invert->setVisible(false);
        m_ui->trgout_time->setVisible(false);
    }
    else {
        m_ui->trgin_invert->setVisible(true);
        m_ui->trgout_invert->setVisible(true);
        m_ui->trgout_time->setVisible(true);
    }

    m_calib = new CalibrationModule(this);
    InitCalibWidgets();
    InitDAQWidgets();
    InitFecWidgets();
    InitHybridWidgets();
    InitVMMWidgets();
    InitVMMChannelWidgets();

    EnableDAQCommunicationButtons(false);
    EnableFECCommunicationButtons(false);
    EnableHybridCommunicationButtons(false);
    EnableVMMCommunicationButtons(false);

    QString fileName = QCoreApplication::applicationDirPath();
    m_execPath = fileName.replace("/vmmdcs.app/Contents/MacOS", "");
    std::string execPath = m_execPath.toStdString();
    if(execPath.find("/debug") !=std::string::npos || execPath.find("/release") !=std::string::npos) {
        m_execPath += "/..";
    }
    QString correctedFileName = this->GetApplicationPath() +  "/../configs/default.json";
    if(FileExists(correctedFileName.toStdString().c_str())){
        LoadConfig("default.json");
    }

    UpdateSystemStatus();
    m_ui->comboBoxI2CSetting->setCurrentIndex(4);
}



void DAQWindow::UpdateSystemStatus() {
    for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
        SetStatus("",fec);
        for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
            m_ui->tableWidget_LinkStatus->item(hyb,fec)->setBackground(Qt::lightGray);
            m_ui->tableWidget_LinkStatus->item(hyb, fec)->setText("");
            m_ui->tableWidget_Temperature->item(hyb*2,fec)->setBackground(Qt::lightGray);
            m_ui->tableWidget_Temperature->item(hyb*2+1,fec)->setBackground(Qt::lightGray);
            m_ui->tableWidget_Temperature->item(hyb*2, fec)->setText("");
            m_ui->tableWidget_Temperature->item(hyb*2+1, fec)->setText("");
        }
        if (this->m_daq.GetFEC(fec)){
            this->m_daq.m_fecs[fec].m_fecConfigModule->SetDebugMode(m_dbg);
            QString text;
            if(g_card_name == "FEN") {
                text = QString::fromStdString(g_card_name) + QString::number(fec) + " "
                        + QString::number(m_map_id_ring_fen[fec].first).rightJustified(2, '0') + "_"
                        + QString::number(m_map_id_ring_fen[fec].second).rightJustified(2, '0');
            }
            else {
                text = QString::fromStdString(g_card_name) + QString::number(fec);
            }
            SetStatus(text,fec);
            for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                if(this->m_daq.m_fecs[fec].GetHybrid(hyb)) {
                    m_ui->tableWidget_LinkStatus->item(hyb, fec)->setText("0");
                    m_ui->tableWidget_LinkStatus->item(hyb,fec)->setBackground(Qt::red);
                    m_ui->tableWidget_Temperature->item(hyb*2, fec)->setText("00");
                    m_ui->tableWidget_Temperature->item(hyb*2+1, fec)->setText("00");
                    m_ui->tableWidget_Temperature->item(hyb*2,fec)->setBackground(Qt::white);
                    m_ui->tableWidget_Temperature->item(hyb*2+1,fec)->setBackground(Qt::white);
                }
            }
        }
    }
}


void DAQWindow::SetStatus(QString text, int n) {
    switch(n) {
    case 0  :
        m_ui->label_status0->setText(text);
        break;
    case 1  :
        m_ui->label_status1->setText(text);
        break;
    case 2  :
        m_ui->label_status2->setText(text);
        break;
    case 3  :
        m_ui->label_status3->setText(text);
        break;
    case 4  :
        m_ui->label_status4->setText(text);
        break;
    case 5  :
        m_ui->label_status5->setText(text);
        break;
    case 6  :
        m_ui->label_status6->setText(text);
        break;
    case 7  :
        m_ui->label_status7->setText(text);
        break;
    default :
        m_ui->label_status0->setText(text);
    }

}

void DAQWindow::EnableDAQCommunicationButtons(bool enable) {
    m_ui->onACQ->setEnabled(enable);
    m_ui->offACQ->setEnabled(enable);
    m_ui->Send->setEnabled(enable);
    m_ui->pushButtonResetLatencyCalibration->setEnabled(enable);
    m_ui->pushButtonTPLatencyCalibration->setEnabled(enable);
}


bool DAQWindow::FileExists(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}


DAQWindow::~DAQWindow()
{
    delete m_ui;
}

void DAQWindow::LoadConfig(QString text){
    std::string fname = text.toStdString();
    std::string filename = fname;
    if (fname == "") {
        std::cout << "No file name specified" << std::endl;
        m_ui->line_configFile->insert("ERROR: no file name given");
    }
    else {
        if(!text.contains(".json")) {
            fname+=".json";
        }
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            m_ui->treeWidgetSystem->clear();
            this->m_daq.SetFEC(j,false);
        }

        bool found = this->m_daqConfig->LoadDAQConf(fname.c_str());
        if (!found){
            std::cout << "File not found" << std::endl;
            m_ui->line_configFile->insert("ERROR: not found");
        }

        else {
            m_map_id_ring_fen.clear();
            m_map_ring_fen_id.clear();
            for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
                if (this->m_daq.GetFEC(fec)){
                    QTreeWidgetItem *cardItem = new QTreeWidgetItem();
                    if(g_card_name == "FEN") {
                        int ring = m_daq.m_fecs[fec].GetRegVal("ring");
                        int fen = m_daq.m_fecs[fec].GetRegVal("fen");
                        m_map_id_ring_fen[fec] = qMakePair(ring, fen);
                        m_map_ring_fen_id[qMakePair(ring,fen)] = fec;

                        QString node =  QString::fromStdString(g_card_name)+" " + QString::number(ring).rightJustified(2, '0')  + "_" + QString::number(fen).rightJustified(2, '0');
                        cardItem->setText(0,node);
                    }
                    else {
                        cardItem->setText(0,QString::fromStdString(g_card_name)+" " + QString::number(fec));
                    }
                    QString description = QString::fromStdString(m_daq.m_fecs[fec].GetInfo("description"));
                    cardItem->setText(1,description);
                    m_ui->treeWidgetSystem->addTopLevelItem(cardItem);
                    for (unsigned short hyb=0; hyb < HYBRIDS_PER_FEC; hyb++){
                        if(this->m_daq.m_fecs[fec].GetHybrid(hyb)){
                            QTreeWidgetItem *hybridItem = new QTreeWidgetItem();
                            hybridItem->setText(0,"Hybrid "+QString::number(hyb));
                            QString description = QString::fromStdString(m_daq.m_fecs[fec].m_hybrids[hyb].GetInfo("description"));
                            hybridItem->setText(1,description);
                            cardItem->addChild(hybridItem);
                            for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                                QTreeWidgetItem *vmmItem = new QTreeWidgetItem();
                                vmmItem->setText(0,"VMM "+QString::number(vmm));
                                QString description = QString::fromStdString(m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].GetInfo("description"));
                                vmmItem->setText(1,description);
                                hybridItem->addChild(vmmItem);
                                std::cout << "vmm " << vmm << " on hybrid "<< hyb << " on fen " << fec << " is active" << std::endl;
                            }
                        }
                    }
                }
            }
            LoadFECSettings();
            LoadHybridSettings();
            LoadVMMSettings();
            LoadVMMChannelSettings();
            UpdateSystemStatus();
            m_ui->tabWidgetTree->setCurrentIndex(0);
            QTreeWidgetItemIterator it( m_ui->treeWidgetSystem);
            while (*it) {
                m_ui->treeWidgetSystem->setCurrentItem((*it));
                break;
            }
        } //else file found
    } //end else not ""
}


void DAQWindow::MeasureVMMI2C() {
    for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
        if (this->m_daq.GetFEC(fec)){
            for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                if(this->m_daq.m_fecs[fec].GetHybrid(hyb)) {
                    for(int vmm=0; vmm<2;vmm++) {
                        int adc_result = 0;

                        if(m_ui->comboBoxI2CSetting->currentIndex() >= 5 && m_ui->cbMeasureThreshold->isChecked()){
                            m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].SetRegi("smx",1, m_ui->comboBoxI2CSetting->currentIndex()-5);
                            if(!m_ui->onACQ->isChecked()) {
                                m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hyb, vmm);
                            }
                            int threshold = m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hyb, vmm, 2);


                            m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].SetRegi("smx",0,m_ui->comboBoxI2CSetting->currentIndex()-5);
                            if(!m_ui->onACQ->isChecked()) {
                                m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hyb, vmm);
                            }
                            adc_result = m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hyb, vmm, 2);
                            adc_result = threshold - adc_result;
                        }
                        //Pulser DAC, Threshold DAC, Band gap, Temperature or just channel without threshold checkbox
                        else {
                            if(!m_ui->onACQ->isChecked()) {
                                m_daq.m_fecs[fec].m_fecConfigModule->ConfigVMM(hyb, vmm);
                            }
                            adc_result = m_daq.m_fecs[fec].m_fecConfigModule->ReadADC(hyb, vmm, 2);
                        }
                        QString text;
                        if(m_ui->comboBoxI2CSetting->currentIndex()==4){
                            double temperature = (725-adc_result)/1.85;
                            text = QString::number(std::round(temperature));
                            if(temperature < -10 || temperature >= 100) {
                                m_ui->tableWidget_Temperature->item(hyb*2+vmm,fec)->setBackground(Qt::blue);
                            }
                            else if(temperature <= 40) {
                                m_ui->tableWidget_Temperature->item(hyb*2+vmm,fec)->setBackground(Qt::green);
                            }
                            else if(temperature <= 60) {
                                m_ui->tableWidget_Temperature->item(hyb*2+vmm,fec)->setBackground(Qt::yellow);
                            }
                            else if(temperature > 60 && temperature <= 70) {
                                m_ui->tableWidget_Temperature->item(hyb*2+vmm,fec)->setBackground(QColor(255,255,0,255));
                            }
                            else if(temperature > 70 && temperature < 100) {
                                m_ui->tableWidget_Temperature->item(hyb*2+vmm,fec)->setBackground(Qt::red);
                            }
                        }
                        else {
                            text = QString::number(adc_result);
                            m_ui->tableWidget_Temperature->item(hyb*2+vmm,fec)->setBackground(Qt::white);
                        }
                        m_ui->tableWidget_Temperature->item(2*hyb+vmm,fec)->setText(text);
                        m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].SetInfo("adc_value",text.toStdString());
                    }
                }
                else {
                    m_ui->tableWidget_Temperature->item(hyb*2,fec)->setBackground(Qt::lightGray);
                    m_ui->tableWidget_Temperature->item(hyb*2+1,fec)->setBackground(Qt::lightGray);
                    m_ui->tableWidget_Temperature->item(2*hyb,fec)->setText("");
                    m_ui->tableWidget_Temperature->item(2*hyb+1,fec)->setText("");
                }
            }
        }
    }
}


void DAQWindow::CheckLinkStatus(int fec, bool readHybridInfo) {
    QString message;
    QVector<QString> linkStatus;
    if(this->m_daq.m_fecs[fec].m_fecConfigModule->CheckLinkStatus(message, linkStatus))
    {
        if(readHybridInfo) {
            g_connection_ok = true;
        }
        for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
            if(this->m_daq.m_fecs[fec].GetHybrid(hyb)) {
                if(readHybridInfo && linkStatus[hyb].toInt() >= 3) {
                    QString hybridID = this->m_daq.m_fecs[fec].m_fecConfigModule->ReadI2C(hyb,0);
                    QString firmwareVersion = this->m_daq.m_fecs[fec].m_fecConfigModule->ReadI2C(hyb,1);
                    QString geoID = this->m_daq.m_fecs[fec].m_fecConfigModule->ReadI2C(hyb,2);

                    //m_ui->hybridID->setText(hybridID);
                    m_daq.m_fecs[fec].m_hybrids[hyb].SetInfo("hybrid_id", hybridID.toStdString());
                    //m_ui->firmwareVersion->setText(firmwareVersion);
                    m_daq.m_fecs[fec].m_hybrids[hyb].SetInfo("firmware_version", firmwareVersion.toStdString());
                    //m_ui->geoID->setText(geoID);
                    m_daq.m_fecs[fec].m_hybrids[hyb].SetInfo("geo_id", geoID.toStdString());

                    std::cout << "FEC " << fec << " - hybrid " << hyb << ": Firmware version " << firmwareVersion.toStdString() << ", hybrid-ID " << hybridID.toStdString() << ", GEO-ID " << geoID.toStdString() << std::endl;
                }
                if(linkStatus[hyb] == "5") {
                    m_ui->tableWidget_LinkStatus->item(hyb, fec)->setBackground(Qt::cyan);
                }
                else if(linkStatus[hyb] == "4") {
                    m_ui->tableWidget_LinkStatus->item(hyb, fec)->setBackground(Qt::green);
                }
                else if(linkStatus[hyb] == "3") {
                    m_ui->tableWidget_LinkStatus->item(hyb, fec)->setBackground(Qt::yellow);
                }
                else if(linkStatus[hyb] == "2") {
                    m_ui->tableWidget_LinkStatus->item(hyb, fec)->setBackground(Qt::red);
                }
                else if(linkStatus[hyb] == "1") {
                    m_ui->tableWidget_LinkStatus->item(hyb, fec)->setBackground(Qt::red);
                }
                else if(linkStatus[hyb] == "0") {
                    m_ui->tableWidget_LinkStatus->item(hyb, fec)->setBackground(Qt::red);
                }
                m_ui->tableWidget_LinkStatus->item(hyb, fec)->setText(linkStatus[hyb]);
                m_daq.m_fecs[m_fecIndex].m_hybrids[hyb].SetInfo("link_status", linkStatus[hyb].toStdString());
            }
            else {
                m_ui->tableWidget_LinkStatus->item(hyb,fec)->setBackground(Qt::lightGray);
                m_ui->tableWidget_LinkStatus->item(hyb, fec)->setText("");
            }
        }
        if(fec == m_fecIndex) {
            m_ui->debugScreen->clear();
            m_ui->debugScreen->insertPlainText(message);
            m_ui->debugScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        }
    }
}


void DAQWindow::openConnection()
{
    if(!m_ui->onACQ->isDown())
    {
        EnableDAQCommunicationButtons(false);
        if(g_slow_control > 0){
            for (unsigned short j=FECS_PER_DAQ-1; j >0; j--){
                if (this->m_daq.GetFEC(j)){
                    QHostAddress ipAddress;
                    ipAddress.setAddress(this->m_daq.m_fecs[j].GetReg("ip_fec"));
                    long theIP = ipAddress.toIPv4Address();
                    int res = this->m_daq.CheckIP_FEC(theIP, j);
                    if(res > -1) {
                        QMessageBox::warning(this, QString::fromStdString(g_card_name) + tr(" IPv4 address"),
                                             "The last octet of the IP address has to be hence unique.\nLast octet of " + QString::fromStdString(g_card_name)
                                             + QString::number(j+1) + " IP address already in use in " + QString::fromStdString(g_card_name) +  " " + QString::number(res) + "\n\n"
                                             +"To change the IP address of a " + QString::fromStdString(g_card_name) +  " in case you use multiple " + QString::fromStdString(g_card_name) +  "s, connect the "
                                             + QString::fromStdString(g_card_name) +  "s one by one and change the IP.\n"
                                             +"The default IP address for " + QString::fromStdString(g_card_name) +  "s is 10.0.0.2.\n",
                                             QMessageBox::Ok);
                        return;
                    }
                    res = this->m_daq.CheckIP_DAQ(theIP);
                    if(res > -1) {
                        QMessageBox::warning(this, QString::fromStdString(g_card_name) + tr(" IPv4 address"),
                                             QString::fromStdString(g_card_name) + " " + QString::number(j+1) + " IP address already used as DAQ IP in " + QString::fromStdString(g_card_name) + " " + QString::number(res),
                                             QMessageBox::Ok);
                        return;
                    }
                    ipAddress.setAddress(this->m_daq.m_fecs[j].GetReg("ip_daq"));
                    theIP = ipAddress.toIPv4Address();
                    res = this->m_daq.CheckIP_DAQ(theIP);
                    if(res == -1) {
                        QMessageBox::warning(this, tr("DAQ IPv4 address"),
                                             "All FECs have to use the same DAQ IP address (IP of the computer where the slow control runs)!",
                                             QMessageBox::Ok);
                        return;
                    }
                }
            }
        }
        m_ui->openConnection->setStyleSheet("QPushButton { color: red; }");
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if (this->m_daq.GetFEC(fec)){
                if(this->m_daq.m_fecs[fec].m_fecConfigModule->Connect()==true){
                    g_connection_ok = false;
                    QMap<QString, QString> registers;
                    if(this->m_daq.m_fecs[fec].m_fecConfigModule->ReadSystemRegisters(registers)) {
                        QString text;
                        if(g_card_name == "FEN") {
                            bool ok;
                            long boardId = registers["board_id"].toLong(&ok,16);
                            if(fec == m_fecIndex) {
                                m_ui->ip_daq->setText(QString::number(boardId));
                            }
                            m_daq.m_fecs[fec].SetInfo("board_id", registers["board_id"].toStdString());
                            text = QString::fromStdString(g_card_name) + QString::number(fec) + " "
                                    + QString::number(m_map_id_ring_fen[fec].first).rightJustified(2, '0') + "_"
                                    + QString::number(m_map_id_ring_fen[fec].second).rightJustified(2, '0') + " "
                                    + " board-ID " + registers["board_id"] + " (" + QString::number(boardId) + ")";
                        }
                        else {
                            text = QString::fromStdString(g_card_name) + QString::number(fec) + " " + registers["FirmwareVers"]+" MAC " + registers["MACvendor"]+registers["MACdevice"]+"\nIP " + registers["FECip"] + ",DAQ " + registers["DAQip"];
                        }
                        SetStatus(text, fec);
                        CheckLinkStatus(fec, true);
                    }
                    else {
                        g_connection_ok = false;
                        QString text;
                        if(g_card_name == "FEN") {
                            text = QString::fromStdString(g_card_name) + QString::number(fec) + " "
                                    + QString::number(m_map_id_ring_fen[fec].first).rightJustified(2, '0') + "_"
                                    + QString::number(m_map_id_ring_fen[fec].second).rightJustified(2, '0') +
                                    + "\nError reading EEPROM!";
                        }
                        else {
                            text = QString::fromStdString(g_card_name) + QString::number(fec) + " - IP " + this->m_daq.m_fecs[fec].GetIP() + "\nError reading EEPROM!";
                        }
                        SetStatus(text, fec);
                    }

                }
                else{
                    g_connection_ok = false;
                    QString text;
                    if(g_card_name == "FEN") {
                        text = QString::fromStdString(g_card_name) + QString::number(fec) + " "
                                + QString::number(m_map_id_ring_fen[fec].first).rightJustified(2, '0') + "_"
                                + QString::number(m_map_id_ring_fen[fec].second).rightJustified(2, '0') +
                                + " - IP " + this->m_daq.m_fecs[fec].GetIP() + "\nError pinging card!";
                    }
                    else {
                        text = QString::fromStdString(g_card_name) + QString::number(fec) + " - IP " + this->m_daq.m_fecs[fec].GetIP() + "\nError pinging card!";
                    }
                    SetStatus(text, fec);
                }
            }
        }
        if(g_connection_ok) {
            m_ui->openConnection->setStyleSheet("QPushButton { color: green; }");
            EnableDAQCommunicationButtons(true);
        }

    }
}


QString DAQWindow::GetApplicationPath()
{
    return m_execPath;
}


void DAQWindow::InitCalibWidgets() {

    connect(m_ui->pushButtonTakeData, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonStoreCorrections, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonSavePDF, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonAbort, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonCSV, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonLog, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonApplyCalib, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonJoinCalib, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonSystemConfig, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonResetLatencyCalibration, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));
    connect(m_ui->pushButtonTPLatencyCalibration, SIGNAL(pressed()),
            this, SLOT( onUpdateCalibSettings() ));

}

void DAQWindow::onUpdateCalibSettings(){
    if(QObject::sender() == m_ui->pushButtonTakeData){
        if(g_connection_ok){
            this->m_daq.ACQHandler(false);
            m_ui->offACQ->setCheckable(true);
            m_ui->offACQ->setChecked(true);
            m_ui->onACQ->setChecked(false);
            m_ui->Send->setEnabled(false);
            if(!m_ui->pushButtonTakeData->isChecked()){
                this->m_calib->m_isAutomatic = false;
                m_ui->pushButtonTakeData->setCheckable(true);
                this->m_calib->StartCalibration();
            }
        }
        else
        {
            m_ui->pushButtonTakeData->setChecked(false);
            m_ui->pushButtonTakeData->setCheckable(false);
        }
    }
    else if(QObject::sender() == m_ui->pushButtonStoreCorrections){
        this->m_calib->SaveCorrections();
    }
    else if(QObject::sender() == m_ui->pushButtonSavePDF){
        this->m_calib->SavePlotsAsPDF();
    }
    else if(QObject::sender() == m_ui->pushButtonAbort){
        if(g_connection_ok){
            this->m_calib->StopDataTaking();
        }
    }
    else if(QObject::sender() == m_ui->pushButtonCSV){
        this->m_calib->SaveDataAsCSV();
    }
    else if(QObject::sender() == m_ui->pushButtonLog){
        this->m_calib->SaveToLog();
    }
    else if(QObject::sender() == m_ui->pushButtonApplyCalib){
        this->m_calib->ApplyCalib();
    }
    else if(QObject::sender() == m_ui->pushButtonJoinCalib){
        this->m_calib->JoinCalib();
    }
    else if(QObject::sender() == m_ui->pushButtonSystemConfig){
        this->m_calib->WriteSystemConfig();
    }
    else if(QObject::sender() == m_ui->pushButtonResetLatencyCalibration){
        this->m_calib->AutomaticLatencyCalib(true);
    }
    else if(QObject::sender() == m_ui->pushButtonTPLatencyCalibration){
        this->m_calib->AutomaticLatencyCalib(false);
    }

}

void DAQWindow::InitDAQWidgets() {
    m_ui->treeWidgetSystem->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_ui->openConnection, SIGNAL(clicked()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->Send, SIGNAL(clicked()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->onACQ, SIGNAL(clicked()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->offACQ, SIGNAL(clicked()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->Button_save, SIGNAL(clicked()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->selectDir, SIGNAL(clicked()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->Button_load, SIGNAL(clicked()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->checkBoxConfigCheck, SIGNAL(clicked()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->treeExpand, SIGNAL(pressed()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->treeCollaps, SIGNAL(pressed()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->treeAdd, SIGNAL(pressed()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->treeDelete, SIGNAL(pressed()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->treeEdit, SIGNAL(pressed()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->treeWidgetSystem, SIGNAL(itemSelectionChanged()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->pushButtonMeasureTemp, SIGNAL(pressed()),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->comboBoxI2CSetting, SIGNAL(currentIndexChanged(int)),
            this, SLOT( onUpdateDAQSettings() ));
    connect(m_ui->tableWidget_LinkStatus, SIGNAL(cellClicked(int,int)),
            this, SLOT( onSelectVMM_TableWidgets(int,int) ));
    connect(m_ui->tableWidget_Temperature, SIGNAL(cellClicked(int,int)),
            this, SLOT( onSelectVMM_TableWidgets(int,int) ));
}

void DAQWindow::onSelectVMM_TableWidgets(int row, int column) {
    int hyb =0;
    int vmm = 0;
    int fec = column;
    if(QObject::sender() == m_ui->tableWidget_LinkStatus) {
        hyb = row;
    }
    else {
        hyb = row/2;
        vmm = row%2;
    }
    if (m_daq.GetFEC(fec) ){
        if( m_daq.m_fecs[fec].GetHybrid(hyb) ){
            m_fecIndex = fec;
            m_hybridIndex = hyb;
            m_vmmIndex = vmm;
            QTreeWidgetItemIterator it( m_ui->treeWidgetSystem);
            while (*it) {
                QString txt = (*it)->text(0);

                if(txt.startsWith("FEN")) {
                    int theRing  =  txt.mid(4,2).toInt();
                    int theFen =  txt.mid(8,2).toInt();
                    int ring = m_map_id_ring_fen[m_fecIndex].first;
                    int fen = m_map_id_ring_fen[m_fecIndex].second;

                    if(fen == theFen && ring == theRing) {
                        for(int n=0; n <(*it)->childCount(); n++) {
                            QString txtHyb = (*it)->child(n)->text(0);
                            if(txtHyb.startsWith("Hybrid")) {
                                int hybId =  txtHyb.mid(7,1).toInt();
                                if(hybId == m_hybridIndex) {
                                    m_ui->treeWidgetSystem->setCurrentItem((*it)->child(n)->child(m_vmmIndex));
                                    return;
                                }
                            }
                        }
                    }

                }
                else if(txt.startsWith("FEC")) {
                    int theFecId =  txt.mid(4,1).toInt();
                    if(theFecId == m_fecIndex) {
                        for(int n=0; n <(*it)->childCount(); n++) {
                            QString txtHyb = (*it)->child(n)->text(0);
                            if(txtHyb.startsWith("Hybrid")) {
                                int hybId =  txtHyb.mid(7,1).toInt();
                                if(hybId == m_hybridIndex) {
                                    m_ui->treeWidgetSystem->setCurrentItem((*it)->child(n)->child(m_vmmIndex));
                                    return;
                                }
                            }
                        }
                    }
                }
                ++it;
            }

        }
    }

}


void DAQWindow::SaveConfig(QString text, bool addDate){
    QString fname = text;
    std::cout << text.toStdString() << std::endl;
    QRegularExpression re("_[0-9]{4}(0[1-9]|1[0-2])(0[1-9]|[1-2][0-9]|3[0-1])-(2[0-3]|[01][0-9])([0-5][0-9])([0-5][0-9])");
    QRegularExpressionMatch match = re.match(text);
    if (match.hasMatch()) {
        int startOffset = match.capturedStart(0);
        fname = text.remove(startOffset,startOffset+14);
        //std::cout << "match " << startOffset << " " << startOffset+14 << " " << fname.toStdString() << std::endl;
    }
    QString theName= "";
    if(addDate) {
        QString theDate = QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss");
        theName = fname + "_" + theDate;
    }
    else {
        theName = fname;
    }
    QString theNameWithExtension = theName + ".json";
    std::cout << theNameWithExtension.toStdString() << std::endl;
    this->m_daqConfig->WriteDAQConf(theNameWithExtension.toStdString().data());
    std::cout << "writing to file " << theNameWithExtension.toStdString() << std::endl;
}


void DAQWindow::onUpdateDAQSettings(){
    if(QObject::sender() == m_ui->comboBoxI2CSetting) {
        if(m_ui->comboBoxI2CSetting->currentIndex() > 0) {
            for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
                if (this->m_daq.GetFEC(fec)){
                    for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                        if(this->m_daq.m_fecs[fec].GetHybrid(hyb)) {
                            if(m_ui->comboBoxI2CSetting->currentIndex()<= 4){
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].SetRegi("sm5_sm0",m_ui->comboBoxI2CSetting->currentIndex()+63);
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[1].SetRegi("sm5_sm0",m_ui->comboBoxI2CSetting->currentIndex()+63);
                            }
                            else {
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].SetRegi("sm5_sm0", m_ui->comboBoxI2CSetting->currentIndex()-5);
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[1].SetRegi("sm5_sm0", m_ui->comboBoxI2CSetting->currentIndex()-5);
                            }
                        }
                    }
                }
            }
            if(GetVMM("scmx") == 1){
                m_ui->sm5_sm0->setCurrentIndex(GetVMM("sm5_sm0")+4);
            }
            else m_ui->sm5_sm0->setCurrentIndex(GetVMM("sm5_sm0")-1);
        }

    }
    else if(QObject::sender() == m_ui->pushButtonMeasureTemp){

        if(m_ui->comboBoxI2CSetting->currentIndex() > 0) {
            for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
                if (this->m_daq.GetFEC(fec)){
                    for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                        if(this->m_daq.m_fecs[fec].GetHybrid(hyb)) {
                            if(m_ui->comboBoxI2CSetting->currentIndex()<= 4){
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].SetRegi("sm5_sm0",m_ui->comboBoxI2CSetting->currentIndex()+63);
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[1].SetRegi("sm5_sm0",m_ui->comboBoxI2CSetting->currentIndex()+63);
                            }
                            else {
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].SetRegi("sm5_sm0", m_ui->comboBoxI2CSetting->currentIndex()-5);
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[1].SetRegi("sm5_sm0", m_ui->comboBoxI2CSetting->currentIndex()-5);
                            }
                        }
                    }
                }
            }
            if(GetVMM("scmx") == 1){
                m_ui->sm5_sm0->setCurrentIndex(GetVMM("sm5_sm0")+4);
            }
            else m_ui->sm5_sm0->setCurrentIndex(GetVMM("sm5_sm0")-1);
        }
        MeasureVMMI2C();
    }
    else if(QObject::sender() == m_ui->openConnection){
        openConnection();
        if(g_connection_ok){
            EnableFECCommunicationButtons(true);
            EnableHybridCommunicationButtons(true);
            EnableVMMCommunicationButtons(true);
        }
        else{
            EnableFECCommunicationButtons(false);
            EnableHybridCommunicationButtons(false);
            EnableVMMCommunicationButtons(false);
        }
        m_ui->tabWidgetTree->setCurrentIndex(1);
    }
    else if(QObject::sender() == m_ui->Send){
        m_ui->offACQ->setCheckable(true);
        m_ui->offACQ->setChecked(true);
        m_ui->onACQ->setChecked(false);
        m_ui->Send->setEnabled(true);
        this->m_daq.ACQHandler(false);
        this->m_daq.SendAll();
    }
    else if(QObject::sender() == m_ui->onACQ){
        this->m_daq.ACQHandler(false);
        m_ui->onACQ->setCheckable(true);
        m_ui->onACQ->setChecked(true);
        m_ui->offACQ->setChecked(false);
        m_ui->Send->setEnabled(false);
        m_ui->comboBoxI2CSetting->setEnabled(false);
        m_ui->sm5_sm0->setEnabled(false);
        this->m_daq.SendAll(true);
        this->m_daq.ACQHandler(true);
        m_ui->offACQ->setStyleSheet("QPushButton { color: black; }");
        m_ui->onACQ->setStyleSheet("QPushButton { color: green; }");
        if(g_slow_control != 1) {
            QThread::msleep(200);
            for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
                if (this->m_daq.GetFEC(fec)){
                    CheckLinkStatus(fec, false);
                }
            }
        }
        else {
            m_ui->linkPB->setEnabled(false);
            m_ui->readSystemParams->setEnabled(false);
            m_ui->pushButtonDAQIP->setEnabled(false);
            m_ui->ReadI2C->setEnabled(false);
            m_ui->readADC->setEnabled(false);
            m_ui->openConnection->setEnabled(false);
            m_ui->pushButtonMeasureTemp->setEnabled(false);
            m_ui->pushButtonResetLatencyCalibration->setEnabled(false);
            m_ui->pushButtonTPLatencyCalibration->setEnabled(false);
        }

    }
    else if(QObject::sender() == m_ui->offACQ){
        m_ui->offACQ->setCheckable(true);
        m_ui->offACQ->setChecked(true);
        m_ui->onACQ->setChecked(false);
        m_ui->Send->setEnabled(true);
        this->m_daq.ACQHandler(false);
        m_ui->comboBoxI2CSetting->setEnabled(true);
        m_ui->sm5_sm0->setEnabled(true);
        m_ui->offACQ->setStyleSheet("QPushButton { color: red; }");
        m_ui->onACQ->setStyleSheet("QPushButton { color: black; }");
        QThread::msleep(200);
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if (this->m_daq.GetFEC(fec)){
                CheckLinkStatus(fec, false);
            }
        }
        if(g_slow_control == 1) {
            m_ui->linkPB->setEnabled(true);
            m_ui->readSystemParams->setEnabled(true);
            m_ui->pushButtonDAQIP->setEnabled(true);
            m_ui->ReadI2C->setEnabled(true);
            m_ui->readADC->setEnabled(true);
            m_ui->openConnection->setEnabled(true);
            m_ui->pushButtonMeasureTemp->setEnabled(true);
            m_ui->pushButtonResetLatencyCalibration->setEnabled(true);
            m_ui->pushButtonTPLatencyCalibration->setEnabled(true);
        }

    }
    else if(QObject::sender() == m_ui->Button_save){
        QString text = m_ui->line_configFile->displayText();
        std::string fname = text.toStdString();
        if (fname == "") {
            std::cout << "No file name specified" << std::endl;
            m_ui->line_configFile->insert("ERROR: no file name given");
        }
        else {
            if(m_config_date_mode == 0) {
                QMessageBox msgBox;
                msgBox.setText("Always add date and time?");
                msgBox.setInformativeText("Do you want to add the date and time in the form YYYYMMDD_HHMMSS to config names?");
                QAbstractButton *myYesButton = msgBox.addButton("Always",QMessageBox::YesRole);
                QAbstractButton *myNoButton = msgBox.addButton("Never", QMessageBox::NoRole);
                QAbstractButton *myAskButton = msgBox.addButton("Ask me", QMessageBox::ActionRole);
                msgBox.setIcon(QMessageBox::Question);
                msgBox.exec();
                if(msgBox.clickedButton() == myNoButton) {
                    m_config_date = false;
                    m_config_date_mode = 2;
                }
                else if(msgBox.clickedButton() == myYesButton) {
                    m_config_date = true;
                    m_config_date_mode = 2;
                }
                else if(msgBox.clickedButton() == myAskButton) {
                    m_config_date_mode = 1;
                }
            }
            if(m_config_date_mode == 1) {
                QMessageBox msgBox;
                msgBox.setText("Add date and time now?");
                msgBox.setInformativeText("Do you want to add the date and time in the form YYYYMMDD_HHMMSS to this config name?");
                QAbstractButton *myYesButton = msgBox.addButton("Yes",QMessageBox::YesRole);
                QAbstractButton *myNoButton = msgBox.addButton("No", QMessageBox::NoRole);
                msgBox.setIcon(QMessageBox::Question);
                msgBox.exec();
                if(msgBox.clickedButton() == myNoButton) {
                    m_config_date = false;
                }
                else if(msgBox.clickedButton() == myYesButton) {
                    m_config_date = true;
                }
            }
            SaveConfig(text, m_config_date);
        }
    }
    else if(QObject::sender() == m_ui->selectDir){
        stringstream sx;

        QFileDialog getdir;
        //    getdir.setProxyModel();
        QString dirStr = QFileDialog::getOpenFileName(this,
                                                      tr("Select config file"), this->GetApplicationPath() + "/../configs",
                                                      tr("Text (*.json)") );
        if(dirStr=="") return;
        if(!dirStr.contains("/configs/")){
            qDebug()<< "Config file not located in config folder/subfolder  -- Abort";
            m_ui->line_configFile->setText("ERROR: config file not in config folder");
            return;
        }
        QString fname = dirStr.split("/").last();
        if(fname.endsWith(".json")) {
            fname.remove(fname.size()-5,5);
        }

        m_ui->line_configFile->setText(fname);
        emit m_ui->Button_load->clicked();
    }
    else if(QObject::sender() == m_ui->Button_load){
        QString text = m_ui->line_configFile->displayText();
        LoadConfig(text);
    }
    else if(QObject::sender() == m_ui->checkBoxConfigCheck){
        if (m_ui->checkBoxConfigCheck->isChecked()) {
            g_use_config_check = true;
        }
        else {
            g_use_config_check = false;
        }
    }
    else if(QObject::sender() == m_ui->treeExpand){
        m_ui->treeWidgetSystem->expandAll();
    }
    else if(QObject::sender() == m_ui->treeCollaps){
        m_ui->treeWidgetSystem->collapseAll();
    }
    else if(QObject::sender() == m_ui->treeDelete){
        QString result = "";
        QMessageBox::StandardButton reply;
        bool ok;
        QString txt = m_ui->treeWidgetSystem->currentItem()->text(0);
        if(txt.startsWith("FEN") || txt.startsWith("FEC")) {
            QMessageBox msgBox;
            msgBox.setText("Delete card:");
            msgBox.setInformativeText("Do you want to delete " + txt + "?");

            QAbstractButton *myYesButton = msgBox.addButton("Yes",QMessageBox::YesRole);
            QAbstractButton *myNoButton = msgBox.addButton("No", QMessageBox::NoRole);
            msgBox.setIcon(QMessageBox::Question);
            msgBox.exec();
            if(msgBox.clickedButton() == myNoButton) {
                return;
            }
            else if(msgBox.clickedButton() == myYesButton) {
                m_daq.SetFEC(m_fecIndex, false);
                for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                    m_daq.m_fecs[m_fecIndex].SetHybrid(hyb, false);
                }
                if(txt.startsWith("FEN")) {
                    int ring = txt.mid(4,2).toInt();
                    int fen = txt.mid(8,2).toInt();
                    m_map_id_ring_fen.erase(m_fecIndex);
                    m_map_ring_fen_id.erase(qMakePair(ring,fen));
                }
                delete m_ui->treeWidgetSystem->currentItem();
            }
        }
        else if(txt.startsWith("Hybrid")) {
            QMessageBox msgBox;
            msgBox.setText("Delete card:");
            msgBox.setInformativeText("Do you want to delete " + txt + "?");
            QAbstractButton *myYesButton = msgBox.addButton("Yes",QMessageBox::YesRole);
            QAbstractButton *myNoButton = msgBox.addButton("No", QMessageBox::NoRole);
            msgBox.setIcon(QMessageBox::Question);
            msgBox.exec();
            if(msgBox.clickedButton() == myNoButton) {
                return;
            }
            else if(msgBox.clickedButton() == myYesButton) {
                m_daq.m_fecs[m_fecIndex].SetHybrid(m_hybridIndex, false);
                delete m_ui->treeWidgetSystem->currentItem();
            }
        }
        UpdateSystemStatus();
    }
    else if(QObject::sender() == m_ui->treeEdit){
        QString txtItem = m_ui->treeWidgetSystem->currentItem()->text(0);
        QString txtDescription = m_ui->treeWidgetSystem->currentItem()->text(1);
        bool ok;
        QString description;
        QStringList resultList;
        int newRing =0;
        int newFen = 0;
        if(txtItem.startsWith("FEN")) {
            int ring = txtItem.mid(4,2).toInt();
            int fen = txtItem.mid(8,2).toInt();
            resultList = QInputDialog::getText(this,txtItem, tr("Edit ring_fen and description:"), QLineEdit::Normal, QString::number(ring).rightJustified(2, '0') + "_"
                                               + QString::number(fen).rightJustified(2, '0')+"," + txtDescription, &ok).split(",");
            if(resultList.size() < 2) {
                ok = false;
            }
            else {
                if(resultList[0].mid(2,1) != "_") {
                    ok = false;
                }
                newRing = resultList[0].mid(0,2).toInt();
                newFen = resultList[0].mid(3,2).toInt();
                if(resultList.size() == 2) {
                    description  =  resultList[1];
                }
                else {
                    for(int n=1;n<resultList.size()-1;n++) {
                        description  +=  resultList[n] +",";
                    }
                    description  +=  resultList[resultList.size()-1];
                }
            }
        }
        else {
            description = QInputDialog::getText(this,txtItem, tr("Edit description:"), QLineEdit::Normal, txtDescription, &ok);
        }
        if (ok) {

            m_ui->treeWidgetSystem->currentItem()->setText(1,description);
            if(txtItem.startsWith("VMM")) {
                int vmm = txtItem.mid(4,1).toInt();
                QString txtParent = m_ui->treeWidgetSystem->currentItem()->parent()->text(0);
                int hybrid = txtParent.mid(6,1).toInt();
                QString txtGrandParent = m_ui->treeWidgetSystem->currentItem()->parent()->parent()->text(0);
                if(txtGrandParent.startsWith("FEN")) {
                    int ring = txtGrandParent.mid(4,2).toInt();
                    int fen = txtGrandParent.mid(8,2).toInt();
                    int fec = m_map_ring_fen_id[qMakePair(ring,fen)];
                    m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].SetInfo("description",description.toStdString());
                }
                else if(txtGrandParent.startsWith("FEC")) {
                    int fec = txtGrandParent.mid(4,1).toInt();
                    m_daq.m_fecs[fec].m_hybrids[hybrid].m_vmms[vmm].SetInfo("description",description.toStdString());
                }
            }
            else if(txtItem.startsWith("Hybrid")) {
                int hybrid = txtItem.mid(6,1).toInt();
                QString txtParent = m_ui->treeWidgetSystem->currentItem()->parent()->text(0);
                if(txtParent.startsWith("FEN")) {
                    int ring = txtParent.mid(4,2).toInt();
                    int fen = txtParent.mid(8,2).toInt();
                    int fec = m_map_ring_fen_id[qMakePair(ring,fen)];
                    m_daq.m_fecs[fec].m_hybrids[hybrid].SetInfo("description",description.toStdString());
                }
                else if(txtParent.startsWith("FEC")) {
                    int fec = txtParent.mid(4,1).toInt();
                    m_daq.m_fecs[fec].m_hybrids[hybrid].SetInfo("description",description.toStdString());
                }

            }
            else if(txtItem.startsWith("FEN")) {
                int ring = txtItem.mid(4,2).toInt();
                int fen = txtItem.mid(8,2).toInt();
                int fec = m_map_ring_fen_id[ qMakePair(ring,fen)];
                m_daq.m_fecs[fec].SetInfo("description",description.toStdString());
                m_daq.m_fecs[fec].SetReg("ring",(unsigned long)newRing);
                m_daq.m_fecs[fec].SetReg("fen",(unsigned long)newFen);
                m_daq.m_fecs[fec].SetId();

                m_map_ring_fen_id.erase(qMakePair(ring,fen));
                m_map_ring_fen_id[qMakePair(newRing,newFen)] = fec;
                m_map_id_ring_fen[fec] = qMakePair(newRing,newFen);
                QString node =  QString::fromStdString(g_card_name)+" " + QString::number(newRing).rightJustified(2, '0')  + "_" + QString::number(newFen).rightJustified(2, '0');
                m_ui->treeWidgetSystem->currentItem()->setText(0,node);
            }
            else if(txtItem.startsWith("FEC")) {
                int fec = txtItem.mid(4,1).toInt();
                m_daq.m_fecs[fec].SetInfo("description",description.toStdString());
            }
        }
    }
    else if(QObject::sender() == m_ui->treeAdd){

        QString result = "";
        QMessageBox::StandardButton reply;
        bool ok;
        QString txt = m_ui->treeWidgetSystem->currentItem()->text(0);

        if(txt.startsWith("FEN") || txt.startsWith("FEC")) {
            int fecIndex = 0;
            if(txt.startsWith("FEN")) {
                int ring = txt.mid(4,2).toInt();
                int fen = txt.mid(8,2).toInt();
                fecIndex = m_map_ring_fen_id[qMakePair((int)ring, (int)fen)];
            }
            else {
                fecIndex = txt.mid(4,1).toInt();
            }
            int numFreeHybrids = 0;
            for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                if(!this->m_daq.m_fecs[fecIndex].GetHybrid(hyb)) {
                    numFreeHybrids++;
                }
            }
            int numFreeFecs = 0;
            for(int fec=0; fec<FECS_PER_DAQ; fec++) {
                if (!this->m_daq.GetFEC(fec)){
                    numFreeFecs++;
                }
            }
            QMessageBox msgBox;
            msgBox.setText("New card:");
            QAbstractButton *myFecButton = nullptr;
            QAbstractButton *myHybridButton = nullptr;
            if(numFreeHybrids > 0 && numFreeFecs > 0) {
                msgBox.setInformativeText("Which card to you want to add?");
                myFecButton = msgBox.addButton(QString::fromStdString(g_card_name), QMessageBox::YesRole);
                myHybridButton = msgBox.addButton("Hybrid", QMessageBox::NoRole);
            }
            else {
                if(numFreeHybrids > 0) {
                    msgBox.setInformativeText("Do you want to add a hybrid?");
                    myHybridButton = msgBox.addButton("Yes", QMessageBox::YesRole);
                }
                else if(numFreeFecs > 0) {
                    msgBox.setInformativeText("Do you want to add a " + QString::fromStdString(g_card_name) + "?");
                    myFecButton = msgBox.addButton("Yes", QMessageBox::YesRole);
                }
                else {
                    msgBox.setInformativeText("Maximum number of hybrids and " + QString::fromStdString(g_card_name) + " reached!");
                }
            }
            QAbstractButton *myCancelNoButton = msgBox.addButton("Cancel", QMessageBox::ApplyRole);
            msgBox.setIcon(QMessageBox::Question);
            msgBox.exec();
            if(msgBox.clickedButton() == myCancelNoButton) {
                return;
            }
            else if(msgBox.clickedButton() == myFecButton) {
                int lastFecIndex = 0;
                for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
                    if (this->m_daq.GetFEC(fec)){
                        lastFecIndex = fec;
                    }
                    else {
                        break;
                    }
                }
                long theIP = 0;
                if(g_slow_control == 0) {
                    theIP = m_daq.m_fecs[lastFecIndex].GetRegVal("ip_fec");
                }
                else {
                    theIP  = m_daq.m_fecs[lastFecIndex].GetRegVal("ip_fec")+1;
                }
                QHostAddress ipAddress;
                ipAddress.setAddress(theIP);
                QString ipString = "";
                QString ringFenString = "";
                unsigned long ring = 0;
                unsigned long fen=0;
                if(g_card_name == "FEN") {
                    ringFenString = QInputDialog::getText(nullptr, tr("Add FEN"),
                                                          tr("Ring and FEN number ?"),
                                                          QLineEdit::Normal,"00_00",&ok);
                    ring = ringFenString.mid(0,2).toInt();
                    fen = ringFenString.mid(3,2).toInt();

                    if(ok && ring >=0 && ring <=11 && fen >=0 && fen <=3) {
                        auto search = m_map_ring_fen_id.find(qMakePair((int)ring, (int)fen));
                        if (search != m_map_ring_fen_id.end()) {

                            QMessageBox msgError;
                            msgError.setText("This ring and fen combination exists already!");
                            msgError.setIcon(QMessageBox::Critical);
                            msgError.setWindowTitle("Danger");
                            msgError.exec();
                        }
                        else {
                            if(g_slow_control == 1) {
                                ipString = QInputDialog::getText(nullptr, tr("Add FEN"),
                                                                 tr("IP for new FEN ?"),
                                                                 QLineEdit::Normal,ipAddress.toString(),&ok);
                            }
                            else {
                                ipString = ipAddress.toString();
                            }
                        }
                    }

                }
                else {
                    ipString = QInputDialog::getText(nullptr, tr("Add FEC"),
                                                     tr("IP for new FEC ?"),
                                                     QLineEdit::Normal,ipAddress.toString(),&ok);
                }
                if(ok && ipString != "") {
                    QHostAddress ipAddress;
                    ipAddress.setAddress(ipString);
                    long ip = ipAddress.toIPv4Address();
                    QTreeWidgetItem *cardItem = new QTreeWidgetItem();
                    if(g_card_name == "FEN") {
                        QString node =  QString::fromStdString(g_card_name)+" " + QString::number(ring).rightJustified(2, '0')  + "_" + QString::number(fen).rightJustified(2, '0');
                        cardItem->setText(0,node);

                        m_daq.SetFEC(lastFecIndex+1, true);
                        m_daq.m_fecs[lastFecIndex+1].SetIP_FEC(ip);
                        m_daq.m_fecs[lastFecIndex+1].SetReg("ring", ring);
                        m_daq.m_fecs[lastFecIndex+1].SetReg("fen", fen);
                        m_daq.m_fecs[lastFecIndex+1].SetId();
                        m_map_id_ring_fen[lastFecIndex+1] = qMakePair((int)ring, (int)fen);
                        m_map_ring_fen_id[qMakePair((int)ring, (int)fen)] = lastFecIndex+1;
                    }
                    else {
                        cardItem->setText(0,QString::fromStdString(g_card_name)+" " + QString::number(lastFecIndex+1));
                        m_daq.SetFEC(lastFecIndex+1, true);
                        m_daq.m_fecs[lastFecIndex+1].SetIP_FEC(ip);
                        m_daq.m_fecs[lastFecIndex+1].SetId();
                    }
                    m_ui->treeWidgetSystem->addTopLevelItem(cardItem);
                }
            }
            else if(msgBox.clickedButton() == myHybridButton) {
                int freeHybridIndex = 0;
                for(int hyb=0; hyb<HYBRIDS_PER_FEC; hyb++) {
                    if(!this->m_daq.m_fecs[fecIndex].GetHybrid(hyb)) {
                        freeHybridIndex = hyb;
                        break;
                    }
                }
                bool ok;
                int hybrid = QInputDialog::getInt(nullptr, "Add hybrid to " + QString::fromStdString(g_card_name) + " " + fecIndex + ":",
                                                  tr("Hybrid number?"),freeHybridIndex,0,7,1,&ok);
                if(ok && !m_daq.m_fecs[fecIndex].GetHybrid(hybrid)) {
                    QTreeWidgetItem *hybridItem = new QTreeWidgetItem();
                    hybridItem->setText(0,"Hybrid "+QString::number(hybrid));
                    m_daq.m_fecs[fecIndex].SetHybrid(hybrid, true);
                    m_ui->treeWidgetSystem->currentItem()->addChild(hybridItem);
                    for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                        QTreeWidgetItem *vmmItem = new QTreeWidgetItem();
                        vmmItem->setText(0,"VMM "+QString::number(vmm));
                        hybridItem->addChild(vmmItem);
                        std::cout << "vmm " << vmm << " on hybrid "<< hybrid << " on fen " << fecIndex << " is active" << std::endl;
                    }
                }
            }
        }
        UpdateSystemStatus();
    }
    else if(QObject::sender() == m_ui->treeWidgetSystem){
        QString title="";
        m_ui->treeAdd->setEnabled(false);
        m_ui->treeEdit->setEnabled(false);
        m_ui->treeDelete->setEnabled(false);
        QString txt = m_ui->treeWidgetSystem->currentItem()->text(0);
        if(txt.startsWith("Hybrid")) {
            m_ui->treeDelete->setEnabled(true);
            m_ui->treeEdit->setEnabled(true);
            m_hybridIndex =  txt.mid(7,1).toInt();
            QString parent = m_ui->treeWidgetSystem->currentItem()->parent()->text(0);

            if(parent.startsWith("FEN")) {
                int ring  =  parent.mid(4,2).toInt();
                int fen =  parent.mid(8,2).toInt();
                m_fecIndex = m_map_ring_fen_id[qMakePair(ring, fen)];
            }
            else if(parent.startsWith("FEC")) {
                m_fecIndex =  parent.mid(4,1).toInt();
            }
            //std::cout << "FEC INDEX " << m_fecIndex << ", HYBRID INDEX " << m_hybridIndex << ", VMM INDEX " << m_vmmIndex << std::endl;
            LoadFECSettings();
            LoadHybridSettings();
            if(m_ui->treeWidgetSystem->currentItem()->childCount() > 0) {
                QString child = m_ui->treeWidgetSystem->currentItem()->child(0)->text(0);
                m_vmmIndex =  child.mid(4,1).toInt();
                LoadVMMSettings();
                LoadVMMChannelSettings();
                m_ui->groupBoxFec->setTitle(parent);
                m_ui->groupBoxHybrid->setTitle(txt);
                bool changeTitle = false;
                if(m_ui->treeWidgetSystem->currentItem()->parent()->text(1) != "") {
                    title +=  m_ui->treeWidgetSystem->currentItem()->parent()->text(1);
                    changeTitle = true;
                }
                if( m_ui->treeWidgetSystem->currentItem()->text(1) != "") {
                    title += " - " + m_ui->treeWidgetSystem->currentItem()->text(1);
                    changeTitle = true;
                }
                if(m_ui->treeWidgetSystem->currentItem()->child(0)->text(1)!= "") {
                    title += " - " + m_ui->treeWidgetSystem->currentItem()->child(0)->text(1);
                    changeTitle = true;
                }
                if(changeTitle) {
                    m_ui->groupBoxVMM->setTitle(child + "     " + title);
                }
                else {
                    m_ui->groupBoxVMM->setTitle(child);
                }
            }
        }
        else if(txt.startsWith("VMM")) {
            m_ui->treeEdit->setEnabled(true);
            m_vmmIndex =  txt.mid(4,1).toInt();
            QString parent = m_ui->treeWidgetSystem->currentItem()->parent()->text(0);
            if(parent.startsWith("Hybrid")) {
                m_hybridIndex =  parent.mid(7,1).toInt();
            }
            QString grandparent = m_ui->treeWidgetSystem->currentItem()->parent()->parent()->text(0);
            if(grandparent.startsWith("FEN")) {
                int ring  =  grandparent.mid(4,2).toInt();
                int fen =  grandparent.mid(8,2).toInt();
                m_fecIndex = m_map_ring_fen_id[qMakePair(ring, fen)];
            }
            else if(grandparent.startsWith("FEC")) {
                m_fecIndex =  grandparent.mid(4,1).toInt();
            }

            //std::cout << "FEC INDEX " << m_fecIndex << ", HYBRID INDEX " << m_hybridIndex << ", VMM INDEX " << m_vmmIndex << std::endl;
            LoadFECSettings();
            LoadHybridSettings();
            LoadVMMSettings();
            LoadVMMChannelSettings();
            m_ui->groupBoxFec->setTitle(grandparent);
            m_ui->groupBoxHybrid->setTitle(parent);
            bool changeTitle = false;
            if(m_ui->treeWidgetSystem->currentItem()->parent()->parent()->text(1) != "") {
                title += m_ui->treeWidgetSystem->currentItem()->parent()->parent()->text(1);
                changeTitle = true;
            }
            if( m_ui->treeWidgetSystem->currentItem()->parent()->text(1) != "") {
                title += " - " +m_ui->treeWidgetSystem->currentItem()->parent()->text(1);
                changeTitle = true;
            }
            if(m_ui->treeWidgetSystem->currentItem()->text(1) != "") {
                title += " - " + m_ui->treeWidgetSystem->currentItem()->text(1);
                changeTitle = true;
            }
            if(changeTitle) {
                m_ui->groupBoxVMM->setTitle(txt + "     " + title);
            }
            else {
                m_ui->groupBoxVMM->setTitle(txt);
            }
        }
        else {
            m_ui->treeAdd->setEnabled(true);
            m_ui->treeDelete->setEnabled(false);
            m_ui->treeEdit->setEnabled(true);
            if(txt.startsWith("FEN")) {
                int ring  =  txt.mid(4,2).toInt();
                int fen =  txt.mid(8,2).toInt();
                m_fecIndex = m_map_ring_fen_id[qMakePair(ring, fen)];
            }
            else if(txt.startsWith("FEC")) {
                m_fecIndex =  txt.mid(4,1).toInt();
            }
            int numFecs = -1;
            for(int fec=0; fec<FECS_PER_DAQ; fec++) {
                if (this->m_daq.GetFEC(fec)){
                    numFecs++;
                }
            }
            if(m_fecIndex == numFecs && m_fecIndex > 0) {
                m_ui->treeDelete->setEnabled(true);
            }
            //std::cout << "FEC INDEX " << m_fecIndex << ", HYBRID INDEX " << m_hybridIndex << ", VMM INDEX " << m_vmmIndex << std::endl;
            LoadFECSettings();
            if(m_ui->treeWidgetSystem->currentItem()->childCount() > 0) {
                QString child = m_ui->treeWidgetSystem->currentItem()->child(0)->text(0);
                m_hybridIndex =  child.mid(7,1).toInt();
                LoadHybridSettings();
                if(m_ui->treeWidgetSystem->currentItem()->child(0)->childCount() > 0) {
                    QString grandchild = m_ui->treeWidgetSystem->currentItem()->child(0)->child(0)->text(0);
                    m_vmmIndex =  grandchild.mid(4,1).toInt();
                    LoadVMMSettings();
                    LoadVMMChannelSettings();
                    m_ui->groupBoxFec->setTitle(txt);
                    m_ui->groupBoxHybrid->setTitle(child);
                    bool changeTitle = false;
                    if(m_ui->treeWidgetSystem->currentItem()->text(1) != "") {
                        title += m_ui->treeWidgetSystem->currentItem()->text(1);
                        changeTitle = true;
                    }
                    if( m_ui->treeWidgetSystem->currentItem()->child(0)->text(1) != "") {
                        title += " - " + m_ui->treeWidgetSystem->currentItem()->child(0)->text(1);
                        changeTitle = true;
                    }
                    if(m_ui->treeWidgetSystem->currentItem()->child(0)->child(0)->text(1) != "") {
                        title += " - " + m_ui->treeWidgetSystem->currentItem()->child(0)->child(0)->text(1);
                        changeTitle = true;
                    }
                    if(changeTitle) {
                        m_ui->groupBoxVMM->setTitle(grandchild + "     " + title);
                    }
                    else {
                        m_ui->groupBoxVMM->setTitle(grandchild);
                    }
                }
                else {
                    m_ui->groupBoxVMM->setTitle("not defined");
                }

            }
            else {
                m_ui->groupBoxHybrid->setTitle("not defined");
                m_ui->groupBoxVMM->setTitle("not defined");
            }
        }


    }
}


void DAQWindow::InitFecWidgets() {
    SetFECToolTips();

    if(g_clock_source <= 1) {
        m_ui->pushButtonPowerCycle->setVisible(false);
        m_ui->pushButtonApplyAllFecs->setText("Apply to all assisters");
        m_ui->trgin_invert->setVisible(false);
        m_ui->trgout_invert->setVisible(false);
        m_ui->trgout_time->setVisible(false);
        m_ui->label_trgin_invert->setVisible(false);
        m_ui->label_trgout_invert->setVisible(false);
        m_ui->label_trgout_time->setVisible(false);
        m_ui->line_trigger->setVisible(false);
    }
    else {
        m_ui->pushButtonPowerCycle->setVisible(false);
        m_ui->pushButtonApplyAllFecs->setText("Apply to all FECs");
        m_ui->trgout_time->clear();
        m_ui->trgout_time->addItem("trg in");
        for(int i=0; i<4096; i++) {
            m_ui->trgout_time->addItem( QString::number(i));
        }
        m_ui->trgin_invert->setVisible(true);
        m_ui->trgout_invert->setVisible(true);
        m_ui->trgout_time->setVisible(true);
        m_ui->label_trgin_invert->setVisible(true);
        m_ui->label_trgout_invert->setVisible(true);
        m_ui->label_trgout_time->setVisible(true);
        m_ui->line_trigger->setVisible(true);
    }

    connect(m_ui->latency_reset, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->latency_data_max, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->latency_data_error, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->tp_latency, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->tp_offset_first, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->tp_offset, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->tp_number, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->debug_data_format, SIGNAL(stateChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->pushButtonApplyAllFecs, SIGNAL(pressed()),
            this, SLOT(onUpdateFECSettings()));

    connect(m_ui->fec_WarmInit, SIGNAL(clicked()),
            this, SLOT( onUpdateFECSettings() ));
    connect(m_ui->onACQ_FEC, SIGNAL(clicked()),
            this, SLOT( onUpdateFECSettings() ));
    connect(m_ui->offACQ_FEC, SIGNAL(clicked()),
            this, SLOT( onUpdateFECSettings() ));

    connect(m_ui->readSystemParams, SIGNAL(pressed()),
            this, SLOT( onUpdateFECSettings() ));
    connect(m_ui->pushButtonFECIP, SIGNAL(pressed()),
            this, SLOT( onUpdateFECSettings() ));
    connect(m_ui->pushButtonDAQIP, SIGNAL(pressed()),
            this, SLOT( onUpdateFECSettings() ));
    connect(m_ui->pushButtonPowerCycle, SIGNAL(pressed()),
            this, SLOT( onUpdateFECSettings() ));
    connect(m_ui->linkPB, SIGNAL(pressed()),
            this, SLOT( onUpdateFECSettings() ));

    connect(m_ui->comboBoxClockSource, SIGNAL(currentIndexChanged(int)),
            this, SLOT( onUpdateFECSettings() ));

    connect(m_ui->trgin_invert, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->trgout_invert, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateFECSettings()));
    connect(m_ui->trgout_time, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateFECSettings()));


}

void DAQWindow::onUpdateFECSettings(){
    if(QObject::sender() == m_ui->fec_WarmInit){
        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
        m_ui->offACQ_FEC->setCheckable(true);
        m_ui->offACQ_FEC->setChecked(true);
        m_ui->onACQ_FEC->setChecked(false);
        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ResetFEC();
        m_ui->onACQ_FEC->setChecked(false);
        m_ui->offACQ_FEC->setChecked(false);
        m_ui->Send->setEnabled(true);
        QThread::msleep(5);
        CheckLinkStatus(m_fecIndex, false);
    }
    else if(QObject::sender() == m_ui->linkPB){
        CheckLinkStatus(m_fecIndex, false);
    }
    else if(QObject::sender() == m_ui->tp_offset_first){
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
    else if(QObject::sender() == m_ui->pushButtonApplyAllFecs){
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if (m_daq.GetFEC(fec) ){
                m_daq.m_fecs[fec].SetReg("tp_offset_first",  (unsigned long) m_ui->tp_offset_first->value() );
                m_daq.m_fecs[fec].SetReg("tp_offset",  (unsigned long) m_ui->tp_offset->value() );
                m_daq.m_fecs[fec].SetReg("tp_number", (unsigned long) m_ui->tp_number->value() );
                m_daq.m_fecs[fec].SetReg("tp_latency",  (unsigned long)m_ui->tp_latency->value() );
                m_daq.m_fecs[fec].SetReg("latency_reset",  (unsigned long)m_ui->latency_reset->value() );
                m_daq.m_fecs[fec].SetReg("latency_data_max",  (unsigned long)m_ui->latency_data_max->value() );
                m_daq.m_fecs[fec].SetReg("latency_data_error",  (unsigned long)m_ui->latency_data_error->value() );
                m_daq.m_fecs[fec].SetReg("debug_data_format",  (unsigned long)m_ui->debug_data_format->isChecked() );
            }
        }

    }
    else if(QObject::sender() == m_ui->onACQ_FEC){
        m_ui->onACQ_FEC->setCheckable(true);
        m_ui->onACQ_FEC->setChecked(true);
        m_ui->offACQ_FEC->setChecked(false);
        m_ui->Send->setEnabled(false);
        m_ui->comboBoxI2CSetting->setEnabled(false);
        m_ui->sm5_sm0->setEnabled(false);
        m_daq.SendAll(true);
        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQon();
        QThread::msleep(1000);
        CheckLinkStatus(m_fecIndex, false);
    }
    else if(QObject::sender() == m_ui->offACQ_FEC){
        m_ui->offACQ_FEC->setCheckable(true);
        m_ui->offACQ_FEC->setChecked(true);
        m_ui->onACQ_FEC->setChecked(false);
        m_ui->Send->setEnabled(true);
        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
        m_ui->comboBoxI2CSetting->setEnabled(true);
        m_ui->sm5_sm0->setEnabled(true);
        QThread::msleep(1000);
        CheckLinkStatus(m_fecIndex, false);
    }
    else if(QObject::sender() == m_ui->comboBoxClockSource){
        if(m_ui->comboBoxClockSource->currentIndex() != g_clock_source) {
            QMessageBox::warning(this, tr("Clock source"),
                                 "Please change the clock source in the vmmdcs.pro file in the build directory.\nThen run qmake, and recompile the project!",
                                 QMessageBox::Ok);
            m_ui->comboBoxClockSource->setCurrentIndex(g_clock_source);

        }
    }
    else if(QObject::sender() == m_ui->readSystemParams){
        m_ui->debugScreen->clear();
        QMap<QString, QString> registers;
        if(m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ReadSystemRegisters(registers)) {

            QString text;
            stringstream sx;
            if(g_card_name == "FEN") {
                bool ok;
                long boardId = registers["board_id"].toLong(&ok,16);
                m_ui->ip_daq->setText(QString::number(boardId));
                text = QString::fromStdString(g_card_name) + QString::number(m_fecIndex) + " "
                        + QString::number( m_map_id_ring_fen[m_fecIndex].first).rightJustified(2, '0') + "_"
                        + QString::number( m_map_id_ring_fen[m_fecIndex].second).rightJustified(2, '0')
                        + " Board-ID " + registers["board_id"] + " (" + QString::number(boardId) + ")";
                SetStatus(text, m_fecIndex);

                m_daq.m_fecs[m_fecIndex].SetInfo("board_id", registers["board_id"].toStdString());

                sx.str("");
                sx << "**********************\n"
                   << " Board-ID:\n" << registers["board_id"].toStdString() << " (" <<  boardId << ")" << "\n\n"
                   << "**********************";
                cout << sx.str() << endl;
            }
            else {
                text = QString::fromStdString(g_card_name) + QString::number(m_fecIndex) + " " + registers["FirmwareVers"]+" MAC " + registers["MACvendor"]+registers["MACdevice"]+"\nIP " + registers["FECip"] + ",DAQ " + registers["DAQip"];
                SetStatus(text, m_fecIndex);

                m_daq.m_fecs[m_fecIndex].SetInfo("firmware_version", registers["FirmwareVers"].toStdString());
                sx.str("");
                sx << "**********************\n"
                   << " Firmware version:\n" << registers["FirmwareVers"].toStdString() << "\n\n"
                   << " MAC Vendor part :\n" << registers["MACvendor"].toStdString() <<  "\n\n"
                   << " MAC device part:\n" << registers["MACdevice"].toStdString() <<  "\n\n"
                   << " FEC IP:\n" << registers["FECip"].toStdString() <<  "\n\n"
                   << " DAQ destination IP:\n" << registers["DAQip"].toStdString()  <<  "\n"
                   << "**********************";
                cout << sx.str() << endl;
            }

            QString message = QString::fromStdString(sx.str());
            m_ui->debugScreen->insertPlainText(message);
            m_ui->debugScreen->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
        }
    }
    else if(QObject::sender() == m_ui->pushButtonPowerCycle){
        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->PowerCycleHybrids();
    }
    else if(QObject::sender() == m_ui->pushButtonDAQIP){
        if(g_slow_control != 0) {
            long theIP = 0x0a000003;
            bool ok;
            theIP = GetFec("ip_daq");
            QHostAddress ipAddress;
            ipAddress.setAddress(theIP);

            QString result = QInputDialog::getText(this, tr("DAQ IPv4 address"), tr("New IPv4 address:"), QLineEdit::Normal,  ipAddress.toString(), &ok);

            if (ok && !result.isEmpty())
            {
                if (!ipAddress.setAddress(result)){
                    QMessageBox::warning(this, tr("DAQ IPv4 address"),
                                         tr("Invalid IPv4 address!"),
                                         QMessageBox::Ok);
                }
                else {
                    theIP = ipAddress.toIPv4Address();
                    int res = m_daq.CheckIP_FEC(theIP, -1);
                    if(res > -1) {
                        QMessageBox::warning(this, tr("DAQ IPv4 address"),
                                             "DAQ IP address already in use as FEC IP in FEC " + QString::number(res),
                                             QMessageBox::Ok);
                        return;
                    }

                    QMessageBox::StandardButton reply;

                    reply = QMessageBox::question(this, "DAQ IPv4 address setting", "The SRS FEC is connected via ethernet cable to a network card on the slow control PC. The DAQ IP address is the address of this network card, and the FEC card has to know it to send data to the PC.\nATTENTION: Do you only want to change the DAQ IP address in the slow control GUI (press NO), or re-program the DAQ IP address in the FEC EEPROM (press YES)?", QMessageBox::Yes | QMessageBox::No );
                    if(reply == QMessageBox::Yes) {
                        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->writeDAQip(theIP);
                    }
                    QThread::msleep(1);
                    SetFec("ip_daq", theIP);
                    m_ui->ip_daq->setText(result);
                    QThread::msleep(1);
                }
            }
        }
        else {
            long theBoardId = 0;
            bool ok;
            theBoardId = QString::fromStdString(m_daq.m_fecs[m_fecIndex].GetInfo("board_id")).toLong(&ok, 16);
            QString sBoardId = QString::fromStdString(m_daq.m_fecs[m_fecIndex].GetInfo("board_id"));
            QString result = QInputDialog::getText(this, tr("Board ID"), tr("New board id:"), QLineEdit::Normal,  QString::number(theBoardId), &ok);

            if (ok && !result.isEmpty())
            {
                QMessageBox::StandardButton reply;

                reply = QMessageBox::question(this, "Board ID", "The board ID is stored on the EEPROM of the assister board.\nATTENTION: Do you only want to change the board ID in the slow control GUI (press NO), or re-program the board ID in the EEPROM (press YES)?", QMessageBox::Yes | QMessageBox::No );
                if(reply == QMessageBox::Yes) {
                    theBoardId = result.toLong();
                    m_daq.m_fecs[m_fecIndex].m_fecConfigModule->writeBoardId(theBoardId);
                    bool ok;
                    unsigned long id = result.toULong(&ok,10);
                    sBoardId = QString("%1").arg(id, 8, 16);
                }
                QThread::msleep(1);
                m_daq.m_fecs[m_fecIndex].SetInfo("board_id", sBoardId.toStdString());
                m_ui->ip_daq->setText(QString::number(theBoardId));
                QThread::msleep(1000);
            }

        }
    }
    else if(QObject::sender() == m_ui->pushButtonFECIP){
        long theIP = 0x0a000002;
        bool ok;
        theIP = GetFec("ip_fec");
        QHostAddress ipAddress;
        ipAddress.setAddress(theIP);
        QHostAddress newIpAddress;
        QString result = "";
        if(g_slow_control == 2) {
            result = QInputDialog::getText(this, tr("FEC IPv4 address"), tr("New IPv4 address:"), QLineEdit::Normal, ipAddress.toString(), &ok);
        }
        else if(g_slow_control == 0) {
            result = QInputDialog::getText(this, tr("Master IPv4 address"), tr("New IPv4 address:"), QLineEdit::Normal, ipAddress.toString(), &ok);
        }
        else {
            result = QInputDialog::getText(this, tr("FEN IPv4 address"), tr("New IPv4 address:"), QLineEdit::Normal, ipAddress.toString(), &ok);
        }

        if (ok && !result.isEmpty())
        {
            if (!ipAddress.setAddress(result)){
                if(g_slow_control == 2) {
                    QMessageBox::warning(this, QString::fromStdString(g_card_name) + tr(" IPv4 address setting"),
                                         tr("Invalid IPv4 address!"),
                                         QMessageBox::Ok);
                }
                else if(g_slow_control == 0) {
                    QMessageBox::warning(this, tr("Master IPv4 address setting"),
                                         tr("Invalid IPv4 address!"),
                                         QMessageBox::Ok);
                }
                else {
                    QMessageBox::warning(this, tr("FEN IPv4 address setting"),
                                         tr("Invalid IPv4 address!"),
                                         QMessageBox::Ok);
                }

            }
            else {
                theIP = ipAddress.toIPv4Address();

                if(g_slow_control > 0) {

                    int res = m_daq.CheckIP_FEC(theIP, m_fecIndex);
                    if(res > -1) {
                        if(g_slow_control == 2) {
                            QMessageBox::warning(this, QString::fromStdString(g_card_name) + tr(" IPv4 address setting"),
                                                 "The last octet of the IP address is the " + QString::fromStdString(g_card_name) + " ID, and has to be hence unique.\n"
                                                                                                                                    "Last octet of " + QString::fromStdString(g_card_name) + " IP address already in use in " + QString::fromStdString(g_card_name) + " " + QString::number(res),
                                                 QMessageBox::Ok);
                        }
                        else {
                            QString ring =  m_daq.m_fecs[res].GetReg("ring");
                            QString fen =  m_daq.m_fecs[res].GetReg("fen");

                            QMessageBox::warning(this, tr("FEN IPv4 address setting"),
                                                 "The FEN IPv4 address has to be unique.\n"
                                                 "Last octet of FEN IP address already in use in ring " + ring + ", FEN " +  fen + " !",
                                                 QMessageBox::Ok);
                        }
                        return;
                    }
                    res = m_daq.CheckIP_DAQ(theIP);
                    if(res > -1) {
                        QMessageBox::warning(this, QString::fromStdString(g_card_name) + tr(" IPv4 address setting"),
                                             QString::fromStdString(g_card_name) + " IP address already in use as DAQ IP in " + QString::fromStdString(g_card_name) + " " + QString::number(res),
                                             QMessageBox::Ok);
                        return;
                    }
                }
                QMessageBox::StandardButton reply;
                if(g_connection_ok) {
                    if(g_slow_control == 2) {
                        reply = QMessageBox::question(this,  "FEC IPv4 address setting", "The SRS FEC is connected via ethernet cable to a network card on the slow control PC. The FEC IP address is the address to which the PC sends its commands. The FEC stores its own IP address on the FEC EEPROM.\nATTENTION: Do you only want to change the FEC IP address in the slow control GUI (press NO), or re-program the FEC IP address in the FEC EEPROM (press YES)?", QMessageBox::Yes | QMessageBox::No );
                    }
                    else if(g_slow_control == 1) {
                        reply = QMessageBox::question(this,  "FEN IPv4 address setting", "The assister FEN is connected via ethernet cable to a network card on the slow control PC. The FEN IP address is the address to which the PC sends its commands. The FEN stores its own IP address on the FEN EEPROM.\nATTENTION: Do you only want to change the FEN IP address in the slow control GUI (press NO), or re-program the FEN IP address in the FEN EEPROM (press YES)?", QMessageBox::Yes | QMessageBox::No );
                    }
                    else {
                        reply = QMessageBox::information(this, "Master IPv4 address setting", "The Master IP address in the slow control GUI will be changed!", QMessageBox::Ok);
                        if(reply == QMessageBox::Ok) {
                            SetFec("ip_fec", theIP);
                            m_ui->ip_fec->setText(result);
                            return;
                        }
                    }
                    if(reply == QMessageBox::Yes) {
                        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->writeFECip(theIP);
                    }
                    QThread::msleep(1);
                    SetFec("ip_fec", theIP);
                    m_daq.m_fecs[m_fecIndex].SetId();
                    m_ui->ip_fec->setText(result);
                    QThread::msleep(1);

                }
                else {
                    if(g_slow_control > 0) {
                        reply = QMessageBox::information(this, QString::fromStdString(g_card_name) + " IPv4 address setting", "The " + QString::fromStdString(g_card_name) + " IP address in the slow control GUI will be changed!", QMessageBox::Ok);
                    }
                    else {
                        reply = QMessageBox::information(this, "Master IPv4 address setting", "The Master IP address in the slow control GUI will be changed!", QMessageBox::Ok);

                    }
                    if(reply == QMessageBox::Ok) {
                        SetFec("ip_fec", theIP);
                        m_daq.m_fecs[m_fecIndex].SetId();
                        m_ui->ip_fec->setText(result);
                    }
                }

            }
        }
    }
}


// ------------------------------------------------------------------------- //
void DAQWindow::SetFECToolTips()
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


}



void DAQWindow::EnableFECCommunicationButtons(bool enable) {
    m_ui->onACQ_FEC->setEnabled(enable);
    m_ui->offACQ_FEC->setEnabled(enable);
    m_ui->fec_WarmInit->setEnabled(enable);
    m_ui->linkPB->setEnabled(enable);
    m_ui->readSystemParams->setEnabled(enable);
    m_ui->pushButtonDAQIP->setEnabled(enable);
    m_ui->pushButtonApplyAllFecs->setEnabled(enable);
}





void DAQWindow::LoadFECSettings(){

    QHostAddress ipAddress;
    ipAddress.setAddress(GetFec( "ip_fec" ));
    m_ui->ip_fec->setText( ipAddress.toString());
    if(g_slow_control == 0) {
        bool ok;
        long boardId = QString::fromStdString(m_daq.m_fecs[m_fecIndex].GetInfo("board_id")).toLong(&ok, 16);
        m_ui->ip_daq->setText(QString::number(boardId));
    }
    else {
        ipAddress.setAddress(GetFec( "ip_daq" ));
        m_ui->ip_daq->setText( ipAddress.toString());
    }



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

bool DAQWindow::SetFec(const char *feature, unsigned long val){
    return m_daq.m_fecs[m_fecIndex].SetReg(feature,  (unsigned long) val );
}
unsigned long DAQWindow::GetFec(const char *feature){
    return m_daq.m_fecs[m_fecIndex].GetRegVal(feature);
}


void DAQWindow::InitHybridWidgets() {
    m_ui->tpSkew->clear();
    for(int i=0;i < 2; i++) {
        for(int n=0;n< 8; n++) {
            QString txt = QStringLiteral("%1 ns").arg((i*g_clock_period)+n*g_clock_period/8.0);
            m_ui->tpSkew->addItem(txt);
        }
    }

    m_ui->tpWidth->clear();
    for(int n=128;n> 0; n=n/2) {
        QString txt = QStringLiteral("%1x%2 ns").arg(n).arg(g_clock_period);
        m_ui->tpWidth->addItem(txt);
    }

    if(g_clock_source == 0) {
        m_ui->ckbc_info->setText("44.03 MHz");
        m_ui->ckdt_info->setText("176.11 MHz");
    }
    if(g_clock_source == 3) {
        m_ui->ckbc_info->setText("40 MHz");
        m_ui->ckdt_info->setText("180 MHz");
    }
    else {
        m_ui->ckbc_info->setText("44.44 MHz");
        m_ui->ckdt_info->setText("177.78 MHz");
    }
    if(g_clock_source == 2 && g_slow_control == 2) {
        m_ui->tpDisable->setVisible(true);
    }
    else {
        m_ui->tpDisable->setVisible(false);
    }
    connect(m_ui->tpSkew, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateHybridSettings()));
    connect(m_ui->tpWidth, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateHybridSettings()));
    connect(m_ui->tpPolarity, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateHybridSettings()));
    connect(m_ui->tpDisable, SIGNAL(stateChanged(int)),
            this, SLOT(onUpdateHybridSettings()));

    connect(m_ui->ApplyAllHybrids, SIGNAL(clicked()),
            this, SLOT(onUpdateHybridSettings()));
    connect(m_ui->ReadI2C, SIGNAL(clicked()),
            this, SLOT(onUpdateHybridSettings()));
}


void DAQWindow::EnableHybridCommunicationButtons(bool enable) {
    m_ui->ApplyAllHybrids->setEnabled(enable);
    m_ui->ReadI2C->setEnabled(enable);
    m_ui->pushButtonMeasureTemp->setEnabled(enable);
}



void DAQWindow::LoadHybridSettings(){
    m_ui->tpSkew->setCurrentIndex(GetHybrid("TP_skew"));
    m_ui->tpWidth->setCurrentIndex(GetHybrid("TP_width"));
    m_ui->tpPolarity->setCurrentIndex(GetHybrid("TP_pol"));
    QString text = QString::fromStdString(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].GetInfo("firmware_version"));
    m_ui->firmwareVersion->setText(text);
    text = QString::fromStdString(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].GetInfo("hybrid_id"));
    QString str;
    str = str + text.mid(0,16) + "\n";
    str = str + text.mid(16,16);

    m_ui->hybridID->setText(str);


    text = QString::fromStdString(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].GetInfo("geo_id"));
    m_ui->geoID->setText(text);

}

bool DAQWindow::SetHybrid(std::string feature, unsigned short value){
    if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].SetReg(feature, value)){
        return true;
    }
    else return false;
}
unsigned short DAQWindow::GetHybrid(std::string feature){
    return m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].GetReg(feature);
}

void DAQWindow::onUpdateHybridSettings(){
    if(QObject::sender() == m_ui->tpSkew){
        if( m_ui->tpSkew->currentIndex() != -1) {
            SetHybrid("TP_skew", m_ui->tpSkew->currentIndex());
        }
    }
    else if(QObject::sender() == m_ui->tpWidth){
        SetHybrid("TP_width", m_ui->tpWidth->currentIndex());
    }
    else if(QObject::sender() == m_ui->tpPolarity){
        SetHybrid("TP_pol", m_ui->tpPolarity->currentIndex());
    }
    else if(QObject::sender() == m_ui->tpDisable){
        SetHybrid("TP_disable", m_ui->tpDisable->isChecked());
    }
    else if(QObject::sender() == m_ui->ApplyAllHybrids){
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if (m_daq.GetFEC(fec) ){
                for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_FEC; hybrid++){
                    if( m_daq.m_fecs[fec].GetHybrid(hybrid) ){
                        m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", m_ui->tpSkew->currentIndex());
                        m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_width", m_ui->tpWidth->currentIndex());
                        m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_pol", m_ui->tpPolarity->currentIndex());

                    }
                }
            }
        }
    }
    else if(QObject::sender() == m_ui->ReadI2C){
        QString result = m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ReadI2C(m_hybridIndex, m_ui->cbChoiceI2C->currentIndex());
        QString text = "";
        for(int n=0; n < result.size(); n=n+16) {
            text += result.mid(n,16);
            text += "\n";
        }
        text = text.mid(0, text.size()-1);
        if(m_ui->cbChoiceI2C->currentIndex() == 0) {
            m_ui->hybridID->setText(text);
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].SetInfo("hybrid_id", result.toStdString());
        }
        else if(m_ui->cbChoiceI2C->currentIndex() == 1) {
            m_ui->firmwareVersion->setText(text);
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].SetInfo("firmware_version", result.toStdString());
        }
        else if(m_ui->cbChoiceI2C->currentIndex() == 2) {
            m_ui->geoID->setText(text);
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].SetInfo("geo_id", result.toStdString());
        }
    }
}


void DAQWindow::InitVMMWidgets() {
    SetVMMToolTips();

    connect(m_ui->nskipm_i, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sL0cktest, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sL0ckinv, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sL0dckinv, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sL0ena, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->truncate_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->nskip_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->window_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->rollover_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->L0offset_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->offset_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->s32, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->stcr, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->ssart, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->srec, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->stlc, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sbip, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->srat, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->slvsbc, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slvstp, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slvstk, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slvsdt, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slvsart, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slvstki, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slvsena, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slvs6b, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->sL0enaV, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slh, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slxh, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->stgc, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));


    connect(m_ui->sdt, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sdp10, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sc10b, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sc8b, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->s10b, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->s8b, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sfrst, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sdcks, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sdcka, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));


    connect(m_ui->sp, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sdp, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sbmx, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sbft, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sbfp, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sbfm, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->slg, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->sm5_sm0, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->sfa, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sfam, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->st, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sfm, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->sg, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->sng, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->ssh, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->stc, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->sttt, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->stot, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->stpp, SIGNAL(pressed()),
            this, SLOT(onUpdateVMMSettings()));



    connect(m_ui->ApplyAllVMM0, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->ApplyAllVMM1, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->ChannelSettingsAllVMM0, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->ChannelSettingsAllVMM1, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->cbIncludeThreshold, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->cbVMM0And1, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->vmmResetAll, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->vmmReset, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->readADC, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));

    connect(m_ui->pbThresholdMinus, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));
    connect(m_ui->pbThresholdPlus, SIGNAL(clicked()),
            this, SLOT(onUpdateVMMSettings()));

}



// ------------------------------------------------------------------------- //
void DAQWindow::SetVMMToolTips()
{
    //////////////////////////////////////////////////////////////////////////
    // -------------------------------------------------------------------- //
    // Creates all the ToolTips for Push bottons etc.
    // -------------------------------------------------------------------- //
    //////////////////////////////////////////////////////////////////////////
    m_ui->sfa->setToolTip("ART enable");
    m_ui->sfam->setToolTip("[0] timing at threshold, [1] timing at peak");


    m_ui->slvsbc->setToolTip("slvs 100 Ω termination on ckbc");
    m_ui->slvstp->setToolTip("slvs 100 Ω termination on cktp");
    m_ui->slvstk->setToolTip("slvs 100 Ω termination on cktk");
    m_ui->slvsdt->setToolTip("slvs 100 Ω termination on ckdt");
    m_ui->slvsart->setToolTip("slvs 100 Ω termination on ckart");
    m_ui->slvstki->setToolTip("slvs 100 Ω termination on cktki");
    m_ui->slvsena->setToolTip("slvs 100 Ω termination on ckena");
    m_ui->slvs6b->setToolTip("slvs 100 Ω termination on ck6b");
    m_ui->stlc->setToolTip("mild tail cancellation (when enabled, overrides sbip)");
    m_ui->nskipm_i->setToolTip("magic number on BCID - 0xFE8");
    m_ui->s32->setToolTip("skips channels 16-47 and makes 15 and 48 neighbors");
    m_ui->sL0ckinv->setToolTip("invert BCCLK");
    m_ui->sL0dckinv->setToolTip("invert DCK");
    m_ui->sbip->setToolTip("bipolar shape");

    m_ui->srec->setToolTip("fast recovery from high charge");
    m_ui->sratLabel->setToolTip("Enables timing ramp at threshold");
    m_ui->ssart->setToolTip("ART flag synchronization (trail to next trail)");
    m_ui->stcr->setToolTip("auto-reset (at the end of the ramp, if no stop occurs)");

    m_ui->spgLabel->setToolTip("input charge polarity");
    m_ui->sdp->setToolTip("disable-at-peak");
    m_ui->sbmx->setToolTip("routes analog monitor to PDO output");
    m_ui->sbft->setToolTip("analog output buffers, enable TDO");
    m_ui->sbfp->setToolTip("analog output buffers, enable PDO");
    m_ui->sbfm->setToolTip("analog output buffers, enable MO");
    m_ui->slg->setToolTip("disable leakage current");
    m_ui->sfm->setToolTip("enables dynamic discharge for AC coupling");
    m_ui->sng->setToolTip("neighbor (channel and chip) triggering enable");

    m_ui->ssh->setToolTip("enables sub-hysteresis discrimination");
    m_ui->sc010bLabel->setToolTip("10-bit ADC conversion time");
    m_ui->sc08bLabel->setToolTip("8-bit ADC conversion time");
    m_ui->s8b->setToolTip("8-bit ADC conversion mode");
    m_ui->s10b->setToolTip("enables high resolution ADCs (10/8-bit ADC enable)");
    m_ui->sdcks->setToolTip("dual clock edge serialized data enable");
    m_ui->sdcka->setToolTip("dual clock edge serialized ART enable");

    m_ui->vmmReset->setToolTip("Hard reset this VMM");
    m_ui->vmmResetAll->setToolTip("Hard reset all activated VMMs");

    m_ui->spinBoxThreshold->setToolTip("Change global threshold for all VMM");
    m_ui->pbThresholdMinus->setToolTip("Increment global threshold for all VMM");
    m_ui->pbThresholdPlus->setToolTip("Decrement global threshold for all VMM");



}

// ------------------------------------------------------------------------- //
void DAQWindow::SetVMMChannelToolTips()
{
    m_ui->SCLabel->setToolTip("large sensor capacitance mode (off <∼200 pF ,on >∼200 pF )");
    m_ui->SLLabel->setToolTip("leakage current disable [0=enabled]");
    m_ui->STLabel->setToolTip("300 fF test capacitor [1=enabled]");
    m_ui->STHLabel->setToolTip("multiplies test capacitor by 10");
    m_ui->SMLabel->setToolTip("mask enable");
    m_ui->SDLabel->setToolTip("trim threshold DAC");
    m_ui->SZ010bLabel->setToolTip("10-bit ADC zero");
    m_ui->SZ08bLabel->setToolTip("8-bit ADC zero");
    m_ui->SZ06bLabel->setToolTip("6-bit ADC zero");
    m_ui->SMXLabel->setToolTip("channel monitor mode ( [0] analog output, [1] trimmed threshold))");
    // ------------------------------------------------------------------------- //
}

unsigned short DAQWindow::GetVMM(std::string feature, int ch){
    unsigned short setting = m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetRegister(feature, ch);
    return setting;
}
// ------------------------------------------------------------------------- //

bool DAQWindow::SetVMM(std::string feature, int value, int ch){
    if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}

bool DAQWindow::SetVMM(std::string feature, bool value, int ch){
    if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}
// ------------------------------------------------------------------------- //


void DAQWindow::LoadVMMSettings()
{
    m_ui->nskipm_i->setChecked(GetVMM("nskipm_i"));
    m_ui->sL0cktest->setChecked(GetVMM("sL0cktest"));
    m_ui->sL0ckinv->setChecked(GetVMM("sL0ckinv"));
    m_ui->sL0dckinv->setChecked(GetVMM("sL0dckinv"));
    m_ui->sL0ena->setChecked(GetVMM("sL0ena"));

    m_ui->truncate_i->setValue(GetVMM("truncate_i"));
    m_ui->nskip_i->setValue(GetVMM("nskip_i"));
    m_ui->window_i->setValue(GetVMM("window_i"));
    m_ui->rollover_i->setValue(GetVMM("rollover_i"));
    m_ui->L0offset_i->setValue(GetVMM("L0offset_i"));
    m_ui->offset_i->setValue(GetVMM("offset_i"));

    m_ui->s32->setChecked(GetVMM("s32"));
    m_ui->stcr->setChecked(GetVMM("stcr"));
    m_ui->ssart->setChecked(GetVMM("ssart"));
    m_ui->srec->setChecked(GetVMM("srec"));
    m_ui->stlc->setChecked(GetVMM("stlc"));
    m_ui->sbip->setChecked(GetVMM("sbip"));
    m_ui->srat->setCurrentIndex(GetVMM("srat"));

    m_ui->slvsbc->setChecked(GetVMM("slvsbc"));
    m_ui->slvstp->setChecked(GetVMM("slvstp"));
    m_ui->slvstk->setChecked(GetVMM("slvstk"));
    m_ui->slvsdt->setChecked(GetVMM("slvsdt"));
    m_ui->slvsart->setChecked(GetVMM("slvsart"));
    m_ui->slvstki->setChecked(GetVMM("slvstki"));
    m_ui->slvsena->setChecked(GetVMM("slvsena"));
    m_ui->slvs6b->setChecked(GetVMM("slvs6b"));

    m_ui->sL0enaV->setChecked(GetVMM("sL0enaV"));
    m_ui->slh->setChecked(GetVMM("slh"));
    m_ui->slxh->setChecked(GetVMM("slxh"));
    m_ui->stgc->setChecked(GetVMM("stgc"));

    m_ui->sttt->setChecked(GetVMM("sttt"));
    m_ui->stpp->setChecked(GetVMM("stpp"));
    m_ui->stot->setChecked(GetVMM("stot"));

    m_ui->sdt->setValue(GetVMM("sdt"));
    int sdp10 = GetVMM("sdp10");
    m_ui->sdp10->setValue(sdp10);
    QString tmp;


    double pulseHeight = m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp10->value(), m_ui->sg->currentIndex());
    double dav_mV = m_calib->PulserDAC_to_mV(m_ui->sdp10->value());
    if(m_ui->sp->currentIndex() == 0) {
        m_ui->dacmvLabel_TP->setText("about "+ tmp.number(pulseHeight*0.001, 'f', 2) + " V pulse height");
        if(pulseHeight > 1200) {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
            m_ui->sdp10->setStyleSheet("QSpinBox { background-color : red; color : white; }");
        }
        else {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
            m_ui->sdp10->setStyleSheet("QSpinBox { background-color : white; color : black; }");
        }
    }
    m_ui->sc10b->setCurrentIndex(GetVMM("sc10b"));
    m_ui->sc8b->setCurrentIndex(GetVMM("sc8b"));
    m_ui->s10b->setChecked(GetVMM("s10b"));
    m_ui->s8b->setChecked(GetVMM("s8b"));
    m_ui->sfrst->setChecked(GetVMM("sfrst"));
    m_ui->sdcka->setChecked(GetVMM("sdcka"));
    m_ui->sdcks->setChecked(GetVMM("sdcks"));
    m_ui->sp->setCurrentIndex(GetVMM("sp"));
    m_ui->sdp->setChecked(GetVMM("sdp"));

    m_ui->sbmx->setChecked(GetVMM("sbmx"));
    m_ui->sbft->setChecked(GetVMM("sbft"));
    m_ui->sbfp->setChecked(GetVMM("sbfp"));
    m_ui->sbfm->setChecked(GetVMM("sbfm"));
    m_ui->slg->setChecked(GetVMM("slg"));

    if(GetVMM("scmx") == 1){
        m_ui->sm5_sm0->setCurrentIndex(GetVMM("sm5_sm0")+4);
    }
    else m_ui->sm5_sm0->setCurrentIndex(GetVMM("sm5_sm0")-1);

    m_ui->sfa->setCurrentIndex(GetVMM("sfa"));
    m_ui->sfam->setCurrentIndex(GetVMM("sfam"));
    m_ui->st->setCurrentIndex(GetVMM("st"));
    m_ui->sfm->setChecked(GetVMM("sfm"));
    m_ui->sdp->setChecked(GetVMM("sdp"));

    m_ui->sg->setCurrentIndex(GetVMM("sg"));
    m_ui->sng->setChecked(GetVMM("sng"));
    m_ui->ssh->setChecked(GetVMM("ssh"));
    m_ui->stc->setCurrentIndex(GetVMM("stc"));
    QString text = QString::fromStdString(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetInfo("adc_value"));
    m_ui->ADCresult->setText(text);

    if(m_ui->cbVMM0And1->isChecked()) {
        m_ui->ApplyAllVMM0->setText("Global Settings");
        m_ui->ApplyAllVMM1->setVisible(false);
        m_ui->labelChangeThreshold->setText("Change threshold all VMM");
    }
    else {
        m_ui->ApplyAllVMM0->setText("Global Settings VMM 0");
        m_ui->ApplyAllVMM1->setVisible(true);
        if(m_vmmIndex == 0) {
            m_ui->labelChangeThreshold->setText("Change threshold all VMM 0");
        }
        else {
            m_ui->labelChangeThreshold->setText("Change threshold all VMM 1");
        }
    }
    m_ui->labelChangeThreshold->setStyleSheet("QLabel { color : red; }");

}

void DAQWindow::LoadVMMChannelSettings() {
    int sd = -1;
    int SZ010 = -1;
    int SZ08 = -1;
    int SZ06 = -1;


    QRegularExpression exp("VMMSC_");

    QList<QPushButton *> listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        listPushButtons[n]->setStyleSheet("");
        if(GetVMM("sc",n)) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }

    exp.setPattern("VMMSL_");
    listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        listPushButtons[n]->setStyleSheet("");
        if(GetVMM("sl",n)) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }

    exp.setPattern("VMMST_");
    listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        listPushButtons[n]->setStyleSheet("");
        if(GetVMM("st",n)) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }

    exp.setPattern("VMMSTH_");
    listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        listPushButtons[n]->setStyleSheet("");
        if(GetVMM("sth",n)) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }

    exp.setPattern("VMMSM_");
    listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        listPushButtons[n]->setStyleSheet("");
        if(GetVMM("sm",n)) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }

    exp.setPattern("VMMSMX_");
    listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        listPushButtons[n]->setStyleSheet("");
        if(GetVMM("smx",n)) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }


    exp.setPattern("VMMSDVoltage_");
    QList<QComboBox *> listComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
    std::sort(listComboBoxes.begin(), listComboBoxes.end(),
              [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listComboBoxes.size(); n++) {
        unsigned short value = GetVMM("sd",n);
        listComboBoxes[n]->setCurrentIndex(value);
    }

    exp.setPattern("VMMSZ010b_");
    listComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
    std::sort(listComboBoxes.begin(), listComboBoxes.end(),
              [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listComboBoxes.size(); n++) {
        unsigned short value = GetVMM("sz10b",n);
        listComboBoxes[n]->setCurrentIndex(value);
    }

    exp.setPattern("VMMSZ08b_");
    listComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
    std::sort(listComboBoxes.begin(), listComboBoxes.end(),
              [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listComboBoxes.size(); n++) {
        unsigned short value = GetVMM("sz08b",n);
        listComboBoxes[n]->setCurrentIndex(value);
    }

    exp.setPattern("VMMSZ06b_");
    listComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
    std::sort(listComboBoxes.begin(), listComboBoxes.end(),
              [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listComboBoxes.size(); n++) {
        unsigned short value = GetVMM("sz06b",n);
        listComboBoxes[n]->setCurrentIndex(value);
    }

    for (int i = 0; i<64; i++){
        // set initial ADC values
        unsigned short ADC10_index = GetVMM("sz10b",i);
        unsigned short ADC08_index = GetVMM("sz08b",i);
        unsigned short ADC06_index = GetVMM("sz06b",i);
        // set initial channel voltage
        unsigned short SD_volt = GetVMM("sd",i);
        if(sd != -1 && sd != SD_volt) {
            sd = 0;
        }
        else {
            sd = SD_volt;
        }
        if(SZ010 != -1 && SZ010 != ADC10_index) {
            SZ010 = 0;
        }
        else {
            SZ010 = ADC10_index;
        }
        if(SZ08 != -1 && SZ08 != ADC08_index) {
            SZ08 = 0;
        }
        else {
            SZ08 = ADC08_index;
        }
        if(SZ06 != -1 && SZ06 != ADC06_index) {
            SZ06 = 0;
        }
        else {
            SZ06 = ADC06_index;
        }
    }
    m_ui->SDLabel->setCurrentIndex(sd);
    m_ui->SZ010bLabel->setCurrentIndex(SZ010);
    m_ui->SZ08bLabel->setCurrentIndex(SZ08);
    m_ui->SZ06bLabel->setCurrentIndex(SZ06);
}


void DAQWindow::onUpdateVMMSettings()
{
    QString tmp;

    if(QObject::sender() == m_ui->nskipm_i){
        SetVMM("nskipm_i", !m_ui->nskipm_i->isChecked());
    }
    else if(QObject::sender() == m_ui->sL0cktest){
        SetVMM("sL0cktest", !m_ui->sL0cktest->isChecked());
    }
    else if(QObject::sender() == m_ui->sL0ckinv){
        SetVMM("sL0ckinv", !m_ui->sL0ckinv->isChecked());
    }
    else if(QObject::sender() == m_ui->sL0dckinv){
        SetVMM("sL0dckinv", !m_ui->sL0dckinv->isChecked());
    }
    else if(QObject::sender() == m_ui->sL0ena){
        SetVMM("sL0ena", !m_ui->sL0ena->isChecked());
    }
    else if(QObject::sender() == m_ui->nskip_i){
        SetVMM("nskip_i", m_ui->nskip_i->value());
    }
    else if(QObject::sender() == m_ui->window_i){
        SetVMM("window_i", m_ui->window_i->value());
    }
    else if(QObject::sender() == m_ui->rollover_i){
        SetVMM("rollover_i", m_ui->rollover_i->value());
    }
    else if(QObject::sender() == m_ui->L0offset_i){
        SetVMM("L0offset_i", m_ui->L0offset_i->value());
    }
    else if(QObject::sender() == m_ui->offset_i){
        SetVMM("offset_i", m_ui->offset_i->value());
    }

    else if(QObject::sender() == m_ui->s32){
        SetVMM("s32", !m_ui->s32->isChecked());
    }
    else if(QObject::sender() == m_ui->stcr){
        SetVMM("stcr", !m_ui->stcr->isChecked());
    }
    else if(QObject::sender() == m_ui->ssart){
        SetVMM("ssart", !m_ui->ssart->isChecked());
    }
    else if(QObject::sender() == m_ui->srec){
        SetVMM("srec", !m_ui->srec->isChecked());
    }
    else if(QObject::sender() == m_ui->stlc){
        SetVMM("stlc", !m_ui->stlc->isChecked());
    }
    else if(QObject::sender() == m_ui->sbip){
        SetVMM("sbip", !m_ui->sbip->isChecked());
    }
    else if(QObject::sender() == m_ui->srat){
        SetVMM("srat", m_ui->srat->currentIndex());
    }

    else if(QObject::sender() == m_ui->slvsbc){
        SetVMM("slvsbc", !m_ui->slvsbc->isChecked());
    }
    else if(QObject::sender() == m_ui->slvstp){
        SetVMM("slvstp", !m_ui->slvstp->isChecked());
    }
    else if(QObject::sender() == m_ui->slvstk){
        SetVMM("slvstk", !m_ui->slvstk->isChecked());
    }
    else if(QObject::sender() == m_ui->slvsdt){
        SetVMM("slvsdt", !m_ui->slvsdt->isChecked());
    }
    else if(QObject::sender() == m_ui->slvsart){
        SetVMM("slvsart", !m_ui->slvsart->isChecked());
    }
    else if(QObject::sender() == m_ui->slvstki){
        SetVMM("slvstki", !m_ui->slvstki->isChecked());
    }
    else if(QObject::sender() == m_ui->slvsena){
        SetVMM("slvsena", !m_ui->slvsena->isChecked());
    }
    else if(QObject::sender() == m_ui->slvs6b){
        SetVMM("slvs6b", !m_ui->slvs6b->isChecked());
    }

    else if(QObject::sender() == m_ui->sL0enaV){
        SetVMM("sL0enaV", m_ui->sL0enaV->isChecked());
    }
    else if(QObject::sender() == m_ui->slh){
        SetVMM("slh", m_ui->slh->isChecked());
    }
    else if(QObject::sender() == m_ui->slxh){
        SetVMM("slxh", m_ui->slxh->isChecked());
    }
    else if(QObject::sender() == m_ui->stgc){
        SetVMM("stgc", m_ui->stgc->isChecked());
    }
    if(QObject::sender() == m_ui->sdt){
        SetVMM("sdt", m_ui->sdt->value());
    }
    else if(QObject::sender() == m_ui->sdp10){
        double pulseHeight = m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp10->value(), m_ui->sg->currentIndex());
        double dav_mV = m_calib->PulserDAC_to_mV(m_ui->sdp10->value());
        if(m_ui->sp->currentIndex() == 0) {
            m_ui->dacmvLabel_TP->setText("about "+ tmp.number(pulseHeight*0.001, 'f', 2) + " V pulse height");
            if(pulseHeight > 1200) {
                m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
                m_ui->sdp10->setStyleSheet("QSpinBox { background-color : red; color : white; }");
            }
            else {
                m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
                m_ui->sdp10->setStyleSheet("QSpinBox { background-color : white; color : black; }");
            }
        }
        SetVMM("sdp10", m_ui->sdp10->value());
    }

    else if(QObject::sender() == m_ui->sc10b){
        SetVMM("sc10b", m_ui->sc10b->currentIndex());
    }
    else if(QObject::sender() == m_ui->sc8b){
        SetVMM("sc8b", m_ui->sc8b->currentIndex());
    }
    else if(QObject::sender() == m_ui->s10b){
        SetVMM("s10b", !m_ui->s10b->isChecked());
    }
    else if(QObject::sender() == m_ui->s8b){
        SetVMM("s8b", !m_ui->s8b->isChecked());
    }
    else if(QObject::sender() == m_ui->sfrst){
        SetVMM("sfrst", !m_ui->sfrst->isChecked());
    }
    else if(QObject::sender() == m_ui->sdcks){
        SetVMM("sdcks", !m_ui->sdcks->isChecked());
    }
    else if(QObject::sender() == m_ui->sdcka){
        SetVMM("sdcka", !m_ui->sdcka->isChecked());
    }
    else if(QObject::sender() == m_ui->sp){
        SetVMM("sp", m_ui->sp->currentIndex());
        double pulseHeight = m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp10->value(), m_ui->sg->currentIndex());
        double dav_mV = m_calib->PulserDAC_to_mV(m_ui->sdp10->value());
        if(m_ui->sp->currentIndex() == 0) {
            m_ui->dacmvLabel_TP->setText("about "+ tmp.number(pulseHeight*0.001, 'f', 2) + " V pulse height");
            if(pulseHeight > 1200) {
                m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
                m_ui->sdp10->setStyleSheet("QSpinBox { background-color : red; color : white; }");
            }
            else {
                m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
                m_ui->sdp10->setStyleSheet("QSpinBox { background-color : white; color : black; }");
            }
        }
    }
    else if(QObject::sender() == m_ui->sdp){
        SetVMM("sdp", !m_ui->sdp->isChecked());
    }
    else if(QObject::sender() == m_ui->sbmx){
        SetVMM("sbmx", !m_ui->sbmx->isChecked());
    }
    else if(QObject::sender() == m_ui->sbft){
        SetVMM("sbft", !m_ui->sbft->isChecked());
    }
    else if(QObject::sender() == m_ui->sbfp){
        SetVMM("sbfp", !m_ui->sbfp->isChecked());
    }
    else if(QObject::sender() == m_ui->sbfm){
        SetVMM("sbfm", !m_ui->sbfm->isChecked());
    }
    else if(QObject::sender() == m_ui->slg){
        SetVMM("slg", !m_ui->slg->isChecked());
    }
    else if(QObject::sender() == m_ui->sttt){
        SetVMM("sttt", !m_ui->sttt->isChecked());
    }
    else if(QObject::sender() == m_ui->stot){
        SetVMM("stot", !m_ui->stot->isChecked());
    }
    else if(QObject::sender() == m_ui->stpp){
        SetVMM("stpp", !m_ui->stpp->isChecked());
    }
    else if(QObject::sender() == m_ui->sm5_sm0){
        if(m_ui->sm5_sm0->currentIndex()<= 3){
            SetVMM("sm5_sm0",m_ui->sm5_sm0->currentIndex()+64);
        }
        else {
            SetVMM("sm5_sm0", m_ui->sm5_sm0->currentIndex()-4);
        }
        m_ui->ADCresult->clear();
    }
    //sfa
    else if(QObject::sender() == m_ui->sfa){
        SetVMM("sfa", m_ui->sfa->currentIndex());
    }
    else if(QObject::sender() == m_ui->sfam){
        if(m_ui->sfam->currentIndex() == 1) {
            SetVMM("sfam", 1);
        }
        else SetVMM("sfam", 0);
    }
    else if(QObject::sender() == m_ui->st){
        SetVMM("st", m_ui->st->currentIndex());
    }
    else if(QObject::sender() == m_ui->sfm){
        SetVMM("sfm", !m_ui->sfm->isChecked());
    }
    else if(QObject::sender() == m_ui->sg){
        SetVMM("sg", m_ui->sg->currentIndex());
        double pulseHeight = m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp10->value(), m_ui->sg->currentIndex());
        double dav_mV = m_calib->PulserDAC_to_mV(m_ui->sdp10->value());
        if(m_ui->sp->currentIndex() == 0) {
            m_ui->dacmvLabel_TP->setText("about "+ tmp.number(pulseHeight*0.001, 'f', 2) + " V pulse height");
            if(pulseHeight > 1200) {
                m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
                m_ui->sdp10->setStyleSheet("QSpinBox { background-color : red; color : white; }");
            }
            else {
                m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
                m_ui->sdp10->setStyleSheet("QSpinBox { background-color : white; color : black; }");
            }
        }
    }
    else if(QObject::sender() == m_ui->sng){
        SetVMM("sng", !m_ui->sng->isChecked());
    }

    else if(QObject::sender() == m_ui->ssh){
        SetVMM("ssh", !m_ui->ssh->isChecked());
    }
    else if(QObject::sender() == m_ui->stc){
        SetVMM("stc", m_ui->stc->currentIndex());
    }
    else if(QObject::sender() == m_ui->pbThresholdMinus){

        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if ( m_daq.GetFEC(fec) ){
                for (unsigned short hyb=0; hyb < HYBRIDS_PER_FEC; hyb++){
                    if( m_daq.m_fecs[fec].GetHybrid(hyb) ){
                        //VMM 0 and 1
                        if(m_ui->cbVMM0And1->isChecked()) {
                            for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                                int threshold = std::max(m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_globalRegs->at("sdt") - m_ui->spinBoxThreshold->value(),0);
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_globalRegs->at("sdt") = threshold;
                            }
                        }
                        else {
                            int threshold = std::max(m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[m_vmmIndex].m_vmmSettings->m_globalRegs->at("sdt") - m_ui->spinBoxThreshold->value(),0);
                            m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[m_vmmIndex].m_vmmSettings->m_globalRegs->at("sdt") = threshold;
                        }
                    }
                }
            }
        }
        LoadVMMSettings();
    }
    else if(QObject::sender() == m_ui->pbThresholdPlus){

        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if ( m_daq.GetFEC(fec) ){
                for (unsigned short hyb=0; hyb < HYBRIDS_PER_FEC; hyb++){
                    if( m_daq.m_fecs[fec].GetHybrid(hyb) ){
                        //VMM 0 and 1
                        if(m_ui->cbVMM0And1->isChecked()) {
                            for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                                int threshold = std::min(m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_globalRegs->at("sdt") + m_ui->spinBoxThreshold->value(),1023);
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_globalRegs->at("sdt") = threshold;
                            }
                        }
                        else {
                            int threshold = std::min(m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[m_vmmIndex].m_vmmSettings->m_globalRegs->at("sdt") + m_ui->spinBoxThreshold->value(),1023);
                            m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[m_vmmIndex].m_vmmSettings->m_globalRegs->at("sdt") = threshold;
                        }
                    }
                }
            }
        }
        LoadVMMSettings();
    }
    else if(QObject::sender() == m_ui->ApplyAllVMM0){
        if(m_vmmIndex != 0 && !m_ui->cbVMM0And1->isChecked()) {
            return;
        }
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if ( m_daq.GetFEC(fec) ){
                for (unsigned short hyb=0; hyb < HYBRIDS_PER_FEC; hyb++){
                    if( m_daq.m_fecs[fec].GetHybrid(hyb) ){
                        //VMM 0 and 1
                        if(m_ui->cbVMM0And1->isChecked()) {
                            for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                                if(!(m_fecIndex==fec && m_hybridIndex==hyb && m_vmmIndex==vmm) ){
                                    int threshold = m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_globalRegs->at("sdt");
                                    (*m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_globalRegs) = (*m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].m_vmmSettings->m_globalRegs);
                                    if(!m_ui->cbIncludeThreshold->isChecked()) {
                                        m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].m_vmmSettings->m_globalRegs->at("sdt") = threshold;
                                    }
                                }
                            }
                        }
                        else {
                            if(!(m_fecIndex==fec && m_hybridIndex==hyb) ){
                                int threshold = m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].m_vmmSettings->m_globalRegs->at("sdt");
                                (*m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].m_vmmSettings->m_globalRegs) = (*m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].m_vmmSettings->m_globalRegs);
                                if(!m_ui->cbIncludeThreshold->isChecked()) {
                                    m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].m_vmmSettings->m_globalRegs->at("sdt") = threshold;
                                }
                            }
                        }
                    }
                }
            }
        }
        LoadVMMSettings();
    }
    else if(QObject::sender() == m_ui->ApplyAllVMM1){
        if(m_vmmIndex != 1) {
            return;
        }
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if ( m_daq.GetFEC(fec) ){
                for (unsigned short hyb=0; hyb < HYBRIDS_PER_FEC; hyb++){
                    if( m_daq.m_fecs[fec].GetHybrid(hyb) ){

                        if(!(m_fecIndex==fec && m_hybridIndex==hyb && m_vmmIndex==1) ){
                            int threshold = m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[1].m_vmmSettings->m_globalRegs->at("sdt");
                            (*m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[1].m_vmmSettings->m_globalRegs) = (*m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].m_vmmSettings->m_globalRegs);
                            if(!m_ui->cbIncludeThreshold->isChecked()) {
                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[1].m_vmmSettings->m_globalRegs->at("sdt") = threshold;
                            }
                        }
                    }
                }
            }
        }
        LoadVMMSettings();
    }
    else if(QObject::sender() == m_ui->vmmResetAll){
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            if (m_daq.GetFEC(j) ){
                m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
            }
        }
        m_ui->offACQ->setCheckable(true);
        m_ui->offACQ->setChecked(true);
        m_ui->onACQ->setChecked(false);
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            if ( m_daq.GetFEC(j) ){
                for (unsigned short k=0; k < HYBRIDS_PER_FEC; k++){
                    if( m_daq.m_fecs[j].GetHybrid(k) ){

                        for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){

                            m_daq.m_fecs[j].SetVMM(k,m,"reset1", 1);
                            m_daq.m_fecs[j].SetVMM(k,m,"reset2", 1);
                            m_daq.m_fecs[j].m_fecConfigModule->ConfigVMM(k, m);
                            QThread::msleep(100);
                            m_daq.m_fecs[j].SetVMM(k,m,"reset1", 0);
                            m_daq.m_fecs[j].SetVMM(k,m,"reset2", 0);
                            m_daq.m_fecs[j].m_fecConfigModule->ConfigVMM(k, m);

                        }

                    }
                }
                //LoadVMMSettings();
            }
        }
        LoadVMMSettings();
        m_ui->Send->setEnabled(true);
    }
    else if(QObject::sender() == m_ui->vmmReset){
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            if (m_daq.GetFEC(j) ){
                m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
            }
        }
        m_ui->offACQ->setCheckable(true);
        m_ui->offACQ->setChecked(true);
        m_ui->onACQ->setChecked(false);
        SetVMM("reset1", 1);
        SetVMM("reset2", 1);
        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ConfigVMM(m_hybridIndex, m_vmmIndex);
        QThread::sleep(1);
        SetVMM("reset1", 0);
        SetVMM("reset2", 0);
        m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ConfigVMM(m_hybridIndex, m_vmmIndex);
        m_ui->Send->setEnabled(true);
    }
    else if(QObject::sender() == m_ui->ChannelSettingsAllVMM0){
        if(m_vmmIndex != 0 && !m_ui->cbVMM0And1->isChecked()) {
            return;
        }
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if ( m_daq.GetFEC(fec) ){
                for (unsigned short hyb=0; hyb < HYBRIDS_PER_FEC; hyb++){
                    if( m_daq.m_fecs[fec].GetHybrid(hyb) ){
                        //VMM 0 and 1
                        if(m_ui->cbVMM0And1->isChecked()) {
                            for (unsigned short vmm=0; vmm < VMMS_PER_HYBRID; vmm++){
                                if(!(m_fecIndex==fec && m_hybridIndex==hyb && m_vmmIndex==vmm) ){
                                    for (unsigned short ch=0; ch < 64; ch++){
                                        for(auto &setting: m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].m_vmmSettings->m_channels[ch].m_channel) {
                                            if(m_ui->cbIncludeThreshold->isChecked()) {
                                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].SetRegi(setting.first, setting.second, ch);
                                            }
                                            else if(setting.first != "sd") {
                                                m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[vmm].SetRegi(setting.first, setting.second, ch);
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            if(!(m_fecIndex==fec && m_hybridIndex==hyb) ){
                                for (unsigned short ch=0; ch < 64; ch++){
                                    for(auto &setting: m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].m_vmmSettings->m_channels[ch].m_channel) {
                                        if(m_ui->cbIncludeThreshold->isChecked()) {
                                            m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].SetRegi(setting.first, setting.second, ch);
                                        }
                                        else if(setting.first != "sd") {
                                            m_daq.m_fecs[fec].m_hybrids[hyb].m_vmms[0].SetRegi(setting.first, setting.second, ch);
                                        }
                                    }
                                }
                            }

                        }
                    }
                }
            }
        }
        LoadVMMChannelSettings();
    }
    else if(QObject::sender() == m_ui->ChannelSettingsAllVMM1){
        if(m_vmmIndex != 1) {
            return;
        }
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            if ( m_daq.GetFEC(j) ){
                for (unsigned short k=0; k < HYBRIDS_PER_FEC; k++){
                    if( m_daq.m_fecs[j].GetHybrid(k) ){

                        if(!(m_fecIndex==j && m_hybridIndex==k) ){
                            for (unsigned short ch=0; ch < 64; ch++){
                                for(auto &setting: m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].m_vmmSettings->m_channels[ch].m_channel) {
                                    if(m_ui->cbIncludeThreshold->isChecked()) {
                                        m_daq.m_fecs[j].m_hybrids[k].m_vmms[1].SetRegi(setting.first, setting.second, ch);
                                    }
                                    else if(setting.first != "sd") {
                                        m_daq.m_fecs[j].m_hybrids[k].m_vmms[1].SetRegi(setting.first, setting.second, ch);
                                    }
                                }
                            }
                        }

                    }
                }
            }
        }
        LoadVMMSettings();

    }
    else if(QObject::sender() == m_ui->cbVMM0And1){
        if(m_ui->cbVMM0And1->isChecked()) {
            m_ui->ApplyAllVMM0->setText("Global Settings");
            m_ui->ApplyAllVMM1->setVisible(false);
            m_ui->labelChangeThreshold->setText("Change threshold all VMM");
            m_ui->ChannelSettingsAllVMM0->setText("Channel Settings");
            m_ui->ChannelSettingsAllVMM1->setVisible(false);
        }
        else {
            m_ui->ApplyAllVMM0->setText("Global Settings VMM 0");
            m_ui->ApplyAllVMM1->setVisible(true);
            m_ui->ChannelSettingsAllVMM0->setText("Channel Settings VMM 0");
            m_ui->ChannelSettingsAllVMM1->setVisible(true);
            if(m_vmmIndex == 0) {
                m_ui->labelChangeThreshold->setText("Change threshold all VMM 0");
            }
            else {
                m_ui->labelChangeThreshold->setText("Change threshold all VMM 1");
            }
        }
    }
    else if(QObject::sender() == m_ui->readADC){
        if(!m_ui->onACQ->isChecked()) {
            m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ConfigVMM(m_hybridIndex, m_vmmIndex);
        }

        int adc_chan = 2; // 0: tdo, 1: pdo, 2: Mo, 3: not used | prepare to read other channels

        int adc_result = m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ReadADC(m_hybridIndex, m_vmmIndex, adc_chan);
        QString text;
        if(m_ui->sm5_sm0->currentIndex()==3){
            double temperature = (725-adc_result)/1.85;
            text = QString::number(temperature);
            text.append(" °C");
            m_ui->ADCresult->setText(text);
        }
        else {
            text = QString::number(adc_result);
            m_ui->ADCresult->setText(text);
        }
        m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("adc_value",text.toStdString());
    }
}




void DAQWindow::EnableVMMCommunicationButtons(bool enable) {
    m_ui->readADC->setEnabled(enable);
    m_ui->vmmReset->setEnabled(enable);
    m_ui->ApplyAllVMM0->setEnabled(enable);
    m_ui->ApplyAllVMM1->setEnabled(enable);
    m_ui->ChannelSettingsAllVMM0->setEnabled(enable);
    m_ui->ChannelSettingsAllVMM1->setEnabled(enable);
    m_ui->vmmResetAll->setEnabled(enable);
}


void DAQWindow::InitVMMChannelWidgets()
{
    SetVMMChannelToolTips();

    QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>();
    for(int n=0; n<childPushButtons.size(); n++) {
        connect(childPushButtons[n],SIGNAL(pressed()),
                this,SLOT(onUpdateVMMChannelSettings()));
    }

    QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>();
    for(int n=0; n<childComboBoxes.size(); n++) {
        if(childComboBoxes[n]->objectName().startsWith("VMMSDVoltage") || childComboBoxes[n]->objectName().startsWith("VMMSZ010b")) {
            connect(childComboBoxes[n],SIGNAL(currentIndexChanged(int)),
                    this,SLOT(onUpdateVMMChannelSettings()));

            childComboBoxes[n]->addItem("0 mV");
            childComboBoxes[n]->addItem("1 mV");
            childComboBoxes[n]->addItem("2 mV");
            childComboBoxes[n]->addItem("3 mV");
            childComboBoxes[n]->addItem("4 mV");
            childComboBoxes[n]->addItem("5 mV");
            childComboBoxes[n]->addItem("6 mV");
            childComboBoxes[n]->addItem("7 mV");
            childComboBoxes[n]->addItem("8 mV");
            childComboBoxes[n]->addItem("9 mV");
            childComboBoxes[n]->addItem("10 mV");
            childComboBoxes[n]->addItem("11 mV");
            childComboBoxes[n]->addItem("12 mV");
            childComboBoxes[n]->addItem("13 mV");
            childComboBoxes[n]->addItem("14 mV");
            childComboBoxes[n]->addItem("15 mV");
            childComboBoxes[n]->addItem("16 mV");
            childComboBoxes[n]->addItem("17 mV");
            childComboBoxes[n]->addItem("18 mV");
            childComboBoxes[n]->addItem("19 mV");
            childComboBoxes[n]->addItem("20 mV");
            childComboBoxes[n]->addItem("21 mV");
            childComboBoxes[n]->addItem("22 mV");
            childComboBoxes[n]->addItem("23 mV");
            childComboBoxes[n]->addItem("24 mV");
            childComboBoxes[n]->addItem("25 mV");
            childComboBoxes[n]->addItem("26 mV");
            childComboBoxes[n]->addItem("27 mV");
            childComboBoxes[n]->addItem("28 mV");
            childComboBoxes[n]->addItem("29 mV");
            childComboBoxes[n]->addItem("30 mV");
            childComboBoxes[n]->addItem("31 mV");
        }
        else if(childComboBoxes[n]->objectName().startsWith("VMMSZ08b")) {
            connect(childComboBoxes[n],SIGNAL(currentIndexChanged(int)),
                    this,SLOT(onUpdateVMMChannelSettings()));
            childComboBoxes[n]->addItem("0 ns");
            childComboBoxes[n]->addItem("1 ns");
            childComboBoxes[n]->addItem("2 ns");
            childComboBoxes[n]->addItem("3 ns");
            childComboBoxes[n]->addItem("4 ns");
            childComboBoxes[n]->addItem("5 ns");
            childComboBoxes[n]->addItem("6 ns");
            childComboBoxes[n]->addItem("7 ns");
            childComboBoxes[n]->addItem("8 ns");
            childComboBoxes[n]->addItem("9 ns");
            childComboBoxes[n]->addItem("10 ns");
            childComboBoxes[n]->addItem("11 ns");
            childComboBoxes[n]->addItem("12 ns");
            childComboBoxes[n]->addItem("13 ns");
            childComboBoxes[n]->addItem("14 ns");
            childComboBoxes[n]->addItem("15 ns");
        }
        else if(childComboBoxes[n]->objectName().startsWith("VMMSZ06b")) {
            connect(childComboBoxes[n],SIGNAL(currentIndexChanged(int)),
                    this,SLOT(onUpdateVMMChannelSettings()));
            childComboBoxes[n]->addItem("0 mV");
            childComboBoxes[n]->addItem("1 mV");
            childComboBoxes[n]->addItem("2 mV");
            childComboBoxes[n]->addItem("3 mV");
            childComboBoxes[n]->addItem("4 mV");
            childComboBoxes[n]->addItem("5 mV");
            childComboBoxes[n]->addItem("6 mV");
            childComboBoxes[n]->addItem("7 mV");
        }
    }

    // -------------------------------------------------------------------- //
    // update channel voltages
    // -------------------------------------------------------------------- //
    connect(m_ui->SDLabel,  SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMChannelSettings()));


    // -------------------------------------------------------------------- //
    // update channel ADC values
    // -------------------------------------------------------------------- //
    connect(m_ui->SZ010bLabel,  SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMChannelSettings()));
    connect(m_ui->SZ08bLabel,   SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMChannelSettings()));
    connect(m_ui->SZ06bLabel,   SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateVMMChannelSettings()));


    // -------------------------------------------------------------------- //
    // update channel states (green boxes in Channel Fields)
    // -------------------------------------------------------------------- //
    connect(m_ui->SCLabel,   SIGNAL(pressed()), this, SLOT(onUpdateVMMChannelSettings()));
    connect(m_ui->SLLabel,   SIGNAL(pressed()), this, SLOT(onUpdateVMMChannelSettings()));
    connect(m_ui->STLabel,   SIGNAL(pressed()), this, SLOT(onUpdateVMMChannelSettings()));
    connect(m_ui->STHLabel,   SIGNAL(pressed()), this, SLOT(onUpdateVMMChannelSettings()));
    connect(m_ui->SMLabel,   SIGNAL(pressed()), this, SLOT(onUpdateVMMChannelSettings()));
    connect(m_ui->SMXLabel,  SIGNAL(pressed()), this, SLOT(onUpdateVMMChannelSettings()));
}


// ------------------------------------------------------------------------- //
void DAQWindow::onUpdateVMMChannelSettings()
{
    QString name = QObject::sender()->objectName();
    //Global settings (buttons)
    if(m_ui->SCLabel == QObject::sender()){

        QRegularExpression exp("VMMSC_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetInfo("all_sc") == "0"){
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("background-color: green");
                SetVMM("sc", 1, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_sc", "1");
        }
        else {
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("");
                SetVMM("sc", 0, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_sc", "0");
        }
    }

    // ***********************  SL  ********************************* //
    else if(m_ui->SLLabel == QObject::sender()){
        QRegularExpression exp("VMMSL_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetInfo("all_sl") == "0"){
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("background-color: green");
                SetVMM("sl", 1, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_sl", "1");
        }
        else {
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("");
                SetVMM("sl", 0, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_sl", "0");
        }
    }
    // ***********************  ST  ********************************* //
    else if(m_ui->STLabel == QObject::sender()){
        QRegularExpression exp("VMMST_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetInfo("all_st") == "0"){
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("background-color: green");
                SetVMM("st", 1, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_st", "1");
        }
        else {
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("");
                SetVMM("st", 0, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_st", "0");
        }
    }
    // ***********************  STH  ********************************* //
    else if(m_ui->STHLabel == QObject::sender()){
        QRegularExpression exp("VMMSTH_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetInfo("all_sth") == "0"){
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("background-color: green");
                SetVMM("sth", 1, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_sth", "1");
        }
        else {
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("");
                SetVMM("sth", 0, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_sth", "0");
        }
    }
    // ***********************  SM  ********************************* //
    else if(m_ui->SMLabel == QObject::sender()){
        QRegularExpression exp("VMMSM_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetInfo("all_sm") == "0"){
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("background-color: green");
                SetVMM("sm", 1, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_sm", "1");
        }
        else {
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("");
                SetVMM("sm", 0, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_sm", "0");
        }
    }
    // ***********************  SMX  ********************************* //
    else if(m_ui->SMXLabel == QObject::sender()){
        QRegularExpression exp("VMMSMX_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetInfo("all_smx") == "0"){
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("background-color: green");
                SetVMM("smx", 1, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_smx", "1");
        }
        else {
            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("");
                SetVMM("smx", 0, n);
            }
            m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetInfo("all_smx", "0");
        }
    }
    //Individual channel buttons (e.g. test pulse setting)
    else if(name.startsWith("VMMSC_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(GetVMM("sc",n) == 0){
            btn->setStyleSheet("background-color: green");
            SetVMM("sc", 1, n);
        }else {
            btn->setStyleSheet("");
            SetVMM("sc", 0, n);
        }
    }
    else if(name.startsWith("VMMSL_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(GetVMM("sl",n) == 0){
            btn->setStyleSheet("background-color: green");
            SetVMM("sl", 1, n);
        }else {
            btn->setStyleSheet("");
            SetVMM("sl", 0, n);
        }
    }
    else if(name.startsWith("VMMST_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(GetVMM("st",n) == 0){
            btn->setStyleSheet("background-color: green");
            SetVMM("st", 1, n);
        }else{
            btn->setStyleSheet("");
            SetVMM("st", 0, n);
        }
    }
    else if(name.startsWith("VMMSTH_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(GetVMM("sth",n) == 0){
            btn->setStyleSheet("background-color: green");
            SetVMM("sth", 1, n);
        }else{
            btn->setStyleSheet("");
            SetVMM("sth", 0, n);
        }
    }
    else if(name.startsWith("VMMSMX_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(GetVMM("smx",n) == 0){
            btn->setStyleSheet("background-color: green");
            SetVMM("smx", 1, n);
        }else{
            btn->setStyleSheet("");
            SetVMM("smx", 0, n);
        }
    }
    else if(name.startsWith("VMMSM_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(GetVMM("sm",n) == 0){
            btn->setStyleSheet("background-color: green");
            SetVMM("sm", 1, n);
        }else {
            btn->setStyleSheet("");
            SetVMM("sm", 0, n);
        }
    }
    //Global comboboxes for all channels
    else if(m_ui->SDLabel == QObject::sender()){
        int index = m_ui->SDLabel->currentIndex();
        QRegularExpression exp("VMMSDVoltage");
        QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
        std::sort(childComboBoxes.begin(), childComboBoxes.end(),
                  [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
        });
        for(int n=0; n<childComboBoxes.size(); n++) {
            SetVMM("sd", index, n);
            childComboBoxes[n]->setCurrentIndex(index);
        }
    }
    else if(m_ui->SZ010bLabel == QObject::sender()){
        int index = m_ui->SZ010bLabel->currentIndex();
        QRegularExpression exp("VMMSZ010b");
        QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
        std::sort(childComboBoxes.begin(), childComboBoxes.end(),
                  [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
        });
        for(int n=0; n<childComboBoxes.size(); n++) {
            SetVMM("sz10b", index, n);
            childComboBoxes[n]->setCurrentIndex(index);
        }
    }
    else if(m_ui->SZ08bLabel == QObject::sender()){
        int index = m_ui->SZ08bLabel->currentIndex();
        QRegularExpression exp("VMMSZ08b");
        QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
        std::sort(childComboBoxes.begin(), childComboBoxes.end(),
                  [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
        });
        for(int n=0; n<childComboBoxes.size(); n++) {
            SetVMM("sz08b", index, n);
            childComboBoxes[n]->setCurrentIndex(index);
        }
    }
    else if(m_ui->SZ06bLabel == QObject::sender()){
        int index = m_ui->SZ06bLabel->currentIndex();
        QRegularExpression exp("VMMSZ06b");
        QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
        std::sort(childComboBoxes.begin(), childComboBoxes.end(),
                  [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
        });
        for(int n=0; n<childComboBoxes.size(); n++) {
            SetVMM("sz06b", index, n);
            childComboBoxes[n]->setCurrentIndex(index);
        }
    }
    else {
        //Comboboxes for individual channels
        QComboBox * cbbox = (QComboBox *)(QObject::sender());
        int index = cbbox->currentIndex();
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(name.startsWith("VMMSDVoltage")) {
            SetVMM("sd", index, n);
            cbbox->setCurrentIndex(index);
        }
        else if(name.startsWith("VMMSZ010b")) {
            SetVMM("sz10b", index, n);
            cbbox->setCurrentIndex(index);
        }
        else if(name.startsWith("VMMSZ08b")) {
            SetVMM("sz08b", index, n);
            cbbox->setCurrentIndex(index);
        }
        else if(name.startsWith("VMMSZ06b")) {
            SetVMM("sz06b", index, n);
            cbbox->setCurrentIndex(index);
        }
    }
}


