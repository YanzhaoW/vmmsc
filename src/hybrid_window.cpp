#include "hybrid_window.h"

HybridWindow::HybridWindow(HDMIWindow *top, unsigned short fec, unsigned short hdmi, unsigned short hybrid, QWidget *parent) :
    QWidget(parent),
    m_hdmiWindow{top},
    m_fecIndex{fec},
    m_hdmiIndex{hdmi},
    m_hybridIndex{hybrid},
    m_ui(new Ui::hybrid_window)
{
    m_ui->setupUi(this);
    UpdateWindow();
    LoadSettings();
    onGlobalCKBCHandler();

    connect(m_ui->Xaxis, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->position, SIGNAL(valueChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->cktk_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->ckbc_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->ckbc_duty_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->ckbc_skew_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->ckdt_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->s6_tkPulses, SIGNAL(valueChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->fecPeriodReset, SIGNAL(valueChanged(int)),
                                    this, SLOT(onUpdateSettings()));

    connect(m_ui->tpSkew, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->tpWidth, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_ui->tpPolarity, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(onUpdateSettings()));
    connect(m_hdmiWindow->m_fecWindow, SIGNAL(ChangeState_FEC()),
            this, SLOT( onGlobalCKBCHandler() ));

    connect(m_ui->ApplyAll, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule, SIGNAL(ReloadHybrid()),
            this, SLOT( onReloadSettings() ));


}

HybridWindow::~HybridWindow()
{
    delete m_ui;
}

void HybridWindow::onGlobalCKBCHandler(){
    if(m_hdmiWindow->m_fecWindow->m_sendstate == "globalCKBCon" ){
        m_ui->ckbc_s6->setEnabled(false);
        SetHybrid("CKBC", m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].GetRegVal("globalCKBC"));

    }
    else if(m_hdmiWindow->m_fecWindow->m_sendstate == "globalCKBCoff" ){
        m_ui->ckbc_s6->setEnabled(true);
        SetHybrid("CKBC", m_ui->ckbc_s6->currentIndex());
    }
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
    QList<QCheckBox*> a = m_ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<vmm && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        m_ui->tabWidget->insertTab(vmm-NotActiveBefore, new VMMWindow(this,m_fecIndex,m_hdmiIndex,m_hybridIndex,vmm), QString(" VMM %0").arg(vmm+1));
        m_ui->tabWidget->setCurrentIndex(vmm-NotActiveBefore);
        m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hdmis[m_hdmiIndex].m_hybrids[m_hybridIndex].SetVMM(vmm, true);
    }
    else {
        m_ui->tabWidget->removeTab(vmm-NotActiveBefore);
        m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hdmis[m_hdmiIndex].m_hybrids[m_hybridIndex].SetVMM(vmm, false);
    }
}

void HybridWindow::UpdateWindow(){
    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
        if (m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hdmis[m_hdmiIndex].m_hybrids[m_hybridIndex].GetVMM(m)){
            if (m == 0 && !m_ui->Box_vmm1->isChecked()){m_ui->Box_vmm1->setChecked(true); on_Box_vmm1_clicked();}
            if (m == 1 && !m_ui->Box_vmm2->isChecked()){m_ui->Box_vmm2->setChecked(true); on_Box_vmm2_clicked();}
        }
    }
}
void HybridWindow::LoadSettings(){

    while(!m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].CheckHybridPos(GetHybrid("Xaxis"), GetHybrid("position"), m_fecIndex, m_hdmiIndex, m_hybridIndex )){
        SetHybrid("position", GetHybrid("position")-1);
    }

    m_ui->Xaxis->setCurrentIndex(GetHybrid("Xaxis"));
    m_ui->position->setValue(GetHybrid("position"));
    m_ui->cktk_s6->setCurrentIndex(GetHybrid("CKTK"));
    m_ui->ckbc_s6->setCurrentIndex(GetHybrid("CKBC"));
    m_ui->ckbc_duty_s6->setCurrentIndex(GetHybrid("CKBC_duty"));
    m_ui->ckbc_skew_s6->setCurrentIndex(GetHybrid("CKBC_skew"));
    m_ui->ckdt_s6->setCurrentIndex(GetHybrid("CKDT"));
    m_ui->s6_tkPulses->setValue(GetHybrid("TK_Pulses"));
    m_ui->fecPeriodReset->setValue(GetHybrid("period"));
    m_ui->tpSkew->setCurrentIndex(GetHybrid("TP_skew"));
    m_ui->tpWidth->setCurrentIndex(GetHybrid("TP_width"));
    m_ui->tpPolarity->setCurrentIndex(GetHybrid("TP_pol"));
}

// ------------------------------------------------------------------------- //
void HybridWindow::onReloadSettings(){
    m_ui->cktk_s6->setCurrentIndex(GetHybrid("CKTK"));
    m_ui->ckbc_s6->setCurrentIndex(GetHybrid("CKBC"));
    m_ui->ckbc_duty_s6->setCurrentIndex(GetHybrid("CKBC_duty"));
    m_ui->ckbc_skew_s6->setCurrentIndex(GetHybrid("CKBC_skew"));
    m_ui->ckdt_s6->setCurrentIndex(GetHybrid("CKDT"));
    m_ui->s6_tkPulses->setValue(GetHybrid("TK_Pulses"));
    m_ui->fecPeriodReset->setValue(GetHybrid("period"));
    m_ui->tpSkew->setCurrentIndex(GetHybrid("TP_skew"));
    m_ui->tpWidth->setCurrentIndex(GetHybrid("TP_width"));
    m_ui->tpPolarity->setCurrentIndex(GetHybrid("TP_pol"));
}
//


