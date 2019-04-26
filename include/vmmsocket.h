
// Qt
#include <QUdpSocket>

// vmm
#include "message_handler.h"

/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------- //
//  VMMSocket
// ----------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////

class VMMSocket : public QObject
{
    Q_OBJECT;

    public :
        explicit VMMSocket(QObject *parent = 0);
        virtual ~VMMSocket(){};

        VMMSocket& SetDebugMode(bool dbg) { m_dbg = dbg; return *this; }
        bool IsDbgActive() { return m_dbg; }

        void LoadMessageHandler(MessageHandler& GetMessageHandler);
        MessageHandler& GetMessageHandler() { return *m_msg; }

        void SetName(std::string n = "") { m_name = n; }
        std::string GetName() { return m_name; }
        void SetBindingPort(quint16 port) { m_bindingPort = port; }
        quint16 GetBindingPort() { return m_bindingPort; }

        // QUdpMethods
        virtual bool hasPendingDatagrams();
        virtual quint64 pendingDatagramSize();
        virtual quint64 readDatagram(char* databuffer, quint64 maxSize,
                QHostAddress* address = 0, quint16* port = 0);

        bool BindSocket(quint16 port = 0,
            QAbstractSocket::BindMode mode = QAbstractSocket::DefaultForPlatform);
        bool IsBound();
        quint64 WriteDatagram(const QByteArray& data, const QHostAddress& host,
                    quint16 port);

        // dummy function to send string datagram to specific port on local host
        void TestUDP();

        bool CheckAndReconnect(std::string fromWhere="");
        void CloseAndDisconnect(std::string fromWhere="");


        QUdpSocket& GetSocket() { return *m_socket; }
        QByteArray GetBuffer() { return m_buffer; }
        QByteArray ProcessReply(const QString &ip_sent_to, quint32 cmd_delay = 0,
                                quint32 count = 0);

        void Print();

    signals :
        void dataReady();

    public slots :
        void readyRead();

    private :
        bool m_dbg;
        MessageHandler *m_msg;
        std::string m_name;
        quint16 m_bindingPort;
        QUdpSocket *m_socket;
        QByteArray m_buffer;
        char* ip_address;

}; // class VMMSocket
