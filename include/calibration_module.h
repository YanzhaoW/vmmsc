#ifndef CALIBRATION_MODULE_H
#define CALIBRATION_MODULE_H


#define MAX_BITS 33
#define NUM_BCID 5
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
#include "interpolation.h"


using namespace std;
using namespace alglib;


class DAQWindow;
class QCustomPlot;
class QCPColorMap;
class QCPColorScale;

class CalibrationModule : public QObject
{
    Q_OBJECT
public:
    explicit CalibrationModule(DAQWindow *top, QObject *parent = nullptr);
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
    void AutomaticLatencyCalib(bool isReset);
    void SaveCorrections();
    void GetActiveVMMs();
    void SaveDataAsCSV();
    void SavePlotsAsPDF();
    void SaveToLog();
    void ApplyCalib(bool isTimewalk=false);
    void JoinCalib();
    void WriteSystemConfig();

    double ThresholdDAC_to_mV(int dac);
    int Threshold_mV_to_DAC(double mV);
    double PulserDAC_to_mV(int dac);
    double PulserDAC_to_PulseHeight_mV(int dac, int gain_idx);
    int PulseHeight_mV_to_PulserDAC( double pulseHeight, int gain_idx);
    void AddVMMsToCombobox();


    bool m_dataAvailable = false;
    bool m_isAutomatic = false;

public slots:
    void readEvent();
    void updatePlot();
    void calibAndPlotChoices();


private:

    DAQWindow *m_daqWindow;
    bool m_dbg = false;
    QUdpSocket *m_udpSocket;


    int m_modeIndex;
    int m_data_modeIndex;
    QString m_runMode;
    QString m_plotType;
    QVector< QCustomPlot * > plotVector;
    QVector< QCPColorScale * > colorScaleVector;
    QVector< QCPColorMap * > colorMapVector;

    //QString m_calibRun;

    std::chrono::high_resolution_clock::time_point m_nodata_start;
    std::chrono::high_resolution_clock::time_point m_nodata_end;
    vector<int> m_vmmActs;
    std::map<int,int> m_fecPosID;
    std::map<int,int> m_fecIDPos;

    std::map<int,int> m_fecLatencyReset;
    std::map<int,int> m_fecLatencyTP;

    //std::map<QString, QString> mapIPFirmware;
    //std::map<QString, int> mapIPFecId;
    void SaveSettings(QString name);
    void LoadSettings(QString name);
    bool CheckModes();
    int GetCalibrationModeIndex(QString mode);
    bool IsCalibration();
    void PlotData();
    void MeasurePedestalOrThreshold(bool isPedestal, bool isThresholdCalibration);
    void MeasurePulserOrThresholdDAC(bool measurePulser);
    void FitLinearData();
    void FitSCurve();
    void Fit4PL();
    void FitErrorCurve();
    void CalculateLatency(bool isReset);
    void AccumulateData();
    void CalculateCorrections();
    double SortVectors( vector<double>& sortedMin, vector<double>& sortedMax);
    void InitializeDataStructures();
    void GetSettings();
    //void SetPlotChoice();
    void Reset();
    int GetFEC(int vmmId);
    int GetHybrid(int vmmId);
    int GetVMM(int vmmId);
    QString CreateDir(QString directory, QString name);

    QString CreateFileName(int polarity=-1, double gain=-1, double st=-1, double tac=-1, int dac=-1);
    void Receive(const char* buffer, long size, int fecId);
    int Receive_VMM3(const char* buffer, long size, int fecId);
    int Parse_VMM3(uint32_t header, uint32_t data1, uint32_t data2, uint32_t data3, uint32_t data4, int readoutIndex);
    void SaveCorrectionsEFU(QString name);

    const static int maxModes = 20;

    MessageHandler *m_msg;

    struct ESSHeader {
        uint8_t m_padding { 0 };
        uint8_t m_version { 0 };
        uint8_t m_cookie1 { 0 };
        uint8_t m_cookie2 { 0 };
        uint8_t m_cookie3 { 0 };
        uint8_t m_type { 0 };
        uint16_t m_length { 0 };
        uint8_t m_outputQ { 0 };
        uint8_t m_timeSrc { 0 };
        uint32_t m_pulseT_high { 0 };
        uint32_t m_pulseT_low { 0 };
        uint32_t m_prevPT_high { 0 };
        uint32_t m_prevPT_low { 0 };
        uint32_t m_seqNo { 0 };
    };


    // bytes
    static const int m_ESSHeaderSize { 30 };
    static const int m_hitSize_VMM3a { 16 };
    static const int m_headerSizeAssister { 4 };
    static const int m_JumboFrameSize { 8950 };

    static const int m_maxHits_VMM3a { static_cast<int>((m_JumboFrameSize - m_ESSHeaderSize) / (m_headerSizeAssister + m_hitSize_VMM3a)) };

    /// Holds data common to all readouts in a packet
    ESSHeader m_ESSheader;


    int Parse_VMM3_SRS(uint32_t data1, uint16_t data2, int fecId);
    int Receive_VMM3_SRS(const char *buffer, long size, int fecId);
    uint32_t Gray2bin32(uint32_t num);

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

    /// Holds data common to all readouts in a packet
    CommonData_VMM3 m_commonData;

    static const int m_SRSHeaderSize_VMM3 { 16 };
    static const int m_hitAndMarkerSize_VMM3 { 6 };
    static const int m_data1Size { 4 };
    static const int m_JumboFrameSize_SRS { 9000 };
    static const int m_maxHits_VMM3 { static_cast<int>(m_data1Size / m_hitAndMarkerSize_VMM3) };

