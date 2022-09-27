
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

        bool CheckAndReconnect(std::string fromWhere="");
        void CloseAndDisconnect(std::string fromWhere="");


        QUdpSocket& GetSocket() { return *m_socket; }
        QByteArray GetBuffer() { return m_buffer; }
        void ProcessReply();

        void Print();

    private :
        bool m_dbg = false;
        MessageHandler *m_msg;
        quint16 m_bindingPort;
        QUdpSocket *m_socket;
        QByteArray m_buffer;
        char* ip_address;

}; // class VMMSocket
