#include "daq_window.h"

daq_window::daq_window(MainWindow *top, QWidget *parent) :
    QMainWindow(parent),
    root_main{top},
    ui(new Ui::daq_window)
{
    ui->setupUi(this);
}

daq_window::~daq_window()
{
    delete ui;
}

void daq_window::on_Box_fec1_clicked()
{
    if (ui->Box_fec1->isChecked()){fecBoxLogic(true,0);}
    else {fecBoxLogic(false,0);}
}
void daq_window::on_Box_fec2_clicked()
{
    if (ui->Box_fec2->isChecked()){fecBoxLogic(true,1);}
    else {fecBoxLogic(false,1);}
}
void daq_window::on_Box_fec3_clicked()
{
    if (ui->Box_fec3->isChecked()){fecBoxLogic(true,2);}
    else {fecBoxLogic(false,2);}
}
void daq_window::on_Box_fec4_clicked()
{
    if (ui->Box_fec4->isChecked()){fecBoxLogic(true,3);}
    else {fecBoxLogic(false,3);}
}
void daq_window::on_Box_fec5_clicked()
{
    if (ui->Box_fec5->isChecked()){fecBoxLogic(true,4);}
    else {fecBoxLogic(false,4);}
}
void daq_window::on_Box_fec6_clicked()
{
    if (ui->Box_fec6->isChecked()){fecBoxLogic(true,5);}
    else {fecBoxLogic(false,5);}
}
void daq_window::on_Box_fec7_clicked()
{
    if (ui->Box_fec7->isChecked()){fecBoxLogic(true,6);}
    else {fecBoxLogic(false,6);}
}
void daq_window::on_Box_fec8_clicked()
{
    if (ui->Box_fec8->isChecked()){fecBoxLogic(true,7);}
    else {fecBoxLogic(false,7);}
}
void daq_window::fecBoxLogic(bool checked, unsigned short fec){
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<fec && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        ui->tabWidget->insertTab(fec-NotActiveBefore, new fec_window(this,fec), QString(" FEC %0").arg(fec+1));
        ui->tabWidget->setCurrentIndex(fec-NotActiveBefore);
        root_main->daq[0].SetFEC(fec,true);
    }
    else {
        ui->tabWidget->removeTab(fec-NotActiveBefore);
        root_main->daq[0].SetFEC(fec,false);
    }
}

