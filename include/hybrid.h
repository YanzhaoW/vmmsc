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
    friend class VMM;
    friend class HybridConfigHandler;
    friend class CalibrationModule;
    friend class TestModule;
    VMM m_vmms[VMMS_PER_HYBRID];

    void LoadDefault();
    bool SetVMM(unsigned short vmm, bool OnOff);
    void SetART(bool OnOff);
    bool GetVMM(unsigned short vmm);
    bool GetART();

    //void SetAxis(int);
    //bool SetPosNo(unsigned short);
    //bool GetAxis();
    //unsigned short GetPosNo();

    bool SetReg(std::string feature, std::string val);
    bool SetReg(std::string feature, int val);
    unsigned short GetReg(std::string feature);
private:
    std::vector<bool> m_vmmActs;//binary to store which vmms are activated
    bool m_art; // binary to store if ART is active

    //int m_axis; // hybrid is reading on x (0) or y(1) or z(2)
    //unsigned short m_posNo; // hybrid is number ? on its axis
    bool SetRegister(std::string feature, std::string value);
    std::map<std::string, unsigned short> m_hybrid;
};

#endif // HYBRID_H
