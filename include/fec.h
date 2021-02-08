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
    friend class FECConfigModule;
    friend class FECWindow;
    friend class VMMWindow;
    friend class DAQWindow;
    friend class DAQ;
    friend class CalibrationModule;
    friend class TestModule;
    HDMI m_hdmis[HDMIS_PER_FEC];

    void LoadDefault();
    void LoadMessageHandler(MessageHandler& m);
    MessageHandler& GetMessageHandler() { return *m_msg; }
    SocketHandler& GetSocketHandler() { return *m_socketHandler; }

    bool SetHDMI(unsigned short m_hdmis, bool OnOff);
    bool GetHDMI(unsigned short m_hdmis);

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

    void SendAll(bool useConfigCheck = false);
    quint16 GetChMap();

    void SetFirmwareVersion(QString version);
    QString GetFirmwareVersion();

    FECConfigModule *m_fecConfigModule;

    long GetID();
    QString GetIP();
    long GetIP_FEC();
    void SetIP_FEC(unsigned long  ip);
    long GetIP_DAQ();
    void SetIP_DAQ(unsigned long  ip);

private:
    bool Set(unsigned short reg, unsigned long val);
    bool CheckAllowedVal(unsigned short reg, const char *val);
    // helper functions
    unsigned long FindVecEntry(unsigned short regval, const char *val);
    bool ConstCharStar_comp(const char *ccs1, const char *ccs2);

    std::vector<bool> m_hdmiActs;//binary to store which hdmis are activated
    MessageHandler *m_msg;
    SocketHandler *m_socketHandler;
    const int numberOfRegisters = 42;
    std::vector<const char*> *m_regNames;
    std::vector<unsigned long> *m_reg;
    char *m_chr;
    int config_error[HDMIS_PER_FEC*VMMS_PER_HYBRID];
    unsigned short GetVMM(int hdmi_index, int vmm_index, std::string feature, int ch=-9999);
    bool SetVMM(int hdmi_index, int vmm_index, std::string feature, int value ,int ch=-9999);
    bool SetVMM(int hdmi_index, int vmm_index, std::string feature, std::string value, int ch=-9999);

    QString m_firmwareVersion;
};
#endif // FEC_H
