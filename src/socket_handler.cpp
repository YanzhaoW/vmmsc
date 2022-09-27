// vmm
#include "socket_handler.h"
#include <QMessageBox>
// Qt
#include <QProcess>
#include <QByteArray>

// std/stl
#include <iostream>
using namespace std;

////////////////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------- //
//  SocketHandler
// ---------------------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////



SocketHandler* SocketHandler::socketHandler= nullptr;

SocketHandler *SocketHandler::getInstance(QObject *parent)
{
    if(socketHandler==nullptr)
    {
        socketHandler = new SocketHandler(parent);
    }
    return socketHandler;
}

void SocketHandler::deleteInstance()
{
   if(socketHandler != nullptr)
   {
        delete socketHandler;
        socketHandler = nullptr;
   }
}



SocketHandler::SocketHandler(QObject* parent) :
    QObject(parent),
    m_dbg(false),
    m_msg(0),
    m_skipProcessing(false),
    n_globalCommandCounter(0),
    m_socket(0),
    m_socketSetup(false)
{
}
// ---------------------------------------------------------------------- //
void SocketHandler::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ---------------------------------------------------------------------- //

void SocketHandler::UpdateCommandCounter()
{
    n_globalCommandCounter++;
    emit on_command_counter_updated();
}
// ---------------------------------------------------------------------- //
void SocketHandler::ResetCommandCounter()
{
    n_globalCommandCounter = 0;
    emit on_command_counter_updated();
}
// ---------------------------------------------------------------------- //
void SocketHandler::AddSocket(quint16 bindingPort,
                              QAbstractSocket::BindMode mode)
{
    bool bind = true;
    if(m_socketSetup) return;
    m_socket = new VMMSocket();
    m_socket->LoadMessageHandler(GetMessageHandler());
    m_socket->SetDebugMode(IsDbgActive());
    m_socket->SetBindingPort(bindingPort);
    bind = m_socket->BindSocket(bindingPort, mode);
    if(bind) m_socketSetup = true;
    m_socket->Print();

}
// ---------------------------------------------------------------------- //
bool SocketHandler::IsFECSocketOK()
{
    bool status = true;
    if(!m_socket) status = false;
    return status;
}

// ---------------------------------------------------------------------- //
bool SocketHandler::SendDatagram(const QByteArray& datagram, const QString& ip,
                                 const quint16& destPort, const QString& callingFn)
{
    std::string fn = "";
    if(callingFn!="") fn = "(" + callingFn.toStdString() + ") ";

    // make sure the socket is connected (bound) to the correct port
    if(!m_socket->CheckAndReconnect(callingFn.toStdString())) {
        return false;
    }


    // now send the data
    if(IsDbgActive()) {
        stringstream sx;
        sx << fn
           << " Data from SC socket sent to "
           << "(IP,port) = (" << ip.toStdString() << ", " << destPort << ") :\n"
           << datagram.toHex().toStdString() << "\n";
        GetMessageHandler()(sx,"SocketHandler::SendDatagram");

    }
    m_socket->WriteDatagram(datagram, QHostAddress(ip), destPort);
    return true;
}
// ---------------------------------------------------------------------- //
bool SocketHandler::WaitForReadyRead(int msec)
{
    bool status = false;
    if(m_skipProcessing) status = true;
    else {
        status = m_socket->GetSocket().waitForReadyRead(msec);
    }
    return status;
}
// ---------------------------------------------------------------------- //
void SocketHandler::ProcessReply()
{
    m_socket->ProcessReply();
}
// ---------------------------------------------------------------------- //
void SocketHandler::CloseAndDisconnect(std::string callingFn)
{
    m_socket->CloseAndDisconnect(callingFn);
}
// ---------------------------------------------------------------------- //
QByteArray SocketHandler::SetBuffer()
{
    return m_socket->GetBuffer();
}
// ---------------------------------------------------------------------- //
VMMSocket& SocketHandler::GetSocket()
{
    stringstream sx;
    if(m_socket == nullptr) {
        GetMessageHandler()("Requested socket (fec) is null!","SocketHandler::getSocket",true);
        QMessageBox msgBox;
        msgBox.setText("Error: Requested socket is null!");
        msgBox.exec();
    }
    return *m_socket;
}
// ---------------------------------------------------------------------- //
void SocketHandler::Print()
{
    if(!m_socket) {
        if(IsDbgActive())
            GetMessageHandler()("SocketHandler currently holds no sockets!",
                                "SocketHandler::Print");
    }
    if(m_socket)
        GetSocket().Print();
}
