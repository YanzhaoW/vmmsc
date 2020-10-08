#ifndef DAQ_H
#define DAQ_H


#include <QObject>
#include <iostream>
#include <cstring>
#include <iterator>
#include <vector>
#include <sstream>
#include <iomanip>

#include "globparameter.h"
#include "fec.h"
#include "message_handler.h"


class DAQ: public QObject
{
    Q_OBJECT
public:
    DAQ();
    ~DAQ();
    friend class Commandline;
    friend class VMMWindow;
    friend class CalibrationModule;
    FEC m_fecs[FECS_PER_DAQ];

    MessageHandler& GetMessageHandler() { return *m_messageHandler; }

    bool SetFEC(unsigned short FEC, bool OnOff);
    bool GetFEC(unsigned short FEC);

    // global registers
    bool SetReg(const char *reg, bool val);
    bool SetReg(int regnum, bool val);
    bool SetReg(const char *reg, int val);
    bool SetReg(int regnum, int val);
    bool SetReg(const char *reg, const char *val);
    bool SetReg(int regnum, const char *val);

    const char *GetReg(const char *reg);
    const char *GetReg(int regnum);
    unsigned short GetRegVal(int regnum);
    unsigned short GetRegVal(const char *reg);

    const char *GetRegName(unsigned short regnum);
    unsigned short GetRegNumber(const char *reg);
    unsigned short GetRegSize();

    void SendAll();

    bool CheckHybridPos(unsigned short Xaxis,  unsigned short position, int fec_index, int hdmi_index, int hybrid_index);
    bool CheckIP(QString ip, int fec_index);

    void ApplyVMMs(int fec_index, int hdmi_index, int hybrid_index, int vmm_index);
    void ApplyHybrids(int fec_index, int hdmi_index, int hybrid_index);
    void ACQHandler(bool on);

private:
    std::map<int, int> *m_hybridPos = new std::map<int, int>; // map< int FEC*100 + hdmi*10 + hybrid, int position>
    std::map<int, int> *m_hybridAxis = new std::map<int, int>; // map< int FEC*100 + hdmi*10 + hybrid, int Xaxis> 0 for Y axis and 1 for Xaxis
    std::map<int, QStringList> m_fecIPs; // map<int number of fec, QStringList corresponding IP>

    bool SetText(unsigned short reg, const char * text);
    bool Set(unsigned short reg, unsigned short val);
    bool CheckAllowedVal(unsigned short reg, const char *val);
    // helper functions
    unsigned short FindVecEntry(unsigned short regval, const char *val);
    bool ConstCharStar_comp(const char *ccs1, const char *ccs2);
    std::vector<const char*> *m_regNames;
    std::vector<unsigned short> *m_reg;
    std::vector<std::string> *m_regText;
    std::vector<std::vector<const char*> > *m_regVals;

    std::vector<bool> m_fecActs;//binary to store which fecs are activated
    char *m_chr;

    MessageHandler *m_messageHandler;
    void SetMessageHandler();

};

#endif // DAQ_H
