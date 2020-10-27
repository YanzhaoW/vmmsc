#ifndef FEC_WINDOW_H
#define FEC_WINDOW_H

#include <QWidget>
#include <iostream>
#include "ui_fec_window.h"

class DAQWindow;

namespace Ui {
class FECWindow;
}

class FECWindow : public QWidget
{
    Q_OBJECT
    friend class HDMIWindow;
    friend class HybridWindow;

public:
    explicit FECWindow(DAQWindow *top, unsigned short fec = 0, QWidget *parent = 0);
    ~FECWindow();
    friend class HDMIWindow;
    friend class HybridWindow;
    friend class VMMWindow;

private slots:
    void on_Box_hdmi1_clicked();
    void on_Box_hdmi2_clicked();
    void on_Box_hdmi3_clicked();
    void on_Box_hdmi4_clicked();
    void on_Box_hdmi5_clicked();
    void on_Box_hdmi6_clicked();
    void on_Box_hdmi7_clicked();
    void on_Box_hdmi8_clicked();
    void on_clearDebugScreen_clicked();

    void onUpdateSettings();
    void onCheckLinkStatus();
    void onResetFEC();

    void onSetReadoutMode(int mode);
    void onACQHandler();

    void on_readSystemParams_pressed();

    void on_pushButtonFECIP_pressed();

    void on_pushButtonDAQIP_pressed();

private:
    DAQWindow *m_daqWindow;
    unsigned short m_fecIndex;
    Ui::fec_window *m_ui;

    void HDMIBoxLogic(bool checked, unsigned short hdmi);
    void UpdateWindow();
    void LoadSettings();
    void SetToolTips();
    bool SetFec(const char *feature, unsigned long val);
    unsigned long GetFec(const char *feature);

    unsigned int internalClockPeriod;
    std::string m_sendstate = "";

signals:
    void ChangeState_FEC();

};

#endif // FEC_WINDOW_H

#ifndef _DAQ_WINDOW_HPP
#include "daq_window.h"
#endif
