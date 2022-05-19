#ifndef VMM_CONFIG_HANDLER_H
#define VMM_CONFIG_HANDLER_H
#include "globparameter.h"

#include <QObject>
#include <fstream>

class DAQWindow;

class VMMConfigHandler : public QObject
{
    Q_OBJECT
public:
    explicit VMMConfigHandler(DAQWindow *top, QObject *parent = 0);
    ~VMMConfigHandler();

    bool LoadAllVMMConf(std::string filename);
    bool WriteAllVMMConf(std::string filename);
    bool LoadSingleVMMConf(const char* filename, unsigned short fec, unsigned short hybrid, unsigned short vmm);
    bool WriteSingleVMMConf(const char* filename, unsigned short fec, unsigned short hybrid, unsigned short vmm);
    bool LoadSingleVMMConf(const char* filename);
    bool WriteSingleVMMConf(const char* filename);

private:
    //char m_execPath[256];
    DAQWindow *m_daqWindow;
    bool GenericAllVMMConf(bool load, std::string filename);
    bool GenericSingleVMMConf(bool load, const char* filename, unsigned short fec, unsigned short hybrid,  unsigned short vmm);
    bool LoadVMMConfig(std::string fname);
    bool WriteVMMConfig(std::string filename, unsigned short fec, unsigned short hybrid, unsigned short vmm);

signals:

public slots:
};

#endif // VMM_CONFIG_HANDLER_H

#ifndef _DAQWINDOW_HPP
#include "daq_window.h"
#endif
