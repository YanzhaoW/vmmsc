#ifndef HYBRID_WINDOW_H
#define HYBRID_WINDOW_H

#include <QWidget>
#include <iostream>
#include "ui_hybrid_window.h"
#include "vmm_window.h"

class FECWindow;

namespace Ui {
class hybrid_window;
}

class HybridWindow : public QWidget
{
    Q_OBJECT
    friend class VMMWindow;
    friend class FECWindow;

public:
    explicit HybridWindow(FECWindow *top, unsigned short fec = 0, unsigned short hybrid = 0, QWidget *parent = 0);
    ~HybridWindow();

public slots:
    void onReloadSettings();
private slots:
    void on_Box_vmm1_clicked();
    void on_Box_vmm2_clicked();
    void onUpdateSettings();
    void on_pbReadI2C_pressed();
private:
    FECWindow *m_fecWindow;
    unsigned short m_fecIndex;
    unsigned short m_hybridIndex;
    Ui::hybrid_window *m_ui;
    void VMMBoxLogic(bool checked, unsigned short vmm);
    void UpdateWindow();
    void LoadSettings();
    bool SetHybrid(std::string feature, unsigned short val);
    unsigned short GetHybrid(std::string feature);
};

#endif // HYBRID_WINDOW_H

#ifndef _FEC_WINDOW_HPP
#include "fec_window.h"
#endif