    int m_numHitsInFrame = 0;
    uint64_t m_numHits = 0;
    int m_bitCount=-1;
    int m_number_bits = 0;
    int m_theChannel = 0;
    int m_theVMM = 0;
    int m_theFEC = 0;
    int m_theDirection = 0;
    int m_scan_type = 0;
    int m_packetCounter = 0;


    bool m_isThresholdCalibration = false;
    double m_gainTable[8] = {0.5,1,3,4.5,6,9,12,16};
    double m_peaktimeTable[4] = {200,100,50,25};
    double m_tacTable[4] = {60,100,350,650};

    QString m_polarityTable[2] = {"negative", "positive"};

    int m_thresholdTable[8] = {243,243,243,243,243,243,243,243};
    int m_threshold=-1;
    //int m_minPulseHeightTable[8] = {949,482,165,120,94,60,49,39};
    //int m_maxPulseHeightTable[8] = {1023,1023,879,590,446,301,227,171};

    //lowest gain, use STH
    int m_minPulseHeightTable[8] = {100,50,103,53,44,26,24,20};
    //int m_minPulseHeightTable[8] = {100,50,165,120,85,60,49,39};
    int m_maxPulseHeightTable[8] = {600,300,879,590,460,301,227,171};
    const static int m_number_bits_adc = 32;
    const static int m_number_bits_tdc = 16;
    const static int m_number_bits_threshold = 32;
    const static int m_number_bits_offline_time = 10;
    const static int m_number_bits_offline_adc= 4;
    const static int m_number_bits_offline_timewalk = 10;
    const static int m_number_bits_offline_latency = 15;
    const static int m_start_reset_latency = 42;
    const static int m_start_tp_latency = 60;

    uint64_t m_srs_timestamp_end[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    uint64_t m_srs_timestamp_start[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    uint64_t m_start;
    uint64_t m_end;

    //double  m_bc_period[FECS_PER_DAQ][HYBRIDS_PER_FEC];
    //double  m_bc_clock[FECS_PER_DAQ][HYBRIDS_PER_FEC];
    double m_tac_slope[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    double m_shaping_time[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    double m_gain[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    int m_polarity[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    int m_timing_at_thr[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];


    //Data Acquisition
    //Data containers for data in Parse_VMM3
    std::vector<double> m_data[MAX_BITS][FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID][64];
    std::vector<double> m_data2[MAX_BITS][FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID][64];
    std::vector<double> m_adc_data[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_channel_data[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];

    //Container for time calibration
    unsigned long m_cnt_bcid[4096];
    std::vector<double> m_fit_start_time[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_fit_start_bcid[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_fit_y[MAX_BITS][FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_mean_per_bcid[NUM_BCID][MAX_BITS][FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_percent_bcid[NUM_BCID][MAX_BITS][FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    int m_reference_BCID = -1;

    //Container for ADC calibration
    std::vector<double> m_mean[MAX_BITS][FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_mean2[MAX_BITS][FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_dac;

    //Container for timewalk calibration
    std::vector<double> m_fit_function[MAX_BITS][FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];


    //Containers for calculated data
    //S-curve
    std::vector<double> m_max_value_x;
    std::vector<double> m_min_value_x;

    //Online ADC and TDC
    std::vector<double> m_calVal[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<int> m_bitVal[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];

    //Plot containers
    //Plots with channels on x-axis
    std::vector<double> m_x;

    //Plots with ADC on x-axis
    std::vector<double> m_adcs;

    //Plots with 1000 time bins on x-axis
    std::vector<double> m_times;

    //Plots with DAC or mV values on x-axis
    //std::vector<double> m_dac_x;
    std::vector<double> m_dac_setting;
    std::vector<double> m_dac_measured[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    int m_pulser_dac = 0;
    double m_pulser_mV = 0;
    int m_threshold_dac = 0;
    double m_threshold_mV = 0;
    double m_bcid_percentage = 0.0;
    
    std::vector<std::string> m_hybrid_labels;
    std::string m_hybrid_id[FECS_PER_DAQ][HYBRIDS_PER_FEC];
    double m_dac_slope[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    double m_dac_offset[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    int m_theIndex = 0;


    //y values for plots with channels or DAC values on x-axis
    std::vector<double> m_y[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];

    //y values for S-curve, the vector contains one value per threshold
    std::vector<double> m_channel_y[64];
    std::vector<double> m_time;
    std::vector<double> m_fit_error_scale;
    std::vector<double> m_fit_error_mean;
    std::vector<double> m_fit_error_sigma;
    std::vector<double> m_fit_scale;
    std::vector<double> m_fit_mean;
    std::vector<double> m_fit_sigma;

    //4PL
    std::vector<double>  m_fit_error_r2[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double>  m_fit_error_rms[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double>  m_fit_a[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double>  m_fit_b[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double>  m_fit_c[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double>  m_fit_d[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];

    //Fit for offline ADC and time calibration
    std::vector<double> m_offset[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_slope[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];

    std::vector<double> m_file_adc_offset[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_file_adc_slope[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_file_time_offset[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_file_time_slope[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_file_timewalk_a[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_file_timewalk_b[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_file_timewalk_c[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];
    std::vector<double> m_file_timewalk_d[FECS_PER_DAQ][HYBRIDS_PER_FEC][VMMS_PER_HYBRID];


    bool m_ignore16;

    std::vector<int> m_BCID;

    QJsonArray *m_calibrationArray[3] ;
    QString m_jsonObjectName = "vmm_calibration";
    std::ofstream m_outFile;
    QString m_configText = "";
    uint64_t m_total_channels=0;
};

#endif // CALIBRATION_MODULE_H
