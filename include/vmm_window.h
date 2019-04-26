#ifndef VMM_WINDOW_H
#define VMM_WINDOW_H

#include <QWidget>
#include "ui_vmm_window.h"

class HybridWindow;

namespace Ui {
class vmm_window;
}

class VMMWindow : public QWidget
{
    Q_OBJECT

public:
    explicit VMMWindow(HybridWindow *top, unsigned short fec = 0, unsigned short hdmi = 0, unsigned short hybrid = 0, unsigned short vmm = 0, QWidget *parent = 0);
    ~VMMWindow();


    QFont Font;
    QGridLayout *channelGridLayout;
    QWidget* dummy;
    void CreateChannelsFields();
    void SetInitialState();
    void SetToolTips();

    /////////////////////////////////////////////////////
    // Channel Fields Buttons
    /////////////////////////////////////////////////////
    QLineEdit *VMMChannel[64];
    QComboBox *VMMSDVoltage[64];
    QComboBox *VMMSZ010bCBox[64];
    QComboBox *VMMSZ08bCBox[64];
    QComboBox *VMMSZ06bCBox[64];

    QPushButton *VMMSC[64];
    QPushButton *VMMSL[64];
    QPushButton *VMMST[64];
    QPushButton *VMMSTH[64];
    QPushButton *VMMSM[64];
    QPushButton *VMMSMX[64];


    QPushButton *SCLabel;
    QPushButton *SLLabel;
    QPushButton *STLabel;
    QPushButton *STHLabel;
    QPushButton *SMLabel;
    QComboBox *SDLabel;
    QComboBox *SZ010bLabel;
    QComboBox *SZ08bLabel;
    QComboBox *SZ06bLabel;
    QPushButton *SMXLabel;


    bool VMMSCBool[64];
    bool VMMSLBool[64];
    bool VMMSTBool[64];
    bool VMMSTHBool[64];
    bool VMMSMBool[64];
    bool VMMSMXBool[64];
    quint8 VMMSDValue[64];
    quint8 VMMSZ010bValue[64];
    quint8 VMMSZ08bValue[64];
    quint8 VMMSZ06bValue[64];

    bool VMMSCBoolAll;
    bool VMMSLBoolAll;
    bool VMMSTBoolAll;
    bool VMMSTHBoolAll;
    bool VMMSMBoolAll;
    bool VMMSMXBoolAll;
    bool VMMSZ010bBoolAll;
    bool VMMSZ08bBoolAll;
    bool VMMSZ06bBoolAll;

    bool eventFilter(QObject *obj, QEvent *event);


private:
    HybridWindow *m_hybridWindow;
    unsigned short m_fecIndex;
    unsigned short m_hdmiIndex;
    unsigned short m_hybridIndex;
    unsigned short m_vmmIndex;
    Ui::vmm_window *m_ui;
    unsigned short GetVMM(std::string feature, int ch=-9999);
    bool SetVMM(std::string feature, int val ,int ch=-9999);
    bool SetVMM(std::string feature, std::string value, int ch=-9999);
    //    void changeDACtoMVs(int);
    void LoadSettings();
public slots:
    // channel fields
    void onUpdateChannelState();
    void onUpdateChannelVoltages(int);
    void onUpdateChannelADCs(int);
    // VMM settings
    void onUpdateSettings();
    void onReloadSettings();

private slots:
    void on_vmmReset_clicked();
    void on_readADC_clicked();
};

#endif // VMM_WINDOW_H

#ifndef _HYBRID_WINDOW_HPP
#include "hybrid_window.h"
#endif
