#ifndef HYBRID_H
#define HYBRID_H

#include "globparameter.h"
#include "vmm.h"

class Hybrid: public QObject
{
    Q_OBJECT
public:
    Hybrid();
    ~Hybrid();
    friend class Commandline;
    friend class vmm;
    friend class hybrid_config_handler;
    VMM vmm[VMMS_PER_HYBRID];

    bool SetVMM(unsigned short vmm, bool OnOff);
    void SetART(bool OnOff);
    bool GetVMM(unsigned short vmm);
    bool GetART();

    void SetPosX(bool);
    bool SetPosNo(unsigned short);
    bool GetPosX();
    unsigned short GetPosNo();

    bool SetReg(std::string feature, std::string val);
    bool SetReg(std::string feature, int val);
    unsigned short GetReg(std::string feature);
private:
    std::vector<bool> vmm_act;//binary to store which vmms are activated
    bool ART; // binary to store if ART is active

    bool PosX; // hybrid is reading x (true) or y (false)
    unsigned short PosNo; // hybrid is number ? on its axis
    bool SetRegister(std::string feature, std::string value);
    std::map<std::string, unsigned short> m_hybrid = {{"Xaxis",0}, {"position", 65535}, {"CKTK",0}, {"CKBC",0}, {"CKBC_skew",0},{"TK_Pulses",2},{"period",4094}, {"TP_skew", 0}, {"TP_width", 0}, {"TP_pol", 0}};
};

#endif // HYBRID_H

