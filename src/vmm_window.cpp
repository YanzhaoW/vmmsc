#include "vmm_window.h"
//#include <unistd.h>

VMMWindow::VMMWindow(HybridWindow *top, unsigned short fec, unsigned short hybrid, unsigned short vmm, QWidget *parent) :
    QWidget(parent),
    m_hybridWindow{top},
    m_fecIndex{fec},
    m_hybridIndex{hybrid},
    m_vmmIndex{vmm},
    m_ui(new Ui::vmm_window)
{
    m_ui->setupUi(this);
    m_ui->ADCresult->setReadOnly(true);

    QElapsedTimer timer;
    timer.start();

    CreateChannelsFields();

    SetToolTips();
    LoadSettings();
    EnableCommunicationButtons(false);

    //connect the settings from the GUI
    // General Settings

    connect(m_ui->nskipm_i, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0cktest, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0ckinv, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0dckinv, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0ena, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->truncate_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->nskip_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->window_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->rollover_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->L0offset_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->offset_i, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->s32, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stcr, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->ssart, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->srec, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stlc, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbip, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->srat, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->slvsbc, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slvstp, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slvstk, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slvsdt, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slvsart, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slvstki, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slvsena, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slvs6b, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->sL0enaV, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slh, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slxh, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stgc, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));


    connect(m_ui->sdt, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdp10, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sc10b, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sc8b, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->s10b, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->s8b, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdcks, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdcka, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));


    connect(m_ui->sp, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdp, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbmx, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbft, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbfp, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbfm, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slg, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->sm5_sm0, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->sfa, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sfam, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->st, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sfm, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->sg, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sng, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->ssh, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stc, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));

    connect(m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->openConnection, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->ApplyAll, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->vmmResetAll, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->vmmReset, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->ChannelSettingsAll, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule, SIGNAL(ReloadVMM()),
            this, SLOT( onReloadSettings() ));

    connect(m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule, SIGNAL(VMMUpdateChannel()),
            this, SLOT( onVMMUpdateChannel() ));
    std::cout << "Created VMM Window " << timer.elapsed() << " ms" << std::endl;

}


VMMWindow::~VMMWindow()
{
    delete m_ui;

}

// ------------------------------------------------------------------------- //
void VMMWindow::SetToolTips()
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


    m_ui->ApplyAll->setToolTip("Applies the settings of current VMM (except channel settings) to all activated VMMs");
    m_ui->ChannelSettingsAll->setToolTip("Copy channel settings of this VMM to all activated VMMs");
    m_ui->vmmReset->setToolTip("Hard reset this VMM");
    m_ui->vmmResetAll->setToolTip("Hard reset all activated VMMs");

}
// ------------------------------------------------------------------------- //

unsigned short VMMWindow::GetVMM(std::string feature, int ch){
    unsigned short setting = m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetRegister(feature, ch);
    return setting;
}
// ------------------------------------------------------------------------- //

bool VMMWindow::SetVMM(std::string feature, int value, int ch){
    if(m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}

bool VMMWindow::SetVMM(std::string feature, bool value, int ch){
    if(m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}
// ------------------------------------------------------------------------- //


void VMMWindow::LoadSettings()
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

    m_ui->sdt->setValue(GetVMM("sdt"));
    int sdp10 = GetVMM("sdp10");
    m_ui->sdp10->setValue(sdp10);
    QString tmp;

    double val = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->ThresholdDAC_to_mV(m_ui->sdt->value());
    m_ui->dacmvLabel->setText(tmp.number(val, 'f', 0) + " mV");
    double pulseHeight = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp10->value(), m_ui->sg->currentIndex());
    double dav_mV = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->PulserDAC_to_mV(m_ui->sdp10->value());
    m_ui->dacmvLabel_TP->setText(tmp.number(dav_mV, 'f', 0) + " mV DAC\n"+ tmp.number(pulseHeight, 'f', 0) + " mV pulse height");
    if(pulseHeight > 1200) {
        m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
        m_ui->sdp10->setStyleSheet("QSpinBox { background-color : red; color : white; }");
    }
    else {
        m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
        m_ui->sdp10->setStyleSheet("QSpinBox { background-color : white; color : black; }");
    }

    m_ui->sc10b->setCurrentIndex(GetVMM("sc10b"));
    m_ui->sc8b->setCurrentIndex(GetVMM("sc8b"));
    m_ui->s10b->setChecked(GetVMM("s10b"));
    m_ui->s8b->setChecked(GetVMM("s8b"));
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


}
// ------------------------------------------------------------------------- //
void VMMWindow::onReloadSettings(){
    LoadSettings();
}
// ------------------------------------------------------------------------- //



