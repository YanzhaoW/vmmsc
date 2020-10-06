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
#include "mainwindow.h"



using namespace std;
class MainWindow;
class QCustomPlot;

#define TIME_FACTOR 3

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

    double ThresholdDAC_to_mV(int dac);
    int Threshold_mV_to_DAC(double mV);
    double PulserDAC_to_mV(int dac);
    double PulserDAC_to_PulseHeight_mV(int dac, int gain_idx);
    int PulseHeight_mV_to_PulserDAC( double pulseHeight, int gain_idx);

    void LoadSettings();
    void SaveSettings();

    bool m_dataAvailable = false;

public slots:
    void readEvent();
    void updatePlot();
    void setPlotChoice();
    void Receive(const char* buffer, long size, QString ip);

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


    bool CheckModes();
    int GetCalibrationModeIndex(QString mode);
    bool IsCalibration();
    void PlotData();
    void MeasurePedestalOrThreshold(bool isPedestal, bool isThresholdCalibration);
    void FitOfflineCalibrationData();
    void AccumulateData();
    void CalculateCorrections();
    double SortVectors( vector<double>& sortedMin, vector<double>& sortedMax);
    void InitializeDataStructures();
    void GetSettings();
    void SetPlotChoice();
    void Reset();
    int GetFEC(int vmmId);
    int GetHDMI(int vmmId);
    int GetVMM(int vmmId);

    QString CreateFileName(QString name,  int polarity=-1, int gain=-1, int peaktime=-1, int tac=-1,int bcclock=-1);

    int Receive_VMM3(const char* buffer, long size, int fecId);
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


    /// Holds data common to all readouts in a packet
    CommonData_VMM3 m_commonData;

    int m_numHitsInFrame = 0;
    int m_numHits = 0;
    int m_bitCount=-1;
    int m_number_bits = 0;
    int m_theChannel = 0;
    int m_theVMM = 0;
    int m_theFEC = 0;
    int m_theDirection = 0;

    int m_maxThreshold = 0;
    int m_minThreshold = 0;
    int m_threshold = 0;
    int m_pulser_dac = 0;

    bool m_isThresholdCalibration = false;
    double m_gainTable[8] = {0.5,1,3,4.5,6,9,12,16};
    double m_peaktimeTable[4] = {200,100,50,25};
    double m_tacTable[4] = {60,100,350,650};
    QString m_bcclock_table[8] = {"160", "160inv", "80", "40", "20", "10", "5", "2.5"};

    QString m_polarityTable[2] = {"negative", "positive"};

    int m_thresholdTable[8] = {220,220,250,250,250,300,300,300};

    int m_minPulseHeightTable[8] = {530,258,123,93,74,66,53,42};
    int m_maxPulseHeightTable[8] = {1023,1023,859,576,436,294,221,167};

    const static int m_number_bits_adc = 32;
    const static int m_number_bits_tdc = 16;
    const static int m_number_bits_threshold = 32;
    const static int m_number_bits_s_curve = 1;

    const static int m_number_bits_offline_time = 4;
    const static int m_number_bits_offline_adc= 4;
    const static int m_number_bits_pedestal = 1;

    uint64_t m_srs_timestamp_end[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    uint64_t m_srs_timestamp_start[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    uint64_t m_start;
    uint64_t m_end;

    double  m_bc_period[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI];
    double m_tac_slope[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];

    //Data Acquisition
    //Data containers for data in Parse_VMM3
    std::vector<double> m_data[32][FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID][64];
    std::vector<double> m_mean[32][FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];

    //Containers for calculated data
    //S-curve
    std::vector<double> m_max_value_x;
    std::vector<double> m_min_value_x;

    //Online ADC and TDC
    std::vector<double> m_calVal[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<int> m_bitVal[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];

    //Fit for offline ADC and time calibration
    std::vector<double> m_offset[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];
    std::vector<double> m_slope[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];


    //Plot containers
    //Plots with channels on x-axis
    std::vector<double> m_x;

    //Plots with DAC or mV values on x-axis
    std::vector<double> m_dac_x;

    //y values for plots with channels on x-axis
    std::vector<double> m_y[FECS_PER_DAQ][HDMIS_PER_FEC][HYBRIDS_PER_HDMI][VMMS_PER_HYBRID];

    //y values for S-curve, the vector contains one value per threshold
    std::vector<double> m_channel_y[64];


    bool m_ignore16;

    std::vector<int> m_BCID;
    QJsonArray *m_calibrationArray[2] ;
    QString m_jsonObjectName = "vmm_calibration";
    std::ofstream m_outFile;

};

#endif // CALIBRATION_MODULE_H



