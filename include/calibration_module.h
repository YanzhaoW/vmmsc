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

#define TIME_FACTOR 4

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

    std::chrono::high_resolution_clock::time_point m_nodata_start;
    std::chrono::high_resolution_clock::time_point m_nodata_end;
    vector<int> m_vmmActs;
    std::map<QString, QString> mapIPFirmware;
    std::map<QString, int> mapIPFecId;

    //void startReceiver();
    //void stopReceiver();
    bool CheckModes();
    int GetCalibrationModeIndex(QString mode);
    bool IsCalibration();
    void PlotData();
    void MeasureThreshold(int the_bit);
    void MeasurePedestal();
    void FitOfflineCalibrationData();
    void AccumulateData();
    void CalculateCorrections();
    double SortVectors( vector<double>& sortedMin, vector<double>& sortedMax);
    void InitializeDataStructures();
    void GetSettings();
    void Reset();
    int GetFEC(int vmmId);
    int GetHDMI(int vmmId);
    int GetVMM(int vmmId);
    QString CreateFileName(QString name,  int polarity=-1, int gain=-1, int peaktime=-1, int tac=-1);


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

    int m_numHitsInFrame = 0;
    int m_numHits = 0;
    int m_bitCount=-1;
    int m_number_bits = 0;
    int m_vmmIndex=0;
    int m_theChannel = 0;
    int m_theDirection = 0;
    double m_rateLimit = 0;
    int m_maxThreshold = 300;
    int m_minThreshold = 50;
    int m_threshold = 0;
    int m_minimumNumHits = 1;
    int m_maskedChannels = 0;
    double m_gainTable[8] = {0.5,1,3,4.5,6,9,12,16};
    double m_peaktimeTable[4] = {200,100,50,25};
    double m_tacTable[4] = {60,100,350,650};
    QString m_polarityTable[2] = {"negative", "positive"};

    int m_thresholdTable[8] = {220,250,250,250,250,300,300,300};
    double m_minGainTable[8] = {500,330,150,120,60,50,40,30};
    double m_maxGainTable[8] = {1023,1023,1023,700,510,320,210,140};



    const static int m_number_bits_adc = 32;
    const static int m_number_bits_tdc = 16;
    const static int m_number_bits_threshold = 1;
    const static int m_number_bits_s_curve = 1;

    const static int m_number_bits_offline_time = 16/TIME_FACTOR;
    const static int m_number_bits_offline_adc= 15;
    const static int m_number_bits_pedestal = 1;

    uint64_t m_srs_timestamp_end[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    uint64_t m_srs_timestamp_start[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    uint64_t m_start;
    uint64_t m_end;

    double  m_bc_period[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI];
    double m_tac_slope[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];

    std::vector<double> m_data[32][FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID][64];
    std::vector<double> m_mean[32][FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<double> m_dac;

    std::vector<double> m_x;
    std::vector<double> m_y[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    //std::vector<double> m_y;
    std::vector<double> m_calVal[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<int> m_bitVal[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<double> m_offset[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<double> m_slope[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];

    bool m_ignore16;

    std::vector<int> m_BCID;
    QJsonArray *m_calibrationArray[2] ;
    QString m_jsonObjectName[2] = {"vmm_adc_calibration","vmm_time_calibration"};
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
