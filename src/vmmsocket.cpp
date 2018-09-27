
// vmm
#include "vmmsocket.h"

// Qt
#include <QByteArray>

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
    m_name(""),
    m_bindingPort(0),
    m_socket(0)
{
    m_socket = new QUdpSocket(this);
    connect(m_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));

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
        bind = m_socket->bind(port, mode);
    }
    else {
       GetMessageHandler()("Socket named " + GetName() + " is null!",
                "VMMSocket::bindSocket",true);
        bind = false;
    }
    if(!bind) {
       GetMessageHandler()("Unable to bind socket named " + GetName(),
                "VMMSocket::bindSocket", true);
    }
    else {
        stringstream sx;
        sx << "Socket named " << GetName() << " successfully bound to port " << port;
       GetMessageHandler()(sx, "VMMSocket::bindSocket");
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
void VMMSocket::TestUDP()
{
    CheckAndReconnect("VMMSocket::TestUDP");

    QByteArray data;
    data.append("hello from udp");
    data.append("  from socket named: ");
    data.append(QString::fromStdString(GetName()));
    m_socket->writeDatagram(data, QHostAddress::LocalHost, 1234);

    CloseAndDisconnect();
}
// ----------------------------------------------------------------------- //
void VMMSocket::readyRead()
{
    if(IsDbgActive())GetMessageHandler()(GetName() + " socket receiving data...",
                "VMMSocket::readyRead");

    if     (GetName()=="fec" || GetName()=="FEC")
        emit dataReady();
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
            sx << fn << "About to rebind socket named " << GetName() << " to port "
               << GetBindingPort();
           GetMessageHandler()(sx, "VMMSocket::checkAndReconnect");
        }
        bool bnd = m_socket->bind(GetBindingPort(), QUdpSocket::ShareAddress);
        if(!bnd) {
            status = false;
            sx.str("");
            sx << fn << "ERROR Unable to re-bind socket named " << GetName() << "to port "
               << GetBindingPort();
           GetMessageHandler()(sx, "VMMSocket::checkAndReconnect");
            CloseAndDisconnect(callingFn);
        }
        else {
            status = true;
            if(IsDbgActive()) { 
                sx.str("");
                sx << fn << "Socket named " << GetName() << " successfully rebound to port "
                   << GetBindingPort();
               GetMessageHandler()(sx,"VMMSocket::checkAndReconnect");
            }
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
    if(IsDbgActive())
        sx << fn << "Closing and disconnecting from host the socket"
           << " named " << GetName() << " (bound on port " << GetBindingPort()
           << ")";
       GetMessageHandler()(sx,"VMMSocket::closeAndDisconnect");
    m_socket->close();
    m_socket->disconnectFromHost();
}
// ----------------------------------------------------------------------- //
QByteArray VMMSocket::ProcessReply(const QString &ip_to_check, quint32 cmd_delay,
                                    quint32 globalCount)
{
    stringstream sx;
    //debug
    sx << GetName() << " socket processing replies for IP: " + ip_to_check.toStdString();
    if(IsDbgActive())GetMessageHandler()("Processing datagram replies for IP: " + ip_to_check.toStdString(),
                        "VMMSocket::processReply");
//    m_dbg = true;
    bool ok;
    QString datagram_hex;
    unsigned int cmd_cnt_to_check = globalCount - cmd_delay; 
    QHostAddress vmmIP;

    QStringList replies; 
    replies.clear();
    QByteArray datagram;
    datagram.clear();

    while(GetSocket().hasPendingDatagrams()) {

        //debug
        if(IsDbgActive()) {
            sx.str("");
            sx << "socket " << GetName() << " has datagrams";
           GetMessageHandler()(sx,"VMMSocket::processReply");sx.str("");
        }

        //debug
        datagram.resize(GetSocket().pendingDatagramSize());
        GetSocket().readDatagram(datagram.data(), datagram.size(), &vmmIP);
        //QString vmm_ip = vmmIP.toIPv4Address().toString();

      //  buffer().resize(socket().pendingDatagramSize());
      //  socket().readDatagram(buffer().data(), buffer().size(), &vmmIP);
      //  qDebug() << "BLAH BLAHreceived datagram hex: " << buffer().toHex();
       vmmIP = QHostAddress(vmmIP.toIPv4Address());
        if(IsDbgActive()) {
            //debug
            sx.str("");
            sx << "Received datagram (hex): \n"
               << datagram.toHex().toStdString() << "\n"
               //<< "from VMM with IP: " << vmm_ip 
               << "from VMM with IP: " << vmmIP.toString().toStdString()
               << " and message size is " << datagram.size();
           GetMessageHandler()(sx,"VMMSocket::processReply");sx.str("");

          //  sx.str("");
          //  sx << "Received datagram (hex): \n"
          //     << buffer().toHex().toStdString() << "\n"
          //     << "from VMM with IP: " << vmmIP.toString().toStdString()
          //     << " and message size is " << buffer().size();
          // GetMessageHandler()(sx, "VMMSocket::processReply");
        }

        datagram_hex.clear();
        //debug
        datagram_hex = datagram.mid(0,4).toHex();
        //datagram_hex = buffer().mid(0,4).toHex();
        quint32 received = datagram_hex.toUInt(&ok,16);
        if(received != cmd_cnt_to_check) {
            sx.str("");
            sx << "Command number received (" << received << ") does not match "
               << "internal command counter expected (" << cmd_cnt_to_check
               << ")";
           GetMessageHandler()(sx, "VMMSocket::processReply",true);
            //debug
            //exit(1);
        }

        // fill our list of VMM replies
        //replies.append(vmm_ip);
        replies.append(vmmIP.toString());
    }//while

    if(IsDbgActive()) {
        for(const auto& ip : replies) {
            sx.str("");
            sx << "VMM with IP [" << ip.toStdString() << "] sent a reply to "
               << "command number: " << cmd_cnt_to_check;
           GetMessageHandler()(sx,"VMMSocket::processReply");
        } // ip
    }

    if(replies.size()>0) {
        for(const auto& ip : replies) {
            // unexpected ip has replied -- let us know which IP's are bad
            if(ip != ip_to_check) {
                sx.str("");
                sx << "VMM with IP [" << ip.toStdString() << " has sent a reply"
                   << " at command " << cmd_cnt_to_check << " to a command"
                   << " not sent to it! Out of sync.";
               GetMessageHandler()(sx, "VMMSocket::processReply",true);
                //debug
                //exit(1);
            } // unexpected ip
        } // ip
    } // replies > 0

    if(!replies.contains(ip_to_check)) {
        sx.str("");
        sx << "VMM with IP " << ip_to_check.toStdString() << " did not"
           << " acknowledge command number: " << cmd_cnt_to_check;
       GetMessageHandler()(sx,"VMMSocket::processReply",true);
        //debug
        //exit(1);
    }

    return datagram;

}
// ----------------------------------------------------------------------- //
void VMMSocket::Print()
{
    stringstream ss;
    ss << "    Name          : " << GetName() << "\n"
       << "    Bound to port : " << GetBindingPort() << "\n"
       << "    Status        : " << m_socket->state();
   GetMessageHandler()(ss,"VMMSocket::Print");
}
