#include "fec_window.h"
#include "hdmi_window.h" // has NOT to be included here. If included in header file: compiler error in mainwindow.h:61:9: error: ‘daq_window’ does not name a type


fec_window::fec_window(daq_window *top, unsigned short fec, QWidget *parent) :
    QWidget(parent),
    root_daq{top},
    fec_index{fec},
    ui(new Ui::fec_window)
{
    ui->setupUi(this);
    updateWindow();
}

fec_window::~fec_window()
{
    delete ui;
}

void fec_window::on_Box_hdmi1_clicked()
{
    std::cout << " Box_hdmi1 clicked" << std::endl;
    if (ui->Box_hdmi1->isChecked()){hdmiBoxLogic(true,0);}
    else {hdmiBoxLogic(false,0);}
}
void fec_window::on_Box_hdmi2_clicked()
{
    if (ui->Box_hdmi2->isChecked()){hdmiBoxLogic(true,1);}
    else {hdmiBoxLogic(false,1);}
}
void fec_window::on_Box_hdmi3_clicked()
{
    if (ui->Box_hdmi3->isChecked()){hdmiBoxLogic(true,2);}
    else {hdmiBoxLogic(false,2);}
}
void fec_window::on_Box_hdmi4_clicked()
{
    if (ui->Box_hdmi4->isChecked()){hdmiBoxLogic(true,3);}
    else {hdmiBoxLogic(false,3);}
}
void fec_window::on_Box_hdmi5_clicked()
{
    if (ui->Box_hdmi5->isChecked()){hdmiBoxLogic(true,4);}
    else {hdmiBoxLogic(false,4);}
}
void fec_window::on_Box_hdmi6_clicked()
{
    if (ui->Box_hdmi6->isChecked()){hdmiBoxLogic(true,5);}
    else {hdmiBoxLogic(false,5);}
}
void fec_window::on_Box_hdmi7_clicked()
{
    if (ui->Box_hdmi7->isChecked()){hdmiBoxLogic(true,6);}
    else {hdmiBoxLogic(false,6);}
}
void fec_window::on_Box_hdmi8_clicked()
{
    if (ui->Box_hdmi8->isChecked()){hdmiBoxLogic(true,7);}
    else {hdmiBoxLogic(false,7);}
}
void fec_window::hdmiBoxLogic(bool checked, unsigned short hdmi){
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<hdmi && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        ui->tabWidget->insertTab(hdmi-NotActiveBefore, new hdmi_window(this,fec_index,hdmi), QString(" HDMI %0").arg(hdmi+1));
        ui->tabWidget->setCurrentIndex(hdmi-NotActiveBefore);
    }
    else {
        ui->tabWidget->removeTab(hdmi-NotActiveBefore);
    }
}


void fec_window::updateWindow(){
    for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
        if(root_daq->root_main->daq[0].fec[fec_index].GetHDMI(k)){
            if (k == 0 && !ui->Box_hdmi1->isChecked()){ui->Box_hdmi1->setChecked(true); on_Box_hdmi1_clicked();}
            if (k == 1 && !ui->Box_hdmi2->isChecked()){ui->Box_hdmi2->setChecked(true); on_Box_hdmi2_clicked();}
            if (k == 2 && !ui->Box_hdmi3->isChecked()){ui->Box_hdmi3->setChecked(true); on_Box_hdmi3_clicked();}
            if (k == 3 && !ui->Box_hdmi4->isChecked()){ui->Box_hdmi4->setChecked(true); on_Box_hdmi4_clicked();}
            if (k == 4 && !ui->Box_hdmi5->isChecked()){ui->Box_hdmi5->setChecked(true); on_Box_hdmi5_clicked();}
            if (k == 5 && !ui->Box_hdmi6->isChecked()){ui->Box_hdmi6->setChecked(true); on_Box_hdmi6_clicked();}
            if (k == 6 && !ui->Box_hdmi7->isChecked()){ui->Box_hdmi7->setChecked(true); on_Box_hdmi7_clicked();}
            if (k == 7 && !ui->Box_hdmi8->isChecked()){ui->Box_hdmi8->setChecked(true); on_Box_hdmi8_clicked();}
        }
    }
}
