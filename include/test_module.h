#ifndef TEST_MODULE_H
#define TEST_MODULE_H

#include <QObject>
//#include <QString>
#include <qcustomplot.h>
//#include <QSerialPort>
//#include <QSerialPortInfo>
#include "globparameter.h"
#include "message_handler.h"
#include <set>
#include "currentmonitor.h"
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

using namespace std;
class DAQWindow;
class CalibrationModule;
class CurrentMonitor;
class TestModule: public QObject
{
    Q_OBJECT
public:
    explicit TestModule(DAQWindow *top, QObject *parent = nullptr);
    bool IsDbg() { return m_dbg; }
    void LoadCalibrationModule(CalibrationModule& calibmod);
    MessageHandler& GetMessageHandler(){ return *m_msg; }
    void StartTest();
    void PrintMData();

    struct HybridResults{
        int h_firmware=0;
        double h_current_1_9V = 0; //current of the 1.9V power supply
        double h_current_2_9V = 0; //current of the 2.9V power supply
        QHash<QString, QHash<QString, QVector<double>>> h_plotsdata;
        QHash<QString, QHash<QString, QVector<double>>> h_histodata;
        QHash<QString,QHash<QString, QVector<double>>> h_chandata;
        QHash<QString, QHash<QString, QString>> h_plotslabel;
        QHash<QString, QString> h_DBExportStrings[2];
        QString h_globalHybridID = "0";
        QString h_measurementID = "";
        bool h_monitoringADC[2] = {1,1};
        double h_monitoringADCcal[2][2];
        //int h_ChannelResults[2][2][64];
        QHash<QString,int> h_ChannelResults[2][64];
        QHash<QString,int> h_VMMResults[2];
        int h_nwchannels[2][2] = {{64,64},{64,64}};
        std::string log = "";
        bool pedestalProblem[2] = {false,false};
    };
    HybridResults m_hResults;
    void PlotData(QVector<double> x, QVector<double> y[], QString xlabel, QString ylabel, QString name="", QString graphlabel="VMM",int datalen = 2);
    void PlotHistogram(QVector<double> data[], double xlow, double xhigh, int nbins, QString xlabel, QString ylabel, QString name, QString graphlabel);
    void PlotChannels(QVector<double> x, QVector<double> y[][64], QString xlabel, QString ylabel, QString name="", QString graphlabel="Ch", int datalen = 64);
    void AddFitToPlot(QVector<double> x, QVector<double> y[], QString graphlabel = "Fit", int datalen = 2);
    bool DeleteLastMeasurement();
    bool ResetHybrid();
    void enableSbip(bool ena);
    QVector<double> evaluateADCCalibrationFit(QVector<double> datax , QVector<double> datay, QString caller, int chip, QVector<double> &params, stringstream *sx);

private:
    CalibrationModule *m_calibmod;
    DAQWindow *m_daqWindow;
    bool m_dbg = true;
    MessageHandler *m_msg;
    bool m_connected = false;
    bool m_acqon=false;
    bool m_currmon = false;
    bool m_firmwareflash = false;
    bool m_forceflash = false;
    bool m_extpulse = false;
    bool m_tempoff = false;
    bool m_neighbor = false;
    bool m_terminator = false;
    bool m_sbip = false;
    bool m_exportblock = false;
    double warntemp;
    int m_FEC_test = -1;
    int m_hybrid_test = -1;
    QVector<QPair<QDateTime,double>> m_currents;
    QVector<QPair<QDateTime,double>> m_temps[2];
    QStringList m_criticaltests = {"Internal","External","Pedestal","ADCCalibrationExternal","ADCCalibrationInternal"};
    std::vector<std::pair<int, std::vector<bool>>> m_activeHybridonFEC;   //Variable that stores which Hybrids are activated (1) on which FEC
    QVector<QCustomPlot*> plotVector;
    CurrentMonitor *m_currentMonitor = nullptr;
    QStringList m_performedTest;
    QTimer m_temptimer;
    QVector<QLabel*> m_labelstoclear;
    QSqlDatabase m_db;
    void GetOptionalSettings();
    void ResetDataStructures();
    int GetTestMode();
    bool ConnectToSRS();
    bool CheckAndProcessLinkStatus();
    void PrintActiveHybridonFEC();
    bool loadHybridFirmware();
    bool CheckFirmware();
    void SetHybridtoTest();
    double ReadTemperature(int vmmnr);
    QString GetHybridID();
    std::string TestPedestal();
    std::string TestBaselineWidth(int nruns, int gain, bool& bad, bool repeat=false);
    std::string TestThreshold();
    std::string TestMonitoringADC();
    std::string TestNeighbouring();
    std::string TestThrTrimmability();
    //bool TestShortCircuit();
    void evaluateResults(std::string results);
    std::set<int> findOutliers(QVector<double> input, double thr);
    QString dataToSqlArray(QVector<double> vec,int len=0);
    QString dataToSqlArray(QVector<int> vec, int len=0);
    QString dataToSqlArray(QVector<double> vec[], int len);
    QString labelsToSqlArray(QHash<QString,QString> labels);
    std::string TestChannelsExternal(int tries=0, int restarts=0);
    std::string TestChannelsInternal();
    void MarkChannel(QString whichtest, int vmm, int chno, int status);
    void MarkChannel(QString whichtest, int vmm, int chno, QString status);
    double getAverage(QVector<double> data);
    double getStdDev(QVector<double> data);
    double getMedian(QVector<double> data);
    double evaluateADCCalibration(QVector<double> dataset, QString caller);
    bool MaskNoisyChannels();
    void exportConfig(QString filename);
    void FitLinear(QVector<double> x, QVector<double> y,double& slope,double& intercept);
    void readSettingFile();
    QString readSettingsDB();
    void getCurveSettings(QString caller, double &ideal, double &range, double &idealH, double &rangeH);
    bool updateGitRepo(QString workingpath, QString measid);
signals:

private slots:
    void on_readTestLog();
    void acqON();
    void acqOFF();
    void handleTemperatures(bool end=false);
    bool SetTermination();

public slots:
    void handleCurrents();
    bool exportResults();
};

#endif // TEST_MODULE_H

#ifndef _DAQWINDOW_HPP
#include "daq_window.h"
#endif
#ifndef _CURRENTMONITOR_H
#include "currentmonitor.h"
#endif

