#ifndef CALIBRATION_MODULE_H
#define CALIBRATION_MODULE_H

#include <QObject>

// qt
#include <QObject>
#include <QFile>
#include <QMap>
#include <QByteArray>
#include <QBitArray>
#include <QUdpSocket>
#include <QList>
#include <QStringList>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDir>
#include <QThread>

class QByteArray;
class QBitArray;
class QUdpSocket;

// std/stl
#include <iostream>

// vmm
#include "message_handler.h"
using namespace std;
class MainWindow;
class VMMSocket;



class calibration_module : public QObject
{
    Q_OBJECT
public:
    explicit calibration_module(MainWindow *top, QObject *parent = 0);
    bool dbg() { return m_dbg; }
    VMMSocket& daqSocket() { return *m_daqSocket; }

    void LoadMessageHandler(MessageHandler& msg);
    MessageHandler& msg() { return *m_msg; }
    bool ignore16() { return m_ignore16; }


    void connectDAQSocket();
    void closeDAQSocket();
    bool calibRun() { return m_calibRun; }

    static quint32 reverse32(QString hex);
    static uint grayToBinary(uint num);
    static QByteArray bitsToBytes(QBitArray bits);
    static QBitArray bytesToBits(QByteArray bytes);

    void CalibADC();
    void SetRun(QString run){calibrun=run;}
    void SetMode(QString mode){calibmode= mode;}
    void StartCalib();


private:
    MainWindow *root_main;
    bool m_dbg = false;
    QUdpSocket *m_DAQSocket;
    void decodeAndWriteData(const QByteArray& datagram);
    VMMSocket *m_daqSocket;
    MessageHandler *m_msg;
    bool m_ignore16;
    // event data OTF
    std::vector<int> _pdo;
    std::vector<int> _tdo;
    std::vector<int> _bcid;
    std::vector<int> _gray;
    std::vector<int> _channelNo;
    std::vector<int> _flag;
    std::vector<int> _thresh;
    std::vector<int> _neighbor;

     int m_channel_for_calib;

     bool m_calibRun;

     vector< vector< vector<int> > > *v_calibvar;
     QString calibmode;
     QString calibrun;
     int eventcount = 0;
     void Counting();
     void Calib();
     void GetCalSetting();
     void PlotADC(int m);

     int bincount=0;

     vector<int> act_vmm;
     void GetActVMM();

     struct result_mean{
         vector<double> channel;
         vector<vector<double>> y_fullrange;
         vector<double> y_calib;
         double calib_value;
         vector<double> v_calval;

     };

    vector<result_mean> *r_mean;
signals:

public slots:
    void readEvent();
    void updatePlot();
};

#endif // CALIBRATION_MODULE_H

#ifndef _MAINWINDOW_HPP
#include "mainwindow.h"
#endif
