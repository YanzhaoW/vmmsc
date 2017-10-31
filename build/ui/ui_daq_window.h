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
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_daq_window
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_6;
    QPushButton *Button_save;
    QPushButton *Button_load;
    QLineEdit *line_configFile;
    QPushButton *selectDir;
    QTabWidget *tabWidget_2;
    QWidget *tab;
    QGridLayout *gridLayout_5;
    QTabWidget *tabWidget;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_12;
    QCheckBox *checkBox;
    QPushButton *trgPulser;
    QPushButton *offACQ;
    QPushButton *onACQ;
    QPushButton *trgExternal;
    QWidget *widget_2;
    QGridLayout *gridLayout_8;
    QLabel *connectionLabel_3;
    QPushButton *Send;
    QPushButton *openConnection_2;
    QSpacerItem *verticalSpacer;
    QLabel *connectionLabel_2;
    QPushButton *reset_warnings;
    QGroupBox *Fec_group_box;
    QGridLayout *gridLayout_3;
    QWidget *widget_3;
    QGridLayout *gridLayout_7;
    QCheckBox *Box_fec1;
    QCheckBox *Box_fec2;
    QCheckBox *Box_fec3;
    QCheckBox *Box_fec4;
    QCheckBox *Box_fec5;
    QCheckBox *Box_fec6;
    QCheckBox *Box_fec7;
    QCheckBox *Box_fec8;
    QWidget *widget;
    QWidget *tab_2;
    QGridLayout *gridLayout_9;
    QGroupBox *Calib_Settings;
    QGridLayout *gridLayout_4;
    QComboBox *VMM_select;
    QLabel *label_vmm;
    QTextEdit *InfoScreen;
    QLabel *label_run;
    QLabel *label_type;
    QSpinBox *Runs;
    QComboBox *Calib_type;
    QPushButton *Data;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QCustomPlot *customPlot;
    QSpacerItem *horizontalSpacer_3;
    QWidget *tab_3;
    QGridLayout *gridLayout_10;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_11;
    QTextEdit *loggingScreen;
    QPushButton *Debug;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *daq_window)
    {
        if (daq_window->objectName().isEmpty())
            daq_window->setObjectName(QStringLiteral("daq_window"));
        daq_window->resize(1500, 1100);
        daq_window->setMinimumSize(QSize(1500, 1100));
        daq_window->setMaximumSize(QSize(999999, 999999));
        centralwidget = new QWidget(daq_window);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setMaximumSize(QSize(400, 70));
        gridLayout_6 = new QGridLayout(groupBox_3);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        Button_save = new QPushButton(groupBox_3);
        Button_save->setObjectName(QStringLiteral("Button_save"));

        gridLayout_6->addWidget(Button_save, 1, 4, 1, 1);

        Button_load = new QPushButton(groupBox_3);
        Button_load->setObjectName(QStringLiteral("Button_load"));

        gridLayout_6->addWidget(Button_load, 1, 3, 1, 1);

        line_configFile = new QLineEdit(groupBox_3);
        line_configFile->setObjectName(QStringLiteral("line_configFile"));

        gridLayout_6->addWidget(line_configFile, 1, 1, 1, 1);

        selectDir = new QPushButton(groupBox_3);
        selectDir->setObjectName(QStringLiteral("selectDir"));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/directory.png"), QSize(), QIcon::Normal, QIcon::Off);
        selectDir->setIcon(icon);
        selectDir->setIconSize(QSize(18, 20));

        gridLayout_6->addWidget(selectDir, 1, 2, 1, 1);


        gridLayout_2->addWidget(groupBox_3, 2, 1, 1, 1);

        tabWidget_2 = new QTabWidget(centralwidget);
        tabWidget_2->setObjectName(QStringLiteral("tabWidget_2"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_5 = new QGridLayout(tab);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        tabWidget = new QTabWidget(tab);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        gridLayout_5->addWidget(tabWidget, 1, 2, 1, 1);

        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(100, 600));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox_6 = new QGroupBox(groupBox);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        gridLayout_12 = new QGridLayout(groupBox_6);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        checkBox = new QCheckBox(groupBox_6);
        checkBox->setObjectName(QStringLiteral("checkBox"));

        gridLayout_12->addWidget(checkBox, 0, 0, 1, 1);

        trgPulser = new QPushButton(groupBox_6);
        trgPulser->setObjectName(QStringLiteral("trgPulser"));
        QFont font;
        font.setPointSize(9);
        trgPulser->setFont(font);
        trgPulser->setFocusPolicy(Qt::NoFocus);
        trgPulser->setCheckable(false);

        gridLayout_12->addWidget(trgPulser, 1, 0, 1, 2);

        offACQ = new QPushButton(groupBox_6);
        offACQ->setObjectName(QStringLiteral("offACQ"));
        offACQ->setFont(font);
        offACQ->setFocusPolicy(Qt::NoFocus);
        offACQ->setCheckable(false);

        gridLayout_12->addWidget(offACQ, 4, 0, 1, 2);

        onACQ = new QPushButton(groupBox_6);
        onACQ->setObjectName(QStringLiteral("onACQ"));
        onACQ->setFont(font);
        onACQ->setFocusPolicy(Qt::NoFocus);
        onACQ->setCheckable(false);

        gridLayout_12->addWidget(onACQ, 3, 0, 1, 2);

        trgExternal = new QPushButton(groupBox_6);
        trgExternal->setObjectName(QStringLiteral("trgExternal"));
        trgExternal->setFont(font);
        trgExternal->setFocusPolicy(Qt::NoFocus);
        trgExternal->setAutoFillBackground(false);
        trgExternal->setCheckable(false);
        trgExternal->setAutoDefault(false);

        gridLayout_12->addWidget(trgExternal, 2, 0, 1, 2);


        gridLayout->addWidget(groupBox_6, 2, 0, 1, 1);

        widget_2 = new QWidget(groupBox);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setMaximumSize(QSize(16777215, 155));
        gridLayout_8 = new QGridLayout(widget_2);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        connectionLabel_3 = new QLabel(widget_2);
        connectionLabel_3->setObjectName(QStringLiteral("connectionLabel_3"));
        connectionLabel_3->setMinimumSize(QSize(0, 30));
        connectionLabel_3->setMaximumSize(QSize(16777215, 22));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(9);
        connectionLabel_3->setFont(font1);

        gridLayout_8->addWidget(connectionLabel_3, 3, 0, 1, 1);

        Send = new QPushButton(widget_2);
        Send->setObjectName(QStringLiteral("Send"));

        gridLayout_8->addWidget(Send, 2, 0, 1, 1);

        openConnection_2 = new QPushButton(widget_2);
        openConnection_2->setObjectName(QStringLiteral("openConnection_2"));
        openConnection_2->setMinimumSize(QSize(0, 22));
        QFont font2;
        font2.setFamily(QStringLiteral("Arial"));
        font2.setPointSize(8);
        openConnection_2->setFont(font2);

        gridLayout_8->addWidget(openConnection_2, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer, 5, 0, 2, 1);

        connectionLabel_2 = new QLabel(widget_2);
        connectionLabel_2->setObjectName(QStringLiteral("connectionLabel_2"));
        connectionLabel_2->setMinimumSize(QSize(0, 20));
        connectionLabel_2->setMaximumSize(QSize(16777215, 16777215));
        connectionLabel_2->setFont(font1);

        gridLayout_8->addWidget(connectionLabel_2, 1, 0, 1, 1);

        reset_warnings = new QPushButton(widget_2);
        reset_warnings->setObjectName(QStringLiteral("reset_warnings"));

        gridLayout_8->addWidget(reset_warnings, 4, 0, 1, 1);


        gridLayout->addWidget(widget_2, 0, 0, 1, 1);

        Fec_group_box = new QGroupBox(groupBox);
        Fec_group_box->setObjectName(QStringLiteral("Fec_group_box"));
        gridLayout_3 = new QGridLayout(Fec_group_box);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        widget_3 = new QWidget(Fec_group_box);
        widget_3->setObjectName(QStringLiteral("widget_3"));
        gridLayout_7 = new QGridLayout(widget_3);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        Box_fec1 = new QCheckBox(widget_3);
        Box_fec1->setObjectName(QStringLiteral("Box_fec1"));

        gridLayout_7->addWidget(Box_fec1, 0, 0, 1, 1);

        Box_fec2 = new QCheckBox(widget_3);
        Box_fec2->setObjectName(QStringLiteral("Box_fec2"));

        gridLayout_7->addWidget(Box_fec2, 1, 0, 1, 1);

        Box_fec3 = new QCheckBox(widget_3);
        Box_fec3->setObjectName(QStringLiteral("Box_fec3"));

        gridLayout_7->addWidget(Box_fec3, 2, 0, 1, 1);

        Box_fec4 = new QCheckBox(widget_3);
        Box_fec4->setObjectName(QStringLiteral("Box_fec4"));

        gridLayout_7->addWidget(Box_fec4, 3, 0, 1, 1);

        Box_fec5 = new QCheckBox(widget_3);
        Box_fec5->setObjectName(QStringLiteral("Box_fec5"));

        gridLayout_7->addWidget(Box_fec5, 4, 0, 1, 1);

        Box_fec6 = new QCheckBox(widget_3);
        Box_fec6->setObjectName(QStringLiteral("Box_fec6"));

        gridLayout_7->addWidget(Box_fec6, 5, 0, 1, 1);

        Box_fec7 = new QCheckBox(widget_3);
        Box_fec7->setObjectName(QStringLiteral("Box_fec7"));

        gridLayout_7->addWidget(Box_fec7, 6, 0, 1, 1);

        Box_fec8 = new QCheckBox(widget_3);
        Box_fec8->setObjectName(QStringLiteral("Box_fec8"));

        gridLayout_7->addWidget(Box_fec8, 7, 0, 1, 1);


        gridLayout_3->addWidget(widget_3, 0, 0, 1, 1);


        gridLayout->addWidget(Fec_group_box, 1, 0, 1, 1);


        gridLayout_5->addWidget(groupBox, 1, 1, 1, 1);

        widget = new QWidget(tab);
        widget->setObjectName(QStringLiteral("widget"));

        gridLayout_5->addWidget(widget, 0, 1, 1, 1);

        tabWidget_2->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout_9 = new QGridLayout(tab_2);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        Calib_Settings = new QGroupBox(tab_2);
        Calib_Settings->setObjectName(QStringLiteral("Calib_Settings"));
        gridLayout_4 = new QGridLayout(Calib_Settings);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        VMM_select = new QComboBox(Calib_Settings);
        VMM_select->setObjectName(QStringLiteral("VMM_select"));

        gridLayout_4->addWidget(VMM_select, 5, 0, 1, 1);

        label_vmm = new QLabel(Calib_Settings);
        label_vmm->setObjectName(QStringLiteral("label_vmm"));

        gridLayout_4->addWidget(label_vmm, 4, 0, 1, 1);

        InfoScreen = new QTextEdit(Calib_Settings);
        InfoScreen->setObjectName(QStringLiteral("InfoScreen"));
        InfoScreen->setFont(font);

        gridLayout_4->addWidget(InfoScreen, 0, 1, 6, 1);

        label_run = new QLabel(Calib_Settings);
        label_run->setObjectName(QStringLiteral("label_run"));

        gridLayout_4->addWidget(label_run, 2, 0, 1, 1);

        label_type = new QLabel(Calib_Settings);
        label_type->setObjectName(QStringLiteral("label_type"));

        gridLayout_4->addWidget(label_type, 0, 0, 1, 1);

        Runs = new QSpinBox(Calib_Settings);
        Runs->setObjectName(QStringLiteral("Runs"));
        Runs->setFont(font1);
        Runs->setMinimum(1);
        Runs->setMaximum(10000);
        Runs->setSingleStep(1);
        Runs->setValue(10);

        gridLayout_4->addWidget(Runs, 3, 0, 1, 1);

        Calib_type = new QComboBox(Calib_Settings);
        Calib_type->setObjectName(QStringLiteral("Calib_type"));

        gridLayout_4->addWidget(Calib_type, 1, 0, 1, 1);


        gridLayout_9->addWidget(Calib_Settings, 0, 2, 1, 1);

        Data = new QPushButton(tab_2);
        Data->setObjectName(QStringLiteral("Data"));
        Data->setMinimumSize(QSize(170, 0));
        Data->setCheckable(true);

        gridLayout_9->addWidget(Data, 0, 0, 1, 1);

        horizontalSpacer = new QSpacerItem(1500, 40, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer, 0, 4, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(70, 20, QSizePolicy::Maximum, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer_2, 0, 1, 1, 1);

        customPlot = new QCustomPlot(tab_2);
        customPlot->setObjectName(QStringLiteral("customPlot"));
        customPlot->setMinimumSize(QSize(1400, 720));

        gridLayout_9->addWidget(customPlot, 1, 0, 1, 3);

        horizontalSpacer_3 = new QSpacerItem(700, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

        gridLayout_9->addItem(horizontalSpacer_3, 0, 3, 1, 1);

        tabWidget_2->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_10 = new QGridLayout(tab_3);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        groupBox_5 = new QGroupBox(tab_3);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_11 = new QGridLayout(groupBox_5);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        loggingScreen = new QTextEdit(groupBox_5);
        loggingScreen->setObjectName(QStringLiteral("loggingScreen"));
        QFont font3;
        font3.setFamily(QStringLiteral("Courier"));
        font3.setPointSize(9);
        loggingScreen->setFont(font3);
        loggingScreen->setLineWrapMode(QTextEdit::NoWrap);
        loggingScreen->setLineWrapColumnOrWidth(150);
        loggingScreen->setTabStopWidth(0);

        gridLayout_11->addWidget(loggingScreen, 0, 0, 1, 1);

        Debug = new QPushButton(groupBox_5);
        Debug->setObjectName(QStringLiteral("Debug"));
        Debug->setFont(font1);
        Debug->setCheckable(true);

        gridLayout_11->addWidget(Debug, 1, 0, 1, 1);


        gridLayout_10->addWidget(groupBox_5, 0, 0, 1, 1);

        tabWidget_2->addTab(tab_3, QString());

        gridLayout_2->addWidget(tabWidget_2, 0, 1, 2, 2);

        daq_window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(daq_window);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1500, 19));
        daq_window->setMenuBar(menubar);
        statusbar = new QStatusBar(daq_window);
        statusbar->setObjectName(QStringLiteral("statusbar"));
        daq_window->setStatusBar(statusbar);

        retranslateUi(daq_window);

        tabWidget_2->setCurrentIndex(0);
        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(daq_window);
    } // setupUi

    void retranslateUi(QMainWindow *daq_window)
    {
        daq_window->setWindowTitle(QApplication::translate("daq_window", "MainWindow", 0));
        groupBox_3->setTitle(QApplication::translate("daq_window", "Config file", 0));
        Button_save->setText(QApplication::translate("daq_window", "Save", 0));
        Button_load->setText(QApplication::translate("daq_window", "Load", 0));
        selectDir->setText(QString());
        groupBox->setTitle(QString());
        groupBox_6->setTitle(QApplication::translate("daq_window", "ACQ", 0));
        checkBox->setText(QApplication::translate("daq_window", " Global ACQ", 0));
        trgPulser->setText(QApplication::translate("daq_window", "Pulser", 0));
        offACQ->setText(QApplication::translate("daq_window", "ACQ Off", 0));
        onACQ->setText(QApplication::translate("daq_window", "ACQ On", 0));
        trgExternal->setText(QApplication::translate("daq_window", "External", 0));
        connectionLabel_3->setText(QString());
        Send->setText(QApplication::translate("daq_window", "Send", 0));
        openConnection_2->setText(QApplication::translate("daq_window", "Open Communication", 0));
        connectionLabel_2->setText(QApplication::translate("daq_window", "  N/A", 0));
        reset_warnings->setText(QApplication::translate("daq_window", "Reset Warnings", 0));
        Fec_group_box->setTitle(QApplication::translate("daq_window", "FEC", 0));
        Box_fec1->setText(QApplication::translate("daq_window", "1", 0));
        Box_fec2->setText(QApplication::translate("daq_window", "2", 0));
        Box_fec3->setText(QApplication::translate("daq_window", "3", 0));
        Box_fec4->setText(QApplication::translate("daq_window", "4", 0));
        Box_fec5->setText(QApplication::translate("daq_window", "5", 0));
        Box_fec6->setText(QApplication::translate("daq_window", "6", 0));
        Box_fec7->setText(QApplication::translate("daq_window", "7", 0));
        Box_fec8->setText(QApplication::translate("daq_window", "8", 0));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab), QApplication::translate("daq_window", "VMM Slow Control", 0));
        Calib_Settings->setTitle(QApplication::translate("daq_window", "Settings", 0));
        label_vmm->setText(QApplication::translate("daq_window", "Display VMM", 0));
        label_run->setText(QApplication::translate("daq_window", "#Runs", 0));
        label_type->setText(QApplication::translate("daq_window", "Calibration Type", 0));
        Calib_type->clear();
        Calib_type->insertItems(0, QStringList()
         << QApplication::translate("daq_window", "ADC", 0)
         << QApplication::translate("daq_window", "TDC", 0)
        );
        Data->setText(QApplication::translate("daq_window", "Start Calibration", 0));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_2), QApplication::translate("daq_window", "Calibration", 0));
        groupBox_5->setTitle(QApplication::translate("daq_window", "Logging", 0));
        Debug->setText(QApplication::translate("daq_window", "Debug", 0));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_3), QApplication::translate("daq_window", "Logging", 0));
    } // retranslateUi

};

namespace Ui {
    class daq_window: public Ui_daq_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAQ_WINDOW_H
