#ifndef HYBRID_CONFIG_HANDLER_H
#define HYBRID_CONFIG_HANDLER_H
#include "globparameter.h"
//#include <unistd.h>
#include "iostream"

#include <QObject>
#include <fstream>
#include <sstream>

class DAQWindow;

class HybridConfigHandler : public QObject
{
    Q_OBJECT
public:
    explicit HybridConfigHandler(DAQWindow *top, QObject *parent = 0);
    ~HybridConfigHandler();
    friend class Commandline;
    bool LoadAllHybridConf(std::string filename);
    bool WriteAllHybridConf(std::string filename);
    bool LoadSingleHybridConf(const char* filename, unsigned short fec, unsigned short hybrid);
    bool WriteSingleHybridConf(const char* filename, unsigned short fec, unsigned short hybrid);
    bool LoadSingleHybridConf(const char* filename);
    bool WriteSingleHybridConf(const char* filename);

private:
    DAQWindow *m_daqWindow;
    bool GenericAllHybridConf(bool load, std::string filename);
    bool GenericSingleHybridConf(bool load, const char* filename, unsigned short fec, unsigned short hybrid);
    bool LoadHybridConfig(std::string fname);
    bool WriteHybridConfig(std::string filename, unsigned short fec, unsigned short hybrid);

signals:

public slots:
};

#endif // HYBRID_CONFIG_HANDLER_H

#ifndef _DAQWINDOW_HPP
#include "daq_window.h"
#endif
