
// vmm
#include "socket_handler.h"

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

SocketHandler::SocketHandler(QObject* parent) :
    QObject(parent),
    m_dbg(false),
    m_msg(0),
    m_pinged(false),
    m_dryrun(false),
    m_skipProcessing(false),
    n_globalCommandCounter(0),
    m_fecSocket(0),
    m_fecSetup(false),
    m_vmmappSocket(0),
    m_vmmappSetup(false)
{
}
// ---------------------------------------------------------------------- //
void SocketHandler::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ---------------------------------------------------------------------- //
void SocketHandler::SetDryRun()
{
    stringstream sx;
    sx << "SocketHandler configured for dry run (will not send anything "
       << "over the network)";
    GetMessageHandler()(sx,"SocketHandler::setDryRun");
    m_dryrun = true;
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
void SocketHandler::AddSocket(std::string name, quint16 bindingPort,
                                    QAbstractSocket::BindMode mode)
{
    bool bind = true;
    QString lname = QString::fromStdString(name).toLower();
    if(lname=="fec") {
        if(m_fecSetup) return;
        m_fecSocket = new VMMSocket();
        m_fecSocket->LoadMessageHandler(GetMessageHandler());
        m_fecSocket->SetDebugMode(IsDbgActive());
        m_fecSocket->SetName(name);
        m_fecSocket->SetBindingPort(bindingPort);
        if(!IsDryRunEnabled())
            bind = m_fecSocket->BindSocket(bindingPort, mode);

        if(bind) m_fecSetup = true;
       GetMessageHandler()("VMMSocket added:","SocketHandler::addSocket");
        m_fecSocket->Print();

    } // fec
//    else if(lname=="daq") {
//     //   if(m_daqSetup) return;
//        m_daqSocket = new VMMSocket();
//        m_daqSocket->LoadMessageHandler(msg());
//        m_daqSocket->setDebug(dbg());
//        m_daqSocket->setName(name);
//        m_daqSocket->setBindingPort(bindingPort);
//     //   if(!dryrun())
//     //       m_daqSocket->bindSocket(bindingPort, mode);
//        
//        m_daqSetup = true;
//       GetMessageHandler()("VMMSocket added:","SocketHandler::addSocket");
//        m_daqSocket->Print();
//    } //daq
    else if(lname=="vmmapp") {
        if(m_vmmappSetup) return;
        m_vmmappSocket = new VMMSocket();
        m_vmmappSocket->LoadMessageHandler(GetMessageHandler());
        m_vmmappSocket->SetDebugMode(IsDbgActive());
        m_vmmappSocket->SetName(name);
        m_vmmappSocket->SetBindingPort(bindingPort);
        if(!IsDryRunEnabled())
            bind = m_vmmappSocket->BindSocket(bindingPort, mode);

        if(bind) m_vmmappSetup = true;
       GetMessageHandler()("VMMSocket added:","SocketHandler::addSocket");
        m_vmmappSocket->Print();
    } //vmmapp
    else {
        stringstream sx;
        sx << "ERROR Current can only add the 'fec' or 'vmmapp' sockets\n"
           << "ERROR You have attemped to add a socket named: " << name;
       GetMessageHandler()(sx,"SocketHandler::addSocket",true);
        exit(1);
    }
}
// ---------------------------------------------------------------------- //
bool SocketHandler::IsFECSocketOK()
{
    bool status = true;
    if(!m_fecSocket) status = false;
    return status;
}
// ---------------------------------------------------------------------- //
bool SocketHandler::IsVMMSocketOK()
{
    bool status = true;
    if(!m_vmmappSocket) status = false;
    return status;
}
// ---------------------------------------------------------------------- //
void SocketHandler::SendDatagram(const QByteArray& datagram, const QString& ip,
            const quint16& destPort, const QString& whichSocket, const QString& callingFn)
{
    std::string fn = "";
    if(callingFn!="") fn = "(" + callingFn.toStdString() + ") ";

    // get the requested VMMSocket
    VMMSocket& socket = GetSocket(whichSocket.toStdString());

    //CHECK STATUS ENUM
    if(!IsPinged()) {
       GetMessageHandler()("ERROR Boards are not in pinged OK state...",
                "SocketHandler::SendDatagram", true);
        exit(1);
    }

    // make sure the socket is connected (bound) to the correct port
    if(!IsDryRunEnabled()) {
        if(!socket.CheckAndReconnect(callingFn.toStdString()))
            exit(1);
    }

    // now send the data
    if(IsDbgActive()) {
        stringstream sx;
        sx << fn
           << (IsDryRunEnabled() ? "[dry run]" : "")
           << " Data from socket '" << socket.GetName() << "' sent to "
           << "(IP,port) = (" << ip.toStdString() << ", " << destPort << ") :\n"
           << datagram.toHex().toStdString() << "\n";
       GetMessageHandler()(sx,"SocketHandler::SendDatagram");

    }
    if(!IsDryRunEnabled())
        socket.WriteDatagram(datagram, QHostAddress(ip), destPort);
}
// ---------------------------------------------------------------------- //
bool SocketHandler::WaitForReadyRead(std::string name, int msec)
{
    bool status = false;
    if(IsDryRunEnabled() || m_skipProcessing) status = true;
    else {
        VMMSocket& vmmsocket = GetSocket(name);
        status = vmmsocket.GetSocket().waitForReadyRead(msec);
    }
    return status;
}
// ---------------------------------------------------------------------- //
QByteArray SocketHandler::ProcessReply(std::string name, const QString& ip_to_check,
                    quint32 cmd_delay)
{
    QByteArray outbuffer;
    quint32 count = GetCommandCounter();

    if(IsDryRunEnabled() || m_skipProcessing) {
       GetMessageHandler()("NOT PROCESSING REPLIES!", "SocketHandler::processReply");
        return outbuffer;
    }

    VMMSocket& socket = GetSocket(name);
    outbuffer = socket.ProcessReply(ip_to_check, cmd_delay, count);
    return outbuffer;
}
// ---------------------------------------------------------------------- //
void SocketHandler::CloseAndDisconnect(std::string name, std::string callingFn)
{
    if(IsDryRunEnabled())
        return;

    VMMSocket& vmmsocket = GetSocket(name);
    vmmsocket.CloseAndDisconnect(callingFn);
}
// ---------------------------------------------------------------------- //
QByteArray SocketHandler::SetBuffer(std::string name)
{
    VMMSocket& vmmsocket = GetSocket(name);
    return vmmsocket.GetBuffer(); 
}
// ---------------------------------------------------------------------- //
VMMSocket& SocketHandler::GetSocket(std::string whichSocket)
{
    stringstream sx;
    if(whichSocket=="") {
        sx.str("");
        sx << "ERROR This method must be passed a string containing the name of "
           << "the desired socket";
       GetMessageHandler()(sx,"SocketHandler::getSocket",true);
        exit(1);
    }
    QString lname = QString::fromStdString(whichSocket).toLower();
    if(lname=="fec") {
        if(m_fecSocket) return *m_fecSocket;
        else {
           GetMessageHandler()("Requested socket (fec) is null!","SocketHandler::getSocket",true);
            exit(1);
        }
    }
    else if(lname=="vmmapp") {
        if(m_vmmappSocket) return *m_vmmappSocket;
        else {
           GetMessageHandler()("Requested socket (vmmapp) is null!","SocketHandler::getSocket",true);
            exit(1);
        }
    }
    else {
        sx.str("");
        sx << "ERROR Currently can only retrieve the 'fec' or 'vmmapp' sockets.\n"
           << "ERROR You have attempted to retrieve a socket named: " << whichSocket;
       GetMessageHandler()(sx, "SocketHandler::getSocket",true);
        exit(1);
    }
}
// ---------------------------------------------------------------------- //
void SocketHandler::Print()
{
    if(!m_fecSocket && !m_vmmappSocket) {
        if(IsDbgActive())
           GetMessageHandler()("SocketHandler currently holds no sockets!",
                                                "SocketHandler::Print");
    }
    if(m_fecSocket)
        GetFECSocket().Print();
    if(m_vmmappSocket)
        GetVMMSocket().Print();
}
