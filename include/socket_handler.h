
#ifndef SOCKET_HANDLER_H
#define SOCKET_HANDLER_H

// Qt
#include <QObject>
#include <QUdpSocket>
#include <QStringList>

// vmm
#include "vmmsocket.h"
#include "message_handler.h"

// std/stl
#include <string>


/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------- //
//  SocketHandler
// ----------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////

class SocketHandler : public QObject
{
    Q_OBJECT;

    public :
        friend class DAQWindow;
        explicit SocketHandler(QObject *parent = 0);
        virtual ~SocketHandler(){};
        SocketHandler& SetDebugMode(bool dbg) { m_dbg = dbg; return *this; }
        bool IsDbgActive() { return m_dbg; }

        void LoadMessageHandler(MessageHandler& GetMessageHandler);
        MessageHandler& GetMessageHandler() { return *m_msg; }

        void SetDryRun();
        bool IsDryRunEnabled() { return m_dryrun; }

        bool IsPinged() { return m_pinged; }
        bool IsPinged(bool val) { m_pinged= val; return m_pinged; }

        // update global command counter
        void UpdateCommandCounter();
        quint32 GetCommandCounter() { return n_globalCommandCounter; }
        // reset global command counter
        void ResetCommandCounter();

        // add sockets
        void AddSocket(std::string name = "",
            quint16 bindingPort = 0,
            QAbstractSocket::BindMode mode = QAbstractSocket::DefaultForPlatform);

        // send data
        void SendDatagram(const QByteArray& datagram, const QString& ip,
                            const quint16& destinationPort,
                            const QString& whichSocket = "",
                            const QString& callingFn = "");

        virtual bool WaitForReadyRead(std::string socketName="", int msec=1000);
        QByteArray ProcessReply(std::string name="", const QString& ip_sent_to="",
                        quint32 cmd_delay=0);
        void CloseAndDisconnect(std::string name="", std::string callingFn="");

        // retrieve sockets
        bool IsFECSocketOK();
        VMMSocket& GetFECSocket()    { return *m_fecSocket; }
        bool IsVMMSocketOK();
        VMMSocket& GetVMMSocket() { return *m_vmmappSocket; }
        bool IsDAQSocketOK();
        VMMSocket& GetDAQSocket()    { return *m_daqSocket; }
        QByteArray SetBuffer(std::string name="");

        // Print
        void Print();


    private :
        bool m_dbg;
        MessageHandler *m_msg;
        bool m_pinged;
        bool m_dryrun;
        bool m_skipProcessing;
        quint32 n_globalCommandCounter;
        VMMSocket *m_fecSocket;
        bool m_fecSetup;
        VMMSocket *m_vmmappSocket;
        bool m_vmmappSetup;
        VMMSocket *m_daqSocket;
        bool m_daqSetup;

        QStringList m_idlist;
        QStringList m_iplist;

        // retrieve socket by name
        VMMSocket& GetSocket(std::string whichSocket="");

    signals :
        void on_command_counter_updated();

    public slots :

}; // class SocketHandler


#endif
