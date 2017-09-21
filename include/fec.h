#ifndef FEC_H
#define FEC_H


#include "globparameter.h"
#include "hdmi.h"
#include "fec_config_module.h"
#include "socket_handler.h"
//class FEC_config_module;

class FEC: public QObject
{
    Q_OBJECT
public:
    FEC();
    ~FEC();
    friend class Commandline;
    friend class FEC_config_module;
    friend class fec_window;
    friend class vmm_window;
    friend class daq_window;
    friend class DAQ;
    friend class calibration_module;
    HDMI hdmi[HDMIS_PER_FEC];

    void LoadDefault(bool calibration = false);
    void LoadMessageHandler(MessageHandler& m);
    MessageHandler& msg() { return *m_msg; }
    SocketHandler& socketHandle() { return *vmmSocketHandler; }

    bool SetHDMI(unsigned short hdmi, bool OnOff);
    bool GetHDMI(unsigned short hdmi);

    // global registers
    bool SetReg(const char *reg, bool val);
    bool SetReg(int regnum, bool val);
    bool SetReg(const char *reg, unsigned long val);
    bool SetReg(int regnum, unsigned long val);
    bool SetReg(const char *reg, const char *val);
    bool SetReg(int regnum, const char *val);

    const char *GetReg(const char *reg);
    const char *GetReg(int regnum);
    unsigned long GetRegVal(int regnum);
    unsigned long GetRegVal(const char *reg);

    const char *GetRegName(unsigned short regnum);
    unsigned short GetRegNumber(const char *reg);
    unsigned short GetRegSize();
    QString GetIP();
    void SendAll();
    quint16 GetChMap();



private:
    bool Set(unsigned short reg, unsigned long val);
    bool CheckAllowedVal(unsigned short reg, const char *val);
    // helper functions
    unsigned long FindVecEntry(unsigned short regval, const char *val);
    bool ConstCharStar_comp(const char *ccs1, const char *ccs2);

    std::vector<bool> hdmi_act;//binary to store which hdmis are activated

    std::vector<const char*> *RegNames;
    std::vector<unsigned long> *Reg;
    char *cchr;

    unsigned short VMM_Get(int hdmi_index, int hybrid_index, int vmm_index, std::string feature, int ch=-9999);
    bool VMM_Set(int hdmi_index, int hybrid_index, int vmm_index, std::string feature, int value ,int ch=-9999);
    bool VMM_Set(int hdmi_index, int hybrid_index, int vmm_index, std::string feature, std::string value, int ch=-9999);

    FEC_config_module *fec_conf_mod;

    MessageHandler *m_msg;
    SocketHandler *vmmSocketHandler;
};
#endif // FEC_H
