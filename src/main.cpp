#include <QApplication>
#include "mainwindow.h"
#include <QStyleFactory>
#include <QtConcurrent/QtConcurrent>

int main(int argc, char *argv[])
{

    QFont Font;
    Font.setFamily("Arial");
    Font.setPixelSize(10);
    QApplication::setFont(Font);

    QApplication a(argc, argv);
    // make appearance same between linux and mac
    a.setStyle(QStyleFactory::create("Fusion"));

    MainWindow w;
    return a.exec();
}
