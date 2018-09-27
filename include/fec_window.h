#ifndef FEC_WINDOW_H
#define FEC_WINDOW_H

#include <QWidget>
#include <iostream>
#include "ui_fec_window.h"

class DAQWindow;

namespace Ui {
class fec_window;
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
    void on_ip4_2_textChanged();
    void on_ip3_2_textChanged();
    void on_ip2_2_textChanged();
    void on_ip1_2_textChanged();
    void on_clearDebugScreen_clicked();

    void onUpdateSettings();
    void onCheckLinkStatus();
    void onWriteFECStatus();
    void onResetFEC();

    void onSetTriggerMode(int mode);
    void onACQHandler();

    void on_readSystemParams_pressed();
    void on_pushButtonFECIP_pressed();

private:
    DAQWindow *m_daqWindow;
    unsigned short m_fecIndex;
    Ui::fec_window *m_ui;

    QString m_linkState;

    void DisplayDebugScreen(QString text);
    void HDMIBoxLogic(bool checked, unsigned short hdmi);
    void UpdateWindow();
    void LoadSettings();
    void SetToolTips();
    bool SetFec(const char *feature, unsigned long val);
    unsigned long GetFec(const char *feature);

};

#endif // FEC_WINDOW_H

#ifndef _DAQ_WINDOW_HPP
#include "daq_window.h"
#endif
