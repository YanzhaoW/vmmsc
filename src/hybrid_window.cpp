#include "hybrid_window.h"
#include "vmm_window.h"

HybridWindow::HybridWindow(FECWindow *top, unsigned short fec, unsigned short hybrid, QWidget *parent) :
    QWidget(parent),
    m_fecWindow{top},
    m_fecIndex{fec},
    m_hybridIndex{hybrid},
    m_ui(new Ui::hybrid_window)
{
    m_ui->setupUi(this);
    m_ui->tpSkew->clear();
    for(int i=0;i < 2; i++) {
        for(int n=0;n< 8; n++) {
            QString txt = QStringLiteral("%1 ns").arg((i*g_clock_period)+n*g_clock_period/8.0);
            m_ui->tpSkew->addItem(txt);
        }
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

    UpdateWindow();

    LoadSettings();
    EnableCommunicationButtons(false);

    connect(m_ui->tpSkew, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->tpWidth, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->tpPolarity, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->ApplyAll, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule, SIGNAL(ReloadHybrid()),
            this, SLOT( onReloadSettings() ));

    connect(m_fecWindow->m_daqWindow->m_ui->openConnection, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));
}

HybridWindow::~HybridWindow()
{
    delete m_ui;
}

void HybridWindow::EnableCommunicationButtons(bool enable) {
    m_ui->ApplyAll->setEnabled(enable);
    m_ui->pbReadI2C->setEnabled(enable);
}


void HybridWindow::on_Box_vmm1_clicked()
{
    if (m_ui->Box_vmm1->isChecked()){VMMBoxLogic(true,0);}
    else {VMMBoxLogic(false,0);}
}
void HybridWindow::on_Box_vmm2_clicked()
{
    if (m_ui->Box_vmm2->isChecked()){VMMBoxLogic(true,1);}
    else {VMMBoxLogic(false,1);}
}

void HybridWindow::VMMBoxLogic(bool checked, unsigned short vmm){
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = m_ui->groupBoxVMMs->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<vmm && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        m_ui->tabWidget->insertTab(vmm-NotActiveBefore, new VMMWindow(this,m_fecIndex,m_hybridIndex,vmm), QString(" VMM %0").arg(vmm));
        m_ui->tabWidget->setCurrentIndex(vmm-NotActiveBefore);
        m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].SetVMM(vmm, true);
    }
    else {
        m_ui->tabWidget->removeTab(vmm-NotActiveBefore);
        m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].SetVMM(vmm, false);
    }
}

void HybridWindow::UpdateWindow(){
    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
        if (m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].GetVMM(m)){
            if (m == 0 && !m_ui->Box_vmm1->isChecked()){m_ui->Box_vmm1->setChecked(true); on_Box_vmm1_clicked();}
            if (m == 1 && !m_ui->Box_vmm2->isChecked()){m_ui->Box_vmm2->setChecked(true); on_Box_vmm2_clicked();}
        }
    }
}

void HybridWindow::LoadSettings(){
    m_ui->tpSkew->setCurrentIndex(GetHybrid("TP_skew"));
    m_ui->tpWidth->setCurrentIndex(GetHybrid("TP_width"));
    m_ui->tpPolarity->setCurrentIndex(GetHybrid("TP_pol"));
}

// ------------------------------------------------------------------------- //
void HybridWindow::onReloadSettings(){
    m_ui->tpSkew->setCurrentIndex(GetHybrid("TP_skew"));
    m_ui->tpWidth->setCurrentIndex(GetHybrid("TP_width"));
    m_ui->tpPolarity->setCurrentIndex(GetHybrid("TP_pol"));
}
//


bool HybridWindow::SetHybrid(std::string feature, unsigned short value){
    if(m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].SetReg(feature, value)){
        return true;
    }
    else return false;
}
unsigned short HybridWindow::GetHybrid(std::string feature){
    return m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].GetReg(feature);
}

void HybridWindow::onUpdateSettings(){
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
    else if(QObject::sender() == m_ui->ApplyAll){
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if (m_fecWindow->m_daqWindow->m_daq.GetFEC(fec) ){
                for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_FEC; hybrid++){
                    if( m_fecWindow->m_daqWindow->m_daq.m_fecs[fec].GetHybrid(hybrid) ){
                        m_fecWindow->m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_skew", m_ui->tpSkew->currentIndex());
                        m_fecWindow->m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_width", m_ui->tpWidth->currentIndex());
                        m_fecWindow->m_daqWindow->m_daq.m_fecs[fec].m_hybrids[hybrid].SetReg("TP_pol", m_ui->tpPolarity->currentIndex());

                    }
                }
            }
        }
        m_fecWindow->m_daqWindow->m_daq.ApplyHybrids();
    }
    else if(QObject::sender() == m_fecWindow->m_daqWindow->m_ui->openConnection){
        if(g_connection_ok){
            EnableCommunicationButtons(true);
        }
        else {
            EnableCommunicationButtons(false);
        }
    }
}

void HybridWindow::on_pbReadI2C_pressed()
{
    QString result = m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ReadI2C(m_hybridIndex, m_ui->cbChoiceI2C->currentIndex());
    QString text = "";
    for(int n=0; n < result.size(); n=n+16) {
        text += result.mid(n,16);
        text += "\n";
    }
    text = text.mid(0, text.size()-1);
    if(m_ui->cbChoiceI2C->currentIndex() == 0) {
        m_ui->hybridID->setText(text);
    }
    else if(m_ui->cbChoiceI2C->currentIndex() == 1) {
        m_ui->firmwareVersion->setText(text);
    }
    else if(m_ui->cbChoiceI2C->currentIndex() == 2) {
        m_ui->geoID->setText(text);
    }
}



void HybridWindow::on_tpSkew_highlighted(int index)
{
    if(g_clock_source_changed) {
        m_ui->tpSkew->clear();
        for(int i=0;i < 2; i++) {
            for(int n=0;n< 8; n++) {
                QString txt = QStringLiteral("%1 ns").arg((i*g_clock_period)+n*g_time_factor);
                m_ui->tpSkew->addItem(txt);
            }
        }
        g_clock_source_changed = false;
    }
}