bool HybridWindow::SetHybrid(std::string feature, unsigned short value){
    if(m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hdmis[m_hdmiIndex].m_hybrids[m_hybridIndex].SetReg(feature, value)){
     return true;
    }
    else return false;
}
unsigned short HybridWindow::GetHybrid(std::string feature){
    return m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hdmis[m_hdmiIndex].m_hybrids[m_hybridIndex].GetReg(feature);
}

void HybridWindow::onUpdateSettings(){

    if(QObject::sender() == m_ui->Xaxis){
        if(m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].CheckHybridPos( m_ui->Xaxis->currentIndex() , GetHybrid("position"), m_fecIndex, m_hdmiIndex, m_hybridIndex )){
           SetHybrid("Xaxis", m_ui->Xaxis->currentIndex());
        }
        else{
            m_ui->Xaxis->setCurrentIndex( GetHybrid("Xaxis") );
            m_hdmiWindow->m_fecWindow->m_daqWindow->SetWarning2("Hybrid position occupied- resetted!", "orange");
        }


    }
    else if(QObject::sender() == m_ui->position){
        if(m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].CheckHybridPos(GetHybrid("Xaxis"), m_ui->position->value() , m_fecIndex, m_hdmiIndex, m_hybridIndex )){
           SetHybrid("position", m_ui->position->value());
        }
        else{
            m_ui->position->setValue( GetHybrid("position") );
            m_hdmiWindow->m_fecWindow->m_daqWindow->SetWarning2("Hybrid position occupied- resetted!", "orange");
        }

    }
    else if(QObject::sender() == m_ui->cktk_s6){
        SetHybrid("CKTK", m_ui->cktk_s6->currentIndex());
    }
    else if(QObject::sender() == m_ui->ckbc_s6){
        SetHybrid("CKBC", m_ui->ckbc_s6->currentIndex());
    }
    else if(QObject::sender() == m_ui->ckbc_duty_s6){
        SetHybrid("CKBC_duty", m_ui->ckbc_duty_s6->currentIndex());
    }
    else if(QObject::sender() == m_ui->ckbc_skew_s6){
        SetHybrid("CKBC_skew", m_ui->ckbc_skew_s6->currentIndex());
    }
    else if(QObject::sender() == m_ui->ckdt_s6){
        SetHybrid("CKDT", m_ui->ckdt_s6->currentIndex());
    }
    else if(QObject::sender() == m_ui->s6_tkPulses){
        SetHybrid("TK_Pulses", m_ui->s6_tkPulses->value());
    }
    else if(QObject::sender() == m_ui->fecPeriodReset){
        SetHybrid("period", m_ui->fecPeriodReset->value());
    }
    else if(QObject::sender() == m_ui->tpSkew){
        SetHybrid("TP_skew", m_ui->tpSkew->currentIndex());
    }
    else if(QObject::sender() == m_ui->tpWidth){
        SetHybrid("TP_width", m_ui->tpWidth->currentIndex());
    }
    else if(QObject::sender() == m_ui->tpPolarity){
        SetHybrid("TP_pol", m_ui->tpPolarity->currentIndex());
    }
    else if(QObject::sender() == m_ui->ApplyAll){
        for (unsigned short fec=0; fec < FECS_PER_DAQ; fec++){
            if (m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].GetFEC(fec) ){
                for (unsigned short hdmi=0; hdmi < HDMIS_PER_FEC; hdmi++){
                    if( m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].GetHDMI(hdmi) ){
                        for (unsigned short hybrid=0; hybrid < HYBRIDS_PER_HDMI; hybrid++){
                            if (m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].GetHybrid(hybrid)){
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("CKTK", m_ui->cktk_s6->currentIndex());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("CKBC", m_ui->ckbc_s6->currentIndex());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("CKBC_duty", m_ui->ckbc_duty_s6->currentIndex());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("CKBC_skew", m_ui->ckbc_skew_s6->currentIndex());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("CKDT", m_ui->ckdt_s6->currentIndex());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("TK_Pulses", m_ui->s6_tkPulses->value());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("period", m_ui->fecPeriodReset->value());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("TP_skew", m_ui->tpSkew->currentIndex());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("TP_width", m_ui->tpWidth->currentIndex());
                                m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[fec].m_hdmis[hdmi].m_hybrids[hybrid].SetReg("TP_pol", m_ui->tpPolarity->currentIndex());

                            }
                        }
                    }
                }
            }
        }
        m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].ApplyHybrids(m_fecIndex, m_hdmiIndex, m_hybridIndex);
    }
}

void HybridWindow::on_pbReadI2C_pressed()
{
  QString result = m_hdmiWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->ReadI2C(m_hdmiIndex, m_hybridIndex, m_ui->cbChoiceI2C->currentIndex());
  m_ui->lineEditResultI2C->setText(result.toUpper());
}

