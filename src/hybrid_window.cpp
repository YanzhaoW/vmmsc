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
    }
    else {
        ui->tabWidget->removeTab(vmm-NotActiveBefore);
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
