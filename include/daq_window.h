#ifndef DAQ_WINDOW_H
#define DAQ_WINDOW_H

#include <QMainWindow>
#include "fec_window.h"
#include "ui_daq_window.h"

class MainWindow;

namespace Ui {
class daq_window;
}


class daq_window : public QMainWindow
{
    Q_OBJECT
    friend class fec_window;
    friend class hdmi_window;
    friend class hybrid_window;
    friend class vmm_window;

public:
    explicit daq_window(MainWindow *top, QWidget *parent = 0);
    ~daq_window();

private slots:
    void on_Box_fec1_clicked();
    void on_Box_fec2_clicked();
    void on_Box_fec3_clicked();
    void on_Box_fec4_clicked();
    void on_Box_fec5_clicked();
    void on_Box_fec6_clicked();
    void on_Box_fec7_clicked();
    void on_Box_fec8_clicked();

    void on_Button_load_clicked();
    void on_Button_save_clicked();

private:
    MainWindow *root_main;
    Ui::daq_window *ui;
    void fecBoxLogic(bool checked, unsigned short fec);
};

#endif // DAQ_WINDOW_H

#ifndef _MAINWINDOW_HPP
#include "mainwindow.h"
#endif
