#ifndef DAQ_WINDOW_H
#define DAQ_WINDOW_H

#include <QMainWindow>
#include "fec_window.h"
#include "ui_daq_window.h"
#include "message_handler.h"

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

    void LoadMessageHandler(MessageHandler& m);
    MessageHandler& msg() { return *m_msg; }
    void SetWarning(QString warning, QString bkgcol );
    void SetWarning2(QString warning, QString bkgcol );
    std::string sendstate = "";
    void LoadConfig(QString text);

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

    void on_openConnection_2_clicked();
    void readLog();

    void on_reset_warnings_clicked();

    void on_Send_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_onACQ_clicked();

    void on_offACQ_clicked();

    void on_trgPulser_clicked();

    void on_trgExternal_clicked();

private:
    MainWindow *root_main;
    Ui::daq_window *ui;
    void fecBoxLogic(bool checked, unsigned short fec);
    MessageHandler *m_msg;

signals:
    void ChangeState();
};

#endif // DAQ_WINDOW_H

#ifndef _MAINWINDOW_HPP
#include "mainwindow.h"
#endif
