#include "vmm_window.h"

vmm_window::vmm_window(hybrid_window *top, unsigned short fec, unsigned short hdmi, unsigned short hybrid, unsigned short vmm, QWidget *parent) :
    QWidget(parent),
    root_hybrid{top},
    fec_index{fec},
    hdmi_index{hdmi},
    hybrid_index{hybrid},
    vmm_index{vmm},
    ui(new Ui::vmm_window)
{
    ui->setupUi(this);

//    ui->stackedWidgetPage1->setStyleSheet("QTabBar::tab { height: 18px; width: 100px; }");
//    ui->stackedWidgetPage1->setTabText(0,"Channel Registers");

    channelGridLayout = new QGridLayout(this);
    dummy = new QWidget(this);
    CreateChannelsFields();
    SetToolTips();
}


vmm_window::~vmm_window()
{
    delete ui;
}
// ------------------------------------------------------------------------- //
void vmm_window::SetToolTips()
{
     //////////////////////////////////////////////////////////////////////////
    // -------------------------------------------------------------------- //
    // Creates all the ToolTips for Push bottons etc.
    // -------------------------------------------------------------------- //
    //////////////////////////////////////////////////////////////////////////

    ui->pushButton_slvs->setToolTip("Enables direct output IOs.");


    ui->pushButton_bc->setToolTip("slvs 100 Ω termination on ckbc");
    ui->pushButton_tp->setToolTip("slvs 100 Ω termination on cktp");
    ui->pushButton_tk->setToolTip("slvs 100 Ω termination on cktk");
    ui->pushButton_dt->setToolTip("slvs 100 Ω termination on ckdt");
    ui->pushButton_art->setToolTip("slvs 100 Ω termination on ckart");
    ui->pushButton_tki->setToolTip("slvs 100 Ω termination on cktki");
    ui->pushButton_ena->setToolTip("slvs 100 Ω termination on ckena");
    ui->pushButton_6b->setToolTip("slvs 100 Ω termination on ck6b");
    ui->pushButton_stlc->setToolTip("mild tail cancellation (when enabled, overrides sbip)");
    ui->pushButton_nskipm_i->setToolTip("magic number on BCID - 0xFE8");
    ui->pushButton_s32->setToolTip("skips channels 16-47 and makes 15 and 48 neighbors");
    ui->pushButton_sL0ckinv->setToolTip("invert BCCLK");
    ui->pushButton_sL0dckinv->setToolTip("invert DCK");
    ui->pushButton_sbip->setToolTip("bipolar shape");
    ui->pushButton_sfrst->setToolTip("fast reset at 6-b completion");
    ui->pushButton_slvs->setToolTip("enables direct output IOs");
    ui->pushButton_srec->setToolTip("fast recovery from high charge");
//    ui->pushButton_srat->setToolTip("timing ramp at threshold");
    ui->pushButton_ssart->setToolTip("ART flag synchronization (trail to next trail)");
    ui->pushButton_stcr->setToolTip("auto-reset (at the end of the ramp, if no stop occurs)");

    ui->spgLabel->setToolTip("input charge polarity");
    ui->pushButton_4->setToolTip("disable-at-peak");
    ui->pushButton_14->setToolTip("routes analog monitor to PDO output");
    ui->pushButton_11->setToolTip("analog output buffers, enable TDO");
    ui->pushButton_10->setToolTip("analog output buffers, enable PDO");
    ui->pushButton_12->setToolTip("analog output buffers, enable MO");
    ui->pushButton_5->setToolTip("disable leakage current");
    ui->pushButton_6->setToolTip("enables dynamic discharge for AC coupling");
    ui->pushButton_3->setToolTip("neighbor (channel and chip) triggering enable");
    ui->stotLabel->setToolTip("timing outputs control 1 (s6b must be disabled)\n• stpp,stot[00,01,10,11]: TtP,ToT,PtP,PtT\n• TtP: threshold-to-peak\n• ToT: time-over-threshold\n• PtP: pulse-at-peak (10ns) (not available with s10b)\n• PtT: peak-to-threshold (not available with s10b)");
    ui->pushButton_17->setToolTip("enables direct-output logic (both timing and s6b)");
    ui->pushButton_7->setToolTip("enables sub-hysteresis discrimination");
        ui->sc010bLabel->setToolTip("10-bit ADC conversion time");
        ui->sc08bLabel->setToolTip("8-bit ADC conversion time");
        ui->sc06bLabel->setToolTip("6-bit ADC conversion time");
        ui->pushButton_15->setToolTip("8-bit ADC conversion mode");
        ui->pushButton_16->setToolTip("enables 6-bit ADC (requires sttt enabled)");
        ui->pushButton->setToolTip("enables high resolution ADCs (10/8-bit ADC enable)");
        ui->pushButton_18->setToolTip("dual clock edge serialized data enable");
        ui->pushButton_9->setToolTip("dual clock edge serialized ART enable");
        ui->pushButton_19->setToolTip("dual clock edge serialized 6-bit enable");
        ui->pushButton_2->setToolTip("tristates analog outputs with token, used in analog mode");




}

