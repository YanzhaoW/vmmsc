#ifndef DAQ_CONFIG_HANDLER_H
#define DAQ_CONFIG_HANDLER_H
#include "globparameter.h"
//#include <unistd.h>

#include <QObject>
#include <fstream>
#include <iostream>


class DAQWindow;

class DAQConfigHandler : public QObject
{
    Q_OBJECT
public:
    explicit DAQConfigHandler(DAQWindow *top, QObject *parent = 0);
    ~DAQConfigHandler();
    bool LoadDAQConf(const char* filename); // only one DAQ implemented
    bool WriteDAQConf(const char* filename);
private:
    DAQWindow *m_daqWindow;
    bool LoadDAQConfig(std::string fname);
    bool WriteDAQConfig(std::string fname);
};

#endif // DAQ_CONFIG_HANDLER_H

#ifndef _DAQWINDOW_HPP
#include "daq_window.h"
#endif
