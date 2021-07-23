#include "hdmi_window.h"


HDMIWindow::HDMIWindow(FECWindow *top, unsigned short fec, unsigned short hdmi, QWidget *parent) :
    QWidget(parent),
    m_fecWindow{top},
    m_fecIndex{fec},
    m_hdmiIndex{hdmi},
    m_ui(new Ui::hdmi_window)
{
    m_ui->setupUi(this);
    UpdateWindow();
}

HDMIWindow::~HDMIWindow()
{
    delete m_ui;
}

void HDMIWindow::on_Box_hybrid1_clicked()
{
    if (m_ui->Box_hybrid1->isChecked()){HybridBoxLogic(true,0);}
    else {HybridBoxLogic(false,0);}
}

void HDMIWindow::HybridBoxLogic(bool checked, unsigned short hybrid){
    unsigned short NotActiveBefore = 0;
    QList<QCheckBox*> a = m_ui->groupBox->findChildren<QCheckBox*>();
    for (unsigned short i = 0; i < a.size(); i++){
        if(i<hybrid && !a.at(i)->isChecked()) NotActiveBefore++;
    }
    if (checked){
        m_ui->tabWidget->insertTab(hybrid-NotActiveBefore, new HybridWindow(this,m_fecIndex,m_hdmiIndex,hybrid), QString(" Hybrid %0").arg(hybrid+1));
        m_ui->tabWidget->setCurrentIndex(hybrid-NotActiveBefore);
        m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hdmis[m_hdmiIndex].SetHybrid(hybrid, true);
    }
    else {
        m_ui->tabWidget->removeTab(hybrid-NotActiveBefore);
        m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hdmis[m_hdmiIndex].SetHybrid(hybrid, false);
    }
}

void HDMIWindow::UpdateWindow(){
    for (unsigned short l=0; l < HYBRIDS_PER_HDMI; l++){
        if (m_fecWindow->m_daqWindow->m_mainWindow->m_daqs[0].m_fecs[m_fecIndex].m_hdmis[m_hdmiIndex].GetHybrid(l)){
            if (l == 0 && !m_ui->Box_hybrid1->isChecked()){m_ui->Box_hybrid1->setChecked(true); on_Box_hybrid1_clicked();}
        }
    }
}

