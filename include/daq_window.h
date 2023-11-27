#ifndef DAQ_WINDOW_H
#define DAQ_WINDOW_H

#include <QWidget>
#include <QPair>
#include "ui_daq_window.h"
#include "calibration_module.h"
#include "globparameter.h"
// vmm
#include "socket_handler.h"
#include "message_handler.h"
#include "daq_config.h"
#include "daq.h"

namespace Ui {
class daq_window;
}


class DAQWindow : public QMainWindow
{
    Q_OBJECT

    friend class DAQConfig;
    friend class CalibrationModule;

    DAQ m_daq;
public:

    explicit DAQWindow(QMainWindow *parent = 0);
    ~DAQWindow();

    void LoadConfig(QString text);
    void SaveConfig(QString text, bool addDate=false);
    bool FileExists(const char *fileName);

    DAQConfig *m_daqConfig;
    DAQWindow *m_daqWindow;
    QString GetApplicationPath();
    bool IsDbgActive() { return m_dbg; }
    void EnableDAQCommunicationButtons(bool enable);
    void SetStatus(QString text, int n);
    CalibrationModule *m_calib;

    //Calibration
    void InitCalibWidgets();

    //DAQ
    void InitDAQWidgets();
    void openConnection();
    void CheckLinkStatus(int fec, bool readHybridInfo);
    void UpdateSystemStatus();
    void MeasureVMMI2C();

    //FEC
    void InitFecWidgets();
    void EnableFECCommunicationButtons(bool enable);
    void LoadFECSettings();
    void SetFECToolTips();
    bool SetFec(const char *feature, unsigned long val);
    unsigned long GetFec(const char *feature);

    //Hybrid
    void InitHybridWidgets();
    void LoadHybridSettings();
    bool SetHybrid(std::string feature, unsigned short val);
    unsigned short GetHybrid(std::string feature);
    void EnableHybridCommunicationButtons(bool enable);

    // VMM Window
    void InitVMMWidgets();
    void InitVMMChannelWidgets();
    void LoadVMMSettings();
    void LoadVMMChannelSettings();
    void SetVMMToolTips();
    void SetVMMChannelToolTips();
    void EnableVMMCommunicationButtons(bool enable);
    unsigned short GetVMM(std::string feature, int ch=-9999);
    bool SetVMM(std::string feature, int val ,int ch=-9999);
    bool SetVMM(std::string feature, bool value, int ch=-9999);

    std::map<int,QPair<int,int>> m_map_id_ring_fen;
    std::map<QPair<int,int>,int> m_map_ring_fen_id;

    bool eventFilter(QObject *obj, QEvent *e);

private slots:
    //Calibration Window
    void onUpdateCalibSettings();

    //DAQ Window
    void onUpdateDAQSettings();
    void onSelectVMM_TableWidgets(int row, int column);

    // FEC Window
    void onUpdateFECSettings();

    // Hybrid Window
    void onUpdateHybridSettings();

    // VMM Window
    void onUpdateVMMSettings();
    void onUpdateVMMChannelSettings();

private:
    Ui::daq_window *m_ui;
    bool m_config_date = false;
    int m_config_date_mode = 0;
    bool m_dbg = false;

    QString m_execPath;

    int m_fecIndex = 0;
    int m_hybridIndex = 0;
    int m_vmmIndex = 0;



};

#endif // DAQ_WINDOW_H

