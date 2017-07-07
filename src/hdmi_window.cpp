#include "hdmi_window.h"


hdmi_window::hdmi_window(fec_window *top, unsigned short fec, unsigned short hdmi, QWidget *parent) :
    QWidget(parent),
    root_fec{top},
    fec_index{fec},
    hdmi_index{hdmi},
    ui(new Ui::hdmi_window)
{
    ui->setupUi(this);
    updateWindow();
}

hdmi_window::~hdmi_window()
{
    delete ui;
}

void hdmi_window::on_Box_hybrid1_clicked()
{
    if (ui->Box_hybrid1->isChecked()){hybridBoxLogic(true,0);}
    else {hybridBoxLogic(false,0);}
}

void hdmi_window::hybridBoxLogic(bool checked, unsigned short hybrid){
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<hybrid && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        ui->tabWidget->insertTab(hybrid-NotActiveBefore, new hybrid_window(this,fec_index,hdmi_index,hybrid), QString(" Hybrid %0").arg(hybrid+1));
        ui->tabWidget->setCurrentIndex(hybrid-NotActiveBefore);
    }
    else {
        ui->tabWidget->removeTab(hybrid-NotActiveBefore);
    }
}

void hdmi_window::updateWindow(){
    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
        if (root_fec->root_daq->root_main->daq[0].fec[fec_index].hdmi[hdmi_index].GetHybrid(l)){
            if (l == 0 && !ui->Box_hybrid1->isChecked()){ui->Box_hybrid1->setChecked(true); on_Box_hybrid1_clicked();}
        }
    }
}