unsigned short vmm_window::VMM_Get(std::string feature, int ch){
    unsigned short setting = root_hybrid->root_hdmi->root_fec->root_daq->root_main->daq[0].fec[fec_index].hdmi[hdmi_index].hybrid[hybrid_index].vmm[vmm_index].GetRegister(feature, ch);
    return setting;
}

bool vmm_window::VMM_Set(std::string feature, int value, int ch){
    if(root_hybrid->root_hdmi->root_fec->root_daq->root_main->daq[0].fec[fec_index].hdmi[hdmi_index].hybrid[hybrid_index].vmm[vmm_index].SetRegi(feature, value, ch)){
     return true;
    }
    else return false;
}


// ------------------------------------------------------------------------- //
void vmm_window::CreateChannelsFields()
{
//    VMM_Set("sd", 1, 2);
    Font.setPointSize(8);
//    int margin = 8;
//    channelGridLayout->setContentsMargins(margin*0.75, margin, margin*2*10, margin);
    channelGridLayout->setHorizontalSpacing(1);
    channelGridLayout->setVerticalSpacing(1);

//    ui->gridLayout_channels->setContentsMargins(margin*0.75, margin, margin*4.4*5, margin);
    ui->gridLayout_channels->setHorizontalSpacing(1);
    ui->gridLayout_channels->setVerticalSpacing(1);
    ui->gridLayout_channels->setGeometry(QRect(620,10,380, 12));
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
        SZ010bLabel->addItem(counter.setNum(i)+" ns");
    }
    for(int i = 0; i < 16; i++) {
        SZ08bLabel->addItem(counter.setNum(i)+" ns");
    }
    for(int i = 0; i < 8; i++) {
        SZ06bLabel->addItem(counter.setNum(i)+" ns");
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
    SDLabel->setFixedSize    (60,19);
    SZ010bLabel->setFixedSize(50,19);
    SZ08bLabel->setFixedSize (50,19);
    SZ06bLabel->setFixedSize (50,19);
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

    int col = 5;
        QLabel *spacer = new QLabel("");
    ui->gridLayout_channels->addWidget(spacer);
    ui->gridLayout_channels->addWidget(SCLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SLLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(STLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(STHLabel,    0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SMLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SDLabel,     0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SMXLabel,    0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SZ010bLabel, 0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SZ08bLabel,  0, col, Qt::AlignCenter); col++;
    ui->gridLayout_channels->addWidget(SZ06bLabel,  0, col, Qt::AlignCenter);


    for (int i = 0; i<64; i++){
        VMMChannel[i] = new QLineEdit(counter.setNum(i+1),ui->stackedWidgetPage1);
        VMMChannel[i]->setAlignment(Qt::AlignHCenter);

        VMMSC[i] = new QPushButton(initialValueRadio,ui->stackedWidgetPage1);
        VMMSL[i] = new QPushButton(initialValueRadio,ui->stackedWidgetPage1);
        VMMST[i] = new QPushButton(initialValueRadio,ui->stackedWidgetPage1);
        VMMSTH[i]= new QPushButton(initialValueRadio,ui->stackedWidgetPage1);
        VMMSM[i] = new QPushButton(initialValueRadio,ui->stackedWidgetPage1);
        VMMSMX[i]= new QPushButton(initialValueRadio,ui->stackedWidgetPage1);


        VMMSC[i]->setFixedSize (15,15);
        VMMST[i]->setFixedSize (15,15);
        VMMSTH[i]->setFixedSize (15,15);
        VMMSL[i]->setFixedSize (15,15);
        VMMSM[i]->setFixedSize (15,15);
        VMMSMX[i]->setFixedSize(15,15);

        VMMSCBool[i]=VMM_Get("sc",i);
        VMMSMBool[i]=VMM_Get("sm",i);
        VMMSTBool[i]=VMM_Get("st",i);
        VMMSTHBool[i]=VMM_Get("sth",i);
        VMMSLBool[i]=VMM_Get("sl",i);
        VMMSMXBool[i]=VMM_Get("smx",i);

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
        VMMSDVoltage[i] = new QComboBox(ui->stackedWidgetPage1);
        VMMSDVoltage[i]->setFixedSize(60,20);
        VMMSDVoltage[i]->setFont(Font);

        for(int j=0;j<32;j++){
            VMMSDVoltage[i]->addItem(counter.setNum(j)+" mV");
        }

        VMMSZ010bCBox[i] = new QComboBox(ui->stackedWidgetPage1);
        VMMSZ010bCBox[i]->setFixedSize(50,20);
        VMMSZ010bCBox[i]->setFont(Font);

        VMMSZ08bCBox[i] = new QComboBox(ui->stackedWidgetPage1);
        VMMSZ08bCBox[i]->setFixedSize(50,20);
        VMMSZ08bCBox[i]->setFont(Font);

        VMMSZ06bCBox[i] = new QComboBox(ui->stackedWidgetPage1);
        VMMSZ06bCBox[i]->setFixedSize(50,20);
        VMMSZ06bCBox[i]->setFont(Font);


        for(int j=0;j<32;j++){
            VMMSZ010bCBox[i]->addItem(counter.setNum(j)+" ns");
        }
        for(int j=0;j<16;j++){
            VMMSZ08bCBox[i]->addItem(counter.setNum(j)+" ns");
        }
        for(int j=0;j<8;j++){
            VMMSZ06bCBox[i]->addItem(counter.setNum(j)+" ns");
        }

       // set initial ADC values
        unsigned short ADC10_index = VMM_Get("ADC0_10",i);
        unsigned short ADC08_index = VMM_Get("ADC0_8",i);
        unsigned short ADC06_index = VMM_Get("ADC0_6",i);
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
        unsigned short SD_volt = VMM_Get("sd",i);
        VMMSDVoltage[i]->setCurrentIndex(SD_volt);
        VMMSDValue[i]=SD_volt;

    } // i

    channelGridLayout->setGeometry(QRect(620,12,50,50));
    channelGridLayout->setSpacing(1);
    channelGridLayout->setGeometry(QRect(620,12,380,2000));
    dummy->setLayout(channelGridLayout);
    //ui->scrollArea->setLayout(channelGridLayout);
    ui->scrollArea->setWidget(dummy);
    ui->scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

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
                                        this, SLOT(updateChannelVoltages(int)));


    // -------------------------------------------------------------------- //
    // update channel ADC values
    // -------------------------------------------------------------------- //
    connect(SZ010bLabel,  SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelADCs(int)));
    connect(SZ08bLabel,   SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelADCs(int)));
    connect(SZ06bLabel,   SIGNAL(currentIndexChanged(int)),
                                        this, SLOT(updateChannelADCs(int)));


    // -------------------------------------------------------------------- //
    // update channel states (green boxes in Channel Fields)
    // -------------------------------------------------------------------- //
    connect(SCLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SLLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(STLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(STHLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SMLabel,   SIGNAL(pressed()), this, SLOT(updateChannelState()));
    connect(SMXLabel,  SIGNAL(pressed()), this, SLOT(updateChannelState()));

    ///////////////////////////////////////////////////////////////////
    // do updates for individual channels
    ///////////////////////////////////////////////////////////////////
    for(int i = 0; i < 64; i++) {

        // ---------- channel voltages ----------- //
        connect(VMMSDVoltage[i],SIGNAL(currentIndexChanged(int)),
                                        this,SLOT(updateChannelVoltages(int)));

        // ---------------- ADCs ---------------- //
        connect(VMMSZ010bCBox[i],SIGNAL(currentIndexChanged(int)),
                                        this,SLOT(updateChannelADCs(int)));
        connect(VMMSZ08bCBox[i],SIGNAL(currentIndexChanged(int)),
                                            this,SLOT(updateChannelADCs(int)));
        connect(VMMSZ06bCBox[i],SIGNAL(currentIndexChanged(int)),
                                            this,SLOT(updateChannelADCs(int)));
        // ----------- channel states ----------- //
        connect(VMMSC[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMSM[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMST[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMSTH[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMSL[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
        connect(VMMSMX[i],SIGNAL(pressed()),this,SLOT(updateChannelState()));
    }

}
// ------------------------------------------------------------------------- //
void vmm_window::updateChannelState()
{
    // ***********************  SC  ********************************* //
    if(SCLabel == QObject::sender()){
        if(VMMSCBoolAll==0){
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSC[j]->setStyleSheet("background-color: green");
                VMMSCBool[j]=true;
//                VMM_Set("sc", 1, j);
            }
            VMMSCBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSC[j]->setStyleSheet("background-color: lightGray");
                VMMSCBool[j]=0;
//                VMM_Set("sc", 0, j);
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
//                VMM_Set("sl", 1, j);
            }
            VMMSLBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSL[j]->setStyleSheet("background-color: lightGray");
                VMMSLBool[j]=0;
//                VMM_Set("sl", 0, j);
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
//                VMM_Set("st", 1, j);
            }
            VMMSTBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMST[j]->setStyleSheet("background-color: lightGray");
                VMMSTBool[j]=0;
//                VMM_Set("st", 0, j);
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
//                VMM_Set("sth", 1, j);
            }
            VMMSTHBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSTH[j]->setStyleSheet("background-color: lightGray");
                VMMSTHBool[j]=0;
//                VMM_Set("sth", 0, j);
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
//                VMM_Set("sm", 1, j);
            }
            VMMSMBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSM[j]->setStyleSheet("background-color: lightGray");
                VMMSMBool[j]=0;
//                VMM_Set("sm", 0, j);
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
//                VMM_Set("smx", 1, j);
            }
            VMMSMXBoolAll=1;
        }else{
            for(int j=0;j<64;j++){
            //for(int j=0;j<32;j++){
                VMMSMX[j]->setStyleSheet("background-color: lightGray");
                VMMSMXBool[j]=0;
//                VMM_Set("smx", 0, j);
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
//                VMM_Set("sc", 1, i);
            }else if(VMMSCBool[i]==1){
                VMMSC[i]->setStyleSheet("background-color: lightGray");
                VMMSCBool[i]=false;
//                VMM_Set("sc", 0, i);
            }
        }else if(VMMST[i] == QObject::sender()){
            if(VMMSTBool[i]==0){
                VMMST[i]->setStyleSheet("background-color: green");
                VMMSTBool[i]=true;
//                VMM_Set("st", 1, i);
            }else if(VMMSTBool[i]==1){
                VMMST[i]->setStyleSheet("background-color: lightGray");
                VMMSTBool[i]=false;
//                VMM_Set("st", 0, i);
            }
        }else if(VMMSTH[i] == QObject::sender()){
            if(VMMSTHBool[i]==0){
                VMMSTH[i]->setStyleSheet("background-color: green");
                VMMSTHBool[i]=true;
//                VMM_Set("sth", 1, i);
            }else if(VMMSTHBool[i]==1){
                VMMSTH[i]->setStyleSheet("background-color: lightGray");
                VMMSTHBool[i]=false;
//                VMM_Set("sth", 0, i);
            }
        }else if(VMMSL[i] == QObject::sender()){
            if(VMMSLBool[i]==0){
                VMMSL[i]->setStyleSheet("background-color: green");
                VMMSLBool[i]=true;
//                VMM_Set("sl", 1, i);
            }else if(VMMSLBool[i]==1){
                VMMSL[i]->setStyleSheet("background-color: lightGray");
                VMMSLBool[i]=false;
//                VMM_Set("sl", 0, i);
            }
        }else if(VMMSM[i] == QObject::sender()){
            if(VMMSMBool[i]==0){
                VMMSM[i]->setStyleSheet("background-color: green");
                VMMSMBool[i]=true;
//                VMM_Set("sm", 1, i);
            }else if(VMMSMBool[i]==1){
                VMMSM[i]->setStyleSheet("background-color: lightGray");
                VMMSMBool[i]=false;
//                VMM_Set("sm", 0, i);
            }
        }else if(VMMSMX[i] == QObject::sender()){
            if(VMMSMXBool[i]==0){
                VMMSMX[i]->setStyleSheet("background-color: green");
                VMMSMXBool[i]=true;
//                VMM_Set("smx", 1, i);
            }else if(VMMSMXBool[i]==1){
                VMMSMX[i]->setStyleSheet("background-color: lightGray");
                VMMSMXBool[i]=false;
//                VMM_Set("smx", 0, i);
            }
        }

    }
}
// ------------------------------------------------------------------------- //
void vmm_window::updateChannelVoltages(int index){
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
            qDebug() << "Voltage of channel " << i << " changed to " << index << ".";
//            VMM_Set("sd", index, i);
        }
    }
}
// ------------------------------------------------------------------------- //
void vmm_window::updateChannelADCs(int index)
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
//            VMM_Set("ADC0_10", index, i);
        }
        if(VMMSZ08bCBox[i] == QObject::sender()){
            VMMSZ08bValue[i]=index;
            //            VMM_Set("ADC0_8", index, i);
        }
        if(VMMSZ06bCBox[i] == QObject::sender()){
            VMMSZ06bValue[i]=index;
            //            VMM_Set("ADC0_6", index, i);
        }
    }
}
// ------------------------------------------------------------------------- //
