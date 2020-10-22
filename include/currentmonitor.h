#ifndef CURRENTMONITOR_H
#define CURRENTMONITOR_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include "message_handler.h"
#include "test_module.h"


using namespace std;
class MainWindow;
class TestModule;
class CurrentMonitor: public QObject
{
    Q_OBJECT
    friend class DAQWindow;
    friend class MainWindow;

    public:
        explicit CurrentMonitor(TestModule *tst, QObject *parent = nullptr);
        explicit CurrentMonitor(const CurrentMonitor &copy);
        QVector<QPair<QDateTime,double>> get_currents_1_9V();
        QVector<QPair<QDateTime,double>> get_currents_2_9V();
        MessageHandler& GetMessageHandler(){ return *m_msg; }
        void LoadMessageHandler(MessageHandler &m);
        void emergencyStop();
        void forceRead();
        bool TestForShortCircuit(double warnlow, double warnhigh);
        void clearCurrents();
        void loadCurrents();
        void loadWarningcurrs(double warn1, double warn2);
private:
        QSerialPort* m_monitor = new QSerialPort();
        QVector<QPair<QDateTime,double>> m_currents;
        MessageHandler *m_msg = new MessageHandler;
        TestModule *m_tst;
        QVector<QPair<QDateTime,double>> m_currents_1_9V;
        QVector<QPair<QDateTime,double>> m_currents_2_9V;
        QVector<QPair<QDateTime,double>> m_zero_currents;
        QTimer timer;
        QProcess* p_monitor = new QProcess();
        double m_warn1_9;
        double m_warn2_9;

public slots:
        void setupCurrentMonitor();
        void startMonitoringProcess();
        void sendQuery();
        void readCurrent();
        void startMonitoring();
        void finishMonitor();
        void finishMonitoringProcess();
        void processFinished(int code);
    signals:
        void finished();
        void ShortCircuit();
        void emergencystopped();
};

#endif // CURRENTMONITOR_H
