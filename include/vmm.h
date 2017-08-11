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

    bool CheckAllowedGlobVal(unsigned short reg, const char *val);
    bool CheckAllowedChanVal(unsigned short reg, const char *val);

    // global registers
    bool SetReg(const char *reg, bool val);
    bool SetReg(int regnum, bool val);
    bool SetReg(const char *reg, int val);
    bool SetReg(int regnum, int val);
    bool SetReg(const char *reg, double val);
    bool SetReg(int regnum, double val);
    bool SetReg(const char *reg, const char *val);
    bool SetReg(int regnum, const char *val);
    // channel registers
    bool SetReg(const char *reg, unsigned short chan, bool val);
    bool SetReg(int regnum, unsigned short chan, bool val);
    bool SetReg(const char *reg, unsigned short chan, int val);
    bool SetReg(int regnum, unsigned short chan, int val);
    bool SetReg(const char *reg, unsigned short chan, const char *val);
    bool SetReg(int regnum, unsigned short chan, const char *val);

    const char *GetReg(const char *reg);
    const char *GetReg(int regnum);
    unsigned short GetRegVal(int regnum);
    unsigned short GetRegVal(const char *reg);
    const char *GetReg(const char *reg, unsigned short chan);
    const char *GetReg(int regnum, unsigned short chan);
    unsigned short GetRegVal(int regnum, unsigned short chan);
    unsigned short GetRegVal(const char *reg, unsigned short chan);


    const char *GetRegGlobName(unsigned short regnum);
    const char *GetRegChanName(unsigned short regnum);
    unsigned short GetRegNumber(const char *reg);
    unsigned short GetRegGlobSize();
    unsigned short GetRegChanSize();
    struct Channel{
        unsigned short VMMSCBool=0;
        unsigned short VMMSLBool=0;
        unsigned short VMMSTBool=0;
        unsigned short VMMSTHBool=0;
        unsigned short VMMSMBool=0;
        unsigned short VMMSMXBool=0;
        unsigned short VMMSDValue=0;
        unsigned short VMMSZ010bValue=0;
        unsigned short VMMSZ08bValue=0;
        unsigned short VMMSZ06bValue=0;
    };

    struct VMM_Settings{
        //
        Channel ch_settings[64];
        std::map<std::string, unsigned short> *m_GlobalReg1 = new std::map<std::string, unsigned short>;
        std::vector<std::string> Names_GReg1 = {"sp", "sdp", "sbmx", "sbft", "sbfp", "sbfm", "slg", "scmx", "monitoring", "sfa", "sfam", "peaktime", "sfm", "gain", "sng", "stot", "sttt", "ssh", "stc", "sdt", "sdp", "convtime_10", "convtime_8", "convtime_6", "s8b", "s6b", "s10b", "sdcks", "sdcka", "sdck6b", "sdrv", "stpp", "slvs", "s32", "stcr", "ssart", "srec", "stlc", "sbip", "srat", "sfrst", "slvsbc", "slvstp", "slvstk",  "slvsdt", "slvsart", "slvstki", "slvsena", "slvs6b", "reset"} ;
        std::vector<std::string> Names_GReg2 ={"sL0dckinv", "sL0ckinv", "nskipm_i" };//not clear whats difference between single resets!!!!!


    };

    VMM_Settings *Regi;
    unsigned short GetRegister( std::string feature, int ch =-9999 );
    bool SetRegister( std::string feature, std::string val, int ch =-9999 );
    bool SetRegi(std::string feature, std::string val, int ch=-9999);
    bool SetRegi(std::string feature, bool val, int ch=-9999);
    bool SetRegi(std::string feature, int val, int ch=-9999);
    bool SetRegi(std::string feature, double val, int ch=-9999);
private:
    bool SetGlobReg(unsigned short reg, unsigned short val);
    bool SetChanReg(unsigned short reg, unsigned short chan, unsigned short val);
    // helper functions
    unsigned short FindVecEntry(std::vector<const char*> reg, const char *val);
    bool ConstCharStar_comp(const char *ccs1, const char *ccs2);

    std::vector<const char*> *RegGlobNames;
    std::vector<const char*> *RegChanNames;
    std::vector<unsigned short> *RegGlob;
    std::vector<std::vector<unsigned short> > *RegChan;
    std::vector<std::vector<const char*> > *RegGlobVals;
    std::vector<std::vector<const char*> > *RegChanVals;
    std::vector<const char*> *IsIntOrBool;
    std::vector<std::string> st;
    void CheckSettings();
};

#endif // VMM_H
