#ifndef DAQ_CONFIG_H
#define DAQ_CONFIG_H
#include "globparameter.h"


#include <QObject>
#include <QJsonObject>
#include <fstream>
#include <iostream>


class DAQWindow;

class DAQConfig : public QObject
{
    Q_OBJECT
public:
    explicit DAQConfig(DAQWindow *top, QObject *parent = 0);
    ~DAQConfig();
    bool LoadDAQConf(const char* filename); // only one DAQ implemented
    bool WriteDAQConf(const char* filename);
private:
    DAQWindow *m_daqWindow;
    bool LoadDAQConfig(std::string fname);
    bool WriteDAQConfig(std::string fname);
};

#endif // DAQ_CONFIG_H

#ifndef _DAQWINDOW_HPP
#include "daq_window.h"
#endif
