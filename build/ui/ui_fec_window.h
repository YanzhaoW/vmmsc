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
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fec_window
{
public:
    QGridLayout *gridLayout_6;
    QWidget *widget;
    QGridLayout *gridLayout_5;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_7;
    QLineEdit *ip1_2;
    QLineEdit *ip3_2;
    QLineEdit *ip2_2;
    QLineEdit *ip4_2;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *verticalSpacer;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_sL0enaV;
    QPushButton *pushButton_sL0ena;
    QLabel *label_2;
    QSpinBox *L0BCoffset;
    QLabel *label_3;
    QSpinBox *L0BCoffset_2;
    QLabel *label_4;
    QSpinBox *L0BCoffset_3;
    QLabel *label_5;
    QSpinBox *L0BCoffset_4;
    QLabel *label_6;
    QSpinBox *L0BCoffset_5;
    QLabel *label_8;
    QSpinBox *L0BCoffset_6;
    QPushButton *pushButton_sL0cktest;
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
    QGroupBox *groupBox_12;
    QGridLayout *gridLayout_3;
    QLabel *label_7;
    QLabel *label_37;
    QSpinBox *pulserDelay_3;
    QSpinBox *bcid_reset_3;
    QLabel *label_22;
    QLabel *label_20;
    QLineEdit *trgPeriod_3;
    QSpinBox *acqSync_3;
    QLabel *label_21;
    QCheckBox *holdOffCheckBox_3;
    QSpinBox *acqWindow_3;
    QPushButton *setTrgAcqConst_3;
    QPushButton *trgPulser_3;
    QPushButton *onACQ_3;
    QPushButton *trgExternal_3;
    QPushButton *offACQ_3;
    QSpacerItem *verticalSpacer_2;
    QGroupBox *groupBox_13;
    QGridLayout *gridLayout;
    QPushButton *fec_reset;
    QPushButton *fec_WarmInit;
    QSpacerItem *verticalSpacer_4;
    QWidget *widget_2;
    QGridLayout *gridLayout_8;
    QTabWidget *tabWidget;

    void setupUi(QWidget *fec_window)
    {
        if (fec_window->objectName().isEmpty())
            fec_window->setObjectName(QStringLiteral("fec_window"));
        fec_window->resize(1284, 1180);
        gridLayout_6 = new QGridLayout(fec_window);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        widget = new QWidget(fec_window);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(155, 900));
        widget->setMaximumSize(QSize(155, 2000));
        gridLayout_5 = new QGridLayout(widget);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        groupBox_3 = new QGroupBox(widget);
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


        gridLayout_5->addWidget(groupBox_3, 8, 0, 1, 1);

        verticalSpacer_3 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_3, 3, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer, 7, 0, 1, 1);

        groupBox_2 = new QGroupBox(widget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        pushButton_sL0enaV = new QPushButton(groupBox_2);
        pushButton_sL0enaV->setObjectName(QStringLiteral("pushButton_sL0enaV"));
        pushButton_sL0enaV->setCheckable(true);

        gridLayout_2->addWidget(pushButton_sL0enaV, 0, 0, 1, 2);

        pushButton_sL0ena = new QPushButton(groupBox_2);
        pushButton_sL0ena->setObjectName(QStringLiteral("pushButton_sL0ena"));
        pushButton_sL0ena->setCheckable(true);

        gridLayout_2->addWidget(pushButton_sL0ena, 1, 0, 1, 2);

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

        L0BCoffset_2 = new QSpinBox(groupBox_2);
        L0BCoffset_2->setObjectName(QStringLiteral("L0BCoffset_2"));
        L0BCoffset_2->setFont(font);
        L0BCoffset_2->setMinimum(0);
        L0BCoffset_2->setMaximum(4094);
        L0BCoffset_2->setSingleStep(1);
        L0BCoffset_2->setValue(0);

        gridLayout_2->addWidget(L0BCoffset_2, 3, 1, 1, 1);

        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout_2->addWidget(label_4, 4, 0, 1, 1);

        L0BCoffset_3 = new QSpinBox(groupBox_2);
        L0BCoffset_3->setObjectName(QStringLiteral("L0BCoffset_3"));
        L0BCoffset_3->setFont(font);
        L0BCoffset_3->setMinimum(0);
        L0BCoffset_3->setMaximum(4094);
        L0BCoffset_3->setSingleStep(1);
        L0BCoffset_3->setValue(0);

        gridLayout_2->addWidget(L0BCoffset_3, 4, 1, 1, 1);

        label_5 = new QLabel(groupBox_2);
        label_5->setObjectName(QStringLiteral("label_5"));

        gridLayout_2->addWidget(label_5, 5, 0, 1, 1);

        L0BCoffset_4 = new QSpinBox(groupBox_2);
        L0BCoffset_4->setObjectName(QStringLiteral("L0BCoffset_4"));
        L0BCoffset_4->setFont(font);
        L0BCoffset_4->setMinimum(0);
        L0BCoffset_4->setMaximum(7);
        L0BCoffset_4->setSingleStep(1);
        L0BCoffset_4->setValue(0);

        gridLayout_2->addWidget(L0BCoffset_4, 5, 1, 1, 1);

        label_6 = new QLabel(groupBox_2);
        label_6->setObjectName(QStringLiteral("label_6"));

        gridLayout_2->addWidget(label_6, 6, 0, 1, 1);

        L0BCoffset_5 = new QSpinBox(groupBox_2);
        L0BCoffset_5->setObjectName(QStringLiteral("L0BCoffset_5"));
        L0BCoffset_5->setFont(font);
        L0BCoffset_5->setMinimum(0);
        L0BCoffset_5->setMaximum(63);
        L0BCoffset_5->setSingleStep(1);
        L0BCoffset_5->setValue(0);

        gridLayout_2->addWidget(L0BCoffset_5, 6, 1, 1, 1);

        label_8 = new QLabel(groupBox_2);
        label_8->setObjectName(QStringLiteral("label_8"));

        gridLayout_2->addWidget(label_8, 7, 0, 1, 1);

        L0BCoffset_6 = new QSpinBox(groupBox_2);
        L0BCoffset_6->setObjectName(QStringLiteral("L0BCoffset_6"));
        L0BCoffset_6->setFont(font);
        L0BCoffset_6->setMinimum(0);
        L0BCoffset_6->setMaximum(127);
        L0BCoffset_6->setSingleStep(1);
        L0BCoffset_6->setValue(0);

        gridLayout_2->addWidget(L0BCoffset_6, 7, 1, 1, 1);

        pushButton_sL0cktest = new QPushButton(groupBox_2);
        pushButton_sL0cktest->setObjectName(QStringLiteral("pushButton_sL0cktest"));
        pushButton_sL0cktest->setCheckable(true);

        gridLayout_2->addWidget(pushButton_sL0cktest, 8, 0, 1, 2);


        gridLayout_5->addWidget(groupBox_2, 6, 0, 1, 1);

        groupBox = new QGroupBox(widget);
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

        gridLayout_4->addWidget(Box_hdmi5, 4, 0, 1, 1);

        Box_hdmi6 = new QCheckBox(groupBox);
        Box_hdmi6->setObjectName(QStringLiteral("Box_hdmi6"));

        gridLayout_4->addWidget(Box_hdmi6, 5, 0, 1, 1);

        Box_hdmi7 = new QCheckBox(groupBox);
        Box_hdmi7->setObjectName(QStringLiteral("Box_hdmi7"));

        gridLayout_4->addWidget(Box_hdmi7, 6, 0, 1, 1);

        Box_hdmi8 = new QCheckBox(groupBox);
        Box_hdmi8->setObjectName(QStringLiteral("Box_hdmi8"));

        gridLayout_4->addWidget(Box_hdmi8, 7, 0, 1, 1);


        gridLayout_5->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_12 = new QGroupBox(widget);
        groupBox_12->setObjectName(QStringLiteral("groupBox_12"));
        groupBox_12->setFont(font);
        groupBox_12->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gridLayout_3 = new QGridLayout(groupBox_12);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_7 = new QLabel(groupBox_12);
        label_7->setObjectName(QStringLiteral("label_7"));
        QFont font1;
        font1.setPointSize(8);
        label_7->setFont(font1);

        gridLayout_3->addWidget(label_7, 0, 0, 1, 1);

        label_37 = new QLabel(groupBox_12);
        label_37->setObjectName(QStringLiteral("label_37"));
        label_37->setFont(font1);
        label_37->setWordWrap(true);

        gridLayout_3->addWidget(label_37, 0, 1, 1, 1);

        pulserDelay_3 = new QSpinBox(groupBox_12);
        pulserDelay_3->setObjectName(QStringLiteral("pulserDelay_3"));
        pulserDelay_3->setFont(font);
        pulserDelay_3->setMinimum(1);
        pulserDelay_3->setMaximum(50000);
        pulserDelay_3->setSingleStep(1);
        pulserDelay_3->setValue(81);

        gridLayout_3->addWidget(pulserDelay_3, 1, 0, 1, 1);

        bcid_reset_3 = new QSpinBox(groupBox_12);
        bcid_reset_3->setObjectName(QStringLiteral("bcid_reset_3"));
        bcid_reset_3->setFont(font);
        bcid_reset_3->setMinimum(0);
        bcid_reset_3->setMaximum(65535);
        bcid_reset_3->setSingleStep(1);
        bcid_reset_3->setValue(0);

        gridLayout_3->addWidget(bcid_reset_3, 1, 1, 1, 1);

        label_22 = new QLabel(groupBox_12);
        label_22->setObjectName(QStringLiteral("label_22"));
        label_22->setFont(font1);
        label_22->setWordWrap(true);

        gridLayout_3->addWidget(label_22, 2, 0, 1, 1);

        label_20 = new QLabel(groupBox_12);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setFont(font1);

        gridLayout_3->addWidget(label_20, 2, 1, 1, 1);

        trgPeriod_3 = new QLineEdit(groupBox_12);
        trgPeriod_3->setObjectName(QStringLiteral("trgPeriod_3"));
        trgPeriod_3->setFont(font);
        trgPeriod_3->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout_3->addWidget(trgPeriod_3, 3, 0, 1, 1);

        acqSync_3 = new QSpinBox(groupBox_12);
        acqSync_3->setObjectName(QStringLiteral("acqSync_3"));
        acqSync_3->setFont(font);
        acqSync_3->setMinimum(1);
        acqSync_3->setMaximum(50000);
        acqSync_3->setSingleStep(1);
        acqSync_3->setValue(100);

        gridLayout_3->addWidget(acqSync_3, 3, 1, 1, 1);

        label_21 = new QLabel(groupBox_12);
        label_21->setObjectName(QStringLiteral("label_21"));
        label_21->setFont(font1);
        label_21->setWordWrap(true);

        gridLayout_3->addWidget(label_21, 4, 0, 1, 1);

        holdOffCheckBox_3 = new QCheckBox(groupBox_12);
        holdOffCheckBox_3->setObjectName(QStringLiteral("holdOffCheckBox_3"));
        holdOffCheckBox_3->setFont(font1);

        gridLayout_3->addWidget(holdOffCheckBox_3, 4, 1, 2, 1);

        acqWindow_3 = new QSpinBox(groupBox_12);
        acqWindow_3->setObjectName(QStringLiteral("acqWindow_3"));
        acqWindow_3->setFont(font);
        acqWindow_3->setMinimum(1);
        acqWindow_3->setMaximum(50000);
        acqWindow_3->setSingleStep(1);
        acqWindow_3->setValue(4096);

        gridLayout_3->addWidget(acqWindow_3, 5, 0, 2, 1);

        setTrgAcqConst_3 = new QPushButton(groupBox_12);
        setTrgAcqConst_3->setObjectName(QStringLiteral("setTrgAcqConst_3"));
        QFont font2;
        font2.setPointSize(14);
        setTrgAcqConst_3->setFont(font2);

        gridLayout_3->addWidget(setTrgAcqConst_3, 6, 1, 1, 1);

        trgPulser_3 = new QPushButton(groupBox_12);
        trgPulser_3->setObjectName(QStringLiteral("trgPulser_3"));
        QFont font3;
        font3.setPointSize(9);
        trgPulser_3->setFont(font3);
        trgPulser_3->setFocusPolicy(Qt::NoFocus);

        gridLayout_3->addWidget(trgPulser_3, 7, 0, 1, 1);

        onACQ_3 = new QPushButton(groupBox_12);
        onACQ_3->setObjectName(QStringLiteral("onACQ_3"));
        onACQ_3->setFont(font3);
        onACQ_3->setFocusPolicy(Qt::NoFocus);

        gridLayout_3->addWidget(onACQ_3, 7, 1, 1, 1);

        trgExternal_3 = new QPushButton(groupBox_12);
        trgExternal_3->setObjectName(QStringLiteral("trgExternal_3"));
        trgExternal_3->setFont(font3);
        trgExternal_3->setFocusPolicy(Qt::NoFocus);
        trgExternal_3->setAutoFillBackground(false);
        trgExternal_3->setAutoDefault(false);

        gridLayout_3->addWidget(trgExternal_3, 8, 0, 1, 1);

        offACQ_3 = new QPushButton(groupBox_12);
        offACQ_3->setObjectName(QStringLiteral("offACQ_3"));
        offACQ_3->setFont(font3);
        offACQ_3->setFocusPolicy(Qt::NoFocus);

        gridLayout_3->addWidget(offACQ_3, 8, 1, 1, 1);


        gridLayout_5->addWidget(groupBox_12, 2, 0, 1, 1);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_2, 5, 0, 1, 1);

        groupBox_13 = new QGroupBox(widget);
        groupBox_13->setObjectName(QStringLiteral("groupBox_13"));
        gridLayout = new QGridLayout(groupBox_13);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        fec_reset = new QPushButton(groupBox_13);
        fec_reset->setObjectName(QStringLiteral("fec_reset"));
        fec_reset->setEnabled(true);
        fec_reset->setFont(font);

        gridLayout->addWidget(fec_reset, 0, 0, 1, 1);

        fec_WarmInit = new QPushButton(groupBox_13);
        fec_WarmInit->setObjectName(QStringLiteral("fec_WarmInit"));
        fec_WarmInit->setEnabled(true);
        fec_WarmInit->setFont(font);

        gridLayout->addWidget(fec_WarmInit, 1, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_13, 4, 0, 1, 1);

        verticalSpacer_4 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer_4, 1, 0, 1, 1);


        gridLayout_6->addWidget(widget, 0, 0, 1, 1);

        widget_2 = new QWidget(fec_window);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        gridLayout_8 = new QGridLayout(widget_2);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        tabWidget = new QTabWidget(widget_2);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setMinimumSize(QSize(800, 800));

        gridLayout_8->addWidget(tabWidget, 0, 0, 1, 1);


        gridLayout_6->addWidget(widget_2, 0, 1, 1, 1);


        retranslateUi(fec_window);

        QMetaObject::connectSlotsByName(fec_window);
    } // setupUi

    void retranslateUi(QWidget *fec_window)
    {
        fec_window->setWindowTitle(QApplication::translate("fec_window", "Form", 0));
        groupBox_3->setTitle(QApplication::translate("fec_window", "IP address FEC", 0));
        ip1_2->setText(QApplication::translate("fec_window", "10", 0));
        ip3_2->setText(QApplication::translate("fec_window", "0", 0));
        ip2_2->setText(QApplication::translate("fec_window", "0", 0));
        ip4_2->setText(QApplication::translate("fec_window", "2", 0));
        groupBox_2->setTitle(QApplication::translate("fec_window", "L0 settings", 0));
        pushButton_sL0enaV->setText(QApplication::translate("fec_window", "sL0enaV", 0));
        pushButton_sL0ena->setText(QApplication::translate("fec_window", "sL0ena", 0));
        label_2->setText(QApplication::translate("fec_window", "l0offset", 0));
        label_3->setText(QApplication::translate("fec_window", "offset", 0));
        label_4->setText(QApplication::translate("fec_window", "rollover", 0));
        label_5->setText(QApplication::translate("fec_window", "window", 0));
        label_6->setText(QApplication::translate("fec_window", "truncate", 0));
        label_8->setText(QApplication::translate("fec_window", "nskip", 0));
        pushButton_sL0cktest->setText(QApplication::translate("fec_window", "sL0cktest", 0));
        groupBox->setTitle(QApplication::translate("fec_window", "HDMI", 0));
        Box_hdmi1->setText(QApplication::translate("fec_window", "1", 0));
        Box_hdmi2->setText(QApplication::translate("fec_window", "2", 0));
        Box_hdmi3->setText(QApplication::translate("fec_window", "3", 0));
        Box_hdmi4->setText(QApplication::translate("fec_window", "4", 0));
        Box_hdmi5->setText(QApplication::translate("fec_window", "5", 0));
        Box_hdmi6->setText(QApplication::translate("fec_window", "6", 0));
        Box_hdmi7->setText(QApplication::translate("fec_window", "7", 0));
        Box_hdmi8->setText(QApplication::translate("fec_window", "8", 0));
        groupBox_12->setTitle(QApplication::translate("fec_window", "Trigger & Acquisition", 0));
        label_7->setText(QApplication::translate("fec_window", "TP Delay", 0));
        label_37->setText(QApplication::translate("fec_window", "BCID Reset", 0));
        label_22->setText(QApplication::translate("fec_window", "Trg Per", 0));
        label_20->setText(QApplication::translate("fec_window", "ACQ Sync", 0));
        trgPeriod_3->setText(QApplication::translate("fec_window", "61A80", 0));
        label_21->setText(QApplication::translate("fec_window", "ACQ Win", 0));
        holdOffCheckBox_3->setText(QApplication::translate("fec_window", "Hold Off", 0));
        setTrgAcqConst_3->setText(QApplication::translate("fec_window", "Set", 0));
        trgPulser_3->setText(QApplication::translate("fec_window", "Pulser", 0));
        onACQ_3->setText(QApplication::translate("fec_window", "ACQ On", 0));
        trgExternal_3->setText(QApplication::translate("fec_window", "External", 0));
        offACQ_3->setText(QApplication::translate("fec_window", "ACQ Off", 0));
        groupBox_13->setTitle(QApplication::translate("fec_window", "Resets", 0));
        fec_reset->setText(QApplication::translate("fec_window", "Reboot FEC", 0));
        fec_WarmInit->setText(QApplication::translate("fec_window", "WarmInit FEC", 0));
    } // retranslateUi

};

namespace Ui {
    class fec_window: public Ui_fec_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FEC_WINDOW_H
