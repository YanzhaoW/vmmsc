/********************************************************************************
** Form generated from reading UI file 'fec_window.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FEC_WINDOW_H
#define UI_FEC_WINDOW_H

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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fec_window
{
public:
    QGridLayout *gridLayout_9;
    QWidget *widget_2;
    QGridLayout *gridLayout_8;
    QTabWidget *tabWidget;
    QTabWidget *tabWidget1;
    QWidget *tabWidgetPage1;
    QGridLayout *gridLayout_6;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_4;
    QCheckBox *Box_hdmi1;
    QCheckBox *Box_hdmi2;
    QCheckBox *Box_hdmi3;
    QCheckBox *Box_hdmi4;
    QCheckBox *Box_hdmi5;
    QCheckBox *Box_hdmi6;
    QCheckBox *Box_hdmi7;
    QCheckBox *Box_hdmi8;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_7;
    QLineEdit *ip1_2;
    QLineEdit *ip3_2;
    QLineEdit *ip2_2;
    QLineEdit *ip4_2;
    QGroupBox *groupBox_12;
    QGridLayout *gridLayout_3;
    QSpinBox *bcid_reset;
    QLabel *label_20;
    QLineEdit *trgPeriod;
    QSpinBox *acqWindow;
    QLabel *label_37;
    QLabel *label_22;
    QLabel *label_21;
    QSpinBox *acqSync;
    QLabel *label_7;
    QSpinBox *pulserDelay;
    QGroupBox *groupBox_6;
    QGridLayout *gridLayout_11;
    QPushButton *onACQ;
    QPushButton *offACQ;
    QPushButton *trgExternal;
    QPushButton *trgPulser;
    QWidget *tab;
    QGridLayout *gridLayout;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QPushButton *sL0enaV;
    QPushButton *sL0ena;
    QLabel *label_2;
    QSpinBox *L0BCoffset;
    QLabel *label_3;
    QSpinBox *L0_offset;
    QLabel *label_4;
    QSpinBox *L0_rollover;
    QLabel *label_5;
    QSpinBox *L0_window;
    QLabel *label_6;
    QSpinBox *L0_truncate;
    QLabel *label_8;
    QSpinBox *L0_nskip;
    QPushButton *sL0cktest;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_5;
    QComboBox *evbld_mode;
    QCheckBox *timeStampResCheckBox;
    QComboBox *evbld_infodata;
    QGroupBox *groupBox_19;
    QTextEdit *debugScreen;
    QPushButton *clearDebugScreenPB;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_10;
    QPushButton *fec_WarmInit;
    QPushButton *linkPB;
    QPushButton *resetLinks;
    QPushButton *fec_reset;

    void setupUi(QWidget *fec_window)
    {
        if (fec_window->objectName().isEmpty())
            fec_window->setObjectName(QStringLiteral("fec_window"));
        fec_window->resize(1061, 918);
        gridLayout_9 = new QGridLayout(fec_window);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        widget_2 = new QWidget(fec_window);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        gridLayout_8 = new QGridLayout(widget_2);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        tabWidget = new QTabWidget(widget_2);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setMinimumSize(QSize(800, 800));

        gridLayout_8->addWidget(tabWidget, 0, 0, 1, 1);


        gridLayout_9->addWidget(widget_2, 0, 1, 1, 1);

        tabWidget1 = new QTabWidget(fec_window);
        tabWidget1->setObjectName(QStringLiteral("tabWidget1"));
        tabWidget1->setMinimumSize(QSize(155, 900));
        tabWidget1->setMaximumSize(QSize(155, 2000));
        tabWidgetPage1 = new QWidget();
        tabWidgetPage1->setObjectName(QStringLiteral("tabWidgetPage1"));
        gridLayout_6 = new QGridLayout(tabWidgetPage1);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        groupBox = new QGroupBox(tabWidgetPage1);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gridLayout_4 = new QGridLayout(groupBox);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        Box_hdmi1 = new QCheckBox(groupBox);
        Box_hdmi1->setObjectName(QStringLiteral("Box_hdmi1"));

        gridLayout_4->addWidget(Box_hdmi1, 0, 0, 1, 1);

        Box_hdmi2 = new QCheckBox(groupBox);
        Box_hdmi2->setObjectName(QStringLiteral("Box_hdmi2"));

        gridLayout_4->addWidget(Box_hdmi2, 1, 0, 1, 1);

        Box_hdmi3 = new QCheckBox(groupBox);
        Box_hdmi3->setObjectName(QStringLiteral("Box_hdmi3"));

        gridLayout_4->addWidget(Box_hdmi3, 2, 0, 1, 1);

        Box_hdmi4 = new QCheckBox(groupBox);
        Box_hdmi4->setObjectName(QStringLiteral("Box_hdmi4"));

        gridLayout_4->addWidget(Box_hdmi4, 3, 0, 1, 1);

        Box_hdmi5 = new QCheckBox(groupBox);
        Box_hdmi5->setObjectName(QStringLiteral("Box_hdmi5"));

        gridLayout_4->addWidget(Box_hdmi5, 0, 1, 1, 1);

        Box_hdmi6 = new QCheckBox(groupBox);
        Box_hdmi6->setObjectName(QStringLiteral("Box_hdmi6"));

        gridLayout_4->addWidget(Box_hdmi6, 1, 1, 1, 1);

        Box_hdmi7 = new QCheckBox(groupBox);
        Box_hdmi7->setObjectName(QStringLiteral("Box_hdmi7"));

        gridLayout_4->addWidget(Box_hdmi7, 2, 1, 1, 1);

        Box_hdmi8 = new QCheckBox(groupBox);
        Box_hdmi8->setObjectName(QStringLiteral("Box_hdmi8"));

        gridLayout_4->addWidget(Box_hdmi8, 3, 1, 1, 1);


        gridLayout_6->addWidget(groupBox, 5, 0, 1, 1);

        groupBox_3 = new QGroupBox(tabWidgetPage1);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setMaximumSize(QSize(150, 65));
        gridLayout_7 = new QGridLayout(groupBox_3);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        ip1_2 = new QLineEdit(groupBox_3);
        ip1_2->setObjectName(QStringLiteral("ip1_2"));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
        ip1_2->setFont(font);
        ip1_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(ip1_2, 0, 0, 1, 1);

        ip3_2 = new QLineEdit(groupBox_3);
        ip3_2->setObjectName(QStringLiteral("ip3_2"));
        ip3_2->setFont(font);
        ip3_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(ip3_2, 0, 2, 1, 1);

        ip2_2 = new QLineEdit(groupBox_3);
        ip2_2->setObjectName(QStringLiteral("ip2_2"));
        ip2_2->setFont(font);
        ip2_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(ip2_2, 0, 1, 1, 1);

        ip4_2 = new QLineEdit(groupBox_3);
        ip4_2->setObjectName(QStringLiteral("ip4_2"));
        ip4_2->setFont(font);
        ip4_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_7->addWidget(ip4_2, 0, 3, 1, 1);


        gridLayout_6->addWidget(groupBox_3, 4, 0, 1, 2);

        groupBox_12 = new QGroupBox(tabWidgetPage1);
        groupBox_12->setObjectName(QStringLiteral("groupBox_12"));
        groupBox_12->setFont(font);
        groupBox_12->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gridLayout_3 = new QGridLayout(groupBox_12);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        bcid_reset = new QSpinBox(groupBox_12);
        bcid_reset->setObjectName(QStringLiteral("bcid_reset"));
        bcid_reset->setFont(font);
        bcid_reset->setMinimum(0);
        bcid_reset->setMaximum(65535);
        bcid_reset->setSingleStep(1);
        bcid_reset->setValue(0);

        gridLayout_3->addWidget(bcid_reset, 1, 1, 1, 1);

        label_20 = new QLabel(groupBox_12);
        label_20->setObjectName(QStringLiteral("label_20"));
        QFont font1;
        font1.setPointSize(8);
        label_20->setFont(font1);

        gridLayout_3->addWidget(label_20, 2, 1, 1, 1);

        trgPeriod = new QLineEdit(groupBox_12);
        trgPeriod->setObjectName(QStringLiteral("trgPeriod"));
        trgPeriod->setFont(font);
        trgPeriod->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(trgPeriod, 3, 0, 1, 1);

        acqWindow = new QSpinBox(groupBox_12);
        acqWindow->setObjectName(QStringLiteral("acqWindow"));
        acqWindow->setFont(font);
        acqWindow->setMinimum(1);
        acqWindow->setMaximum(50000);
        acqWindow->setSingleStep(1);
        acqWindow->setValue(4096);

        gridLayout_3->addWidget(acqWindow, 6, 0, 1, 1);

        label_37 = new QLabel(groupBox_12);
        label_37->setObjectName(QStringLiteral("label_37"));
        label_37->setFont(font1);
        label_37->setWordWrap(true);

        gridLayout_3->addWidget(label_37, 0, 1, 1, 1);

        label_22 = new QLabel(groupBox_12);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setFont(font1);
        label_22->setWordWrap(true);

        gridLayout_3->addWidget(label_22, 2, 0, 1, 1);

        label_21 = new QLabel(groupBox_12);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setFont(font1);
        label_21->setWordWrap(true);

        gridLayout_3->addWidget(label_21, 4, 0, 1, 1);

        acqSync = new QSpinBox(groupBox_12);
        acqSync->setObjectName(QStringLiteral("acqSync"));
        acqSync->setFont(font);
        acqSync->setMinimum(1);
        acqSync->setMaximum(50000);
        acqSync->setSingleStep(1);
        acqSync->setValue(100);

        gridLayout_3->addWidget(acqSync, 3, 1, 1, 1);

        label_7 = new QLabel(groupBox_12);
        label_7->setObjectName(QStringLiteral("label_7"));
        label_7->setFont(font1);

        gridLayout_3->addWidget(label_7, 0, 0, 1, 1);

        pulserDelay = new QSpinBox(groupBox_12);
        pulserDelay->setObjectName(QStringLiteral("pulserDelay"));
        pulserDelay->setFont(font);
        pulserDelay->setMinimum(1);
        pulserDelay->setMaximum(50000);
        pulserDelay->setSingleStep(1);
        pulserDelay->setValue(81);

        gridLayout_3->addWidget(pulserDelay, 1, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_12, 6, 0, 1, 1);

        groupBox_6 = new QGroupBox(tabWidgetPage1);
        groupBox_6->setObjectName(QStringLiteral("groupBox_6"));
        gridLayout_11 = new QGridLayout(groupBox_6);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        onACQ = new QPushButton(groupBox_6);
        onACQ->setObjectName(QStringLiteral("onACQ"));
        QFont font2;
        font2.setPointSize(9);
        onACQ->setFont(font2);
        onACQ->setFocusPolicy(Qt::NoFocus);
        onACQ->setCheckable(false);

        gridLayout_11->addWidget(onACQ, 2, 0, 1, 2);

        offACQ = new QPushButton(groupBox_6);
        offACQ->setObjectName(QStringLiteral("offACQ"));
        offACQ->setFont(font2);
        offACQ->setFocusPolicy(Qt::NoFocus);
        offACQ->setCheckable(false);

        gridLayout_11->addWidget(offACQ, 3, 0, 1, 2);

        trgExternal = new QPushButton(groupBox_6);
        trgExternal->setObjectName(QStringLiteral("trgExternal"));
        trgExternal->setFont(font2);
        trgExternal->setFocusPolicy(Qt::NoFocus);
        trgExternal->setAutoFillBackground(false);
        trgExternal->setCheckable(false);
        trgExternal->setAutoDefault(false);

        gridLayout_11->addWidget(trgExternal, 1, 0, 1, 2);

        trgPulser = new QPushButton(groupBox_6);
        trgPulser->setObjectName(QStringLiteral("trgPulser"));
        trgPulser->setFont(font2);
        trgPulser->setFocusPolicy(Qt::NoFocus);
        trgPulser->setCheckable(false);

        gridLayout_11->addWidget(trgPulser, 0, 0, 1, 2);


        gridLayout_6->addWidget(groupBox_6, 7, 0, 1, 1);

        tabWidget1->addTab(tabWidgetPage1, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout = new QGridLayout(tab);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        sL0enaV = new QPushButton(groupBox_2);
        sL0enaV->setObjectName(QStringLiteral("sL0enaV"));
        sL0enaV->setCheckable(true);

        gridLayout_2->addWidget(sL0enaV, 0, 0, 1, 2);

        sL0ena = new QPushButton(groupBox_2);
        sL0ena->setObjectName(QStringLiteral("sL0ena"));
        sL0ena->setCheckable(true);

        gridLayout_2->addWidget(sL0ena, 1, 0, 1, 2);

        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout_2->addWidget(label_2, 2, 0, 1, 1);

        L0BCoffset = new QSpinBox(groupBox_2);
        L0BCoffset->setObjectName(QStringLiteral("L0BCoffset"));
        L0BCoffset->setFont(font);
        L0BCoffset->setMinimum(0);
        L0BCoffset->setMaximum(4094);
        L0BCoffset->setSingleStep(1);
        L0BCoffset->setValue(0);

        gridLayout_2->addWidget(L0BCoffset, 2, 1, 1, 1);

        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_2->addWidget(label_3, 3, 0, 1, 1);

        L0_offset = new QSpinBox(groupBox_2);
        L0_offset->setObjectName(QStringLiteral("L0_offset"));
        L0_offset->setFont(font);
        L0_offset->setMinimum(0);
        L0_offset->setMaximum(4094);
        L0_offset->setSingleStep(1);
        L0_offset->setValue(0);

        gridLayout_2->addWidget(L0_offset, 3, 1, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 4, 0, 1, 1);

        L0_rollover = new QSpinBox(groupBox_2);
        L0_rollover->setObjectName(QStringLiteral("L0_rollover"));
        L0_rollover->setFont(font);
        L0_rollover->setMinimum(0);
        L0_rollover->setMaximum(4094);
        L0_rollover->setSingleStep(1);
        L0_rollover->setValue(0);

        gridLayout_2->addWidget(L0_rollover, 4, 1, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 5, 0, 1, 1);

        L0_window = new QSpinBox(groupBox_2);
        L0_window->setObjectName(QStringLiteral("L0_window"));
        L0_window->setFont(font);
        L0_window->setMinimum(0);
        L0_window->setMaximum(7);
        L0_window->setSingleStep(1);
        L0_window->setValue(0);

        gridLayout_2->addWidget(L0_window, 5, 1, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 6, 0, 1, 1);

        L0_truncate = new QSpinBox(groupBox_2);
        L0_truncate->setObjectName(QStringLiteral("L0_truncate"));
        L0_truncate->setFont(font);
        L0_truncate->setMinimum(0);
        L0_truncate->setMaximum(63);
        L0_truncate->setSingleStep(1);
        L0_truncate->setValue(0);

        gridLayout_2->addWidget(L0_truncate, 6, 1, 1, 1);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_2->addWidget(label_8, 7, 0, 1, 1);

        L0_nskip = new QSpinBox(groupBox_2);
        L0_nskip->setObjectName(QStringLiteral("L0_nskip"));
        L0_nskip->setFont(font);
        L0_nskip->setMinimum(0);
        L0_nskip->setMaximum(127);
        L0_nskip->setSingleStep(1);
        L0_nskip->setValue(0);

        gridLayout_2->addWidget(L0_nskip, 7, 1, 1, 1);

        sL0cktest = new QPushButton(groupBox_2);
        sL0cktest->setObjectName(QStringLiteral("sL0cktest"));
        sL0cktest->setCheckable(true);

        gridLayout_2->addWidget(sL0cktest, 8, 0, 1, 2);


        gridLayout->addWidget(groupBox_2, 0, 0, 1, 2);

        groupBox_4 = new QGroupBox(tab);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        gridLayout_5 = new QGridLayout(groupBox_4);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        evbld_mode = new QComboBox(groupBox_4);
        evbld_mode->setObjectName(QStringLiteral("evbld_mode"));
        evbld_mode->setFont(font);

        gridLayout_5->addWidget(evbld_mode, 0, 0, 2, 1);

        timeStampResCheckBox = new QCheckBox(groupBox_4);
        timeStampResCheckBox->setObjectName(QStringLiteral("timeStampResCheckBox"));
        timeStampResCheckBox->setFont(font1);

        gridLayout_5->addWidget(timeStampResCheckBox, 3, 0, 1, 1);

        evbld_infodata = new QComboBox(groupBox_4);
        evbld_infodata->setObjectName(QStringLiteral("evbld_infodata"));
        evbld_infodata->setFont(font);

        gridLayout_5->addWidget(evbld_infodata, 2, 0, 1, 1);


        gridLayout->addWidget(groupBox_4, 2, 0, 2, 2);

        groupBox_19 = new QGroupBox(tab);
        groupBox_19->setObjectName(QStringLiteral("groupBox_19"));
        groupBox_19->setMinimumSize(QSize(0, 230));
        debugScreen = new QTextEdit(groupBox_19);
        debugScreen->setObjectName(QStringLiteral("debugScreen"));
        debugScreen->setGeometry(QRect(0, 50, 131, 171));
        debugScreen->setFont(font2);
        clearDebugScreenPB = new QPushButton(groupBox_19);
        clearDebugScreenPB->setObjectName(QStringLiteral("clearDebugScreenPB"));
        clearDebugScreenPB->setGeometry(QRect(10, 30, 41, 16));
        clearDebugScreenPB->setFont(font);

        gridLayout->addWidget(groupBox_19, 5, 0, 1, 2);

        groupBox_5 = new QGroupBox(tab);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        gridLayout_10 = new QGridLayout(groupBox_5);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        fec_WarmInit = new QPushButton(groupBox_5);
        fec_WarmInit->setObjectName(QStringLiteral("fec_WarmInit"));
        fec_WarmInit->setEnabled(true);
        fec_WarmInit->setFont(font);

        gridLayout_10->addWidget(fec_WarmInit, 0, 0, 1, 1);

        linkPB = new QPushButton(groupBox_5);
        linkPB->setObjectName(QStringLiteral("linkPB"));
        linkPB->setFont(font2);

        gridLayout_10->addWidget(linkPB, 2, 0, 1, 1);

        resetLinks = new QPushButton(groupBox_5);
        resetLinks->setObjectName(QStringLiteral("resetLinks"));
        resetLinks->setFont(font2);

        gridLayout_10->addWidget(resetLinks, 3, 0, 1, 1);

        fec_reset = new QPushButton(groupBox_5);
        fec_reset->setObjectName(QStringLiteral("fec_reset"));
        fec_reset->setEnabled(true);
        fec_reset->setFont(font);

        gridLayout_10->addWidget(fec_reset, 1, 0, 1, 1);


        gridLayout->addWidget(groupBox_5, 4, 0, 1, 2);

        tabWidget1->addTab(tab, QString());

        gridLayout_9->addWidget(tabWidget1, 0, 0, 1, 1);


        retranslateUi(fec_window);

        tabWidget1->setCurrentIndex(1);
        evbld_mode->setCurrentIndex(0);
        evbld_infodata->setCurrentIndex(5);


        QMetaObject::connectSlotsByName(fec_window);
    } // setupUi

    void retranslateUi(QWidget *fec_window)
    {
        fec_window->setWindowTitle(QApplication::translate("fec_window", "Form", 0));
        groupBox->setTitle(QApplication::translate("fec_window", "HDMI", 0));
        Box_hdmi1->setText(QApplication::translate("fec_window", "1", 0));
        Box_hdmi2->setText(QApplication::translate("fec_window", "2", 0));
        Box_hdmi3->setText(QApplication::translate("fec_window", "3", 0));
        Box_hdmi4->setText(QApplication::translate("fec_window", "4", 0));
        Box_hdmi5->setText(QApplication::translate("fec_window", "5", 0));
        Box_hdmi6->setText(QApplication::translate("fec_window", "6", 0));
        Box_hdmi7->setText(QApplication::translate("fec_window", "7", 0));
        Box_hdmi8->setText(QApplication::translate("fec_window", "8", 0));
        groupBox_3->setTitle(QApplication::translate("fec_window", "IP address FEC", 0));
        ip1_2->setText(QApplication::translate("fec_window", "10", 0));
        ip3_2->setText(QApplication::translate("fec_window", "0", 0));
        ip2_2->setText(QApplication::translate("fec_window", "0", 0));
        ip4_2->setText(QApplication::translate("fec_window", "2", 0));
        groupBox_12->setTitle(QApplication::translate("fec_window", "Trigger & Acquisition", 0));
        label_20->setText(QApplication::translate("fec_window", "ACQ Sync", 0));
        trgPeriod->setText(QApplication::translate("fec_window", "61A80", 0));
        label_37->setText(QApplication::translate("fec_window", "BCID Reset", 0));
        label_22->setText(QApplication::translate("fec_window", "Trg Per", 0));
        label_21->setText(QApplication::translate("fec_window", "ACQ Win", 0));
        label_7->setText(QApplication::translate("fec_window", "TP Delay", 0));
        groupBox_6->setTitle(QApplication::translate("fec_window", "ACQ", 0));
        onACQ->setText(QApplication::translate("fec_window", "ACQ On", 0));
        offACQ->setText(QApplication::translate("fec_window", "ACQ Off", 0));
        trgExternal->setText(QApplication::translate("fec_window", "External", 0));
        trgPulser->setText(QApplication::translate("fec_window", "Pulser", 0));
        tabWidget1->setTabText(tabWidget1->indexOf(tabWidgetPage1), QApplication::translate("fec_window", "General", 0));
        groupBox_2->setTitle(QApplication::translate("fec_window", "L0 settings", 0));
        sL0enaV->setText(QApplication::translate("fec_window", "sL0enaV", 0));
        sL0ena->setText(QApplication::translate("fec_window", "sL0ena", 0));
        label_2->setText(QApplication::translate("fec_window", "l0offset", 0));
        label_3->setText(QApplication::translate("fec_window", "offset", 0));
        label_4->setText(QApplication::translate("fec_window", "rollover", 0));
        label_5->setText(QApplication::translate("fec_window", "window", 0));
        label_6->setText(QApplication::translate("fec_window", "truncate", 0));
        label_8->setText(QApplication::translate("fec_window", "nskip", 0));
        sL0cktest->setText(QApplication::translate("fec_window", "sL0cktest", 0));
        groupBox_4->setTitle(QString());
        evbld_mode->clear();
        evbld_mode->insertItems(0, QStringList()
         << QApplication::translate("fec_window", "Frame Cnt", 0)
         << QApplication::translate("fec_window", "Global Frame Cnt", 0)
         << QApplication::translate("fec_window", "Timestamp + Frame Cnt", 0)
        );
        timeStampResCheckBox->setText(QApplication::translate("fec_window", "High Res.", 0));
        evbld_infodata->clear();
        evbld_infodata->insertItems(0, QStringList()
         << QApplication::translate("fec_window", "HINFO+Datalength", 0)
         << QApplication::translate("fec_window", "Trigger Cnt+Datalength", 0)
         << QApplication::translate("fec_window", "Trigger Cnt", 0)
         << QApplication::translate("fec_window", "Trigger Timestamp+Datalength", 0)
         << QApplication::translate("fec_window", "Trigger Timestamp", 0)
         << QApplication::translate("fec_window", "Trigger Cnt+Trigger Timestamp", 0)
        );
        groupBox_19->setTitle(QApplication::translate("fec_window", "FEC Response", 0));
        clearDebugScreenPB->setText(QApplication::translate("fec_window", "Clear", 0));
        groupBox_5->setTitle(QString());
        fec_WarmInit->setText(QApplication::translate("fec_window", "WarmInit FEC", 0));
        linkPB->setText(QApplication::translate("fec_window", "Link Status", 0));
        resetLinks->setText(QApplication::translate("fec_window", "Reset Links", 0));
        fec_reset->setText(QApplication::translate("fec_window", "Reboot FEC", 0));
        tabWidget1->setTabText(tabWidget1->indexOf(tab), QApplication::translate("fec_window", "Advanced", 0));
    } // retranslateUi

};

namespace Ui {
    class fec_window: public Ui_fec_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FEC_WINDOW_H
