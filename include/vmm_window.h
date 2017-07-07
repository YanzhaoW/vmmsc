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

private:
    hybrid_window *root_hybrid;
    unsigned short fec_index;
    unsigned short hybrid_index;
    unsigned short hdmi_index;
    unsigned short vmm_index;
    Ui::vmm_window *ui;
};

#endif // VMM_WINDOW_H

#ifndef _HYBRID_WINDOW_HPP
#include "hybrid_window.h"
#endif
