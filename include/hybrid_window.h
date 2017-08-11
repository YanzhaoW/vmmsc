#ifndef HYBRID_WINDOW_H
#define HYBRID_WINDOW_H

#include <QWidget>
#include <iostream>
#include "ui_hybrid_window.h"
#include "vmm_window.h"

class hdmi_window;

namespace Ui {
class hybrid_window;
}

class hybrid_window : public QWidget
{
    Q_OBJECT

public:
    explicit hybrid_window(hdmi_window *top, unsigned short fec = 0, unsigned short hdmi = 0, unsigned short hybrid = 0, QWidget *parent = 0);
    ~hybrid_window();
    friend class vmm_window;

private slots:
    void on_Box_vmm1_clicked();
    void on_Box_vmm2_clicked();
private:
    hdmi_window *root_hdmi;
    unsigned short fec_index;
    unsigned short hdmi_index;
    unsigned short hybrid_index;
    Ui::hybrid_window *ui;
    void vmmBoxLogic(bool checked, unsigned short vmm);
    void updateWindow();
};

#endif // HYBRID_WINDOW_H

#ifndef _HDMI_WINDOW_HPP
#include "hdmi_window.h"
#endif
