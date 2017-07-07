#ifndef FEC_WINDOW_H
#define FEC_WINDOW_H

#include <QWidget>
#include <iostream>
#include "ui_fec_window.h"

class daq_window;

namespace Ui {
class fec_window;
}

class fec_window : public QWidget
{
    Q_OBJECT
    friend class hdmi_window;
    friend class hybrid_window;

public:
    explicit fec_window(daq_window *top, unsigned short fec = 0, QWidget *parent = 0);
    ~fec_window();

private slots:
    void on_Box_hdmi1_clicked();
    void on_Box_hdmi2_clicked();
    void on_Box_hdmi3_clicked();
    void on_Box_hdmi4_clicked();
    void on_Box_hdmi5_clicked();
    void on_Box_hdmi6_clicked();
    void on_Box_hdmi7_clicked();
    void on_Box_hdmi8_clicked();

private:
    daq_window *root_daq;
    unsigned short fec_index;
    Ui::fec_window *ui;

    void hdmiBoxLogic(bool checked, unsigned short hdmi);
    void updateWindow();

};

#endif // FEC_WINDOW_H

#ifndef _DAQ_WINDOW_HPP
#include "daq_window.h"
#endif
