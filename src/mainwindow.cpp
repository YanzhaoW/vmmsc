#include "mainwindow.h"




// std/stl
#include <iostream>
#include <sstream>
using namespace std;


// ------------------------------------------------------------------------- //
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    //thread
    m_daq_act (DAQS_PER_GUIWINDOW),
    m_socketHandler(0),
    m_messageHandler(0)
{
    QString fileName = QCoreApplication::applicationDirPath();
    m_execPath = fileName.replace("/vmmdcs.app/Contents/MacOS", "");

    m_daq_act[0] = 1;
    std::cout << "stating commandline" << std::endl;
    m_commandLine = new Commandline(this,1);
    m_future1 = QtConcurrent::run(m_commandLine, &Commandline::StartCommandline);
    std::cout << "commandline started" << std::endl;

    m_vmmConfigHandler = new VMMConfigHandler(this);
    m_hybridConfigHandler = new HybridConfigHandler(this);
    m_daqConfigHandler = new DAQConfigHandler(this);
    m_fecConfigHandler = new FECConfigHandler(this);
    m_daqWindow = new DAQWindow(this);
    m_daqWindow->setWindowTitle("VMM3 - SRS DCS new");
    m_daqWindow->show();
    //thread

    m_messageHandler = new MessageHandler();
    m_messageHandler->SetMessageSize(75);
    m_messageHandler->SetGUI(true);

    /////////////////////////////////////////////////////////////////////
    //-----------------------------------------------------------------//
    // VMM handles
    //-----------------------------------------------------------------//
    /////////////////////////////////////////////////////////////////////
    m_socketHandler = new SocketHandler();

    m_socketHandler->LoadMessageHandler(GetMessageHandler());


    m_dbg = false;
    m_socketHandler ->SetDebugMode(false);
    m_calib = new CalibrationModule(this);


}

// ------------------------------------------------------------------------- //
MainWindow::~MainWindow()
{

}


QString MainWindow::GetApplicationPath()
{
    return m_execPath;
}

void MainWindow::ResetAll(){// for test purpose, not functioning correctly
    //    delete daq_act;
    m_daq_act[DAQS_PER_GUIWINDOW];
}
