#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <unistd.h>

#include <QtConcurrentRun>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include "calibration_module.h"


// vmm
#include "socket_handler.h"
#include "message_handler.h"
#include "commandline.h"
#include "vmm_config_handler.h"
#include "hybrid_config_handler.h"
#include "daq_config_handler.h"
#include "fec_config_handler.h"
#include "daq_window.h"
#include "daq.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
    friend class Commandline;
    friend class VMMConfigHandler;
    friend class HybridConfigHandler;
    friend class DAQConfigHandler;
    friend class FECConfigHandler;
    friend class DAQWindow;
    friend class FECWindow;
    friend class HDMIWindow;
    friend class HybridWindow;
    friend class VMMWindow;
    friend class CalibrationModule;
    DAQ m_daqs[DAQS_PER_GUIWINDOW];


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Commandline *m_commandLine;
    VMMConfigHandler *m_vmmConfigHandler;
    HybridConfigHandler *m_hybridConfigHandler;
    DAQConfigHandler *m_daqConfigHandler;
    FECConfigHandler *m_fecConfigHandler;
    DAQWindow *m_daqWindow;
    bool IsDbgActive() { return m_dbg; }
    void ResetAll();

    // methods to grab the VMM tools
    //////////////////////////////////////////////////////
    SocketHandler& GetSocketHandler() { return *m_socketHandler; }
    MessageHandler& GetMessageHandler()         { return *m_messageHandler; }

    CalibrationModule *m_calib;

    
private:
    bool m_dbg;
    QFuture<int> m_future1;

    std::vector<bool> m_daq_act;//binary to store which fecs are activated

    SocketHandler *m_socketHandler;
    MessageHandler *m_messageHandler;
    


};


#endif // MAINWINDOW_H
