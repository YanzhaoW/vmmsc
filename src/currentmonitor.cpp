#include "currentmonitor.h"
#include "message_handler.h"
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QDataStream>
#include <QBuffer>
#include <QTimer>

CurrentMonitor::CurrentMonitor(TestModule *tst, QObject *parent): m_tst{tst}
{
    cout << "Creating monitor" << std::endl;
    //setupCurrentMonitor();
    connect(&timer,SIGNAL(timeout()),this, SLOT(sendQuery()));
    connect(p_monitor,SIGNAL(finished(int)),this,SLOT(processFinished(int)));

}

void CurrentMonitor::setupCurrentMonitor()
{   //cout << "Setting up Monitor" << std::endl;
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    //GetMessageHandler()("Available Ports:","CurrentMonitor::setupCurrentMonitor()");
    for(int i=0; i< ports.size();i++){
        const QSerialPortInfo port = ports[i];
        stringstream desc;
        desc << port.description().toStdString() << "\t";
        desc << port.portName().toStdString()<< "\t";
        desc << port.serialNumber().toStdString()<< "\t";
        desc << port.vendorIdentifier()<< "\t";
        desc << port.productIdentifier()<< "\t";
        //GetMessageHandler()(desc, "CurrentMonitor::setupCurrentMonitor");
        if(port.vendorIdentifier()==1027 && port.productIdentifier()==24577){
            m_monitor->setPort(port);
            m_monitor->setBaudRate(QSerialPort::Baud9600);
            m_monitor->setParity(QSerialPort::NoParity);
            m_monitor->setDataBits(QSerialPort::Data8);
            m_monitor->setStopBits(QSerialPort::OneStop);
            m_monitor->setFlowControl(QSerialPort::NoFlowControl);
            m_monitor->open(QIODevice::ReadWrite);
            if(m_monitor->isOpen()){
                connect(m_monitor,SIGNAL(readyRead()),this,SLOT(readCurrent()));
            }
            else{
                GetMessageHandler()("Communication to Power supply could not be established :(","Currentmonitor::setupCurrentMonitor");
            }
        }
    }
}

void CurrentMonitor::startMonitoringProcess()
{
    QString dir = QCoreApplication::applicationDirPath()+"/../testing/";
    GetMessageHandler()(dir.toStdString(), "Currmon::startmonitoringprocess");
    p_monitor->setWorkingDirectory(dir);
    QString process = "python3";
    QStringList args;
    args << "./../testing/powersupply.py"<< QString::number(m_warn1_9) << QString::number(m_warn2_9);
    p_monitor->setProgram(process);
    p_monitor->setArguments(args);
    p_monitor->start();
}

QVector<QPair<QDateTime, double> > CurrentMonitor::get_currents_1_9V()
{
    return m_currents_1_9V;
}

QVector<QPair<QDateTime, double> > CurrentMonitor::get_currents_2_9V()
{
    return m_currents_2_9V;
}

void CurrentMonitor::LoadMessageHandler(MessageHandler& m)
{
        m_msg = &m;
}

void CurrentMonitor::readCurrent()
{
    //cout <<"reading current"<<std::endl;
    //cout << m_monitor->bytesAvailable() << std::endl;
    QByteArray read = "";
    while(m_monitor->canReadLine()){
            read += m_monitor->readAll();
            read.chop(1);
    }

    QString strread = QString::fromStdString(read.toStdString());
    strread.remove('A');
    QDateTime theTime = QDateTime::currentDateTime();
    double current =strread.toDouble();
    //cout << "current is: " <<current << std::endl;
    if(current >1.0){
        m_currents_1_9V.push_back(qMakePair(theTime,current));
        //cout << "current is: " <<current << std::endl;
    }
    else if(current<=1.0 && current>=0.01){

        m_currents_2_9V.push_back(qMakePair(theTime,current));
        //cout << "current is: " <<current << std::endl;
    }
    else{
        m_zero_currents.push_back(qMakePair(theTime,current));
    }
}

void CurrentMonitor::startMonitoring()
{
    m_monitor->open(QIODevice::ReadWrite);
    const QByteArray msg1("VSET2:2.9\r\nVSET1:1.9\r\nISET2:0.4\r\nISET1:2.0\r\n");
    m_monitor->write(msg1);
    m_monitor->waitForBytesWritten();
    const QByteArray msg("OUT1\r\n");
    m_monitor->write(msg);
    m_monitor->waitForBytesWritten();
    timer.start(500);
    sendQuery();
}

