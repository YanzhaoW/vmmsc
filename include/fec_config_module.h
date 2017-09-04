#ifndef FEC_CONFIG_MODULE_H
#define FEC_CONFIG_MODULE_H

// vmm
#include "config_handler.h"
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

// boost
#include <boost/format.hpp>


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

    FEC_config_module& LoadConfig(ConfigHandler& config);
    FEC_config_module& LoadSocket(SocketHandler& socket);

    void SendConfig(int hdmi_index, int hybrid_index, int vmm_index);
    void fillGlobalRegisters(std::vector<QString>& globalRegisters, int hdmi_index, int hybrid_index, int vmm_index);
    void fillGlobalRegisters2(std::vector<QString>& globalRegisters, int hdmi_index, int hybrid_index, int vmm_index);
    void fillChannelRegisters(std::vector<QString>& channelRegisters, int hdmi_index, int hybrid_index, int vmm_index);
    SocketHandler& socket() { return *m_socketHandler; }
    ConfigHandler& config() { return *m_configHandler; }
    void testing();
    int Connect();

    void configTP( int hdmi_index, int hybrid_index);
    void setS6Resets(int s6_tk_pulses, bool set_s6_autoReset, bool set_s6_fecReset, int s6_fec_periodRest);
    void s6clocks(int cktk, int ckbc, int ckbc_skew);
    void setTriggerAcqConstants();
    void setEventHeaders(const int bld_info, const int bld_mode, bool highRes);
private:
    FEC *fec;
    bool m_dbg;

    SocketHandler *m_socketHandler;
    ConfigHandler *m_configHandler;
    MessageHandler* m_msg;
signals:

public slots:
};

#endif // FEC_CONFIG_MODULE_H

#ifndef _FEC_HPP
#include "fec.h"
#endif
