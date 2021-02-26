#include <QApplication>
#include "daq_window.h"
#include <QStyleFactory>
#include <QtConcurrent/QtConcurrent>

int main(int argc, char *argv[])
{

    QFont Font;
    Font.setFamily("Arial");
    Font.setPixelSize(12);
    QApplication::setFont(Font);

    QApplication a(argc, argv);
    // make appearance same between linux and mac
    a.setStyle(QStyleFactory::create("Fusion"));

    DAQWindow w;
    w.show();
    return a.exec();
}
