#include "mainwindow.h"




// std/stl
#include <iostream>
#include <sstream>
using namespace std;


// ------------------------------------------------------------------------- //
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    vmmSocketHandler(0),
    vmmMessageHandler(0),
    calib(new calibration_module(this)),
    //thread
    daq_act (DAQS_PER_GUIWINDOW)
{
    daq_act[0] = 1;
    std::cout << "stating commandline" << std::endl;
    b = new Commandline(this,1);
    f1 = QtConcurrent::run(b, &Commandline::StartCommandline);
       std::cout << "commandline started" << std::endl;

       vmmconfhandl = new VMM_config_handler(this);
       hybridconfhandl = new hybrid_config_handler(this);
        daqconfhandl = new DAQ_config_handler(this);
        fecconfhandl = new FEC_config_handler(this);
        daqwindow = new daq_window(this);
        daqwindow->setWindowTitle("VMM3 - SRS DCS new");
        daqwindow->show();
    //thread

    vmmMessageHandler = new MessageHandler();
    vmmMessageHandler->setMessageSize(75);
    vmmMessageHandler->setGUI(true);

    /////////////////////////////////////////////////////////////////////
    //-----------------------------------------------------------------//
    // VMM handles
    //-----------------------------------------------------------------//
    /////////////////////////////////////////////////////////////////////
    vmmSocketHandler = new SocketHandler();

    vmmSocketHandler->LoadMessageHandler(msg());


    m_dbg = false;
    vmmSocketHandler ->setDebug(false);


}

// ------------------------------------------------------------------------- //
MainWindow::~MainWindow()
{

}
void MainWindow::ResetCalib(){
    delete calib;
    calib = new calibration_module(this);
}
