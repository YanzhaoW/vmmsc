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

class VMM: public QObject
{
    Q_OBJECT
public:
    VMM();
    ~VMM();

    void LoadDefault();
    struct Channel{
        std::map<std::string, unsigned short> m_channel;
    };

    struct VMMSettings{
        Channel m_channels[64];

        std::map<std::string, unsigned short> *m_globalRegs = new std::map<std::string, unsigned short>;
        std::vector<std::string> m_names_GReg1 = {"sp", "sdp", "sbmx", "sbft", "sbfp",
                                                  "sbfm", "slg", "sm5_sm0", "scmx", "sfa",
                                                  "sfam", "st", "sfm", "sg", "sng",
                                                  "stot", "sttt", "ssh", "stc",
                                                  "sdt", "sdp10", "sc10b", "sc8b", "sc6b",
                                                  "s8b", "s6b", "s10b", "sdcks", "sdcka",
                                                  "sdck6b", "sdrv", "stpp",
                                                  "slvs", "s32", "stcr", "ssart", "srec",
                                                  "stlc", "sbip", "srat", "sfrst", "slvsbc",
                                                  "slvstp", "slvstk",  "slvsdt", "slvsart", "slvstki",
                                                  "slvsena", "slvs6b", "sL0enaV", "slh", "slxh",
                                                  "stgc", "reset1", "reset2"} ;

        std::vector<std::string> m_names_GReg2 ={"nskipm_i", "sL0cktest", "sL0dckinv","sL0ckinv", "sL0ena",
                                                 "truncate_i","nskip_i", "window_i",
                                                 "rollover_i","L0offset_i", "offset_i"};
    };

    VMMSettings *m_vmmSettings;
    double GetRegister( std::string feature, int ch =-9999 );
    bool SetRegister( std::string feature, int val, int ch =-9999 );
    bool SetRegi(std::string feature, int val, int ch=-9999);
    bool SetRegi(std::string feature, bool val, int ch=-9999);
    bool SetInfo(std::string feature, std::string val);
    std::string GetInfo(std::string feature);

private:
    std::map<std::string, std::string> m_vmm_info = {{"all_sc", "0"}, {"all_sl", "0"}, {"all_st", "0"}, {"all_sth", "0"},{"all_sm", "0"}, {"all_sd", "0"},{"all_smx", "0"},
                                                     {"all_sz10b", "0"},{"all_sz08b", "0"}, {"all_sz06b", "0"},{"adc_value", ""}, {"description",""}};
};

#endif // VMM_H
