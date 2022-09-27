
// vmm
#include "vmmsocket.h"

// Qt
#include <QByteArray>
#include <QMessageBox>
// std/stl
#include <sstream>
#include <iostream>
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------- //
//  VMMSocket
// ----------------------------------------------------------------------- //
/////////////////////////////////////////////////////////////////////////////

VMMSocket::VMMSocket(QObject* parent) :
    QObject(parent),
    m_dbg(false),
    m_msg(0),
    m_socket(0)
{
    m_socket = new QUdpSocket(this);
}
// ----------------------------------------------------------------------- //
void VMMSocket::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ----------------------------------------------------------------------- //
bool VMMSocket::BindSocket(quint16 port, QAbstractSocket::BindMode mode)
{
    bool bind = true;
    if(m_socket) {
        //bind = m_socket->bind(port, QAbstractSocket:: ShareAddress);
        bind = m_socket->bind(port, QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
    }
    else {
        GetMessageHandler()("SC Socket is null!",
                            "VMMSocket::bindSocket",true);
        bind = false;
    }
    if(!bind) {
        GetMessageHandler()("Unable to bind SC socket",
                            "VMMSocket::bindSocket", true);
    }
    else {
        if(IsDbgActive()) {
            stringstream sx;
            sx << "Socket successfully bound to port " << port;
            GetMessageHandler()(sx, "VMMSocket::bindSocket");
        }
    }
    return bind;
}
// ----------------------------------------------------------------------- //
bool VMMSocket::IsBound()
{
    return GetSocket().state() == 4;
}
// ----------------------------------------------------------------------- //
bool VMMSocket::hasPendingDatagrams()
{
    return m_socket->hasPendingDatagrams();
}
// ----------------------------------------------------------------------- //
quint64 VMMSocket::pendingDatagramSize()
{
    return m_socket->pendingDatagramSize();
}
// ----------------------------------------------------------------------- //
quint64 VMMSocket::readDatagram(char* data, quint64 maxSize,
                                QHostAddress* address, quint16* port)
{
    return m_socket->readDatagram(data, maxSize, address, port);
}


// ----------------------------------------------------------------------- //
quint64 VMMSocket::WriteDatagram(const QByteArray& datagram,
                                 const QHostAddress& host, quint16 port)
{
    return m_socket->writeDatagram(datagram, host, port);
}

// ----------------------------------------------------------------------- //
bool VMMSocket::CheckAndReconnect(std::string callingFn)
{
    stringstream sx;
    bool status = true;
    if(m_socket->state() == QAbstractSocket::UnconnectedState) {
        status = false;
        string fn = "";
        if(callingFn!="") fn = "(" + callingFn + ") ";

        if(IsDbgActive()) {
            sx << fn << "About to rebind SC socket to port "
               << GetBindingPort();
            GetMessageHandler()(sx, "VMMSocket::checkAndReconnect");
        }
        bool bnd = BindSocket(GetBindingPort(), QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
        if(!bnd) {
            status = false;
            CloseAndDisconnect(callingFn);
        }
        else {
            status = true;
        }
    }
    return status;
}

// ----------------------------------------------------------------------- //
void VMMSocket::CloseAndDisconnect(std::string callingFn)
{
    stringstream sx;
    string fn = "";
    if(callingFn!="") fn = "(" + callingFn + ") ";
    if(IsDbgActive()) {
        sx << fn << "Closing and disconnecting from host the SC socket"
           << " (bound on port " << GetBindingPort()
           << ")";
        GetMessageHandler()(sx,"VMMSocket::closeAndDisconnect");
    }
    m_socket->close();
    m_socket->disconnectFromHost();
}
// ----------------------------------------------------------------------- //
void VMMSocket::ProcessReply()
{
    stringstream sx;
    bool ok;
    QHostAddress vmmIP;

    QByteArray datagram;
    datagram.clear();

    while(GetSocket().hasPendingDatagrams()) {
        datagram.resize(GetSocket().pendingDatagramSize());
        GetSocket().readDatagram(datagram.data(), datagram.size(), &vmmIP);
    }//while
}
// ----------------------------------------------------------------------- //
void VMMSocket::Print()
{
    stringstream ss;
    ss << "    Name          : SC socket\n"
       << "    Bound to port : " << GetBindingPort() << "\n"
       << "    Status        : " << m_socket->state();
    GetMessageHandler()(ss,"VMMSocket::Print");
}
