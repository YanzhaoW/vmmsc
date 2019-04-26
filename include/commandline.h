#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <QObject>
#include <iostream>
#include "vmm.h"

class MainWindow;

class Commandline: public QObject
{
    Q_OBJECT
    friend class MainWindow;
    friend class VMM;
    friend class VMMConfigHandler;

    public:
        Commandline(MainWindow *top, int value, QObject* parent = 0);
        ~Commandline();
        int StartCommandline();
        int EndCommandline();

    private:
    MainWindow *m_mainWindow;
    int m_index;
    bool m_running;


};

#endif // COMMANDLINE_H

#ifndef _MAINWINDOW_HPP
#include "mainwindow.h"
#endif
