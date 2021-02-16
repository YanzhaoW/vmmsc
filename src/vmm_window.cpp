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

    //    ui->stackedWidgetPage1->setStyleSheet("QTabBar::tab { height: 18px; width: 100px; }");
    //    ui->stackedWidgetPage1->setTabText(0,"Channel Registers");

    dummy = new QWidget(this);

    channelGridLayout = new QGridLayout(this);

    CreateChannelsFields();
    SetToolTips();
    LoadSettings();
    m_ui->vmmReset->setEnabled(false);
    m_ui->ApplyAll->setEnabled(false);
    m_ui->ChannelSettingsAll->setEnabled(false);
    m_ui->vmmResetAll->setEnabled(false);
    m_ui->readADC->setEnabled(false);
    //connect the settings from the GUI
    // General Settings


    connect(m_ui->sdt, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdp_2, SIGNAL(valueChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sp, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sm5_sm0, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sg, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stc, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->srat, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sng, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdp, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdrv, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->ssh, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->adcs, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->s8b, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->s6b, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->sc010b, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sc08b, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sc06b, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdcks, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdcka, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sdck6b, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));

    //Advanced Settings
    connect(m_ui->sttt, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stot, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stpp, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->st, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sfam, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateSettings()));
    //    connect(ui->ART, SIGNAL(pressed()),
    //                                    this, SLOT(updateSettings()));
    connect(m_ui->sbft, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbfp, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbfm, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbmx, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->slg, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sfm, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->slvs, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stcr, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->ssart, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->s32, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->stlc, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->srec, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sbip, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sfrst, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->nskipm_i, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0ckinv, SIGNAL(pressed()),
            this, SLOT(onUpdateSettings()));
    connect(m_ui->sL0dckinv, SIGNAL(pressed()),
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


    connect(m_hybridWindow->m_fecWindow->m_daqWindow->ui->openConnection, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->ApplyAll, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->vmmResetAll, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->vmmReset, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_ui->ChannelSettingsAll, SIGNAL(clicked()),
            this, SLOT(onUpdateSettings()));

    connect(m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule, SIGNAL(ReloadVMM()),
            this, SLOT( onReloadSettings() ));

    connect(m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule, SIGNAL(VMMUpdateChannel()),
            this, SLOT( onVMMUpdateChannel() ));

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
    m_ui->sfrst->setToolTip("fast reset at 6-b completion");
    m_ui->slvs->setToolTip("enables direct output IOs");
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
    m_ui->stotLabel->setToolTip("timing outputs control 1 (s6b must be disabled)\n• stpp,stot[00,01,10,11]: TtP,ToT,PtP,PtT\n• TtP: threshold-to-peak\n• ToT: time-over-threshold\n• PtP: pulse-at-peak (10ns) (not available with s10b)\n• PtT: peak-to-threshold (not available with s10b)");
    m_ui->sttt->setToolTip("enables direct-output logic (both timing and s6b)");
    m_ui->ssh->setToolTip("enables sub-hysteresis discrimination");
    m_ui->sc010bLabel->setToolTip("10-bit ADC conversion time");
    m_ui->sc08bLabel->setToolTip("8-bit ADC conversion time");
    m_ui->sc06bLabel->setToolTip("6-bit ADC conversion time");
    m_ui->s8b->setToolTip("8-bit ADC conversion mode");
    m_ui->s6b->setToolTip("enables 6-bit ADC (requires sttt enabled)");
    m_ui->adcs->setToolTip("enables high resolution ADCs (10/8-bit ADC enable)");
    m_ui->sdcks->setToolTip("dual clock edge serialized data enable");
    m_ui->sdcka->setToolTip("dual clock edge serialized ART enable");
    m_ui->sdck6b->setToolTip("dual clock edge serialized 6-bit enable");
    m_ui->sdrv->setToolTip("tristates analog outputs with token, used in analog mode");

    m_ui->ApplyAll->setToolTip("Applies the settings of current VMM (except channel settings) to all activated VMMs");


}
// ------------------------------------------------------------------------- //

unsigned short VMMWindow::GetVMM(std::string feature, int ch){
    unsigned short setting = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].GetRegister(feature, ch);
    return setting;
}
// ------------------------------------------------------------------------- //

