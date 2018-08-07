#include "hybrid_window.h"

hybrid_window::hybrid_window(hdmi_window *top, unsigned short fec, unsigned short hdmi, unsigned short hybrid, QWidget *parent) :
    QWidget(parent),
    root_hdmi{top},
    fec_index{fec},
    hdmi_index{hdmi},
    hybrid_index{hybrid},
    ui(new Ui::hybrid_window)
{
    ui->setupUi(this);
    updateWindow();
    LoadSettings();

    connect(ui->Xaxis, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->position, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->cktk_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->ckbc_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->ckbc_skew_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->ckdt_s6, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->s6_tkPulses, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->fecPeriodReset, SIGNAL(valueChanged(int)),
                                    this, SLOT(updateSettings()));

    connect(ui->tpSkew, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->tpWidth, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));
    connect(ui->tpPolarity, SIGNAL(currentIndexChanged(int)),
                                    this, SLOT(updateSettings()));


}

hybrid_window::~hybrid_window()
{
    delete ui;
}

void hybrid_window::on_Box_vmm1_clicked()
{
    if (ui->Box_vmm1->isChecked()){vmmBoxLogic(true,0);}
    else {vmmBoxLogic(false,0);}
}
void hybrid_window::on_Box_vmm2_clicked()
{
    if (ui->Box_vmm2->isChecked()){vmmBoxLogic(true,1);}
    else {vmmBoxLogic(false,1);}
}

void hybrid_window::vmmBoxLogic(bool checked, unsigned short vmm){
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<vmm && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        ui->tabWidget->insertTab(vmm-NotActiveBefore, new vmm_window(this,fec_index,hdmi_index,hybrid_index,vmm), QString(" VMM %0").arg(vmm+1));
        ui->tabWidget->setCurrentIndex(vmm-NotActiveBefore);
        root_hdmi->root_fec->root_daq->root_main->daq[0].fec[fec_index].hdmi[hdmi_index].hybrid[hybrid_index].SetVMM(vmm, true);
    }
    else {
        ui->tabWidget->removeTab(vmm-NotActiveBefore);
        root_hdmi->root_fec->root_daq->root_main->daq[0].fec[fec_index].hdmi[hdmi_index].hybrid[hybrid_index].SetVMM(vmm, false);
    }
}

void hybrid_window::updateWindow(){
    for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
        if (root_hdmi->root_fec->root_daq->root_main->daq[0].fec[fec_index].hdmi[hdmi_index].hybrid[hybrid_index].GetVMM(m)){
            if (m == 0 && !ui->Box_vmm1->isChecked()){ui->Box_vmm1->setChecked(true); on_Box_vmm1_clicked();}
            if (m == 1 && !ui->Box_vmm2->isChecked()){ui->Box_vmm2->setChecked(true); on_Box_vmm2_clicked();}
        }
    }
}
void hybrid_window::LoadSettings(){

    while(!root_hdmi->root_fec->root_daq->root_main->daq[0].CheckHybridPos(Hybrid_Get("Xaxis"), Hybrid_Get("position"), fec_index, hdmi_index, hybrid_index )){
        Hybrid_Set("position", Hybrid_Get("position")-1);
    }

    ui->Xaxis->setCurrentIndex(Hybrid_Get("Xaxis"));
    ui->position->setValue(Hybrid_Get("position"));
    ui->cktk_s6->setCurrentIndex(Hybrid_Get("CKTK"));
    ui->ckbc_s6->setCurrentIndex(Hybrid_Get("CKBC"));
    ui->ckbc_skew_s6->setCurrentIndex(Hybrid_Get("CKBC_skew"));
    ui->ckdt_s6->setCurrentIndex(Hybrid_Get("CKDT"));
    ui->s6_tkPulses->setValue(Hybrid_Get("TK_Pulses"));
    ui->fecPeriodReset->setValue(Hybrid_Get("period"));
    ui->tpSkew->setCurrentIndex(Hybrid_Get("TP_skew"));
    ui->tpWidth->setCurrentIndex(Hybrid_Get("TP_width"));
    ui->tpPolarity->setCurrentIndex(Hybrid_Get("TP_pol"));
}

bool hybrid_window::Hybrid_Set(std::string feature, unsigned short value){
    if(root_hdmi->root_fec->root_daq->root_main->daq[0].fec[fec_index].hdmi[hdmi_index].hybrid[hybrid_index].SetReg(feature, value)){
     return true;
    }
    else return false;
}
unsigned short hybrid_window::Hybrid_Get(std::string feature){
    return root_hdmi->root_fec->root_daq->root_main->daq[0].fec[fec_index].hdmi[hdmi_index].hybrid[hybrid_index].GetReg(feature);
}

void hybrid_window::updateSettings(){

    if(QObject::sender() == ui->Xaxis){
        if(root_hdmi->root_fec->root_daq->root_main->daq[0].CheckHybridPos( ui->Xaxis->currentIndex() , Hybrid_Get("position"), fec_index, hdmi_index, hybrid_index )){
           Hybrid_Set("Xaxis", ui->Xaxis->currentIndex());
        }
        else{
            ui->Xaxis->setCurrentIndex( Hybrid_Get("Xaxis") );
            root_hdmi->root_fec->root_daq->SetWarning2("Hybrid position occupied- resetted!", "orange");
        }


    }
    else if(QObject::sender() == ui->position){
        if(root_hdmi->root_fec->root_daq->root_main->daq[0].CheckHybridPos(Hybrid_Get("Xaxis"), ui->position->value() , fec_index, hdmi_index, hybrid_index )){
           Hybrid_Set("position", ui->position->value());
        }
        else{
            ui->position->setValue( Hybrid_Get("position") );
            root_hdmi->root_fec->root_daq->SetWarning2("Hybrid position occupied- resetted!", "orange");
        }

    }
    else if(QObject::sender() == ui->cktk_s6){
        Hybrid_Set("CKTK", ui->cktk_s6->currentIndex());
    }
    else if(QObject::sender() == ui->ckbc_s6){
        Hybrid_Set("CKBC", ui->ckbc_s6->currentIndex());
    }
    else if(QObject::sender() == ui->ckbc_skew_s6){
        Hybrid_Set("CKBC_skew", ui->ckbc_skew_s6->currentIndex());
    }
    else if(QObject::sender() == ui->ckdt_s6){
        Hybrid_Set("CKDT", ui->ckdt_s6->currentIndex());
    }
    else if(QObject::sender() == ui->s6_tkPulses){
        Hybrid_Set("TK_Pulses", ui->s6_tkPulses->value());
    }
    else if(QObject::sender() == ui->fecPeriodReset){
        Hybrid_Set("period", ui->fecPeriodReset->value());
    }
    else if(QObject::sender() == ui->tpSkew){
        Hybrid_Set("TP_skew", ui->tpSkew->currentIndex());
    }
    else if(QObject::sender() == ui->tpWidth){
        Hybrid_Set("TP_width", ui->tpWidth->currentIndex());
    }
    else if(QObject::sender() == ui->tpPolarity){
        Hybrid_Set("TP_pol", ui->tpPolarity->currentIndex());
    }




}
