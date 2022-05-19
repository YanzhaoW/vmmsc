#include <QApplication>
#include "daq_window.h"
#include <QStyleFactory>
#include <QtConcurrent/QtConcurrent>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QFont Font;
    Font.setFamily("Arial");
    Font.setPointSize(11);
    app.setFont(Font);
    app.setStyle(QStyleFactory::create("Fusion"));
    QApplication::setFont(Font);

    DAQWindow w;
    w.show();
    return app.exec();
}
