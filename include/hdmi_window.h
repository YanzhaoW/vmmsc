#ifndef HDMI_WINDOW_H
#define HDMI_WINDOW_H

#include <QWidget>
#include <iostream>
#include "ui_hdmi_window.h"
#include "hybrid_window.h"

class FECWindow;

namespace Ui {
class hdmi_window;
}

class HDMIWindow : public QWidget
{
    Q_OBJECT
    friend class HybridWindow;
    friend class VMMWindow;
    friend class FECWindow;

public:
    explicit HDMIWindow(FECWindow *top, unsigned short fec = 0, unsigned short hdmi = 0, QWidget *parent = 0);
    ~HDMIWindow();

private slots:
    void on_Box_hybrid1_clicked();

private:
    FECWindow *m_fecWindow;
    unsigned short m_fecIndex;
    unsigned short m_hdmiIndex;
    Ui::hdmi_window *m_ui;
    void HybridBoxLogic(bool checked, unsigned short hybrid);
    void UpdateWindow();

};

#endif // HDMI_WINDOW_H

#ifndef _FEC_WINDOW_HPP
#include "fec_window.h"
#endif
