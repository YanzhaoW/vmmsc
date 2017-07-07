#include "vmm_window.h"

vmm_window::vmm_window(hybrid_window *top, unsigned short fec, unsigned short hdmi, unsigned short hybrid, unsigned short vmm, QWidget *parent) :
    QWidget(parent),
    root_hybrid{top},
    fec_index{fec},
    hdmi_index{hdmi},
    hybrid_index{hybrid},
    vmm_index{vmm},
    ui(new Ui::vmm_window)
{
    ui->setupUi(this);
}

vmm_window::~vmm_window()
{
    delete ui;
}
