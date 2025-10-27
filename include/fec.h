#ifndef FEC_H
#define FEC_H


#include "globparameter.h"
#include "hybrid.h"
#include "fec_config_module.h"
#include "socket_handler.h"


class FEC: public QObject
{
    Q_OBJECT
public:
    FEC();
    ~FEC();
    Hybrid m_hybrids[HYBRIDS_PER_FEC];

    void LoadDefault();
    void LoadMessageHandler(MessageHandler& m);
    MessageHandler& GetMessageHandler() { return *m_msg; }
    SocketHandler& GetSocketHandler() { return *m_socketHandler; }

    bool SetHybrid(unsigned short m_hybrids, bool OnOff);
    bool GetHybrid(unsigned short m_hybrids);

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

    unsigned short GetVMM(int hybrid_index, int vmm_index, std::string feature, int ch=-9999);
    bool SetVMM(int hybrid_index, int vmm_index, std::string feature, int value ,int ch=-9999);
    bool SetVMM(int hybrid_index, int vmm_index, std::string feature, std::string value, int ch=-9999);

    void SendAll(bool useConfigCheck = false);
    quint16 GetChMap();
    FECConfigModule *m_fecConfigModule;

    bool SetId();
    long GetID();
    QString GetIP();
    long GetIP_FEC();
    void SetIP_FEC(unsigned long  ip);
    long GetIP_DAQ();
    void SetIP_DAQ(unsigned long  ip);
    void SetIndex(int n) {m_index = n; if(g_clock_source==0) {m_id=32*n;}}
    int GetIndex() {return m_index;}
    bool SetInfo(std::string feature, std::string val);
    std::string GetInfo(std::string feature);
private:
    bool Set(unsigned short reg, unsigned long val);
    bool CheckAllowedVal(unsigned short reg, const char *val);
    // helper functions
    unsigned long FindVecEntry(unsigned short regval, const char *val);
    bool ConstCharStar_comp(const char *ccs1, const char *ccs2);

    std::vector<bool> m_hybridActs;//binary to store which hybrids are activated
    MessageHandler *m_msg;
    SocketHandler *m_socketHandler;
    const int numberOfRegisters = 42;
    std::vector<const char*> *m_regNames;
    std::vector<unsigned long> *m_reg;
    char *m_chr;
    bool config_error[HYBRIDS_PER_FEC*VMMS_PER_HYBRID];
    std::map<std::string, std::string> m_fec_info;
    int m_index=0;
    //For FEC, last part of the IP address, for assister combination from ring and FEN
    int m_id=0;

};
#endif // FEC_H
