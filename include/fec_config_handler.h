#ifndef FEC_CONFIG_HANDLER_H
#define FEC_CONFIG_HANDLER_H

//#include <unistd.h>

#include <QObject>
#include <fstream>

class DAQWindow;

class FECConfigHandler : public QObject
{
    Q_OBJECT
public:
    explicit FECConfigHandler(DAQWindow *top, QObject *parent = 0);
    ~FECConfigHandler();
    bool LoadAllFECConf(std::string filename);
    bool WriteAllFECConf(std::string filename);
    bool LoadSingleFECConf(const char* filename, unsigned short fec);
    bool WriteSingleFECConf(const char* filename, unsigned short fec);
    bool LoadSingleFECConf(const char* filename);
    bool WriteSingleFECConf(const char* filename);
private:
    DAQWindow *m_daqWindow;
    bool GenericAllFECConf(bool load, std::string filename);
    bool GenericSingleFECConf(bool load, const char* filename, unsigned short fec);
    bool LoadFECConfig(std::string fname);
    bool WriteFECConfig(std::string filename, unsigned short fec);
};

#endif // FEC_CONFIG_HANDLER_H

#ifndef _DAQWINDOW_HPP
#include "daq_window.h"
#endif
