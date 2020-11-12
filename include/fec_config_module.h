#ifndef FEC_CONFIG_MODULE_H
#define FEC_CONFIG_MODULE_H

// vmm
#include "socket_handler.h"
#include "message_handler.h"

// std/stl
#include <iostream>
using namespace std;

// Qt
#include <QObject>
#include <QString>
#include <QDataStream>
#include <QByteArray>
#include <QProcess>



class FEC;

class FECConfigModule : public QObject
{
    Q_OBJECT
public:
    explicit FECConfigModule(FEC *top, QObject *parent = 0);
    FECConfigModule& SetDebugMode(bool dbg) { m_dbg = dbg; return *this; }
    bool IsDbgEnabled() { return m_dbg; }

    friend class FEC;

    void LoadMessageHandler(MessageHandler& m);
    MessageHandler& GetMessageHandler() { return *m_messageHandler; }

    FECConfigModule& LoadSocket(SocketHandler& GetSocketHandler);

    bool SendConfig(int hdmi_index, int hybrid_index, int vmm_index);
    void FillGlobalRegisters(std::vector<QString>& globalRegisters, int hdmi_index, int hybrid_index, int vmm_index);
    void FillGlobalRegisters2(std::vector<QString>& globalRegisters, int hdmi_index, int hybrid_index, int vmm_index);
    void FillChannelRegisters(std::vector<QString>& channelRegisters, int hdmi_index, int hybrid_index, int vmm_index);

    SocketHandler& GetSocketHandler() { return *m_socketHandler; }
    int Connect();

    void ConfigTP( int hdmi_index, int hybrid_index);
    void SetS6clocks(int hdmi_index, int hybrid_index);
    void SetTriggerAcqConstants(int hdmi_index, int vmm_index);
    void CheckLinkStatus(bool& readOK, QString & message);
    void ResetLinks();
    void ResetFEC();
    void ReadSystemRegisters(QMap<QString, QString>& registers);
    void writeFECip(int FECip);
    void writeDAQip(int DAQip);
    void SetReadoutMode();
    void ACQon(bool broadcast=false);
    void ACQoff(bool broadcast=false);
    void SetMask();
    int ReadADC(int hdmi_index, int hybrid_index, int vmm_index, int adc_chan);
    QString ReadI2C(int hdmi_index, int choice);
    QString ReadGeoPos(int hdmi_index);
    QString ReadIDChip(int hdmi_index);
    bool CheckConfigurationOfVMMs(int hdmi_index, int vmm_index);
    QString CommunicateWithHybridI2C(int hdmi_index, int rw, int reg, int value, int bytes);
    void VMMLoadEmit();
    void HybridLoadEmit();
private:
    FEC *m_fec;
    bool m_dbg;
    std::vector<int> m_hdmi_i2c;

    SocketHandler *m_socketHandler;
//    ConfigHandler *m_configHandler;
    MessageHandler* m_messageHandler;
signals:
    void CheckLinks();
    void ReloadVMM();
    void ReloadHybrid();

public slots:
};

#endif // FEC_CONFIG_MODULE_H

#ifndef _FEC_HPP
#include "fec.h"
#endif
