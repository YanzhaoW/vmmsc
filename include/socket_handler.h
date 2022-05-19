
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
        explicit SocketHandler(QObject *parent = 0);
        virtual ~SocketHandler(){};
        SocketHandler& SetDebugMode(bool dbg) { m_dbg = dbg; return *this; }
        bool IsDbgActive() { return m_dbg; }

        void LoadMessageHandler(MessageHandler& GetMessageHandler);
        MessageHandler& GetMessageHandler() { return *m_msg; }


        bool IsPinged() { return m_pinged; }
        void SetPinged(bool val) { m_pinged= val;}

        // update global command counter
        void UpdateCommandCounter();
        quint32 GetCommandCounter() { return n_globalCommandCounter; }
        // reset global command counter
        void ResetCommandCounter();

        // add sockets
        void AddSocket(quint16 bindingPort = 0,
            QAbstractSocket::BindMode mode = QAbstractSocket::DefaultForPlatform);

        // send data
        bool SendDatagram(const QByteArray& datagram, const QString& ip,
                            const quint16& destinationPort,
                             const QString& callingFn = "");

        virtual bool WaitForReadyRead(int msec=1000);
        void ProcessReply();
        void CloseAndDisconnect(std::string callingFn="");

        // retrieve sockets
        bool IsFECSocketOK();
        QByteArray SetBuffer();

        // Print
        void Print();
        // retrieve socket
        VMMSocket& GetSocket();

    private :
        bool m_dbg = false;
        MessageHandler *m_msg;
        bool m_pinged=false;
        bool m_skipProcessing;
        quint32 n_globalCommandCounter;
        VMMSocket *m_socket;
        bool m_socketSetup;

        QStringList m_idlist;
        QStringList m_iplist;



    signals :
        void on_command_counter_updated();

    public slots :

}; // class SocketHandler


#endif
