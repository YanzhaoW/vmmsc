/********************************************************************************
** Form generated from reading UI file 'vmm_window.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VMM_WINDOW_H
#define UI_VMM_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_vmm_window
{
public:
    QGridLayout *gridLayout_4;
    QWidget *widget;
    QGridLayout *gridLayout_3;
    QStackedWidget *stackedWidget;
    QWidget *stackedWidgetPage1;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout_channels;
    QLabel *label;
    QPushButton *asic_reset_2;
    QPushButton *asic_reset;
    QTabWidget *tabWidget_4;
    QWidget *tabWidget_4Page1;
    QGridLayout *gridLayout_7;
    QFrame *frame;
    QGridLayout *gridLayout_15;
    QPushButton *pushButton_3;
    QPushButton *pushButton_4;
    QPushButton *pushButton_2;
    QPushButton *pushButton_7;
    QGridLayout *gridLayout_16;
    QComboBox *sg;
    QLabel *stcLabel;
    QLabel *sgLabel;
    QLabel *spgLabel;
    QComboBox *spg;
    QComboBox *stc;
    QLabel *sm5_sm0Label;
    QComboBox *sm5_sm0;
    QGridLayout *gridLayout_13;
    QComboBox *sfam_2;
    QGridLayout *gridLayout_14;
    QLabel *sfamLabel_2;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_10;
    QPushButton *pushButton_15;
    QComboBox *sc010b;
    QLabel *sc010bLabel;
    QPushButton *pushButton;
    QPushButton *pushButton_16;
    QLabel *sc06bLabel;
    QComboBox *sc08b;
    QLabel *sc08bLabel;
    QComboBox *sc06b;
    QGroupBox *groupBox1;
    QGridLayout *gridLayout_11;
    QPushButton *pushButton_9;
    QPushButton *pushButton_18;
    QPushButton *pushButton_19;
    QGroupBox *gridGroupBox_4;
    QGridLayout *gridLayout_6;
    QSpinBox *sdt;
    QSpinBox *sdp_2;
    QLabel *sdpLabel_2;
    QLabel *sdtLabel;
    QLabel *dacmvLabel_TP;
    QLabel *dacmvLabel;
    QWidget *tab;
    QGridLayout *gridLayout_8;
    QGroupBox *gridGroupBox_3;
    QGridLayout *gridLayout;
    QPushButton *pushButton_stlc;
    QPushButton *pushButton_ssart;
    QPushButton *pushButton_sL0dckinv;
    QPushButton *pushButton_sL0ckinv;
    QPushButton *pushButton_srec;
    QPushButton *pushButton_slvs;
    QPushButton *pushButton_s32;
    QPushButton *pushButton_stcr;
    QPushButton *pushButton_sbip;
    QPushButton *pushButton_sfrst;
    QPushButton *pushButton_nskipm_i;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QPushButton *pushButton_bc;
    QPushButton *pushButton_tp;
    QPushButton *pushButton_tk;
    QPushButton *pushButton_dt;
    QPushButton *pushButton_art;
    QPushButton *pushButton_tki;
    QPushButton *pushButton_ena;
    QPushButton *pushButton_6b;
    QGridLayout *gridLayout_12;
    QPushButton *pushButton_8;
    QComboBox *sfam;
    QLabel *sfamLabel;
    QLabel *stLabel;
    QComboBox *st;
    QGridLayout *gridLayout_5;
    QPushButton *pushButton_13;
    QPushButton *pushButton_14;
    QPushButton *pushButton_11;
    QPushButton *pushButton_6;
    QPushButton *pushButton_10;
    QPushButton *pushButton_5;
    QPushButton *pushButton_12;
    QGridLayout *gridLayout_9;
    QPushButton *pushButton_17;
    QComboBox *stpp;
    QComboBox *stot;
    QLabel *stotLabel;

    void setupUi(QWidget *vmm_window)
    {
        if (vmm_window->objectName().isEmpty())
            vmm_window->setObjectName(QStringLiteral("vmm_window"));
        vmm_window->resize(844, 814);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(vmm_window->sizePolicy().hasHeightForWidth());
        vmm_window->setSizePolicy(sizePolicy);
        vmm_window->setMinimumSize(QSize(844, 0));
        gridLayout_4 = new QGridLayout(vmm_window);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        widget = new QWidget(vmm_window);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(500, 740));
        gridLayout_3 = new QGridLayout(widget);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        stackedWidget = new QStackedWidget(widget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setMinimumSize(QSize(480, 700));
        stackedWidget->setMaximumSize(QSize(480, 16777215));
        QFont font;
        font.setPointSize(10);
        stackedWidget->setFont(font);
        stackedWidgetPage1 = new QWidget();
        stackedWidgetPage1->setObjectName(QStringLiteral("stackedWidgetPage1"));
        scrollArea = new QScrollArea(stackedWidgetPage1);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setGeometry(QRect(10, 34, 461, 661));
        scrollArea->setLineWidth(3);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 445, 659));
        scrollArea->setWidget(scrollAreaWidgetContents);
        gridLayoutWidget = new QWidget(stackedWidgetPage1);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(30, 0, 411, 31));
        gridLayout_channels = new QGridLayout(gridLayoutWidget);
        gridLayout_channels->setObjectName(QStringLiteral("gridLayout_channels"));
        gridLayout_channels->setContentsMargins(0, 0, 0, 0);
        stackedWidget->addWidget(stackedWidgetPage1);
        scrollArea->raise();
        gridLayoutWidget->raise();

        gridLayout_3->addWidget(stackedWidget, 1, 0, 1, 1);

        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 0, 0, 1, 1);


        gridLayout_4->addWidget(widget, 0, 1, 1, 1);

        asic_reset_2 = new QPushButton(vmm_window);
        asic_reset_2->setObjectName(QStringLiteral("asic_reset_2"));
        asic_reset_2->setEnabled(true);
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(9);
        asic_reset_2->setFont(font1);

        gridLayout_4->addWidget(asic_reset_2, 1, 0, 1, 1);

        asic_reset = new QPushButton(vmm_window);
        asic_reset->setObjectName(QStringLiteral("asic_reset"));
        asic_reset->setEnabled(true);
        asic_reset->setFont(font1);

        gridLayout_4->addWidget(asic_reset, 2, 0, 1, 1);

        tabWidget_4 = new QTabWidget(vmm_window);
        tabWidget_4->setObjectName(QStringLiteral("tabWidget_4"));
        tabWidget_4->setMaximumSize(QSize(320, 16777215));
        tabWidget_4->setFont(font1);
        tabWidget_4Page1 = new QWidget();
        tabWidget_4Page1->setObjectName(QStringLiteral("tabWidget_4Page1"));
        gridLayout_7 = new QGridLayout(tabWidget_4Page1);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        frame = new QFrame(tabWidget_4Page1);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        gridLayout_15 = new QGridLayout(frame);
        gridLayout_15->setObjectName(QStringLiteral("gridLayout_15"));
        pushButton_3 = new QPushButton(frame);
        pushButton_3->setObjectName(QStringLiteral("pushButton_3"));
        pushButton_3->setCheckable(true);

        gridLayout_15->addWidget(pushButton_3, 0, 0, 1, 1);

        pushButton_4 = new QPushButton(frame);
        pushButton_4->setObjectName(QStringLiteral("pushButton_4"));
        pushButton_4->setCheckable(true);

        gridLayout_15->addWidget(pushButton_4, 0, 1, 1, 1);

        pushButton_2 = new QPushButton(frame);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        pushButton_2->setCheckable(true);

        gridLayout_15->addWidget(pushButton_2, 1, 0, 1, 1);

        pushButton_7 = new QPushButton(frame);
        pushButton_7->setObjectName(QStringLiteral("pushButton_7"));
        pushButton_7->setCheckable(true);

        gridLayout_15->addWidget(pushButton_7, 1, 1, 1, 1);


        gridLayout_7->addWidget(frame, 2, 0, 1, 2);

        gridLayout_16 = new QGridLayout();
        gridLayout_16->setObjectName(QStringLiteral("gridLayout_16"));
        sg = new QComboBox(tabWidget_4Page1);
        sg->setObjectName(QStringLiteral("sg"));
        sg->setFont(font1);

        gridLayout_16->addWidget(sg, 2, 1, 1, 1);

        stcLabel = new QLabel(tabWidget_4Page1);
        stcLabel->setObjectName(QStringLiteral("stcLabel"));
        stcLabel->setFont(font1);

        gridLayout_16->addWidget(stcLabel, 3, 0, 1, 1);

        sgLabel = new QLabel(tabWidget_4Page1);
        sgLabel->setObjectName(QStringLiteral("sgLabel"));
        sgLabel->setFont(font1);

        gridLayout_16->addWidget(sgLabel, 2, 0, 1, 1);

        spgLabel = new QLabel(tabWidget_4Page1);
        spgLabel->setObjectName(QStringLiteral("spgLabel"));
        spgLabel->setFont(font1);

        gridLayout_16->addWidget(spgLabel, 0, 0, 1, 1);

        spg = new QComboBox(tabWidget_4Page1);
        spg->setObjectName(QStringLiteral("spg"));
        spg->setFont(font1);

        gridLayout_16->addWidget(spg, 0, 1, 1, 1);

        stc = new QComboBox(tabWidget_4Page1);
        stc->setObjectName(QStringLiteral("stc"));
        stc->setFont(font1);
        stc->setMaxVisibleItems(16);

        gridLayout_16->addWidget(stc, 3, 1, 1, 1);

        sm5_sm0Label = new QLabel(tabWidget_4Page1);
        sm5_sm0Label->setObjectName(QStringLiteral("sm5_sm0Label"));
        sm5_sm0Label->setFont(font1);

        gridLayout_16->addWidget(sm5_sm0Label, 1, 0, 1, 1);

        sm5_sm0 = new QComboBox(tabWidget_4Page1);
        sm5_sm0->setObjectName(QStringLiteral("sm5_sm0"));
        sm5_sm0->setFont(font1);

        gridLayout_16->addWidget(sm5_sm0, 1, 1, 1, 1);


        gridLayout_7->addLayout(gridLayout_16, 0, 0, 1, 2);

        gridLayout_13 = new QGridLayout();
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        sfam_2 = new QComboBox(tabWidget_4Page1);
        sfam_2->setObjectName(QStringLiteral("sfam_2"));
        sfam_2->setFont(font1);

        gridLayout_13->addWidget(sfam_2, 0, 1, 1, 1);

        gridLayout_14 = new QGridLayout();
        gridLayout_14->setObjectName(QStringLiteral("gridLayout_14"));
        sfamLabel_2 = new QLabel(tabWidget_4Page1);
        sfamLabel_2->setObjectName(QStringLiteral("sfamLabel_2"));
        sfamLabel_2->setFont(font1);

        gridLayout_14->addWidget(sfamLabel_2, 0, 0, 1, 1);


        gridLayout_13->addLayout(gridLayout_14, 0, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_13, 1, 0, 1, 2);

        groupBox = new QGroupBox(tabWidget_4Page1);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_10 = new QGridLayout(groupBox);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        pushButton_15 = new QPushButton(groupBox);
        pushButton_15->setObjectName(QStringLiteral("pushButton_15"));
        pushButton_15->setCheckable(true);

        gridLayout_10->addWidget(pushButton_15, 0, 2, 1, 2);

        sc010b = new QComboBox(groupBox);
        sc010b->setObjectName(QStringLiteral("sc010b"));
        sc010b->setFont(font1);

        gridLayout_10->addWidget(sc010b, 1, 1, 1, 1);

        sc010bLabel = new QLabel(groupBox);
        sc010bLabel->setObjectName(QStringLiteral("sc010bLabel"));
        sc010bLabel->setFont(font1);

        gridLayout_10->addWidget(sc010bLabel, 1, 0, 1, 1);

        pushButton = new QPushButton(groupBox);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setCheckable(true);

        gridLayout_10->addWidget(pushButton, 0, 0, 1, 2);

        pushButton_16 = new QPushButton(groupBox);
        pushButton_16->setObjectName(QStringLiteral("pushButton_16"));
        pushButton_16->setCheckable(true);

        gridLayout_10->addWidget(pushButton_16, 0, 4, 1, 2);

        sc06bLabel = new QLabel(groupBox);
        sc06bLabel->setObjectName(QStringLiteral("sc06bLabel"));
        sc06bLabel->setFont(font1);

        gridLayout_10->addWidget(sc06bLabel, 2, 0, 1, 1);

        sc08b = new QComboBox(groupBox);
        sc08b->setObjectName(QStringLiteral("sc08b"));
        sc08b->setFont(font1);

        gridLayout_10->addWidget(sc08b, 1, 4, 1, 1);

        sc08bLabel = new QLabel(groupBox);
        sc08bLabel->setObjectName(QStringLiteral("sc08bLabel"));
        sc08bLabel->setFont(font1);

        gridLayout_10->addWidget(sc08bLabel, 1, 3, 1, 1);

        sc06b = new QComboBox(groupBox);
        sc06b->setObjectName(QStringLiteral("sc06b"));
        sc06b->setFont(font1);

        gridLayout_10->addWidget(sc06b, 2, 1, 1, 1);


        gridLayout_7->addWidget(groupBox, 3, 0, 1, 2);

        groupBox1 = new QGroupBox(tabWidget_4Page1);
        groupBox1->setObjectName(QStringLiteral("groupBox1"));
        gridLayout_11 = new QGridLayout(groupBox1);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        pushButton_9 = new QPushButton(groupBox1);
        pushButton_9->setObjectName(QStringLiteral("pushButton_9"));
        pushButton_9->setCheckable(true);

        gridLayout_11->addWidget(pushButton_9, 0, 0, 1, 1);

        pushButton_18 = new QPushButton(groupBox1);
        pushButton_18->setObjectName(QStringLiteral("pushButton_18"));
        pushButton_18->setCheckable(true);

        gridLayout_11->addWidget(pushButton_18, 0, 1, 1, 1);

        pushButton_19 = new QPushButton(groupBox1);
        pushButton_19->setObjectName(QStringLiteral("pushButton_19"));
        pushButton_19->setCheckable(true);

        gridLayout_11->addWidget(pushButton_19, 0, 2, 1, 1);


        gridLayout_7->addWidget(groupBox1, 4, 0, 1, 2);

        gridGroupBox_4 = new QGroupBox(tabWidget_4Page1);
        gridGroupBox_4->setObjectName(QStringLiteral("gridGroupBox_4"));
        gridLayout_6 = new QGridLayout(gridGroupBox_4);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        sdt = new QSpinBox(gridGroupBox_4);
        sdt->setObjectName(QStringLiteral("sdt"));
        sdt->setFont(font1);
        sdt->setMaximum(1023);
        sdt->setValue(300);

        gridLayout_6->addWidget(sdt, 0, 1, 1, 1);

        sdp_2 = new QSpinBox(gridGroupBox_4);
        sdp_2->setObjectName(QStringLiteral("sdp_2"));
        sdp_2->setFont(font1);
        sdp_2->setMaximum(1023);
        sdp_2->setValue(300);

        gridLayout_6->addWidget(sdp_2, 2, 1, 1, 1);

        sdpLabel_2 = new QLabel(gridGroupBox_4);
        sdpLabel_2->setObjectName(QStringLiteral("sdpLabel_2"));
        sdpLabel_2->setFont(font1);

        gridLayout_6->addWidget(sdpLabel_2, 2, 0, 1, 1);

        sdtLabel = new QLabel(gridGroupBox_4);
        sdtLabel->setObjectName(QStringLiteral("sdtLabel"));
        sdtLabel->setFont(font1);

        gridLayout_6->addWidget(sdtLabel, 0, 0, 1, 1);

        dacmvLabel_TP = new QLabel(gridGroupBox_4);
        dacmvLabel_TP->setObjectName(QStringLiteral("dacmvLabel_TP"));
        QFont font2;
        font2.setPointSize(9);
        dacmvLabel_TP->setFont(font2);

        gridLayout_6->addWidget(dacmvLabel_TP, 0, 2, 1, 1);

        dacmvLabel = new QLabel(gridGroupBox_4);
        dacmvLabel->setObjectName(QStringLiteral("dacmvLabel"));
        dacmvLabel->setFont(font2);

        gridLayout_6->addWidget(dacmvLabel, 2, 2, 1, 1);


        gridLayout_7->addWidget(gridGroupBox_4, 7, 0, 1, 2);

        tabWidget_4->addTab(tabWidget_4Page1, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_8 = new QGridLayout(tab);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridGroupBox_3 = new QGroupBox(tab);
        gridGroupBox_3->setObjectName(QStringLiteral("gridGroupBox_3"));
        gridLayout = new QGridLayout(gridGroupBox_3);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pushButton_stlc = new QPushButton(gridGroupBox_3);
        pushButton_stlc->setObjectName(QStringLiteral("pushButton_stlc"));
        pushButton_stlc->setCheckable(true);

        gridLayout->addWidget(pushButton_stlc, 1, 1, 1, 1);

        pushButton_ssart = new QPushButton(gridGroupBox_3);
        pushButton_ssart->setObjectName(QStringLiteral("pushButton_ssart"));
        pushButton_ssart->setCheckable(true);

        gridLayout->addWidget(pushButton_ssart, 0, 2, 1, 1);

        pushButton_sL0dckinv = new QPushButton(gridGroupBox_3);
        pushButton_sL0dckinv->setObjectName(QStringLiteral("pushButton_sL0dckinv"));
        pushButton_sL0dckinv->setCheckable(true);

        gridLayout->addWidget(pushButton_sL0dckinv, 1, 3, 1, 1);

        pushButton_sL0ckinv = new QPushButton(gridGroupBox_3);
        pushButton_sL0ckinv->setObjectName(QStringLiteral("pushButton_sL0ckinv"));
        pushButton_sL0ckinv->setCheckable(true);

        gridLayout->addWidget(pushButton_sL0ckinv, 0, 3, 1, 1);

        pushButton_srec = new QPushButton(gridGroupBox_3);
        pushButton_srec->setObjectName(QStringLiteral("pushButton_srec"));
        pushButton_srec->setCheckable(true);

        gridLayout->addWidget(pushButton_srec, 1, 2, 1, 1);

        pushButton_slvs = new QPushButton(gridGroupBox_3);
        pushButton_slvs->setObjectName(QStringLiteral("pushButton_slvs"));
        pushButton_slvs->setCheckable(true);

        gridLayout->addWidget(pushButton_slvs, 0, 0, 1, 1);

        pushButton_s32 = new QPushButton(gridGroupBox_3);
        pushButton_s32->setObjectName(QStringLiteral("pushButton_s32"));
        pushButton_s32->setCheckable(true);

        gridLayout->addWidget(pushButton_s32, 1, 0, 1, 1);

        pushButton_stcr = new QPushButton(gridGroupBox_3);
        pushButton_stcr->setObjectName(QStringLiteral("pushButton_stcr"));
        pushButton_stcr->setCheckable(true);

        gridLayout->addWidget(pushButton_stcr, 0, 1, 1, 1);

        pushButton_sbip = new QPushButton(gridGroupBox_3);
        pushButton_sbip->setObjectName(QStringLiteral("pushButton_sbip"));
        pushButton_sbip->setCheckable(true);

        gridLayout->addWidget(pushButton_sbip, 2, 0, 1, 1);

        pushButton_sfrst = new QPushButton(gridGroupBox_3);
        pushButton_sfrst->setObjectName(QStringLiteral("pushButton_sfrst"));
        pushButton_sfrst->setCheckable(true);

        gridLayout->addWidget(pushButton_sfrst, 2, 1, 1, 1);

        pushButton_nskipm_i = new QPushButton(gridGroupBox_3);
        pushButton_nskipm_i->setObjectName(QStringLiteral("pushButton_nskipm_i"));
        pushButton_nskipm_i->setCheckable(true);

        gridLayout->addWidget(pushButton_nskipm_i, 2, 2, 1, 1);


        gridLayout_8->addWidget(gridGroupBox_3, 3, 0, 1, 1);

        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMaximumSize(QSize(400, 16777215));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        pushButton_bc = new QPushButton(groupBox_2);
        pushButton_bc->setObjectName(QStringLiteral("pushButton_bc"));
        pushButton_bc->setCheckable(true);
        pushButton_bc->setChecked(false);

        gridLayout_2->addWidget(pushButton_bc, 0, 0, 1, 1);

        pushButton_tp = new QPushButton(groupBox_2);
        pushButton_tp->setObjectName(QStringLiteral("pushButton_tp"));
        pushButton_tp->setCheckable(true);

        gridLayout_2->addWidget(pushButton_tp, 0, 1, 1, 1);

        pushButton_tk = new QPushButton(groupBox_2);
        pushButton_tk->setObjectName(QStringLiteral("pushButton_tk"));
        pushButton_tk->setCheckable(true);

        gridLayout_2->addWidget(pushButton_tk, 0, 2, 1, 1);

        pushButton_dt = new QPushButton(groupBox_2);
        pushButton_dt->setObjectName(QStringLiteral("pushButton_dt"));
        pushButton_dt->setCheckable(true);

        gridLayout_2->addWidget(pushButton_dt, 0, 3, 1, 1);

        pushButton_art = new QPushButton(groupBox_2);
        pushButton_art->setObjectName(QStringLiteral("pushButton_art"));
        pushButton_art->setCheckable(true);

        gridLayout_2->addWidget(pushButton_art, 1, 0, 1, 1);

        pushButton_tki = new QPushButton(groupBox_2);
        pushButton_tki->setObjectName(QStringLiteral("pushButton_tki"));
        pushButton_tki->setCheckable(true);

        gridLayout_2->addWidget(pushButton_tki, 1, 1, 1, 1);

        pushButton_ena = new QPushButton(groupBox_2);
        pushButton_ena->setObjectName(QStringLiteral("pushButton_ena"));
        pushButton_ena->setCheckable(true);

        gridLayout_2->addWidget(pushButton_ena, 1, 2, 1, 1);

        pushButton_6b = new QPushButton(groupBox_2);
        pushButton_6b->setObjectName(QStringLiteral("pushButton_6b"));
        pushButton_6b->setCheckable(true);

        gridLayout_2->addWidget(pushButton_6b, 1, 3, 1, 1);


        gridLayout_8->addWidget(groupBox_2, 4, 0, 1, 1);

        gridLayout_12 = new QGridLayout();
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        pushButton_8 = new QPushButton(tab);
        pushButton_8->setObjectName(QStringLiteral("pushButton_8"));
        pushButton_8->setCheckable(true);

        gridLayout_12->addWidget(pushButton_8, 1, 0, 1, 1);

        sfam = new QComboBox(tab);
        sfam->setObjectName(QStringLiteral("sfam"));
        sfam->setFont(font1);

        gridLayout_12->addWidget(sfam, 1, 2, 1, 1);

        sfamLabel = new QLabel(tab);
        sfamLabel->setObjectName(QStringLiteral("sfamLabel"));
        sfamLabel->setFont(font1);

        gridLayout_12->addWidget(sfamLabel, 1, 1, 1, 1);

        stLabel = new QLabel(tab);
        stLabel->setObjectName(QStringLiteral("stLabel"));
        stLabel->setFont(font1);

        gridLayout_12->addWidget(stLabel, 2, 0, 1, 1);

        st = new QComboBox(tab);
        st->setObjectName(QStringLiteral("st"));
        st->setFont(font1);

        gridLayout_12->addWidget(st, 2, 1, 1, 1);


        gridLayout_8->addLayout(gridLayout_12, 0, 0, 1, 1);

        gridLayout_5 = new QGridLayout();
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        pushButton_13 = new QPushButton(tab);
        pushButton_13->setObjectName(QStringLiteral("pushButton_13"));
        pushButton_13->setCheckable(true);

        gridLayout_5->addWidget(pushButton_13, 2, 2, 1, 1);

        pushButton_14 = new QPushButton(tab);
        pushButton_14->setObjectName(QStringLiteral("pushButton_14"));
        pushButton_14->setCheckable(true);

        gridLayout_5->addWidget(pushButton_14, 2, 1, 1, 1);

        pushButton_11 = new QPushButton(tab);
        pushButton_11->setObjectName(QStringLiteral("pushButton_11"));
        pushButton_11->setCheckable(true);

        gridLayout_5->addWidget(pushButton_11, 0, 1, 1, 1);

        pushButton_6 = new QPushButton(tab);
        pushButton_6->setObjectName(QStringLiteral("pushButton_6"));
        pushButton_6->setCheckable(true);

        gridLayout_5->addWidget(pushButton_6, 3, 2, 1, 1);

        pushButton_10 = new QPushButton(tab);
        pushButton_10->setObjectName(QStringLiteral("pushButton_10"));
        pushButton_10->setCheckable(true);
        pushButton_10->setAutoDefault(false);

        gridLayout_5->addWidget(pushButton_10, 0, 2, 1, 1);

        pushButton_5 = new QPushButton(tab);
        pushButton_5->setObjectName(QStringLiteral("pushButton_5"));
        pushButton_5->setCheckable(true);

        gridLayout_5->addWidget(pushButton_5, 3, 1, 1, 1);

        pushButton_12 = new QPushButton(tab);
        pushButton_12->setObjectName(QStringLiteral("pushButton_12"));
        pushButton_12->setCheckable(true);

        gridLayout_5->addWidget(pushButton_12, 1, 1, 1, 1);


        gridLayout_8->addLayout(gridLayout_5, 1, 0, 1, 1);

        gridLayout_9 = new QGridLayout();
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        pushButton_17 = new QPushButton(tab);
        pushButton_17->setObjectName(QStringLiteral("pushButton_17"));
        pushButton_17->setCheckable(true);

        gridLayout_9->addWidget(pushButton_17, 1, 0, 1, 1);

        stpp = new QComboBox(tab);
        stpp->setObjectName(QStringLiteral("stpp"));
        stpp->setFont(font1);

        gridLayout_9->addWidget(stpp, 1, 2, 1, 1);

        stot = new QComboBox(tab);
        stot->setObjectName(QStringLiteral("stot"));
        stot->setFont(font1);

        gridLayout_9->addWidget(stot, 1, 3, 1, 1);

        stotLabel = new QLabel(tab);
        stotLabel->setObjectName(QStringLiteral("stotLabel"));
        stotLabel->setFont(font1);

        gridLayout_9->addWidget(stotLabel, 1, 1, 1, 1);


        gridLayout_8->addLayout(gridLayout_9, 2, 0, 1, 1);

        tabWidget_4->addTab(tab, QString());

        gridLayout_4->addWidget(tabWidget_4, 0, 0, 1, 1);


        retranslateUi(vmm_window);

        tabWidget_4->setCurrentIndex(0);
        sg->setCurrentIndex(2);
        spg->setCurrentIndex(0);
        stc->setCurrentIndex(0);
        st->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(vmm_window);
    } // setupUi

    void retranslateUi(QWidget *vmm_window)
    {
        vmm_window->setWindowTitle(QApplication::translate("vmm_window", "Form", 0));
        label->setText(QApplication::translate("vmm_window", "Channel Settings", 0));
        asic_reset_2->setText(QApplication::translate("vmm_window", "Hard Reset", 0));
        asic_reset->setText(QApplication::translate("vmm_window", "Reset VMM3", 0));
        pushButton_3->setText(QApplication::translate("vmm_window", "Neighbor Trigger (sng)", 0));
        pushButton_4->setText(QApplication::translate("vmm_window", "Disable At Peak", 0));
        pushButton_2->setText(QApplication::translate("vmm_window", "Analog tristates", 0));
        pushButton_7->setText(QApplication::translate("vmm_window", "Sub Hysterisis ", 0));
        sg->clear();
        sg->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "0.5 mV/fC", 0)
         << QApplication::translate("vmm_window", "1.0 mV/fC", 0)
         << QApplication::translate("vmm_window", "3.0 mV/fC", 0)
         << QApplication::translate("vmm_window", "4.5 mV/fC", 0)
         << QApplication::translate("vmm_window", "6.0 mV/fC", 0)
         << QApplication::translate("vmm_window", "9.0 mV/fC", 0)
         << QApplication::translate("vmm_window", "12.0 mV/fC", 0)
         << QApplication::translate("vmm_window", "16.0 mV/fC", 0)
        );
        stcLabel->setText(QApplication::translate("vmm_window", "TAC Slop Adj (stc)", 0));
        sgLabel->setText(QApplication::translate("vmm_window", "Gain (sg)", 0));
        spgLabel->setText(QApplication::translate("vmm_window", "Input charge polarity", 0));
        spg->clear();
        spg->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "negative", 0)
         << QApplication::translate("vmm_window", "positive", 0)
        );
        stc->clear();
        stc->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "60 ns", 0)
         << QApplication::translate("vmm_window", "100 ns", 0)
         << QApplication::translate("vmm_window", "350 ns", 0)
         << QApplication::translate("vmm_window", "650 ns", 0)
        );
        sm5_sm0Label->setText(QApplication::translate("vmm_window", "Analog (Channel) Monitor", 0));
        sm5_sm0->clear();
        sm5_sm0->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Pulser DAC", 0)
         << QApplication::translate("vmm_window", "Threshold DAC", 0)
         << QApplication::translate("vmm_window", "Bandgap reference", 0)
         << QApplication::translate("vmm_window", "Temperature sensor", 0)
         << QApplication::translate("vmm_window", "1", 0)
         << QApplication::translate("vmm_window", "2", 0)
         << QApplication::translate("vmm_window", "3", 0)
         << QApplication::translate("vmm_window", "4", 0)
         << QApplication::translate("vmm_window", "5", 0)
         << QApplication::translate("vmm_window", "6", 0)
         << QApplication::translate("vmm_window", "7", 0)
         << QApplication::translate("vmm_window", "8", 0)
         << QApplication::translate("vmm_window", "9", 0)
         << QApplication::translate("vmm_window", "10", 0)
         << QApplication::translate("vmm_window", "11", 0)
         << QApplication::translate("vmm_window", "12", 0)
         << QApplication::translate("vmm_window", "13", 0)
         << QApplication::translate("vmm_window", "14", 0)
         << QApplication::translate("vmm_window", "15", 0)
         << QApplication::translate("vmm_window", "16", 0)
         << QApplication::translate("vmm_window", "17", 0)
         << QApplication::translate("vmm_window", "18", 0)
         << QApplication::translate("vmm_window", "19", 0)
         << QApplication::translate("vmm_window", "20", 0)
         << QApplication::translate("vmm_window", "21", 0)
         << QApplication::translate("vmm_window", "22", 0)
         << QApplication::translate("vmm_window", "23", 0)
         << QApplication::translate("vmm_window", "24", 0)
         << QApplication::translate("vmm_window", "25", 0)
         << QApplication::translate("vmm_window", "26", 0)
         << QApplication::translate("vmm_window", "27", 0)
         << QApplication::translate("vmm_window", "28", 0)
         << QApplication::translate("vmm_window", "29", 0)
         << QApplication::translate("vmm_window", "30", 0)
         << QApplication::translate("vmm_window", "31", 0)
         << QApplication::translate("vmm_window", "32", 0)
         << QApplication::translate("vmm_window", "33", 0)
         << QApplication::translate("vmm_window", "34", 0)
         << QApplication::translate("vmm_window", "35", 0)
         << QApplication::translate("vmm_window", "36", 0)
         << QApplication::translate("vmm_window", "37", 0)
         << QApplication::translate("vmm_window", "38", 0)
         << QApplication::translate("vmm_window", "39", 0)
         << QApplication::translate("vmm_window", "40", 0)
         << QApplication::translate("vmm_window", "41", 0)
         << QApplication::translate("vmm_window", "42", 0)
         << QApplication::translate("vmm_window", "43", 0)
         << QApplication::translate("vmm_window", "44", 0)
         << QApplication::translate("vmm_window", "45", 0)
         << QApplication::translate("vmm_window", "46", 0)
         << QApplication::translate("vmm_window", "47", 0)
         << QApplication::translate("vmm_window", "48", 0)
         << QApplication::translate("vmm_window", "49", 0)
         << QApplication::translate("vmm_window", "50", 0)
         << QApplication::translate("vmm_window", "51", 0)
         << QApplication::translate("vmm_window", "52", 0)
         << QApplication::translate("vmm_window", "53", 0)
         << QApplication::translate("vmm_window", "54", 0)
         << QApplication::translate("vmm_window", "55", 0)
         << QApplication::translate("vmm_window", "56", 0)
         << QApplication::translate("vmm_window", "57", 0)
         << QApplication::translate("vmm_window", "58", 0)
         << QApplication::translate("vmm_window", "59", 0)
         << QApplication::translate("vmm_window", "60", 0)
         << QApplication::translate("vmm_window", "61", 0)
         << QApplication::translate("vmm_window", "62", 0)
         << QApplication::translate("vmm_window", "63", 0)
         << QApplication::translate("vmm_window", "64", 0)
        );
        sfam_2->clear();
        sfam_2->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Timing At Threshold", 0)
         << QApplication::translate("vmm_window", "Timing At Peak", 0)
        );
        sfamLabel_2->setText(QApplication::translate("vmm_window", "SRAT Mode", 0));
        groupBox->setTitle(QApplication::translate("vmm_window", "ADC ", 0));
        pushButton_15->setText(QApplication::translate("vmm_window", "8-bit Conv. Mode", 0));
        sc010b->clear();
        sc010b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "200ns", 0)
         << QApplication::translate("vmm_window", "+60ns", 0)
        );
        sc010bLabel->setText(QApplication::translate("vmm_window", "10b ADC", 0));
        pushButton->setText(QApplication::translate("vmm_window", "ADCs", 0));
        pushButton_16->setText(QApplication::translate("vmm_window", "6-bit", 0));
        sc06bLabel->setText(QApplication::translate("vmm_window", "6b ADC", 0));
        sc08b->clear();
        sc08b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "100ns", 0)
         << QApplication::translate("vmm_window", "+60ns", 0)
        );
        sc08bLabel->setText(QApplication::translate("vmm_window", "8b ADC", 0));
        sc06b->clear();
        sc06b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Low", 0)
         << QApplication::translate("vmm_window", "Middle", 0)
         << QApplication::translate("vmm_window", "Up", 0)
        );
        groupBox1->setTitle(QApplication::translate("vmm_window", "Dual Clock", 0));
        pushButton_9->setText(QApplication::translate("vmm_window", "Dual Clock ART", 0));
        pushButton_18->setText(QApplication::translate("vmm_window", "Dual Clock Data", 0));
        pushButton_19->setText(QApplication::translate("vmm_window", "Dual Clock 6-bit", 0));
        sdpLabel_2->setText(QApplication::translate("vmm_window", "Test Pulse DAC", 0));
        sdtLabel->setText(QApplication::translate("vmm_window", "Threshold DAC", 0));
        dacmvLabel_TP->setText(QApplication::translate("vmm_window", "TextLabel", 0));
        dacmvLabel->setText(QApplication::translate("vmm_window", "TextLabel", 0));
        tabWidget_4->setTabText(tabWidget_4->indexOf(tabWidget_4Page1), QApplication::translate("vmm_window", "General Settings", 0));
        pushButton_stlc->setText(QApplication::translate("vmm_window", "stlc", 0));
        pushButton_ssart->setText(QApplication::translate("vmm_window", "ssart", 0));
        pushButton_sL0dckinv->setText(QApplication::translate("vmm_window", "sL0dckinv", 0));
        pushButton_sL0ckinv->setText(QApplication::translate("vmm_window", "sL0ckinv", 0));
        pushButton_srec->setText(QApplication::translate("vmm_window", "srec", 0));
        pushButton_slvs->setText(QApplication::translate("vmm_window", "slvs", 0));
        pushButton_s32->setText(QApplication::translate("vmm_window", "s32", 0));
        pushButton_stcr->setText(QApplication::translate("vmm_window", "stcr", 0));
        pushButton_sbip->setText(QApplication::translate("vmm_window", "sbip", 0));
        pushButton_sfrst->setText(QApplication::translate("vmm_window", "sfrst", 0));
        pushButton_nskipm_i->setText(QApplication::translate("vmm_window", "nskipm_i", 0));
        groupBox_2->setTitle(QApplication::translate("vmm_window", "SLVS 100 Ohm termination", 0));
        pushButton_bc->setText(QApplication::translate("vmm_window", "bc", 0));
        pushButton_tp->setText(QApplication::translate("vmm_window", "tp", 0));
        pushButton_tk->setText(QApplication::translate("vmm_window", "tk", 0));
        pushButton_dt->setText(QApplication::translate("vmm_window", "dt", 0));
        pushButton_art->setText(QApplication::translate("vmm_window", "art", 0));
        pushButton_tki->setText(QApplication::translate("vmm_window", "tki", 0));
        pushButton_ena->setText(QApplication::translate("vmm_window", "ena", 0));
        pushButton_6b->setText(QApplication::translate("vmm_window", "6b", 0));
        pushButton_8->setText(QApplication::translate("vmm_window", "ART", 0));
        sfam->clear();
        sfam->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Timing At Threshold", 0)
         << QApplication::translate("vmm_window", "Timing At Peak", 0)
        );
        sfamLabel->setText(QApplication::translate("vmm_window", "Mode", 0));
        stLabel->setText(QApplication::translate("vmm_window", "Peak time (st)", 0));
        st->clear();
        st->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "200 ns", 0)
         << QApplication::translate("vmm_window", "100 ns", 0)
         << QApplication::translate("vmm_window", "50 ns", 0)
         << QApplication::translate("vmm_window", "25 ns", 0)
        );
        pushButton_13->setText(QApplication::translate("vmm_window", "SCMX", 0));
        pushButton_14->setText(QApplication::translate("vmm_window", "SBMX", 0));
        pushButton_11->setText(QApplication::translate("vmm_window", "sbft", 0));
        pushButton_6->setText(QApplication::translate("vmm_window", "Double Leak", 0));
        pushButton_10->setText(QApplication::translate("vmm_window", "sbfp", 0));
        pushButton_5->setText(QApplication::translate("vmm_window", "Leak. Curr.", 0));
        pushButton_12->setText(QApplication::translate("vmm_window", "sbfm", 0));
        pushButton_17->setText(QApplication::translate("vmm_window", "Direct Time", 0));
        stpp->clear();
        stpp->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "0", 0)
         << QApplication::translate("vmm_window", "1", 0)
        );
        stot->clear();
        stot->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "0", 0)
         << QApplication::translate("vmm_window", "1", 0)
        );
        stotLabel->setText(QApplication::translate("vmm_window", "             Mode", 0));
        tabWidget_4->setTabText(tabWidget_4->indexOf(tab), QApplication::translate("vmm_window", "Advanced Settings", 0));
    } // retranslateUi

};

namespace Ui {
    class vmm_window: public Ui_vmm_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VMM_WINDOW_H
