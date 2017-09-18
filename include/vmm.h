#ifndef VMM_H
#define VMM_H

#include <QObject>
#include <iostream>
#include <cstring>
#include <iterator>
#include <vector>
#include <sstream>
#include <iomanip>

#include "globparameter.h"
//using namespace std;

class VMM: public QObject
{
    Q_OBJECT
public:
    VMM();
    ~VMM();
    friend class Commandline;


    void LoadDefault(bool calib = false, std::map<std::string, unsigned short> m_calib = {{"sc", 0}}, int channel =-9999 );
    struct Channel{
        std::map<std::string, unsigned short> m_channel;
    };

    struct VMM_Settings{
        //
        Channel ch_settings[64];
        std::map<std::string, unsigned short> *m_GlobalReg1 = new std::map<std::string, unsigned short>;
        std::vector<std::string> Names_GReg1 = {"sp", "sdp", "sbmx", "sbft", "sbfp", "sbfm", "slg", "scmx", "monitoring", "sfa", "sfam", "peaktime", "sfm", "gain", "sng", "stot", "sttt", "ssh", "stc", "sdt", "sdp_2", "convtime_10", "convtime_8", "convtime_6", "s8b", "s6b", "s10b", "sdcks", "sdcka", "sdck6b", "sdrv", "stpp", "slvs", "s32", "stcr", "ssart", "srec", "stlc", "sbip", "srat", "sfrst", "slvsbc", "slvstp", "slvstk",  "slvsdt", "slvsart", "slvstki", "slvsena", "slvs6b", "reset1", "reset2"} ;
        std::vector<std::string> Names_GReg2 ={"sL0dckinv", "sL0ckinv", "nskipm_i" };//not clear whats difference between single resets!!!!!


    };

    VMM_Settings *Regi;
    unsigned short GetRegister( std::string feature, int ch =-9999 );
    bool SetRegister( std::string feature, std::string val, int ch =-9999 );
    bool SetRegi(std::string feature, std::string val, int ch=-9999);
    bool SetRegi(std::string feature, int val, int ch=-9999);
    bool SetRegi(std::string feature, double val, int ch=-9999);
private:

};

#endif // VMM_H
