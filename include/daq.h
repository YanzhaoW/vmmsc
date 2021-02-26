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
    friend class VMMWindow;
    friend class CalibrationModule;
    friend class TestModule;
    FEC m_fecs[FECS_PER_DAQ];

    int CheckIP_DAQ(long ip);
    int CheckIP_FEC(long ip, int fec_index);

    MessageHandler& GetMessageHandler() { return *m_messageHandler; }
    bool SetFEC(unsigned short FEC, bool OnOff);
    bool GetFEC(unsigned short FEC);


    void SendAll(bool useConfigCheck=false);

    bool CheckHybridPos(unsigned short axis,  unsigned short position, int fec_index, int hybrid_index);
    bool ValidNewFECIP(unsigned int ip);

    void ApplyVMMs(int fec_index, int hybrid_index, int vmm_index, bool isReset = false);
    void ApplyChannelSettingsVMMs(int fec_index, int hybrid_index, int vmm_index);
    void ApplyHybrids(int fec_index);
    void ACQHandler(bool on);
   // unsigned int GetFECID_from_IP()
private:
    std::map<int, int> *m_hybridPos = new std::map<int, int>; // map< int FEC*100 + hybrid_index*10, int position>
    std::map<int, int> *m_hybridAxis = new std::map<int, int>; // map< int FEC*100 + hybrid_index*10, int axis> 0 for x axis and 1 for y


    std::vector<bool> m_fecActs;//binary to store which fecs are activated
    char *m_chr;

    MessageHandler *m_messageHandler;
    void SetMessageHandler();

};

#endif // DAQ_H
