#ifndef HDMI_WINDOW_H
#define HDMI_WINDOW_H

#include <QWidget>
#include <iostream>
#include "ui_hdmi_window.h"
#include "hybrid_window.h"

class fec_window;

namespace Ui {
class hdmi_window;
}

class hdmi_window : public QWidget
{
    Q_OBJECT
    friend class hybrid_window;

public:
    explicit hdmi_window(fec_window *top, unsigned short fec = 0, unsigned short hdmi = 0, QWidget *parent = 0);
    ~hdmi_window();

private slots:
    void on_Box_hybrid1_clicked();

private:
    fec_window *root_fec;
    unsigned short fec_index;
    unsigned short hdmi_index;
    Ui::hdmi_window *ui;
    void hybridBoxLogic(bool checked, unsigned short hybrid);
    void updateWindow();

};

#endif // HDMI_WINDOW_H

#ifndef _FEC_WINDOW_HPP
#include "fec_window.h"
#endif
