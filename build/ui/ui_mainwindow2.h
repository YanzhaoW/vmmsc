/********************************************************************************
** Form generated from reading UI file 'mainwindow2.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW2_H
#define UI_MAINWINDOW2_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow2
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout_2;
    QCheckBox *Box_fec1;
    QCheckBox *Box_fec2;
    QCheckBox *Box_fec3;
    QCheckBox *Box_fec4;
    QCheckBox *Box_fec5;
    QCheckBox *Box_fec6;
    QCheckBox *Box_fec7;
    QCheckBox *Box_fec8;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow2)
    {
        if (MainWindow2->objectName().isEmpty())
            MainWindow2->setObjectName(QStringLiteral("MainWindow2"));
        MainWindow2->resize(800, 600);
        centralwidget = new QWidget(MainWindow2);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout_2 = new QVBoxLayout(groupBox);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        Box_fec1 = new QCheckBox(groupBox);
        Box_fec1->setObjectName(QStringLiteral("Box_fec1"));

        verticalLayout_2->addWidget(Box_fec1);

        Box_fec2 = new QCheckBox(groupBox);
        Box_fec2->setObjectName(QStringLiteral("Box_fec2"));

        verticalLayout_2->addWidget(Box_fec2);

        Box_fec3 = new QCheckBox(groupBox);
        Box_fec3->setObjectName(QStringLiteral("Box_fec3"));

        verticalLayout_2->addWidget(Box_fec3);

        Box_fec4 = new QCheckBox(groupBox);
        Box_fec4->setObjectName(QStringLiteral("Box_fec4"));

        verticalLayout_2->addWidget(Box_fec4);

        Box_fec5 = new QCheckBox(groupBox);
        Box_fec5->setObjectName(QStringLiteral("Box_fec5"));

        verticalLayout_2->addWidget(Box_fec5);

        Box_fec6 = new QCheckBox(groupBox);
        Box_fec6->setObjectName(QStringLiteral("Box_fec6"));

        verticalLayout_2->addWidget(Box_fec6);

        Box_fec7 = new QCheckBox(groupBox);
        Box_fec7->setObjectName(QStringLiteral("Box_fec7"));

        verticalLayout_2->addWidget(Box_fec7);

        Box_fec8 = new QCheckBox(groupBox);
        Box_fec8->setObjectName(QStringLiteral("Box_fec8"));

        verticalLayout_2->addWidget(Box_fec8);


        horizontalLayout->addWidget(groupBox);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        verticalLayout->addWidget(tabWidget);


        horizontalLayout->addLayout(verticalLayout);

        MainWindow2->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow2);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 19));
        MainWindow2->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow2);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        MainWindow2->setStatusBar(statusbar);

        retranslateUi(MainWindow2);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(MainWindow2);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow2)
    {
        MainWindow2->setWindowTitle(QApplication::translate("MainWindow2", "MainWindow", 0));
        groupBox->setTitle(QApplication::translate("MainWindow2", "FEC", 0));
        Box_fec1->setText(QApplication::translate("MainWindow2", "1", 0));
        Box_fec2->setText(QApplication::translate("MainWindow2", "2", 0));
        Box_fec3->setText(QApplication::translate("MainWindow2", "3", 0));
        Box_fec4->setText(QApplication::translate("MainWindow2", "4", 0));
        Box_fec5->setText(QApplication::translate("MainWindow2", "5", 0));
        Box_fec6->setText(QApplication::translate("MainWindow2", "6", 0));
        Box_fec7->setText(QApplication::translate("MainWindow2", "7", 0));
        Box_fec8->setText(QApplication::translate("MainWindow2", "8", 0));
    } // retranslateUi

};

namespace Ui {
    class MainWindow2: public Ui_MainWindow2 {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW2_H