void VMMWindow::onVMMUpdateChannel() {
    int sd = -1;
    int SZ010 = -1;
    int SZ08 = -1;
    int SZ06 = -1;


    QRegularExpression exp("VMMSC");

    QList<QPushButton *> listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        VMMSCBool[n]=GetVMM("sc",n);
        listPushButtons[n]->setStyleSheet("");
        if(VMMSCBool[n]) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }

    exp.setPattern("VMMSL");
    listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        VMMSLBool[n]=GetVMM("sl",n);
        listPushButtons[n]->setStyleSheet("");
        if(VMMSLBool[n]) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }

    exp.setPattern("VMMST");
    listPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
    std::sort(listPushButtons.begin(), listPushButtons.end(),
              [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listPushButtons.size(); n++) {
        VMMSTBool[n]=GetVMM("st",n);
        listPushButtons[n]->setStyleSheet("");
        if(VMMSTBool[n]) {
            listPushButtons[n]->setStyleSheet("background-color: green");
        }
    }

    exp.setPattern("VMMSDVoltage");
    QList<QComboBox *> listComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
    std::sort(listComboBoxes.begin(), listComboBoxes.end(),
              [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listComboBoxes.size(); n++) {
        unsigned short value = GetVMM("sd",n);
        VMMSDValue[n]=value;
        listComboBoxes[n]->setCurrentIndex(value);
    }

    exp.setPattern("VMMSZ010b");
    listComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
    std::sort(listComboBoxes.begin(), listComboBoxes.end(),
              [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listComboBoxes.size(); n++) {
        unsigned short value = GetVMM("sz10b",n);
        VMMSZ010bValue[n]=value;
        listComboBoxes[n]->setCurrentIndex(value);
    }

    exp.setPattern("VMMSZ08b");
    listComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
    std::sort(listComboBoxes.begin(), listComboBoxes.end(),
              [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listComboBoxes.size(); n++) {
        unsigned short value = GetVMM("sz08b",n);
        VMMSZ08bValue[n]=value;
        listComboBoxes[n]->setCurrentIndex(value);
    }

    exp.setPattern("VMMSZ06b");
    listComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
    std::sort(listComboBoxes.begin(), listComboBoxes.end(),
              [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
    });
    for(int n=0; n<listComboBoxes.size(); n++) {
        unsigned short value = GetVMM("sz06b",n);
        VMMSZ06bValue[n]=value;
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


void VMMWindow::onUpdateSettings()
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
        double val = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->ThresholdDAC_to_mV(m_ui->sdt->value());
        m_ui->dacmvLabel->setText(tmp.number(val, 'f', 0) + " mV");
    }
    else if(QObject::sender() == m_ui->sdp10){
        double pulseHeight = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp10->value(), m_ui->sg->currentIndex());
        double dav_mV = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->PulserDAC_to_mV(m_ui->sdp10->value());
        m_ui->dacmvLabel_TP->setText(tmp.number(dav_mV, 'f', 0) + " mV DAC\n"+ tmp.number(pulseHeight, 'f', 0) + " mV pulse height");
        if(pulseHeight > 1200) {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
            m_ui->sdp10->setStyleSheet("QSpinBox { background-color : red; color : white; }");
        }
        else {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
            m_ui->sdp10->setStyleSheet("QSpinBox { background-color : white; color : black; }");
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
    else if(QObject::sender() == m_ui->sdcks){
        SetVMM("sdcks", !m_ui->sdcks->isChecked());
    }
    else if(QObject::sender() == m_ui->sdcka){
        SetVMM("sdcka", !m_ui->sdcka->isChecked());
    }
    else if(QObject::sender() == m_ui->sp){
        SetVMM("sp", m_ui->sp->currentIndex());
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
    else if(QObject::sender() == m_ui->sm5_sm0){
        if(m_ui->sm5_sm0->currentIndex()<= 3){
            SetVMM("sm5_sm0",m_ui->sm5_sm0->currentIndex()+64);
        }
        else {
            SetVMM("sm5_sm0", m_ui->sm5_sm0->currentIndex()-4);
        }
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
        double val = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->ThresholdDAC_to_mV(m_ui->sdt->value());
        m_ui->dacmvLabel->setText(tmp.number(val, 'f', 0) + " mV");
        double pulseHeight = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp10->value(), m_ui->sg->currentIndex());
        double dav_mV = m_hybridWindow->m_fecWindow->m_daqWindow->m_calib->PulserDAC_to_mV(m_ui->sdp10->value());
        m_ui->dacmvLabel_TP->setText(tmp.number(dav_mV, 'f', 0) + " mV DAC\n"+ tmp.number(pulseHeight, 'f', 0) + " mV pulse height");
        if(pulseHeight > 1200) {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
            m_ui->sdp10->setStyleSheet("QSpinBox { background-color : red; color : white; }");
        }
        else {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
            m_ui->sdp10->setStyleSheet("QSpinBox { background-color : white; color : black; }");
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
    else if(QObject::sender() == m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->openConnection){
        if(g_connection_ok){
            EnableCommunicationButtons(true);
        }
        else {
            EnableCommunicationButtons(false);
        }
    }

    else if(QObject::sender() == m_ui->ApplyAll){
        m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.ApplyVMMs(m_fecIndex, m_hybridIndex, m_vmmIndex, false);
    }
    else if(QObject::sender() == m_ui->vmmResetAll){
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            if (m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.GetFEC(j) ){
                m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
            }
        }
        m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->offACQ->setCheckable(true);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->offACQ->setChecked(true);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->onACQ->setChecked(false);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.ApplyVMMs(m_fecIndex, m_hybridIndex, m_vmmIndex, true);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->Send->setEnabled(true);
    }
    else if(QObject::sender() == m_ui->vmmReset){
        for (unsigned short j=0; j < FECS_PER_DAQ; j++){
            if (m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.GetFEC(j) ){
                m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
            }
        }
        m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->offACQ->setCheckable(true);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->offACQ->setChecked(true);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->onACQ->setChecked(false);
        SetVMM("reset1", 1);
        SetVMM("reset2", 1);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->SendConfig(m_hybridIndex, m_vmmIndex);
        QThread::sleep(1);
        SetVMM("reset1", 0);
        SetVMM("reset2", 0);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->SendConfig(m_hybridIndex, m_vmmIndex);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->Send->setEnabled(true);
    }
    else if(QObject::sender() == m_ui->ChannelSettingsAll){
        m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.ApplyChannelSettingsVMMs(m_fecIndex, m_hybridIndex, m_vmmIndex);
    }

}

void VMMWindow::EnableCommunicationButtons(bool enable) {
    m_ui->readADC->setEnabled(enable);
    m_ui->vmmReset->setEnabled(enable);
    m_ui->ApplyAll->setEnabled(enable);
    m_ui->ChannelSettingsAll->setEnabled(enable);
    m_ui->vmmResetAll->setEnabled(enable);
}


void VMMWindow::CreateChannelsFields()
{
    VMMSCBoolAll = false;
    VMMSLBoolAll = false;
    VMMSTBoolAll = false;
    VMMSTHBoolAll = false;
    VMMSMBoolAll = false;
    VMMSMXBoolAll = false;
    VMMSZ010bBoolAll = false;
    VMMSZ08bBoolAll = false;
    VMMSZ06bBoolAll = false;

    QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>();
    for(int n=0; n<childPushButtons.size(); n++) {
        connect(childPushButtons[n],SIGNAL(pressed()),
                this,SLOT(onUpdatePushButtons()));
    }

    QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>();
    for(int n=0; n<childComboBoxes.size(); n++) {
        if(childComboBoxes[n]->objectName().startsWith("VMMSDVoltage") || childComboBoxes[n]->objectName().startsWith("VMMSZ010b")) {
            connect(childComboBoxes[n],SIGNAL(currentIndexChanged(int)),
                    this,SLOT(onUpdateComboBoxes(int)));

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
                    this,SLOT(onUpdateComboBoxes(int)));
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
                    this,SLOT(onUpdateComboBoxes(int)));
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

    //////////////////////////////////////////////////////////////////////////
    // -------------------------------------------------------------------- //
    // channel connections
    // -------------------------------------------------------------------- //
    //////////////////////////////////////////////////////////////////////////

    // -------------------------------------------------------------------- //
    // update channel voltages
    // -------------------------------------------------------------------- //
    connect(m_ui->SDLabel,  SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateComboBoxes(int)));


    // -------------------------------------------------------------------- //
    // update channel ADC values
    // -------------------------------------------------------------------- //
    connect(m_ui->SZ010bLabel,  SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateComboBoxes(int)));
    connect(m_ui->SZ08bLabel,   SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateComboBoxes(int)));
    connect(m_ui->SZ06bLabel,   SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateComboBoxes(int)));


    // -------------------------------------------------------------------- //
    // update channel states (green boxes in Channel Fields)
    // -------------------------------------------------------------------- //
    connect(m_ui->SCLabel,   SIGNAL(pressed()), this, SLOT(onUpdatePushButtons()));
    connect(m_ui->SLLabel,   SIGNAL(pressed()), this, SLOT(onUpdatePushButtons()));
    connect(m_ui->STLabel,   SIGNAL(pressed()), this, SLOT(onUpdatePushButtons()));
    connect(m_ui->STHLabel,   SIGNAL(pressed()), this, SLOT(onUpdatePushButtons()));
    connect(m_ui->SMLabel,   SIGNAL(pressed()), this, SLOT(onUpdatePushButtons()));
    connect(m_ui->SMXLabel,  SIGNAL(pressed()), this, SLOT(onUpdatePushButtons()));



}

// ------------------------------------------------------------------------- //
void VMMWindow::onUpdatePushButtons()
{
    QString name = QObject::sender()->objectName();
    // ***********************  SC  ********************************* //
    if(m_ui->SCLabel == QObject::sender()){

        QRegularExpression exp("VMMSC_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(VMMSCBoolAll==0){

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("background-color: green");
                VMMSCBool[n]=true;
                SetVMM("sc", 1, n);
                VMMSCBoolAll=1;
            }
        }
        else {

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("");
                VMMSCBool[n]=false;
                SetVMM("sc", 0, n);
                VMMSCBoolAll=0;
            }
        }

    }

    // ***********************  SL  ********************************* //
    else if(m_ui->SLLabel == QObject::sender()){
        QRegularExpression exp("VMMSL_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(VMMSLBoolAll==0){

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("background-color: green");
                VMMSLBool[n]=true;
                SetVMM("sl", 1, n);
                VMMSLBoolAll=1;
            }
        }
        else {

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("");
                VMMSLBool[n]=false;
                SetVMM("sl", 0, n);
                VMMSLBoolAll=0;
            }
        }
    }
    // ***********************  ST  ********************************* //
    else if(m_ui->STLabel == QObject::sender()){
        QRegularExpression exp("VMMST_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(VMMSTBoolAll==0){

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("background-color: green");
                VMMSTBool[n]=true;
                SetVMM("st", 1, n);
                VMMSTBoolAll=1;
            }
        }
        else {

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("");
                VMMSTBool[n]=false;
                SetVMM("st", 0, n);
                VMMSTBoolAll=0;
            }
        }
    }
    // ***********************  STH  ********************************* //
    else if(m_ui->STHLabel == QObject::sender()){
        QRegularExpression exp("VMMSTH_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(VMMSTHBoolAll==0){

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("background-color: green");
                VMMSTHBool[n]=true;
                SetVMM("sth", 1, n);
                VMMSTHBoolAll=1;
            }
        }
        else {

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("");
                VMMSTHBool[n]=false;
                SetVMM("sth", 0, n);
                VMMSTHBoolAll=0;
            }
        }
    }
    // ***********************  SM  ********************************* //
    else if(m_ui->SMLabel == QObject::sender()){
        QRegularExpression exp("VMMSM_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(VMMSMBoolAll==0){

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("background-color: green");
                VMMSMBool[n]=true;
                SetVMM("sm", 1, n);
                VMMSMBoolAll=1;
            }
        }
        else {

            for(int n=0; n<childPushButtons.size(); n++) {

                childPushButtons[n]->setStyleSheet("");
                VMMSMBool[n]=false;
                SetVMM("sm", 0, n);
                VMMSMBoolAll=0;
            }
        }
    }
    // ***********************  SMX  ********************************* //
    else if(m_ui->SMXLabel == QObject::sender()){
        QRegularExpression exp("VMMSMX_");
        QList<QPushButton *> childPushButtons = m_ui->scrollArea->findChildren<QPushButton *>(exp);
        std::sort(childPushButtons.begin(), childPushButtons.end(),
                  [](const QPushButton* x, const QPushButton* y) -> bool { return x->objectName() <  y->objectName();
        });
        if(VMMSMXBoolAll==false){

            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("background-color: green");
                VMMSMXBool[n]=true;
                SetVMM("smx", 1, n);
                VMMSMXBoolAll=true;
            }
        }
        else {

            for(int n=0; n<childPushButtons.size(); n++) {
                childPushButtons[n]->setStyleSheet("");
                VMMSMXBool[n]=false;
                SetVMM("smx", 0, n);
                VMMSMXBoolAll=false;
            }
        }
    }

    else if(name.startsWith("VMMSC_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(VMMSCBool[n]==0){
            btn->setStyleSheet("background-color: green");
            VMMSCBool[n]=true;
            SetVMM("sc", 1, n);
        }else if(VMMSCBool[n]==1){
            btn->setStyleSheet("");
            VMMSCBool[n]=false;
            SetVMM("sc", 0, n);
        }
    }
    else if(name.startsWith("VMMSL_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(VMMSLBool[n]==0){
            btn->setStyleSheet("background-color: green");
            VMMSLBool[n]=true;
            SetVMM("sl", 1, n);
        }else if(VMMSLBool[n]==1){
            btn->setStyleSheet("");
            VMMSLBool[n]=false;
            SetVMM("sl", 0, n);
        }
    }
    else if(name.startsWith("VMMST_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(VMMSTBool[n]==0){
            btn->setStyleSheet("background-color: green");
            VMMSTBool[n]=true;
            SetVMM("st", 1, n);
        }else if(VMMSTBool[n]==1){
            btn->setStyleSheet("");
            VMMSTBool[n]=false;
            SetVMM("st", 0, n);
        }
    }
    else if(name.startsWith("VMMSTH_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(VMMSTHBool[n]==0){
            btn->setStyleSheet("background-color: green");
            VMMSTHBool[n]=true;
            SetVMM("sth", 1, n);
        }else if(VMMSTHBool[n]==1){
            btn->setStyleSheet("");
            VMMSTHBool[n]=false;
            SetVMM("sth", 0, n);
        }
    }
    else if(name.startsWith("VMMSMX_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(VMMSMXBool[n]==0){
            btn->setStyleSheet("background-color: green");
            VMMSMXBool[n]=true;
            SetVMM("smx", 1, n);
        }else if(VMMSMXBool[n]==1){
            btn->setStyleSheet("");
            VMMSMXBool[n]=false;
            SetVMM("smx", 0, n);
        }
    }
    else if(name.startsWith("VMMSM_")) {
        QPushButton * btn = (QPushButton *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        if(VMMSMBool[n]==0){
            btn->setStyleSheet("background-color: green");
            VMMSMBool[n]=true;
            SetVMM("sm", 1, n);
        }else if(VMMSMBool[n]==1){
            btn->setStyleSheet("");
            VMMSMBool[n]=false;
            SetVMM("sm", 0, n);
        }
    }

}
// ------------------------------------------------------------------------- //
void VMMWindow::onUpdateComboBoxes(int index){
    QString name =  QObject::sender()->objectName();
    if(m_ui->SDLabel == QObject::sender()){
        QRegularExpression exp("VMMSDVoltage");
        QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
        std::sort(childComboBoxes.begin(), childComboBoxes.end(),
                  [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
        });
        for(int n=0; n<childComboBoxes.size(); n++) {
            VMMSDValue[n]=index;
            SetVMM("sd", index, n);
            childComboBoxes[n]->setCurrentIndex(index);
        }
    }
    else if(m_ui->SZ010bLabel == QObject::sender()){
        QRegularExpression exp("VMMSZ010b");
        QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
        std::sort(childComboBoxes.begin(), childComboBoxes.end(),
                  [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
        });
        for(int n=0; n<childComboBoxes.size(); n++) {
            VMMSDValue[n]=index;
            SetVMM("sz10b", index, n);
            childComboBoxes[n]->setCurrentIndex(index);
        }
    }
    else if(m_ui->SZ08bLabel == QObject::sender()){
        QRegularExpression exp("VMMSZ08b");
        QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
        std::sort(childComboBoxes.begin(), childComboBoxes.end(),
                  [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
        });
        for(int n=0; n<childComboBoxes.size(); n++) {
            VMMSDValue[n]=index;
            SetVMM("sz08b", index, n);
            childComboBoxes[n]->setCurrentIndex(index);
        }
    }
    else if(m_ui->SZ06bLabel == QObject::sender()){
        QRegularExpression exp("VMMSZ06b");
        QList<QComboBox *> childComboBoxes = m_ui->scrollArea->findChildren<QComboBox *>(exp);
        std::sort(childComboBoxes.begin(), childComboBoxes.end(),
                  [](const QComboBox* x, const QComboBox* y) -> bool { return x->objectName() <  y->objectName();
        });
        for(int n=0; n<childComboBoxes.size(); n++) {
            VMMSDValue[n]=index;
            SetVMM("sz06b", index, n);
            childComboBoxes[n]->setCurrentIndex(index);
        }
    }
    else if(name.startsWith("VMMSDVoltage")) {
        QComboBox * cbbox = (QComboBox *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        VMMSDValue[n]=index;
        SetVMM("sd", index, n);
        cbbox->setCurrentIndex(index);
    }
    else if(name.startsWith("VMMSZ010b")) {
        QComboBox * cbbox = (QComboBox *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        VMMSZ010bValue[n]=index;
        SetVMM("sz10b", index, n);
        cbbox->setCurrentIndex(index);
    }
    else if(name.startsWith("VMMSZ08b")) {
        QComboBox * cbbox = (QComboBox *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        VMMSZ08bValue[n]=index;
        SetVMM("sz08b", index, n);
        cbbox->setCurrentIndex(index);
    }
    else if(name.startsWith("VMMSZ06b")) {
        QComboBox * cbbox = (QComboBox *)(QObject::sender());
        int size = name.size();
        int idx =  name.indexOf('_');
        int n = name.mid(idx+1,size-idx).toInt();
        VMMSZ06bValue[n]=index;
        SetVMM("sz06b", index, n);
        cbbox->setCurrentIndex(index);
    }
}



void VMMWindow::on_readADC_clicked()
{
    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
        if (m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.GetFEC(j) ){
            m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ACQoff();
        }
    }
    m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->offACQ->setCheckable(true);
    m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->offACQ->setChecked(true);
    m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->onACQ->setChecked(false);
    m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->SendConfig(m_hybridIndex, m_vmmIndex);
    m_hybridWindow->m_fecWindow->m_daqWindow->m_ui->Send->setEnabled(true);

    int adc_chan = 2; // 0: tdo, 1: pdo, 2: Mo, 3: not used | prepare to read other channels

    int adc_result = m_hybridWindow->m_fecWindow->m_daqWindow->m_daq.m_fecs[m_fecIndex].m_fecConfigModule->ReadADC(m_hybridIndex, m_vmmIndex, adc_chan);
    double temperature = (725-adc_result)/1.85;

    if(m_ui->sm5_sm0->currentIndex()==3){
        QString text = QString::number(temperature);
        text.append(" °C");
        m_ui->ADCresult->setText(text);
    }
    else {
        QString text = QString::number(adc_result);
        m_ui->ADCresult->setText(text);
    }
    QDateTime current = QDateTime::currentDateTime();
    QString str =  current.toString(Qt::TextDate) ;
}