void daq_window::on_Button_load_clicked()
{

    QString text = ui->line_configFile->displayText();
    std::string fname = text.toStdString();
    if (fname == "") {
        std::cout << "No file name specified" << std::endl;
        ui->line_configFile->insert("ERROR: no file name given");
    }
    else {

        fname+=".txt";
        bool found = root_main->daqconfhandl->LoadDAQConf(fname.c_str());
        if (!found){
            std::cout << "File not found" << std::endl;
            ui->line_configFile->insert("ERROR: not found");
        }

        else {
            for (unsigned short j=0; j < FECS_PER_DAQ; j++){

                    if (j==0) {ui->Box_fec1->setChecked(false);on_Box_fec1_clicked();}
                    if (j==1 ){ui->Box_fec2->setChecked(false);on_Box_fec2_clicked();}
                    if (j==2 ){ui->Box_fec3->setChecked(false);on_Box_fec3_clicked();}
                    if (j==3 ){ui->Box_fec4->setChecked(false);on_Box_fec4_clicked();}
                    if (j==4 ){ui->Box_fec5->setChecked(false);on_Box_fec5_clicked();}
                    if (j==5 ){ui->Box_fec6->setChecked(false);on_Box_fec6_clicked();}
                    if (j==6 ){ui->Box_fec7->setChecked(false);on_Box_fec7_clicked();}
                    if (j==7 ){ui->Box_fec8->setChecked(false);on_Box_fec8_clicked();}
            }
            root_main->daqconfhandl->LoadDAQConf(fname.c_str());
            std::cout << "loading file " << fname << std::endl;
            for (unsigned short i=0; i < DAQS_PER_GUIWINDOW; i++){
                if (root_main->daq_act[i]){
                    for (unsigned short j=0; j < FECS_PER_DAQ; j++){
                        if (root_main->daq[i].GetFEC(j)){
                            if (j==0 && !ui->Box_fec1->isChecked()){ui->Box_fec1->setChecked(true);on_Box_fec1_clicked();}
                            if (j==1 && !ui->Box_fec2->isChecked()){ui->Box_fec2->setChecked(true);on_Box_fec2_clicked();}
                            if (j==2 && !ui->Box_fec3->isChecked()){ui->Box_fec3->setChecked(true);on_Box_fec3_clicked();}
                            if (j==3 && !ui->Box_fec4->isChecked()){ui->Box_fec4->setChecked(true);on_Box_fec4_clicked();}
                            if (j==4 && !ui->Box_fec5->isChecked()){ui->Box_fec5->setChecked(true);on_Box_fec5_clicked();}
                            if (j==5 && !ui->Box_fec6->isChecked()){ui->Box_fec6->setChecked(true);on_Box_fec6_clicked();}
                            if (j==6 && !ui->Box_fec7->isChecked()){ui->Box_fec7->setChecked(true);on_Box_fec7_clicked();}
                            if (j==7 && !ui->Box_fec8->isChecked()){ui->Box_fec8->setChecked(true);on_Box_fec8_clicked();}
                            for (unsigned short k=0; k < HDMIS_PER_FEC; k++){
                                if(root_main->daq[i].fec[j].GetHDMI(k)){
                                    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
                                        if (root_main->daq[i].fec[j].hdmi[k].GetHybrid(l)){
                                            for (unsigned short m=0; m < VMMS_PER_HYBRID; m++){
                                                if (root_main->daq[i].fec[j].hdmi[k].hybrid[l].GetVMM(m)){
                                                    std::cout << "vmm " << m << " on hybrid " << l << "(pos " << root_main->daq[i].fec[j].hdmi[k].hybrid[l].GetPosNo()<< ", " <<root_main->daq[i].fec[j].hdmi[k].hybrid[l].GetPosX() << ") on hmdi "<< k << " on fec " << j << " on daq " << i << " is active" << std::endl;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else{
                            if (j==0 && ui->Box_fec1->isChecked()){ui->Box_fec1->setChecked(false);on_Box_fec1_clicked();}
                            if (j==1 && ui->Box_fec2->isChecked()){ui->Box_fec2->setChecked(false);on_Box_fec2_clicked();}
                            if (j==2 && ui->Box_fec3->isChecked()){ui->Box_fec3->setChecked(false);on_Box_fec3_clicked();}
                            if (j==3 && ui->Box_fec4->isChecked()){ui->Box_fec4->setChecked(false);on_Box_fec4_clicked();}
                            if (j==4 && ui->Box_fec5->isChecked()){ui->Box_fec5->setChecked(false);on_Box_fec5_clicked();}
                            if (j==5 && ui->Box_fec6->isChecked()){ui->Box_fec6->setChecked(false);on_Box_fec6_clicked();}
                            if (j==6 && ui->Box_fec7->isChecked()){ui->Box_fec7->setChecked(false);on_Box_fec7_clicked();}
                            if (j==7 && ui->Box_fec8->isChecked()){ui->Box_fec8->setChecked(false);on_Box_fec8_clicked();}
                        }
                    }
                }
             }
        } //else file found
    } //end else not ""

}

void daq_window::on_Button_save_clicked()
{
    QString text = ui->line_configFile->displayText();
    std::string fname = text.toStdString();
    if (fname == "") {
        std::cout << "No file name specified" << std::endl;
        ui->line_configFile->insert("ERROR: no file name given");
    }
    else {
        fname+=".txt";
        root_main->daqconfhandl->WriteDAQConf(fname.c_str());
        std::cout << "loading file " << fname << std::endl;
    }
}
