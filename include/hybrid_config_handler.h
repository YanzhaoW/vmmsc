#ifndef HYBRID_CONFIG_HANDLER_H
#define HYBRID_CONFIG_HANDLER_H
#include "globparameter.h"
#include <unistd.h>
#include "iostream"

#include <QObject>
#include <fstream>
#include <sstream>

class MainWindow;

class hybrid_config_handler : public QObject
{
    Q_OBJECT
public:
    explicit hybrid_config_handler(MainWindow *top, QObject *parent = 0);
    ~hybrid_config_handler();
    friend class Commandline;
    bool LoadAllHybridConf(std::string filename);
    bool WriteAllHybridConf(std::string filename);
    bool LoadSingleHybridConf(const char* filename, unsigned short daq, unsigned short fec, unsigned short hdmi, unsigned short hybrid);
    bool WriteSingleHybridConf(const char* filename, unsigned short daq, unsigned short fec, unsigned short hdmi, unsigned short hybrid);
    bool LoadSingleHybridConf(const char* filename);
    bool WriteSingleHybridConf(const char* filename);

private:
    char ExecPath[256];
    MainWindow *root1;
    bool GenericAllHybridConf(bool load, std::string filename);
    bool GenericSingleHybridConf(bool load, const char* filename, unsigned short daq, unsigned short fec, unsigned short hdmi, unsigned short hybrid);
    bool LoadHybridConfig(std::string fname);
    bool WriteHybridConfig(std::string filename, unsigned short daq, unsigned short fec, unsigned short hdmi, unsigned short hybrid);

signals:

public slots:
};

#endif // HYBRID_CONFIG_HANDLER_H

#ifndef _MAINWINDOW_HPP
#include "mainwindow.h"
#endif
