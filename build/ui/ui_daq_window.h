/********************************************************************************
** Form generated from reading UI file 'daq_window.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DAQ_WINDOW_H
#define UI_DAQ_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_daq_window
{
public:
    QWidget *centralwidget;
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QCheckBox *Box_fec1;
    QCheckBox *Box_fec2;
    QCheckBox *Box_fec3;
    QCheckBox *Box_fec4;
    QCheckBox *Box_fec5;
    QCheckBox *Box_fec6;
    QCheckBox *Box_fec7;
    QCheckBox *Box_fec8;
    QPushButton *Button_load;
    QPushButton *Button_save;
    QLineEdit *line_configFile;
    QLabel *label_configFile;
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *daq_window)
    {
        if (daq_window->objectName().isEmpty())
            daq_window->setObjectName(QStringLiteral("daq_window"));
        daq_window->resize(1345, 917);
        centralwidget = new QWidget(daq_window);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        horizontalLayout = new QHBoxLayout(centralwidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(100, 600));
        Box_fec1 = new QCheckBox(groupBox);
        Box_fec1->setObjectName(QStringLiteral("Box_fec1"));
        Box_fec1->setGeometry(QRect(12, 29, 33, 20));
        Box_fec2 = new QCheckBox(groupBox);
        Box_fec2->setObjectName(QStringLiteral("Box_fec2"));
        Box_fec2->setGeometry(QRect(12, 55, 33, 20));
        Box_fec3 = new QCheckBox(groupBox);
        Box_fec3->setObjectName(QStringLiteral("Box_fec3"));
        Box_fec3->setGeometry(QRect(12, 81, 33, 20));
        Box_fec4 = new QCheckBox(groupBox);
        Box_fec4->setObjectName(QStringLiteral("Box_fec4"));
        Box_fec4->setGeometry(QRect(12, 107, 33, 20));
        Box_fec5 = new QCheckBox(groupBox);
        Box_fec5->setObjectName(QStringLiteral("Box_fec5"));
        Box_fec5->setGeometry(QRect(12, 133, 33, 20));
        Box_fec6 = new QCheckBox(groupBox);
        Box_fec6->setObjectName(QStringLiteral("Box_fec6"));
        Box_fec6->setGeometry(QRect(12, 159, 33, 20));
        Box_fec7 = new QCheckBox(groupBox);
        Box_fec7->setObjectName(QStringLiteral("Box_fec7"));
        Box_fec7->setGeometry(QRect(12, 185, 33, 20));
        Box_fec8 = new QCheckBox(groupBox);
        Box_fec8->setObjectName(QStringLiteral("Box_fec8"));
        Box_fec8->setGeometry(QRect(12, 211, 33, 20));
        Button_load = new QPushButton(groupBox);
        Button_load->setObjectName(QStringLiteral("Button_load"));
        Button_load->setGeometry(QRect(12, 479, 80, 22));
        Button_save = new QPushButton(groupBox);
        Button_save->setObjectName(QStringLiteral("Button_save"));
        Button_save->setGeometry(QRect(12, 507, 80, 22));
        line_configFile = new QLineEdit(groupBox);
        line_configFile->setObjectName(QStringLiteral("line_configFile"));
        line_configFile->setGeometry(QRect(12, 451, 81, 22));
        label_configFile = new QLabel(groupBox);
        label_configFile->setObjectName(QStringLiteral("label_configFile"));
        label_configFile->setGeometry(QRect(20, 430, 61, 16));

        horizontalLayout->addWidget(groupBox);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        verticalLayout->addWidget(tabWidget);


        horizontalLayout->addLayout(verticalLayout);

        daq_window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(daq_window);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1345, 19));
        daq_window->setMenuBar(menubar);
        statusbar = new QStatusBar(daq_window);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        daq_window->setStatusBar(statusbar);

        retranslateUi(daq_window);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(daq_window);
    } // setupUi

    void retranslateUi(QMainWindow *daq_window)
    {
        daq_window->setWindowTitle(QApplication::translate("daq_window", "MainWindow", 0));
        groupBox->setTitle(QApplication::translate("daq_window", "FEC", 0));
        Box_fec1->setText(QApplication::translate("daq_window", "1", 0));
        Box_fec2->setText(QApplication::translate("daq_window", "2", 0));
        Box_fec3->setText(QApplication::translate("daq_window", "3", 0));
        Box_fec4->setText(QApplication::translate("daq_window", "4", 0));
        Box_fec5->setText(QApplication::translate("daq_window", "5", 0));
        Box_fec6->setText(QApplication::translate("daq_window", "6", 0));
        Box_fec7->setText(QApplication::translate("daq_window", "7", 0));
        Box_fec8->setText(QApplication::translate("daq_window", "8", 0));
        Button_load->setText(QApplication::translate("daq_window", "Load", 0));
        Button_save->setText(QApplication::translate("daq_window", "Save", 0));
        label_configFile->setText(QApplication::translate("daq_window", "Config file", 0));
    } // retranslateUi

};

namespace Ui {
    class daq_window: public Ui_daq_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAQ_WINDOW_H