bool VMMWindow::SetVMM(std::string feature, int value, int ch){
    if(m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}
bool VMMWindow::SetVMM(std::string feature, std::string value, int ch){
    if(m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hybrids[m_hybridIndex].m_vmms[m_vmmIndex].SetRegi(feature, value, ch)){
        return true;
    }
    else return false;
}

// ------------------------------------------------------------------------- //


void VMMWindow::LoadSettings()
{
    //Loading General settings and updating GUI
    m_ui->sp->setCurrentIndex(GetVMM("sp"));
    int gain = GetVMM("gain");
    m_ui->sg->setCurrentIndex(gain);
    if(!GetVMM("scmx")){
        m_ui->sm5_sm0->setCurrentIndex(GetVMM("monitoring")-1);
    }
    else m_ui->sm5_sm0->setCurrentIndex(GetVMM("monitoring")+4);
    m_ui->stc->setCurrentIndex(GetVMM("stc"));
    m_ui->srat->setCurrentIndex(GetVMM("srat"));
    m_ui->sng->setChecked(GetVMM("sng"));
    m_ui->sdp->setChecked(GetVMM("sdp"));
    m_ui->sdrv->setChecked(GetVMM("sdrv"));
    m_ui->ssh->setChecked(GetVMM("ssh"));
    m_ui->adcs->setChecked(GetVMM("s10b"));
    m_ui->s8b->setChecked(GetVMM("s8b"));
    m_ui->s6b->setChecked(GetVMM("s6b"));
    m_ui->sc010b->setCurrentIndex(GetVMM("convtime_10"));
    m_ui->sc08b->setCurrentIndex(GetVMM("convtime_8"));
    m_ui->sc06b->setCurrentIndex(GetVMM("convtime_6"));
    m_ui->sdcka->setChecked(GetVMM("sdcka"));
    m_ui->sdcks->setChecked(GetVMM("sdcks"));
    m_ui->sdck6b->setChecked(GetVMM("sdck6b"));
    m_ui->sdt->setValue(GetVMM("sdt"));
    int sdp_2 = GetVMM("sdp_2");
    m_ui->sdp_2->setValue(sdp_2);
    QString tmp;

    double val = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->ThresholdDAC_to_mV(m_ui->sdt->value());
    m_ui->dacmvLabel->setText(tmp.number(val, 'f', 0) + " mV");
    double pulseHeight = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp_2->value(), m_ui->sg->currentIndex());
    double dav_mV = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->PulserDAC_to_mV(m_ui->sdp_2->value());
    m_ui->dacmvLabel_TP->setText(tmp.number(dav_mV, 'f', 0) + " mV DAC\n"+ tmp.number(pulseHeight, 'f', 0) + " mV pulse height");
    if(pulseHeight > 1200) {
        m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
        m_ui->sdp_2->setStyleSheet("QSpinBox { background-color : red; color : white; }");
    }
    else {
        m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
        m_ui->sdp_2->setStyleSheet("QSpinBox { background-color : white; color : black; }");
    }

    //Loading Advanced settings and updating GUI
    //    ui->ART->setChecked(VMM_Get("sfa"));
    if(GetVMM("sfa")) m_ui->sfam->setCurrentIndex(GetVMM("sfam")+1);
    else m_ui->sfam->setCurrentIndex(GetVMM("sfa"));
    m_ui->st->setCurrentIndex(GetVMM("peaktime"));
    m_ui->sbfm->setChecked(GetVMM("sbfm"));
    m_ui->sbfp->setChecked(GetVMM("sbfp"));
    m_ui->sbft->setChecked(GetVMM("sbft"));
    m_ui->sbmx->setChecked(GetVMM("sbmx"));
    //    ui->scmx->setChecked(VMM_Get("scmx"));
    m_ui->sfm->setChecked(GetVMM("sfm"));
    m_ui->slg->setChecked(GetVMM("slg"));
    m_ui->sttt->setChecked(GetVMM("sttt"));
    m_ui->stpp->setCurrentIndex(GetVMM("stpp"));
    m_ui->stot->setCurrentIndex(GetVMM("stot"));


    m_ui->nskipm_i->setChecked(GetVMM("nskipm_i"));
    m_ui->s32->setChecked(GetVMM("s32"));
    m_ui->stlc->setChecked(GetVMM("stlc"));
    m_ui->sL0ckinv->setChecked(GetVMM("sL0ckinv"));
    m_ui->sL0dckinv->setChecked(GetVMM("sL0dckinv"));
    m_ui->sbip->setChecked(GetVMM("sbip"));
    m_ui->sfrst->setChecked(GetVMM("sfrst"));
    m_ui->slvs->setChecked(GetVMM("slvs"));
    m_ui->srec->setChecked(GetVMM("srec"));
    m_ui->ssart->setChecked(GetVMM("ssart"));
    m_ui->stcr->setChecked(GetVMM("stcr"));


    m_ui->slvsbc->setChecked(GetVMM("slvsbc"));
    m_ui->slvstp->setChecked(GetVMM("slvstp"));
    m_ui->slvstk->setChecked(GetVMM("slvstk"));
    m_ui->slvsdt->setChecked(GetVMM("slvsdt"));
    m_ui->slvsart->setChecked(GetVMM("slvsart"));
    m_ui->slvstki->setChecked(GetVMM("slvstki"));
    m_ui->slvsena->setChecked(GetVMM("slvsena"));
    m_ui->slvs6b->setChecked(GetVMM("slvs6b"));
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
    for (int i = 0; i<64; i++){
        VMMSCBool[i]=GetVMM("sc",i);
        VMMSMBool[i]=GetVMM("sm",i);
        VMMSTBool[i]=GetVMM("st",i);
        VMMSTHBool[i]=GetVMM("sth",i);
        VMMSLBool[i]=GetVMM("sl",i);
        VMMSMXBool[i]=GetVMM("smx",i);

        VMMSC[i]->setStyleSheet("background-color: lightGray");
        VMMSM[i]->setStyleSheet("background-color: lightGray");
        VMMST[i]->setStyleSheet("background-color: lightGray");
        VMMSTH[i]->setStyleSheet("background-color: lightGray");
        VMMSL[i]->setStyleSheet("background-color: lightGray");
        VMMSMX[i]->setStyleSheet("background-color: lightGray");
        if(VMMSCBool[i]==1) VMMSC[i]->setStyleSheet("background-color: green");
        if(VMMSMBool[i]==1) VMMSM[i]->setStyleSheet("background-color: green");
        if(VMMSTBool[i]==1) VMMST[i]->setStyleSheet("background-color: green");
        if(VMMSTHBool[i]==1) VMMSTH[i]->setStyleSheet("background-color: green");
        if(VMMSLBool[i]==1) VMMSL[i]->setStyleSheet("background-color: green");
        if(VMMSMXBool[i]==1) VMMSMX[i]->setStyleSheet("background-color: green");

        // set initial ADC values
        unsigned short ADC10_index = GetVMM("ADC0_10",i);
        unsigned short ADC08_index = GetVMM("ADC0_8",i);
        unsigned short ADC06_index = GetVMM("ADC0_6",i);
        VMMSZ010bCBox[i]->setCurrentIndex(ADC10_index);
        VMMSZ010bValue[i]=ADC10_index;
        VMMSZ08bCBox[i]->setCurrentIndex(ADC08_index);
        VMMSZ08bValue[i]=ADC08_index;
        VMMSZ06bCBox[i]->setCurrentIndex(ADC06_index);
        VMMSZ06bValue[i]=ADC06_index;

        // set initial channel voltage
        unsigned short SD_volt = GetVMM("sd",i);
        VMMSDVoltage[i]->setCurrentIndex(SD_volt);
        VMMSDValue[i]=SD_volt;
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
    SDLabel->setCurrentIndex(sd);
    SZ010bLabel->setCurrentIndex(SZ010);
    SZ08bLabel->setCurrentIndex(SZ08);
    SZ06bLabel->setCurrentIndex(SZ06);
}


void VMMWindow::onUpdateSettings()
{
    QString tmp;
    if(QObject::sender() == m_ui->sdt){
        SetVMM("sdt", m_ui->sdt->value());
        double val = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->ThresholdDAC_to_mV(m_ui->sdt->value());
        m_ui->dacmvLabel->setText(tmp.number(val, 'f', 0) + " mV");
    }
    else if(QObject::sender() == m_ui->sdp_2){
        double pulseHeight = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp_2->value(), m_ui->sg->currentIndex());
        double dav_mV = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->PulserDAC_to_mV(m_ui->sdp_2->value());
        m_ui->dacmvLabel_TP->setText(tmp.number(dav_mV, 'f', 0) + " mV DAC\n"+ tmp.number(pulseHeight, 'f', 0) + " mV pulse height");
        if(pulseHeight > 1200) {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
            m_ui->sdp_2->setStyleSheet("QSpinBox { background-color : red; color : white; }");
        }
        else {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
            m_ui->sdp_2->setStyleSheet("QSpinBox { background-color : white; color : black; }");
        }

        SetVMM("sdp_2", m_ui->sdp_2->value());
    }
    else if(QObject::sender() == m_ui->sp){
        SetVMM("sp", m_ui->sp->currentIndex());
    }
    else if(QObject::sender() == m_ui->sm5_sm0){
        if(m_ui->sm5_sm0->currentIndex()<=3){
            std::string mm_val[4] = {"Pulser_DAC", "Threshold_DAC", "Bandgap_reference", "Temperature_sensor"};
            SetVMM("monitoring",mm_val[m_ui->sm5_sm0->currentIndex()]);
        }
        else SetVMM("monitoring", m_ui->sm5_sm0->currentIndex()-4);//starting with channel 0
    }
    else if(QObject::sender() == m_ui->sg){
        SetVMM("gain", m_ui->sg->currentIndex());
        double val = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->ThresholdDAC_to_mV(m_ui->sdt->value());
        m_ui->dacmvLabel->setText(tmp.number(val, 'f', 0) + " mV");
        double pulseHeight = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->PulserDAC_to_PulseHeight_mV(m_ui->sdp_2->value(), m_ui->sg->currentIndex());
        double dav_mV = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_calib->PulserDAC_to_mV(m_ui->sdp_2->value());
        m_ui->dacmvLabel_TP->setText(tmp.number(dav_mV, 'f', 0) + " mV DAC\n"+ tmp.number(pulseHeight, 'f', 0) + " mV pulse height");
        if(pulseHeight > 1200) {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : red; }");
            m_ui->sdp_2->setStyleSheet("QSpinBox { background-color : red; color : white; }");
        }
        else {
            m_ui->dacmvLabel_TP->setStyleSheet("QLabel { color : black; }");
            m_ui->sdp_2->setStyleSheet("QSpinBox { background-color : white; color : black; }");
        }
    }
    else if(QObject::sender() == m_ui->stc){
        SetVMM("stc", m_ui->stc->currentIndex());
    }
    else if(QObject::sender() == m_ui->srat){
        SetVMM("srat", m_ui->srat->currentIndex());
    }
    else if(QObject::sender() == m_ui->sng){
        SetVMM("sng", !m_ui->sng->isChecked());
    }
    else if(QObject::sender() == m_ui->sdp){
        SetVMM("sdp", !m_ui->sdp->isChecked());
    }
    else if(QObject::sender() == m_ui->sdrv){
        SetVMM("sdrv", !m_ui->sdrv->isChecked());
    }
    else if(QObject::sender() == m_ui->ssh){
        SetVMM("ssh", !m_ui->ssh->isChecked());
    }
    else if(QObject::sender() == m_ui->adcs){
        SetVMM("s10b", !m_ui->adcs->isChecked());
    }
    else if(QObject::sender() == m_ui->s8b){
        SetVMM("s8b", !m_ui->s8b->isChecked());
    }
    else if(QObject::sender() == m_ui->s6b){
        SetVMM("s6b", !m_ui->s6b->isChecked());
        if(!m_ui->s6b->isChecked()){
            m_ui->sttt->setChecked(true);
            SetVMM("sttt",1);
        }
    }
    else if(QObject::sender() == m_ui->sc010b){
        SetVMM("convtime_10", m_ui->sc010b->currentIndex());
    }
    else if(QObject::sender() == m_ui->sc08b){
        SetVMM("convtime_8", m_ui->sc08b->currentIndex());
    }
    else if(QObject::sender() == m_ui->sc06b){
        SetVMM("convtime_6", m_ui->sc06b->currentIndex());
    }

    else if(QObject::sender() == m_ui->sdcks){
        SetVMM("sdcks", !m_ui->sdcks->isChecked());
    }
    else if(QObject::sender() == m_ui->sdcka){
        SetVMM("sdcka", !m_ui->sdcka->isChecked());
    }
    else if(QObject::sender() == m_ui->sdck6b){
        SetVMM("sdck6b", !m_ui->sdck6b->isChecked());
    }
    else if(QObject::sender() == m_ui->sttt){
        SetVMM("sttt", !m_ui->sttt->isChecked());
        if(m_ui->sttt->isChecked()) {
            SetVMM("s6b",0);
            m_ui->s6b->setChecked(false);
        }
    }

    else if(QObject::sender() == m_ui->stpp){
        SetVMM("stpp", m_ui->stpp->currentIndex());
    }
    else if(QObject::sender() == m_ui->stot){
        SetVMM("stot", m_ui->stot->currentIndex());
    }
    else if(QObject::sender() == m_ui->st){
        SetVMM("peaktime", m_ui->st->currentIndex());
    }
    else if(QObject::sender() == m_ui->sfam){
        if(m_ui->sfam->currentIndex()>0) {
            SetVMM("sfam", m_ui->sfam->currentIndex()-1);
            SetVMM("sfa",1);
        }
        else SetVMM("sfa", 0);
    }
    //    else if(QObject::sender() == ui->ART){
    //        VMM_Set("sfa", !ui->ART->isChecked());
    //    }
    else if(QObject::sender() == m_ui->sbft){
        SetVMM("sbft", !m_ui->sbft->isChecked());
    }
    else if(QObject::sender() == m_ui->sbfp){
        SetVMM("sbfp", !m_ui->sbfp->isChecked());
    }
    else if(QObject::sender() == m_ui->sbfm){
        SetVMM("sbfm", !m_ui->sbfm->isChecked());
    }
    else if(QObject::sender() == m_ui->sbmx){
        SetVMM("sbmx", !m_ui->sbmx->isChecked());
    }
    else if(QObject::sender() == m_ui->slg){
        SetVMM("slg", !m_ui->slg->isChecked());
    }
    else if(QObject::sender() == m_ui->sfm){
        SetVMM("sfm", !m_ui->sfm->isChecked());
    }
    else if(QObject::sender() == m_ui->slvs){
        SetVMM("slvs", !m_ui->slvs->isChecked());
    }
    else if(QObject::sender() == m_ui->stcr){
        SetVMM("stcr", !m_ui->stcr->isChecked());
    }
    else if(QObject::sender() == m_ui->ssart){
        SetVMM("ssart", !m_ui->ssart->isChecked());
    }
    else if(QObject::sender() == m_ui->s32){
        SetVMM("s32", !m_ui->s32->isChecked());
    }
    else if(QObject::sender() == m_ui->stlc){
        SetVMM("stlc", !m_ui->stlc->isChecked());
    }
    else if(QObject::sender() == m_ui->srec){
        SetVMM("srec", !m_ui->srec->isChecked());
    }
    else if(QObject::sender() == m_ui->sbip){
        SetVMM("sbip", !m_ui->sbip->isChecked());
    }
    else if(QObject::sender() == m_ui->sfrst){
        SetVMM("sfrst", !m_ui->sfrst->isChecked());
    }
    else if(QObject::sender() == m_ui->nskipm_i){
        SetVMM("nskipm_i", !m_ui->nskipm_i->isChecked());
    }
    else if(QObject::sender() == m_ui->sL0ckinv){
        SetVMM("sL0ckinv", !m_ui->sL0ckinv->isChecked());
    }
    else if(QObject::sender() == m_ui->sL0dckinv){
        SetVMM("sL0dckinv", !m_ui->sL0dckinv->isChecked());
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
    else if(QObject::sender() == m_hybridWindow->m_fecWindow->m_daqWindow->ui->openConnection){
        if(m_hybridWindow->m_fecWindow->m_daqWindow->ui->connectionLabel->text()==QString("all alive")){
            m_ui->readADC->setEnabled(true);
            m_ui->vmmReset->setEnabled(true);
            m_ui->ApplyAll->setEnabled(true);
            m_ui->ChannelSettingsAll->setEnabled(true);
            m_ui->vmmResetAll->setEnabled(true);
        }
        else {
            m_ui->readADC->setEnabled(false);
            m_ui->vmmReset->setEnabled(false);
            m_ui->ApplyAll->setEnabled(false);
            m_ui->ChannelSettingsAll->setEnabled(false);
            m_ui->vmmResetAll->setEnabled(false);
        }
    }

    else if(QObject::sender() == m_ui->ApplyAll){
        m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].ApplyVMMs(m_fecIndex, m_hybridIndex, m_vmmIndex, false);
    }
    else if(QObject::sender() == m_ui->vmmResetAll){
        m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].ApplyVMMs(m_fecIndex, m_hybridIndex, m_vmmIndex, true);
    }
    else if(QObject::sender() == m_ui->vmmReset){
        SetVMM("reset1", 1);
        SetVMM("reset2", 1);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->SendConfig(m_hybridIndex, m_vmmIndex);
        QThread::sleep(1);
        SetVMM("reset1", 0);
        SetVMM("reset2", 0);
        m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->SendConfig(m_hybridIndex, m_vmmIndex);
    }
    else if(QObject::sender() == m_ui->ChannelSettingsAll){
       m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].ApplyChannelSettingsVMMs(m_fecIndex, m_hybridIndex, m_vmmIndex);
    }

}

