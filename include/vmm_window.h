#ifndef VMM_WINDOW_H
#define VMM_WINDOW_H

#include <QWidget>
#include "ui_vmm_window.h"

class hybrid_window;

namespace Ui {
class vmm_window;
}

class vmm_window : public QWidget
{
    Q_OBJECT

public:
    explicit vmm_window(hybrid_window *top, unsigned short fec = 0, unsigned short hdmi = 0, unsigned short hybrid = 0, unsigned short vmm = 0, QWidget *parent = 0);
    ~vmm_window();


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


private:
    hybrid_window *root_hybrid;
    unsigned short fec_index;
    unsigned short hybrid_index;
    unsigned short hdmi_index;
    unsigned short vmm_index;
    Ui::vmm_window *ui;
    unsigned short VMM_Get(std::string feature, int ch=-9999);
    bool VMM_Set(std::string feature, int val ,int ch=-9999);
    bool VMM_Set(std::string feature, std::string value, int ch=-9999);
//    void changeDACtoMVs(int);
    void LoadSettings();
public slots:
    // channel fields
    void updateChannelState();
    void updateChannelVoltages(int);
    void updateChannelADCs(int);
    // VMM settings
     void updateSettings();

};

#endif // VMM_WINDOW_H

#ifndef _HYBRID_WINDOW_HPP
#include "hybrid_window.h"
#endif
