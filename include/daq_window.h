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


class DAQWindow : public QMainWindow
{
    Q_OBJECT
    friend class FECWindow;
    friend class HDMIWindow;
    friend class HybridWindow;
    friend class VMMWindow;
    friend class CalibrationModule;

public:
    explicit DAQWindow(MainWindow *top, QWidget *parent = 0);
    ~DAQWindow();

    void LoadMessageHandler(MessageHandler& m);
    MessageHandler& GetMessageHandler() { return *m_msg; }
    void SetWarning(QString warning, QString bkgcol );
    void SetWarning2(QString warning, QString bkgcol );

    void LoadConfig(QString text);
    bool FileExists(const char *fileName);
    int GetFecIP(int id);
    //void PlotXY(std::vector<double> x, std::vector<double> y);

private slots:
    void on_Box_fec1_clicked();
    void on_Box_fec2_clicked();
    void on_Box_fec3_clicked();
    void on_Box_fec4_clicked();
    void on_Box_fec5_clicked();
    void on_Box_fec6_clicked();
    void on_Box_fec7_clicked();
    void on_Box_fec8_clicked();

    void on_openConnection_2_clicked();
    void on_readLog();

    void on_reset_warnings_clicked();

    void on_Send_clicked();

    void on_checkBoxGlobalDAQ_stateChanged();

    void on_onACQ_clicked();

    void on_offACQ_clicked();

    void on_trgPulser_clicked();

    void on_trgExternal_clicked();

    void on_Debug_pressed();

    void on_pushButtonTakeData_pressed();

    void on_pushButtonStoreCorrections_pressed();

    void on_comboBoxRunMode_currentIndexChanged(int index);

    void on_comboBoxCalibrationType_currentIndexChanged(int index);


    void on_pushButtonSavePDF_pressed();

    void on_pushButtonAbort_pressed();





public slots:
    // select the output directory
    //void on_output_directory_select();
    void on_Button_save_clicked();
    void on_selectDir_clicked();
     void on_Button_load_clicked();

private:
    MainWindow *m_mainWindow;
    Ui::daq_window *ui;
    void fecBoxLogic(bool checked, unsigned short fec);
    MessageHandler *m_msg;
    std::string m_sendstate = "";
    std::map<int,int> m_ip_fec;

signals:
    void ChangeState();
    void SetDebug();
};

#endif // DAQ_WINDOW_H

#ifndef _MAINWINDOW_HPP
#include "mainwindow.h"
#endif