void CurrentMonitor::finishMonitor()
{
    timer.stop();
    const QByteArray msg("OUT0\r\n");
    GetMessageHandler()("Turning off the Power Supply", "CurrentMonitor::finishMonitor");
    m_monitor->write(msg);
    m_monitor->waitForBytesWritten();
    QThread::usleep(1000);
    m_monitor->write(msg);
    QThread::usleep(1000);
    m_monitor->write(msg);
    GetMessageHandler()("Closed and stopped timer","CurrentMonitor::finishMonitor");
    m_monitor->close();
    emit finished();
}

void CurrentMonitor::finishMonitoringProcess()
{
    p_monitor->terminate();
}

void CurrentMonitor::processFinished(int code)
{
    if(code == 42){
        GetMessageHandler()("Short Circuit detected, automated shutoff","CurrentMonitor::processFinished");
        emit ShortCircuit();
    }
    else{
        emit finished();
    }
}

void CurrentMonitor::sendQuery(){
    const QByteArray msg("IOUT1?\r\nIOUT2?\r\n");
    m_monitor->write(msg);
    m_monitor->waitForBytesWritten();
//    m_monitor->waitForReadyRead();
}

void CurrentMonitor::forceRead(){
    const QByteArray msg("IOUT1?\r\nIOUT2?\r\n");
    m_monitor->write(msg);
    m_monitor->waitForBytesWritten();
    m_monitor->waitForReadyRead();
    this->readCurrent();
}

bool CurrentMonitor::TestForShortCircuit(double warnlow, double warnhigh)
{
    timer.stop();
    bool continueb = true;
    bool nocurrs = false;
    int n_currs = 0;
    bool ret =false;
    while(continueb){
        QCoreApplication::processEvents();
        sendQuery();
        if(!m_currents_1_9V.isEmpty()&& !m_currents_2_9V.isEmpty()){
            continueb = false;
        }
        else if(m_zero_currents.size()>20){
            continueb = false;
            nocurrs = true;
        }
    }
    QThread::msleep(500);
    if(m_currents_1_9V[0].second >= m_warn1_9){
        emergencyStop();
        cout << "Emergency stopped"<<endl;
        ret = true;
    }
    if(m_currents_2_9V[0].second >= m_warn2_9){
        emergencyStop();
        cout << "Emergency stopped"<<endl;
        ret = true;
    }
    if(nocurrs==true){
        emergencyStop();
        ret = true;
    }
    timer.start(500);
    return ret;

}

void CurrentMonitor::clearCurrents()
{
    m_currents_1_9V.clear();
    m_currents_2_9V.clear();
    m_zero_currents.clear();
    return;
}

void CurrentMonitor::loadCurrents()
{
    QString filepath = QCoreApplication::applicationDirPath()+"/../testing/currents.dat";
    QFile file(filepath);
    QDir dir(QCoreApplication::applicationDirPath()+"/../testing");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        GetMessageHandler()("File not open","CurrentMonitor::loadCurrents");
        return;
    }
    else{
        while(!file.atEnd()){
            QString line = QString(file.readLine());
            line.remove('\n');
            cout << line.toStdString() << endl;
            QStringList splitted = line.split(QRegExp("\\s+"));
            QDateTime thetime = QDateTime::fromString(splitted[0],"yyyyMMddhhmmss.zzz");
            QString c1= splitted[1];
            c1.remove('A');
            double curr1 = c1.toDouble();
            QString c2= splitted[2];
            c2.remove('A');
            double curr2 = c2.toDouble();
            m_currents_1_9V.push_back(qMakePair(thetime,curr1));
            m_currents_2_9V.push_back(qMakePair(thetime,curr2));
        }
    }
    dir.remove("currents.dat");
}

void CurrentMonitor::loadWarningcurrs(double warn1,double warn2){
    m_warn1_9 = warn1;
    m_warn2_9 = warn2;
}

void CurrentMonitor::emergencyStop(){
    const QByteArray msg("OUT0\r\n");
    GetMessageHandler()("Emergency Shutoff, stopping tests and turning off power","CurrentMonitor::emergencyStop");
    m_monitor->write(msg);
    m_monitor->waitForBytesWritten();
    QThread::usleep(1000);
    m_monitor->write(msg);
    m_monitor->waitForBytesWritten();
    emit emergencystopped();
}
