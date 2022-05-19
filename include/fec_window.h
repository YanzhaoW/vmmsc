#ifndef FEC_WINDOW_H
#define FEC_WINDOW_H

#include <QWidget>
#include <iostream>

class DAQWindow;

namespace Ui {
class FECWindow;
}

class FECWindow : public QWidget
{
    Q_OBJECT
    friend class HybridWindow;
    friend class VMMWindow;

public:
    explicit FECWindow(DAQWindow *top, unsigned short fec = 0, QWidget *parent = 0);
    ~FECWindow();


private slots:
    void onUpdateSettings();
    void onCheckLinkStatus();
    void onResetFEC();
    void onACQHandler();

    void on_readSystemParams_pressed();

    void on_pushButtonFECIP_pressed();

    void on_pushButtonDAQIP_pressed();


    void on_pushButtonPowerCycle_pressed();

private:
    DAQWindow *m_daqWindow;
    unsigned short m_fecIndex;
    Ui::fec_window *m_ui;

    void hybridBoxLogic(bool checked, unsigned short hybrid);
    void UpdateWindow();
    void LoadSettings();
    void SetToolTips();
    bool SetFec(const char *feature, unsigned long val);
    unsigned long GetFec(const char *feature);
    void EnableCommunicationButtons(bool enable);

    std::string m_sendstate = "";

signals:
    void ChangeState_FEC();

};

#endif // FEC_WINDOW_H

#ifndef _DAQ_WINDOW_HPP
#include "daq_window.h"
#endif
