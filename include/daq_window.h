#ifndef DAQ_WINDOW_H
#define DAQ_WINDOW_H

#include <QMainWindow>
#include "fec_window.h"
#include "ui_daq_window.h"

#include "calibration_module.h"
#include "test_module.h"
#include "currentmonitor.h"


// vmm
#include "socket_handler.h"
#include "message_handler.h"
#include "vmm_config_handler.h"
#include "hybrid_config_handler.h"
#include "daq_config_handler.h"
#include "fec_config_handler.h"
#include "daq.h"

class TestModule;

namespace Ui {
class daq_window;
}


class DAQWindow : public QMainWindow
{
    Q_OBJECT

    friend class VMMConfigHandler;
    friend class HybridConfigHandler;
    friend class DAQConfigHandler;
    friend class FECConfigHandler;
    friend class FECWindow;
    friend class HybridWindow;
    friend class VMMWindow;
    friend class CalibrationModule;
    friend class currentmonitor;
    friend class TestModule;

    DAQ m_daq;
public:
    explicit DAQWindow(QWidget *parent = 0);
    ~DAQWindow();

    SocketHandler& GetSocketHandler() { return *m_socketHandler; }
    void LoadMessageHandler(MessageHandler& m);
    MessageHandler& GetMessageHandler() { return *m_msg; }
    void SetConnectionMessage(QString warning, QString bkgcol );
    void SetWarningMessage(QString warning, QString bkgcol );
    void LoadConfig(QString text);
    bool FileExists(const char *fileName);

    VMMConfigHandler *m_vmmConfigHandler;
    HybridConfigHandler *m_hybridConfigHandler;
    DAQConfigHandler *m_daqConfigHandler;
    FECConfigHandler *m_fecConfigHandler;
    DAQWindow *m_daqWindow;
    QString GetApplicationPath();
    bool IsDbgActive() { return m_dbg; }


    CalibrationModule *m_calib;
    TestModule *m_test;

private slots:
    void on_Box_fec1_clicked();
    void on_Box_fec2_clicked();
    void on_Box_fec3_clicked();
    void on_Box_fec4_clicked();
    void on_Box_fec5_clicked();
    void on_Box_fec6_clicked();
    void on_Box_fec7_clicked();
    void on_Box_fec8_clicked();

    void on_openConnection_clicked();
    void on_readLog();

    void on_reset_warnings_clicked();

    void on_Send_clicked();

    void on_onACQ_clicked();

    void on_offACQ_clicked();

    void on_Debug_pressed();

    void on_pushButtonTakeData_pressed();

    void on_pushButtonStoreCorrections_pressed();

    //void on_comboBoxRunMode_currentIndexChanged(int index);

    //void on_comboBoxCalibrationType_currentIndexChanged(int index);


    void on_pushButtonSavePDF_pressed();

    void on_pushButtonAbort_pressed();

    void on_pushButtonStartTest_pressed();

    void on_pushButtonClearTestLog_pressed();

    void on_pushButtonSavePlotL_clicked();

    void on_comboBox_selectPlotL_currentIndexChanged(const QString &arg1);

    void on_checkBox_readcurrent_stateChanged(int arg1);

    void on_lineEdit_1_9V_textChanged(const QString &arg1);

    void on_lineEdit_2_9V_textChanged(const QString &arg1);

    void on_pushButtonDeleteLast_clicked();

    void on_pushButtonNewHybrid_clicked();

    void on_pushButtonCSV_pressed();

public slots:
    // select the output directory
    //void on_output_directory_select();
    void on_Button_save_clicked();
    void on_selectDir_clicked();
     void on_Button_load_clicked();

private:
    Ui::daq_window *ui;
    void fecBoxLogic(bool checked, unsigned short fec);
    MessageHandler *m_msg;
    std::string m_sendstate = "";
    bool m_dbg;

    SocketHandler *m_socketHandler;
    MessageHandler *m_messageHandler;
    QString m_execPath;

signals:
    void ChangeState();
    void SetDebug();
};

#endif // DAQ_WINDOW_H