// ------------------------------------------------------------------------- //
bool VMMWindow::eventFilter(QObject *obj, QEvent *event)
{
    if(event->type() == QEvent::Wheel)
    {
        //if(obj->hasFocus()) {
        //            ui->scrollArea->setFocus();
        event->ignore();
        //}
        //    qDebug() << "Wheel event blocked";
        return true;
    }
    return false;
}

void VMMWindow::CreateChannelsFields()
{
    //    VMM_Set("sd", 1, 2);
    Font.setPointSize(10);
    Font.setFamily("Arial");
    //    int margin = 8;
    //    channelGridLayout->setContentsMargins(margin*0.75, margin, margin*2*10, margin);
    channelGridLayout->setHorizontalSpacing(1);
    channelGridLayout->setVerticalSpacing(1);
    //    ui->gridLayout_channels->setContentsMargins(margin*0.75, margin, margin*4.4*5, margin);
    m_ui->gridLayout_channels->setHorizontalSpacing(1);
    m_ui->gridLayout_channels->setVerticalSpacing(1);
    m_ui->gridLayout_channels->setGeometry(QRect(700,10,420, 12));
    //    ui->gridLayout_channels->setSpacing(1);
    QString initialValueRadio = "";
    QString counter;


    SCLabel = new QPushButton("SC");
    SLLabel = new QPushButton("SL");
    STLabel = new QPushButton("ST");
    STHLabel = new QPushButton("STH");
    SMLabel = new QPushButton("SM");
    SDLabel = new QComboBox();
    SZ010bLabel = new QComboBox();
    SZ08bLabel = new QComboBox();
    SZ06bLabel = new QComboBox();
    SMXLabel = new QPushButton("SMX");

    SCLabel->setToolTip("large sensor capacitance mode (off <∼200 pF ,on >∼200 pF )");
    SLLabel->setToolTip("leakage current disable [0=enabled]");
    STLabel ->setToolTip("300 fF test capacitor [1=enabled]");
    STHLabel ->setToolTip("multiplies test capacitor by 10");
    SMLabel ->setToolTip("mask enable");
    SDLabel ->setToolTip("trim threshold DAC");
    SZ010bLabel ->setToolTip("10-bit ADC zero");
    SZ08bLabel ->setToolTip("8-bit ADC zero");
    SZ06bLabel ->setToolTip("6-bit ADC zero");
    SMXLabel ->setToolTip("channel monitor mode ( [0] analog output, [1] trimmed threshold))");

    for(int i = 0; i < 32; i++) {
        SDLabel->addItem(counter.setNum(i)+" mV");
    }
    for(int i = 0; i < 32; i++) {
        SZ010bLabel->addItem(counter.setNum(i)+" mV");
    }
    for(int i = 0; i < 16; i++) {
        SZ08bLabel->addItem(counter.setNum(i)+" ns");
    }
    for(int i = 0; i < 8; i++) {
        SZ06bLabel->addItem(counter.setNum(i)+" mV");
    }


    VMMSCBoolAll=0;
    VMMSLBoolAll=0;
    VMMSTBoolAll=0;
    VMMSTHBoolAll=0;
    VMMSMBoolAll=0;
    VMMSMXBoolAll=0;
    VMMSZ010bBoolAll=0;
    VMMSZ08bBoolAll=0;
    VMMSZ06bBoolAll=0;

    SCLabel->setFixedSize(20,15);
    SLLabel->setFixedSize(20,15);
    STLabel->setFixedSize(20,15);
    STHLabel->setFixedSize(30,15);
    SMLabel->setFixedSize(20,15);
    SDLabel->setFixedSize    (58,19);
    SZ010bLabel->setFixedSize(59,19);
    SZ08bLabel->setFixedSize (53,19);
    SZ06bLabel->setFixedSize (52,19);
    SMXLabel->setFixedSize(30,15);

    SDLabel     ->installEventFilter(this);
    SZ010bLabel ->installEventFilter(this);
    SZ08bLabel  ->installEventFilter(this);
    SZ06bLabel  ->installEventFilter(this);

    STHLabel->setFont(Font);
    SCLabel->setFont(Font);
    SLLabel->setFont(Font);
    STLabel->setFont(Font);
    SMLabel->setFont(Font);
    SDLabel->setFont(Font);
    SZ010bLabel->setFont(Font);
    SZ08bLabel->setFont(Font);
    SZ06bLabel->setFont(Font);
    SMXLabel->setFont(Font);

    int col = 0;
    QLabel *spacer1 = new QLabel("");
    QLabel *spacer2 = new QLabel("");
    QLabel *labelSd = new QLabel("SD");
    QLabel *labelSz10b = new QLabel("SZ010b");
    QLabel *labelSz08b = new QLabel("SZ08b");
    QLabel *labelSz06b = new QLabel("SZ06b");
    spacer1->setFixedSize    (20,19);
    spacer2->setFixedSize    (20,19);
    labelSd->setFixedSize    (58,19);
    labelSz10b->setFixedSize(59,19);
    labelSz08b->setFixedSize (53,19);
    labelSz06b->setFixedSize (52,19);
    labelSd->setAlignment(Qt::AlignCenter);
    labelSz10b->setAlignment(Qt::AlignCenter);
    labelSz08b->setAlignment(Qt::AlignCenter);
    labelSz06b->setAlignment(Qt::AlignCenter);
    m_ui->gridLayout_channels->addWidget(spacer1,     0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(spacer1,     0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(spacer1,     0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(spacer1,     0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(spacer1,    0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(spacer1,     0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(labelSd,     0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(spacer1,    0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(labelSz10b, 0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(labelSz08b,  0, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(labelSz06b,  0, col, Qt::AlignCenter);

    col = 0;
    m_ui->gridLayout_channels->addWidget(spacer2,     1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(SCLabel,     1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(SLLabel,     1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(STLabel,     1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(STHLabel,    1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(SMLabel,     1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(SDLabel,     1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(SMXLabel,    1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(SZ010bLabel, 1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(SZ08bLabel,  1, col, Qt::AlignCenter); col++;
    m_ui->gridLayout_channels->addWidget(SZ06bLabel,  1, col, Qt::AlignCenter);

    for (int i = 0; i<64; i++){
        //VMMChannel[i] = new QLineEdit(counter.setNum(i+1),m_ui->stackedWidgetPage1);
        VMMChannel[i] = new QLineEdit(counter.setNum(i),m_ui->stackedWidgetPage1);
        VMMChannel[i]->setAlignment(Qt::AlignHCenter);

        VMMSC[i] = new QPushButton(initialValueRadio,m_ui->stackedWidgetPage1);
        VMMSL[i] = new QPushButton(initialValueRadio,m_ui->stackedWidgetPage1);
        VMMST[i] = new QPushButton(initialValueRadio,m_ui->stackedWidgetPage1);
        VMMSTH[i]= new QPushButton(initialValueRadio,m_ui->stackedWidgetPage1);
        VMMSM[i] = new QPushButton(initialValueRadio,m_ui->stackedWidgetPage1);
        VMMSMX[i]= new QPushButton(initialValueRadio,m_ui->stackedWidgetPage1);


        VMMSC[i]->setFixedSize (15,15);
        VMMST[i]->setFixedSize (15,15);
        VMMSTH[i]->setFixedSize (15,15);
        VMMSL[i]->setFixedSize (15,15);
        VMMSM[i]->setFixedSize (15,15);
        VMMSMX[i]->setFixedSize(15,15);

        VMMSCBool[i]=GetVMM("sc",i);
        VMMSMBool[i]=GetVMM("sm",i);
        VMMSTBool[i]=GetVMM("st",i);
        VMMSTHBool[i]=GetVMM("sth",i);
        VMMSLBool[i]=GetVMM("sl",i);
        VMMSMXBool[i]=GetVMM("smx",i);

        VMMSC[i]->setStyleSheet("background-color: lightGray");
        VMMSM[i]->setStyleSheet("background-color: lightGray");
        VMMST[i]->setStyleSheet("background-color: lightGray");
        VMMSTH[i]->setStyleSheet("background-color: lightGray");
        VMMSL[i]->setStyleSheet("background-color: lightGray");
        VMMSMX[i]->setStyleSheet("background-color: lightGray");
        if(VMMSCBool[i]==1) VMMSC[i]->setStyleSheet("background-color: green");
        if(VMMSMBool[i]==1) VMMSM[i]->setStyleSheet("background-color: green");
        if(VMMSTBool[i]==1) VMMST[i]->setStyleSheet("background-color: green");
        if(VMMSTHBool[i]==1) VMMSTH[i]->setStyleSheet("background-color: green");
        if(VMMSLBool[i]==1) VMMSL[i]->setStyleSheet("background-color: green");
        if(VMMSMXBool[i]==1) VMMSMX[i]->setStyleSheet("background-color: green");


        VMMChannel[i]->setEnabled(0);
        VMMSDVoltage[i] = new QComboBox(m_ui->stackedWidgetPage1);
        VMMSDVoltage[i]->setFixedSize(58,20);
        VMMSDVoltage[i]->setFont(Font);
        //        VMMSDVoltage[i]->setFocusPolicy(Qt::StrongFocus);
        for(int j=0;j<32;j++){
            VMMSDVoltage[i]->addItem(counter.setNum(j)+" mV");
        }

        VMMSZ010bCBox[i] = new QComboBox(m_ui->stackedWidgetPage1);
        VMMSZ010bCBox[i]->setFixedSize(58,20);
        VMMSZ010bCBox[i]->setFont(Font);

        VMMSZ08bCBox[i] = new QComboBox(m_ui->stackedWidgetPage1);
        VMMSZ08bCBox[i]->setFixedSize(52,20);
        VMMSZ08bCBox[i]->setFont(Font);

        VMMSZ06bCBox[i] = new QComboBox(m_ui->stackedWidgetPage1);
        VMMSZ06bCBox[i]->setFixedSize(52,20);
        VMMSZ06bCBox[i]->setFont(Font);


        for(int j=0;j<32;j++){
            VMMSZ010bCBox[i]->addItem(counter.setNum(j)+" mV");
        }
        for(int j=0;j<16;j++){
            VMMSZ08bCBox[i]->addItem(counter.setNum(j)+" ns");
        }
        for(int j=0;j<8;j++){
            VMMSZ06bCBox[i]->addItem(counter.setNum(j)+" mV");
        }

        // set initial ADC values
        unsigned short ADC10_index = GetVMM("ADC0_10",i);
        unsigned short ADC08_index = GetVMM("ADC0_8",i);
        unsigned short ADC06_index = GetVMM("ADC0_6",i);
        VMMSZ010bCBox[i]->setCurrentIndex(ADC10_index);
        VMMSZ010bValue[i]=ADC10_index;
        VMMSZ08bCBox[i]->setCurrentIndex(ADC08_index);
        VMMSZ08bValue[i]=ADC08_index;
        VMMSZ06bCBox[i]->setCurrentIndex(ADC06_index);
        VMMSZ06bValue[i]=ADC06_index;


        VMMChannel[i]->setFixedSize(20,18);

        channelGridLayout->addWidget(VMMChannel[i],         i+1,1, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSC[i],              i+1,2, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSL[i],              i+1,3, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMST[i],              i+1,4, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSTH[i],             i+1,5, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSM[i],              i+1,6, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSDVoltage[i],       i+1,7, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSMX[i],             i+1,8, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSZ010bCBox[i],      i+1,9, Qt::AlignCenter);
        VMMSZ010bCBox[i]->setFocusPolicy(Qt::StrongFocus);
        channelGridLayout->addWidget(VMMSZ08bCBox[i],       i+1,10, Qt::AlignCenter);
        channelGridLayout->addWidget(VMMSZ06bCBox[i],       i+1,11, Qt::AlignCenter);

        VMMSDVoltage[i]         ->installEventFilter(this);
        VMMSDVoltage[i]->setFocusPolicy(Qt::ClickFocus);
        VMMSZ010bCBox[i]        ->installEventFilter(this);
        VMMSZ08bCBox[i]         ->installEventFilter(this);
        VMMSZ06bCBox[i]         ->installEventFilter(this);

        // set initial channel voltage
        unsigned short SD_volt = GetVMM("sd",i);
        VMMSDVoltage[i]->setCurrentIndex(SD_volt);
        VMMSDValue[i]=SD_volt;

    } // i

    channelGridLayout->setGeometry(QRect(700,12,50,50));
    channelGridLayout->setSpacing(1);
    channelGridLayout->setGeometry(QRect(700,12,400,2000));
    dummy->setLayout(channelGridLayout);

    //ui->scrollArea->setLayout(channelGridLayout);
    m_ui->scrollArea->setWidget(dummy);

    m_ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    //ui->tab_3->setGeometry(QRect(620,12,50,50));
    //ui->tab_3->setGeometry(QRect(620,12,730,700));
    //ui->tab_3->setLayout(channelGridLayout);

    //////////////////////////////////////////////////////////////////////////
    // -------------------------------------------------------------------- //
    // channel connections
    // -------------------------------------------------------------------- //
    //////////////////////////////////////////////////////////////////////////

    // -------------------------------------------------------------------- //
    // update channel voltages
    // -------------------------------------------------------------------- //
    connect(SDLabel,  SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateChannelVoltages(int)));


    // -------------------------------------------------------------------- //
    // update channel ADC values
    // -------------------------------------------------------------------- //
    connect(SZ010bLabel,  SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateChannelADCs(int)));
    connect(SZ08bLabel,   SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateChannelADCs(int)));
    connect(SZ06bLabel,   SIGNAL(currentIndexChanged(int)),
            this, SLOT(onUpdateChannelADCs(int)));


    // -------------------------------------------------------------------- //
    // update channel states (green boxes in Channel Fields)
    // -------------------------------------------------------------------- //
    connect(SCLabel,   SIGNAL(pressed()), this, SLOT(onUpdateChannelState()));
    connect(SLLabel,   SIGNAL(pressed()), this, SLOT(onUpdateChannelState()));
    connect(STLabel,   SIGNAL(pressed()), this, SLOT(onUpdateChannelState()));
    connect(STHLabel,   SIGNAL(pressed()), this, SLOT(onUpdateChannelState()));
    connect(SMLabel,   SIGNAL(pressed()), this, SLOT(onUpdateChannelState()));
    connect(SMXLabel,  SIGNAL(pressed()), this, SLOT(onUpdateChannelState()));

    ///////////////////////////////////////////////////////////////////
    // do updates for individual channels
    ///////////////////////////////////////////////////////////////////
    for(int i = 0; i < 64; i++) {

        // ---------- channel voltages ----------- //
        connect(VMMSDVoltage[i],SIGNAL(currentIndexChanged(int)),
                this,SLOT(onUpdateChannelVoltages(int)));

        // ---------------- ADCs ---------------- //
        connect(VMMSZ010bCBox[i],SIGNAL(currentIndexChanged(int)),
                this,SLOT(onUpdateChannelADCs(int)));
        connect(VMMSZ08bCBox[i],SIGNAL(currentIndexChanged(int)),
                this,SLOT(onUpdateChannelADCs(int)));
        connect(VMMSZ06bCBox[i],SIGNAL(currentIndexChanged(int)),
                this,SLOT(onUpdateChannelADCs(int)));
        // ----------- channel states ----------- //
        connect(VMMSC[i],SIGNAL(pressed()),this,SLOT(onUpdateChannelState()));
        connect(VMMSM[i],SIGNAL(pressed()),this,SLOT(onUpdateChannelState()));
        connect(VMMST[i],SIGNAL(pressed()),this,SLOT(onUpdateChannelState()));
        connect(VMMSTH[i],SIGNAL(pressed()),this,SLOT(onUpdateChannelState()));
        connect(VMMSL[i],SIGNAL(pressed()),this,SLOT(onUpdateChannelState()));
        connect(VMMSMX[i],SIGNAL(pressed()),this,SLOT(onUpdateChannelState()));
    }

}
// ------------------------------------------------------------------------- //
void VMMWindow::onUpdateChannelState()
{
    // ***********************  SC  ********************************* //
    if(SCLabel == QObject::sender()){
        if(VMMSCBoolAll==0){
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSC[j]->setStyleSheet("background-color: green");
                VMMSCBool[j]=true;
                SetVMM("sc", 1, j);
            }
            VMMSCBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSC[j]->setStyleSheet("background-color: lightGray");
                VMMSCBool[j]=0;
                SetVMM("sc", 0, j);
            }
            VMMSCBoolAll=0;
        }
    }

    // ***********************  SL  ********************************* //
    if(SLLabel == QObject::sender()){
        if(VMMSLBoolAll==0){
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSL[j]->setStyleSheet("background-color: green");
                VMMSLBool[j]=true;
                SetVMM("sl", 1, j);
            }
            VMMSLBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSL[j]->setStyleSheet("background-color: lightGray");
                VMMSLBool[j]=0;
                SetVMM("sl", 0, j);
            }
            VMMSLBoolAll=0;
        }
    }
    // ***********************  ST  ********************************* //
    if(STLabel == QObject::sender()){
        if(VMMSTBoolAll==0){
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMST[j]->setStyleSheet("background-color: green");
                VMMSTBool[j]=true;
                SetVMM("st", 1, j);
            }
            VMMSTBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMST[j]->setStyleSheet("background-color: lightGray");
                VMMSTBool[j]=0;
                SetVMM("st", 0, j);
            }
            VMMSTBoolAll=0;
        }
    }
    // ***********************  STH  ********************************* //
    if(STHLabel == QObject::sender()){
        if(VMMSTHBoolAll==0){
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSTH[j]->setStyleSheet("background-color: green");
                VMMSTHBool[j]=true;
                SetVMM("sth", 1, j);
            }
            VMMSTHBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSTH[j]->setStyleSheet("background-color: lightGray");
                VMMSTHBool[j]=0;
                SetVMM("sth", 0, j);
            }
            VMMSTHBoolAll=0;
        }
    }
    // ***********************  SM  ********************************* //
    if(SMLabel == QObject::sender()){
        if(VMMSMBoolAll==0){
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSM[j]->setStyleSheet("background-color: green");
                VMMSMBool[j]=true;
                SetVMM("sm", 1, j);
            }
            VMMSMBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSM[j]->setStyleSheet("background-color: lightGray");
                VMMSMBool[j]=0;
                SetVMM("sm", 0, j);
            }
            VMMSMBoolAll=0;
        }
    }
    // ***********************  SMX  ********************************* //
    if(SMXLabel == QObject::sender()){
        if(VMMSMXBoolAll==0){
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSMX[j]->setStyleSheet("background-color: green");
                VMMSMXBool[j]=true;
                SetVMM("smx", 1, j);
            }
            VMMSMXBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
                //for(int j=0;j<32;j++){
                VMMSMX[j]->setStyleSheet("background-color: lightGray");
                VMMSMXBool[j]=0;
                SetVMM("smx", 0, j);
            }
            VMMSMXBoolAll=0;
        }
    }
    // *********************  Loop Individually  ********************** //
    for(int i=0;i<64;i++){
        if(VMMSC[i] == QObject::sender()){
            if(VMMSCBool[i]==0){
                VMMSC[i]->setStyleSheet("background-color: green");
                VMMSCBool[i]=true;
                SetVMM("sc", 1, i);
            }else if(VMMSCBool[i]==1){
                VMMSC[i]->setStyleSheet("background-color: lightGray");
                VMMSCBool[i]=false;
                SetVMM("sc", 0, i);
            }
        }else if(VMMST[i] == QObject::sender()){
            if(VMMSTBool[i]==0){
                VMMST[i]->setStyleSheet("background-color: green");
                VMMSTBool[i]=true;
                SetVMM("st", 1, i);
            }else if(VMMSTBool[i]==1){
                VMMST[i]->setStyleSheet("background-color: lightGray");
                VMMSTBool[i]=false;
                SetVMM("st", 0, i);
            }
        }else if(VMMSTH[i] == QObject::sender()){
            if(VMMSTHBool[i]==0){
                VMMSTH[i]->setStyleSheet("background-color: green");
                VMMSTHBool[i]=true;
                SetVMM("sth", 1, i);
            }else if(VMMSTHBool[i]==1){
                VMMSTH[i]->setStyleSheet("background-color: lightGray");
                VMMSTHBool[i]=false;
                SetVMM("sth", 0, i);
            }
        }else if(VMMSL[i] == QObject::sender()){
            if(VMMSLBool[i]==0){
                VMMSL[i]->setStyleSheet("background-color: green");
                VMMSLBool[i]=true;
                SetVMM("sl", 1, i);
            }else if(VMMSLBool[i]==1){
                VMMSL[i]->setStyleSheet("background-color: lightGray");
                VMMSLBool[i]=false;
                SetVMM("sl", 0, i);
            }
        }else if(VMMSM[i] == QObject::sender()){
            if(VMMSMBool[i]==0){
                VMMSM[i]->setStyleSheet("background-color: green");
                VMMSMBool[i]=true;
                SetVMM("sm", 1, i);
            }else if(VMMSMBool[i]==1){
                VMMSM[i]->setStyleSheet("background-color: lightGray");
                VMMSMBool[i]=false;
                SetVMM("sm", 0, i);
            }
        }else if(VMMSMX[i] == QObject::sender()){
            if(VMMSMXBool[i]==0){
                VMMSMX[i]->setStyleSheet("background-color: green");
                VMMSMXBool[i]=true;
                SetVMM("smx", 1, i);
            }else if(VMMSMXBool[i]==1){
                VMMSMX[i]->setStyleSheet("background-color: lightGray");
                VMMSMXBool[i]=false;
                SetVMM("smx", 0, i);
            }
        }

    }
}
// ------------------------------------------------------------------------- //
void VMMWindow::onUpdateChannelVoltages(int index){
    // ***********************  SD  ******************************** //
    if(SDLabel == QObject::sender()){
        for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
            VMMSDVoltage[j]->setCurrentIndex(index);
            VMMSDValue[j]=index;
        }
    }

    for(int i=0;i<64;i++){
        if(VMMSDVoltage[i] == QObject::sender()){
            VMMSDValue[i]=index;
            //qDebug() << "Voltage of channel " << i << " changed to " << index << ".";
            SetVMM("sd", index, i);
        }
    }
}
// ------------------------------------------------------------------------- //
void VMMWindow::onUpdateChannelADCs(int index)
{
    // ***********************  SD  ******************************* //
    for(int j=0;j<64;j++){
        //for(int j=0;j<32;j++){
        if(SZ010bLabel == QObject::sender()){
            VMMSZ010bCBox[j]->setCurrentIndex(index);
            VMMSZ010bValue[j]=index;
        }
        if(SZ08bLabel == QObject::sender()){
            VMMSZ08bCBox[j]->setCurrentIndex(index);
            VMMSZ08bValue[j]=index;
        }
        if(SZ06bLabel == QObject::sender()){
            VMMSZ06bCBox[j]->setCurrentIndex(index);
            VMMSZ06bValue[j]=index;
        }
    }

    for(int i=0;i<64;i++){
        if(VMMSZ010bCBox[i] == QObject::sender()){
            VMMSZ010bValue[i]=index;
            SetVMM("ADC0_10", index, i);
        }
        if(VMMSZ08bCBox[i] == QObject::sender()){
            VMMSZ08bValue[i]=index;
            SetVMM("ADC0_8", index, i);
        }
        if(VMMSZ06bCBox[i] == QObject::sender()){
            VMMSZ06bValue[i]=index;
            SetVMM("ADC0_6", index, i);
        }
    }
}
// ------------------------------------------------------------------------- //



void VMMWindow::on_readADC_clicked()
{
    int adc_chan = 2; // 0: tdo, 1: pdo, 2: Mo, 3: not used | prepare to read other channels

    int adc_result = m_hybridWindow->m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_fecConfigModule->ReadADC(m_hybridIndex, m_vmmIndex, adc_chan);
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






