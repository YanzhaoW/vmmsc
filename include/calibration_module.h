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
#include <QJsonObject>

class QByteArray;
class QBitArray;
class QUdpSocket;

// std/stl
#include <iostream>
#include <fstream>
#include <chrono>

#include "globparameter.h"
// vmm
#include "message_handler.h"


using namespace std;
class MainWindow;
class QCustomPlot;



class CalibrationModule : public QObject
{
    Q_OBJECT
public:
    explicit CalibrationModule(MainWindow *top, QObject *parent = nullptr);
    bool IsDbgActive() { return m_dbg; }
    void StopDataTaking();
    void StartDataTaking();

    void LoadMessageHandler(MessageHandler& GetMessageHandler);
    MessageHandler& GetMessageHandler() { return *m_msg; }
    bool Ignore16() { return m_ignore16; }


    void ConnectDAQSocket();
    void CloseDAQSocket();
    void StartCalibration();
    void DoCalibrationStep();

    void SaveCorrections();
    void GetActiveVMMs();
    void SavePlotsAsPDF();

    bool m_dataAvailable = false;
private:

    MainWindow *m_mainWindow;
    bool m_dbg = true;
    QUdpSocket *m_udpSocket;


    int m_modeIndex;
    QString m_runMode;
    QString m_plotType;
    QVector< QCustomPlot * > plotVector;
    //QString m_calibRun;

    std::chrono::high_resolution_clock::time_point m_start;
    std::chrono::high_resolution_clock::time_point m_end;
    std::chrono::high_resolution_clock::time_point m_nodata_start;
    std::chrono::high_resolution_clock::time_point m_nodata_end;
    vector<int> m_vmmActs;
    std::map<QString, QString> mapIPFirmware;
    std::map<QString, int> mapIPFecId;

    void startReceiver();
    void stopReceiver();
    bool CheckModes();
    int GetCalibrationModeIndex(QString mode);
    bool IsCalibration();
    void PlotData();
    void MeasurePedestal();
    void FitOfflineCalibrationData();
    void AccumulateData();
    void CalculateCorrections();
    double SortVectors( vector<double>& sortedMin, vector<double>& sortedMax);
    void InitializeDataStructures();
    void GetSettings();
    int GetFEC(int vmmId);
    int GetHDMI(int vmmId);
    int GetVMM(int vmmId);



    int Receive_VMM2(const char* buffer, int size, int fecId);
    int Receive_VMM3(const char* buffer, int size, int fecId);
    int Parse_VMM2(uint32_t data1, uint32_t data2, uint32_t vmmid, int fecId);
    int Parse_VMM3(uint32_t data1, uint16_t data2, int fecId);

    uint32_t Reversebits32(uint32_t x);
    uint16_t Reversebits16(uint16_t x);
    uint32_t Gray2bin32(uint32_t num);


    const static int maxModes = 20;
    bool m_isCalibrated[maxModes];

    MessageHandler *m_msg;

    uint32_t m_lastUdpTimeStamp=0;

    struct SRSHeader_VMM2 {
        uint32_t m_frameCounter{ 0 };   /// frame counter packet field
        uint32_t m_dataId { 0 }; /// data type identifier packet field
        uint32_t m_udpTimeStamp; /// Transmission time for UDP packet
    };

    struct SRSHeader_VMM3 {
        uint32_t m_frameCounter { 0 };   /// frame counter packet field
        uint32_t m_dataId { 0 }; /// data type identifier packet field + ID of the FEC card (0-255)
        uint32_t m_udpTimeStamp { 0 };   /// Transmission time for UDP packet
        uint32_t m_offsetOverflow { 0 }; /// offset overflow in last frame (1 bit per VMM)
    };

    struct CommonData_VMM3 {
        uint32_t m_dataId { 0 }; /// data type identifier packet field + ID of the FEC card (0-255)
        uint8_t m_fecId { 255 };
        uint64_t m_lastFrameCounter { 0 };
        bool m_fcIsInitialized { false };
        uint64_t m_frameCounter { 0 };   /// frame counter packet field
        uint32_t m_udpTimeStamp { 0 };   /// Transmission time for UDP packet
        uint32_t m_offsetOverflow { 0 }; /// offset overflow in last frame (1 bit per VMM)
    };

    // bytes
    static const int m_SRSHeaderSize_VMM3 { 16 };
    static const int m_SRSHeaderSize_VMM2 { 12 };
    static const int m_hitAndMarkerSize_VMM3 { 6 };
    static const int m_hitSize_VMM2 { 8 };
    static const int m_data1Size { 4 };
    static const int m_JumboFrameSize { 9000 };

    static const int m_maxHits_VMM2 { static_cast<int>(m_JumboFrameSize / m_hitSize_VMM2) };
    static const int m_maxHits_VMM3 { static_cast<int>(m_JumboFrameSize / m_hitAndMarkerSize_VMM3) };
    /// Maximum capacity of data array
    // static const int m_maxHits { (int) (m_JumboFrameSize / m_hitAndMarkerSize) };

    /// Holds data common to all readouts in a packet
    CommonData_VMM3 m_commonData;

    int m_numHits = 0;
    int m_bitCount=-1;
    int m_number_bits = 0;
    int m_vmmIndex=0;

    int m_maxThreshold = 350;
    int m_minThreshold = 200;
    int m_threshold = 0;

    const static int m_number_bits_adc = 32;
    const static int m_number_bits_tdc = 16;
    const static int m_number_bits_threshold = 2;
    const static int m_number_bits_offline_time = 16;
    const static int m_number_bits_offline_adc= 15;
    const static int m_number_bits_pedestal = 3;

    double  m_bc_period[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI];
    double m_tac_slope[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];

    std::vector<double> m_data[32][FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID][64];
    std::vector<double> m_mean[32][FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<double> m_x;
    std::vector<double> m_y[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    //std::vector<double> m_y;
    std::vector<double> m_calVal[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<int> m_bitVal[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<double> m_offset[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<double> m_slope[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];

    bool m_ignore16;

    std::vector<int> m_BCID;
    QJsonObject * m_jsonObject;
    std::ofstream m_outFile;

    int m_old_sdp2[1000];
    int m_old_TP_skew[1000];
    int m_old_sdt[1000];
signals:

public slots:
    void readEvent();
    void updatePlot();
    void Receive(const char* buffer, int size, QString ip);
};

#endif // CALIBRATION_MODULE_H

#ifndef _MAINWINDOW_HPP
#include "mainwindow.h"
#endif
