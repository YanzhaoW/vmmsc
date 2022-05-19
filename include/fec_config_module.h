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

    bool SendConfig(int hybrid_index, int vmm_index, bool enableConfigCheck=false);
    void FillGlobalRegisters(std::vector<QString>& globalRegisters, int hybrid_index, int vmm_index);
    void FillGlobalRegisters2(std::vector<QString>& globalRegisters, int hybrid_index, int vmm_index);
    void FillChannelRegisters(std::vector<QString>& channelRegisters, int hybrid_index, int vmm_index);

    SocketHandler& GetSocketHandler() { return *m_socketHandler; }
    bool Connect();

    void ConfigHybrid( int hybrid_index);
    void SetTriggerAcqConstants();
    void PowerCycleHybrids();
    bool CheckLinkStatus(QString & message, QVector<QString> & linkstatus);
    void ResetLinks();
    void ResetFEC();
    bool ReadSystemRegisters(QMap<QString, QString>& registers);
    void writeFECip(int FECip);
    void writeDAQip(int DAQip);
    void ACQon(bool broadcast=false);
    void ACQoff(bool broadcast=false);
    void SetMask();
    int ReadADC(int hybrid_index, int vmm_index, int adc_chan);
    QString ReadI2C(int hybrid_index, int choice);
    QString ReadGeoPos(int hybrid_index);
    QString ReadIDChip(int hybrid_index);
    QString ReadFirmwareVersion(int hybrid_index);
    QString ReadADC_I2C(int hybrid_index);
    bool CheckConfigurationOfVMMs(int hybrid_index, int vmm_index);
    QString CommunicateWithHybridI2C(int i2c_addr, int hybrid_index, int rw, int reg_value, int bytes);
    bool CheckIpPortSize(QHostAddress ipRead, QString ip,int portRead, int port, int size);
private:
    FEC *m_fec;
    bool m_dbg = false;
    //std::vector<int> m_hybrid_i2c;

    SocketHandler *m_socketHandler;
//    ConfigHandler *m_configHandler;
    MessageHandler* m_messageHandler;
signals:
    void CheckLinks();


public slots:
};

#endif // FEC_CONFIG_MODULE_H

#ifndef _FEC_HPP
#include "fec.h"
#endif
