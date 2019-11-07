#ifndef HYBRID_WINDOW_H
#define HYBRID_WINDOW_H

#include <QWidget>
#include <iostream>
#include "ui_hybrid_window.h"
#include "vmm_window.h"

class HDMIWindow;

namespace Ui {
class hybrid_window;
}

class HybridWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HybridWindow(HDMIWindow *top, unsigned short fec = 0, unsigned short hdmi = 0, unsigned short hybrid = 0, QWidget *parent = 0);
    ~HybridWindow();
    friend class VMMWindow;

private slots:
    void on_Box_vmm1_clicked();
    void on_Box_vmm2_clicked();
    void onUpdateSettings();
    void onGlobalCKBCHandler();
    void on_pbReadI2C_pressed();
    void on_pushButton_setAllHybrids_pressed();

private:
    HDMIWindow *m_hdmiWindow;
    unsigned short m_fecIndex;
    unsigned short m_hdmiIndex;
    unsigned short m_hybridIndex;
    Ui::hybrid_window *m_ui;
    void VMMBoxLogic(bool checked, unsigned short vmm);
    void UpdateWindow();
    void LoadSettings();
    bool SetHybrid(std::string feature, unsigned short val);
    unsigned short GetHybrid(std::string feature);
};

#endif // HYBRID_WINDOW_H

#ifndef _HDMI_WINDOW_HPP
#include "hdmi_window.h"
#endif
