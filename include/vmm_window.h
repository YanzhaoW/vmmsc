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
    explicit VMMWindow(HybridWindow *top, unsigned short fec = 0, unsigned short hybrid = 0, unsigned short vmm = 0, QWidget *parent = 0);
    ~VMMWindow();

    QFont Font;
    void CreateChannelsFields();
    void SetInitialState();
    void SetToolTips();

    /////////////////////////////////////////////////////
    // Channel Fields Buttons
    /////////////////////////////////////////////////////

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

    bool VMMSCBoolAll = false;
    bool VMMSLBoolAll = false;
    bool VMMSTBoolAll = false;
    bool VMMSTHBoolAll = false;
    bool VMMSMBoolAll = false;
    bool VMMSMXBoolAll = false;
    bool VMMSZ010bBoolAll = false;
    bool VMMSZ08bBoolAll = false;
    bool VMMSZ06bBoolAll = false;

private:
    HybridWindow *m_hybridWindow;
    unsigned short m_fecIndex;
    unsigned short m_hybridIndex;
    unsigned short m_vmmIndex;
    Ui::vmm_window *m_ui;
    unsigned short GetVMM(std::string feature, int ch=-9999);
    bool SetVMM(std::string feature, int val ,int ch=-9999);
    bool SetVMM(std::string feature, bool value, int ch=-9999);
    //    void changeDACtoMVs(int);
    void LoadSettings();
    void EnableCommunicationButtons(bool enable);
public slots:
    // channel fields
    void onUpdatePushButtons();
    void onUpdateComboBoxes(int);
    // VMM settings
    void onUpdateSettings();
    void onReloadSettings();
    void onVMMUpdateChannel();

private slots:
    void on_readADC_clicked();
};

#endif // VMM_WINDOW_H

#ifndef _HYBRID_WINDOW_HPP
#include "hybrid_window.h"
#endif
