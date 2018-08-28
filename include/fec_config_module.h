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

class FEC_config_module : public QObject
{
    Q_OBJECT
public:
    explicit FEC_config_module(FEC *top, QObject *parent = 0);
    FEC_config_module& setDebug(bool dbg) { m_dbg = dbg; return *this; }
    bool dbg() { return m_dbg; }

    friend class FEC;

    void LoadMessageHandler(MessageHandler& m);
    MessageHandler& msg() { return *m_msg; }

    FEC_config_module& LoadSocket(SocketHandler& socket);

    void SendConfig(int hdmi_index, int hybrid_index, int vmm_index);
    void fillGlobalRegisters(std::vector<QString>& globalRegisters, int hdmi_index, int hybrid_index, int vmm_index);
    void fillGlobalRegisters2(std::vector<QString>& globalRegisters, int hdmi_index, int hybrid_index, int vmm_index);
    void fillChannelRegisters(std::vector<QString>& channelRegisters, int hdmi_index, int hybrid_index, int vmm_index);
    SocketHandler& socket() { return *m_socketHandler; }
    int Connect();

    void configTP( int hdmi_index, int hybrid_index);
    void setS6Resets(int hdmi_index, int hybrid_index);
    void s6clocks(int hdmi_index, int hybrid_index);
    void setTriggerAcqConstants(int hdmi_index, int hybrid_index, int vmm_index);
    void setEventHeaders(int hdmi_index, int hybrid_index, int vmm_index);
    void checkLinkStatus();
    void resetLinks();
    void resetFEC(bool do_reset);
    void readSysReg();
    void writeFECip();
    void setTriggerMode();
    void ACQon(bool broadcast=false);
    void ACQoff(bool broadcast=false);
    void setMask();
    int ReadADC(int hdmi_index, int hybrid_index, int vmm_index, int adc_chan);
    void VMMLoadEmit();

private:
    FEC *fec;
    bool m_dbg;

    SocketHandler *m_socketHandler;
//    ConfigHandler *m_configHandler;
    MessageHandler* m_msg;
signals:
    void checkLinks();
    void reloadVMM();

public slots:
};

#endif // FEC_CONFIG_MODULE_H

#ifndef _FEC_HPP
#include "fec.h"
#endif
