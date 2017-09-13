#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <unistd.h>

#include <QtConcurrentRun>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>


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
    friend class VMM_config_handler;
    friend class hybrid_config_handler;
    friend class DAQ_config_handler;
    friend class FEC_config_handler;
    friend class daq_window;
    friend class fec_window;
    friend class hdmi_window;
    friend class hybrid_window;
    friend class vmm_window;
    DAQ daq[DAQS_PER_GUIWINDOW];
    private:
        bool m_dbg;
         QFuture<int> f1;

        std::vector<bool> daq_act;//binary to store which fecs are activated

        SocketHandler *vmmSocketHandler;
        MessageHandler *vmmMessageHandler;

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();
        Commandline *b;
        VMM_config_handler *vmmconfhandl;
        hybrid_config_handler *hybridconfhandl;
        DAQ_config_handler *daqconfhandl;
        FEC_config_handler *fecconfhandl;
        daq_window *daqwindow;
        bool dbg() { return m_dbg; }


        // methods to grab the VMM tools
        //////////////////////////////////////////////////////
        SocketHandler& socketHandle() { return *vmmSocketHandler; }
        MessageHandler& msg()         { return *vmmMessageHandler; }


    

    


};


#endif // MAINWINDOW_H
