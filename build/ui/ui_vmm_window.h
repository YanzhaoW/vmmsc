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
    QPushButton *sng;
    QPushButton *sdp;
    QPushButton *sdrv;
    QPushButton *ssh;
    QGridLayout *gridLayout_16;
    QComboBox *sg;
    QLabel *stcLabel;
    QLabel *sgLabel;
    QLabel *spgLabel;
    QComboBox *sp;
    QComboBox *stc;
    QLabel *sm5_sm0Label;
    QComboBox *sm5_sm0;
    QGridLayout *gridLayout_13;
    QComboBox *srat;
    QGridLayout *gridLayout_14;
    QLabel *sratLabel;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_10;
    QPushButton *s8b;
    QComboBox *sc010b;
    QLabel *sc010bLabel;
    QPushButton *adcs;
    QPushButton *s6b;
    QLabel *sc06bLabel;
    QComboBox *sc08b;
    QLabel *sc08bLabel;
    QComboBox *sc06b;
    QGroupBox *groupBox1;
    QGridLayout *gridLayout_11;
    QPushButton *sdcka;
    QPushButton *sdcks;
    QPushButton *sdck6b;
    QGroupBox *gridGroupBox_4;
    QGridLayout *gridLayout_6;
    QSpinBox *sdt;
    QSpinBox *sdp_2;
    QLabel *sdpLabel_2;
    QLabel *sdtLabel;
    QLabel *dacmvLabel;
    QLabel *dacmvLabel_TP;
    QWidget *tab;
    QGridLayout *gridLayout_8;
    QGroupBox *gridGroupBox_3;
    QGridLayout *gridLayout;
    QPushButton *ssart;
    QPushButton *slvs;
    QPushButton *stcr;
    QPushButton *sL0dckinv;
    QPushButton *s32;
    QPushButton *stlc;
    QPushButton *srec;
    QPushButton *sbip;
    QPushButton *sL0ckinv;
    QPushButton *sfrst;
    QPushButton *nskipm_i;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QPushButton *slvsbc;
    QPushButton *slvstp;
    QPushButton *slvstk;
    QPushButton *slvsdt;
    QPushButton *slvsart;
    QPushButton *slvstki;
    QPushButton *slvsena;
    QPushButton *slvs6b;
    QGridLayout *gridLayout_12;
    QPushButton *ART;
    QComboBox *sfam;
    QLabel *sfamLabel;
    QLabel *stLabel;
    QComboBox *st;
    QGridLayout *gridLayout_5;
    QPushButton *sbft;
    QPushButton *sfm;
    QPushButton *sbfp;
    QPushButton *slg;
    QPushButton *sbfm;
    QPushButton *sbmx;
    QGridLayout *gridLayout_9;
    QPushButton *sttt;
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
        sng = new QPushButton(frame);
        sng->setObjectName(QStringLiteral("sng"));
        sng->setCheckable(true);

        gridLayout_15->addWidget(sng, 0, 0, 1, 1);

        sdp = new QPushButton(frame);
        sdp->setObjectName(QStringLiteral("sdp"));
        sdp->setCheckable(true);

        gridLayout_15->addWidget(sdp, 0, 1, 1, 1);

        sdrv = new QPushButton(frame);
        sdrv->setObjectName(QStringLiteral("sdrv"));
        sdrv->setCheckable(true);

        gridLayout_15->addWidget(sdrv, 1, 0, 1, 1);

        ssh = new QPushButton(frame);
        ssh->setObjectName(QStringLiteral("ssh"));
        ssh->setCheckable(true);

        gridLayout_15->addWidget(ssh, 1, 1, 1, 1);


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

        sp = new QComboBox(tabWidget_4Page1);
        sp->setObjectName(QStringLiteral("sp"));
        sp->setFont(font1);

        gridLayout_16->addWidget(sp, 0, 1, 1, 1);

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
        srat = new QComboBox(tabWidget_4Page1);
        srat->setObjectName(QStringLiteral("srat"));
        srat->setFont(font1);

        gridLayout_13->addWidget(srat, 0, 1, 1, 1);

        gridLayout_14 = new QGridLayout();
        gridLayout_14->setObjectName(QStringLiteral("gridLayout_14"));
        sratLabel = new QLabel(tabWidget_4Page1);
        sratLabel->setObjectName(QStringLiteral("sratLabel"));
        sratLabel->setFont(font1);

        gridLayout_14->addWidget(sratLabel, 0, 0, 1, 1);


        gridLayout_13->addLayout(gridLayout_14, 0, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_13, 1, 0, 1, 2);

        groupBox = new QGroupBox(tabWidget_4Page1);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_10 = new QGridLayout(groupBox);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        s8b = new QPushButton(groupBox);
        s8b->setObjectName(QStringLiteral("s8b"));
        s8b->setCheckable(true);

        gridLayout_10->addWidget(s8b, 0, 2, 1, 2);

        sc010b = new QComboBox(groupBox);
        sc010b->setObjectName(QStringLiteral("sc010b"));
        sc010b->setFont(font1);

        gridLayout_10->addWidget(sc010b, 1, 1, 1, 1);

        sc010bLabel = new QLabel(groupBox);
        sc010bLabel->setObjectName(QStringLiteral("sc010bLabel"));
        sc010bLabel->setFont(font1);

        gridLayout_10->addWidget(sc010bLabel, 1, 0, 1, 1);

        adcs = new QPushButton(groupBox);
        adcs->setObjectName(QStringLiteral("adcs"));
        adcs->setCheckable(true);

        gridLayout_10->addWidget(adcs, 0, 0, 1, 2);

        s6b = new QPushButton(groupBox);
        s6b->setObjectName(QStringLiteral("s6b"));
        s6b->setCheckable(true);

        gridLayout_10->addWidget(s6b, 0, 4, 1, 2);

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
        sdcka = new QPushButton(groupBox1);
        sdcka->setObjectName(QStringLiteral("sdcka"));
        sdcka->setCheckable(true);

        gridLayout_11->addWidget(sdcka, 0, 0, 1, 1);

        sdcks = new QPushButton(groupBox1);
        sdcks->setObjectName(QStringLiteral("sdcks"));
        sdcks->setCheckable(true);

        gridLayout_11->addWidget(sdcks, 0, 1, 1, 1);

        sdck6b = new QPushButton(groupBox1);
        sdck6b->setObjectName(QStringLiteral("sdck6b"));
        sdck6b->setCheckable(true);

        gridLayout_11->addWidget(sdck6b, 0, 2, 1, 1);


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

        dacmvLabel = new QLabel(gridGroupBox_4);
        dacmvLabel->setObjectName(QStringLiteral("dacmvLabel"));
        QFont font2;
        font2.setPointSize(9);
        dacmvLabel->setFont(font2);

        gridLayout_6->addWidget(dacmvLabel, 0, 2, 1, 1);

        dacmvLabel_TP = new QLabel(gridGroupBox_4);
        dacmvLabel_TP->setObjectName(QStringLiteral("dacmvLabel_TP"));
        dacmvLabel_TP->setFont(font2);

        gridLayout_6->addWidget(dacmvLabel_TP, 2, 2, 1, 1);


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
        ssart = new QPushButton(gridGroupBox_3);
        ssart->setObjectName(QStringLiteral("ssart"));
        ssart->setCheckable(true);

        gridLayout->addWidget(ssart, 0, 2, 1, 1);

        slvs = new QPushButton(gridGroupBox_3);
        slvs->setObjectName(QStringLiteral("slvs"));
        slvs->setCheckable(true);

        gridLayout->addWidget(slvs, 0, 0, 1, 1);

        stcr = new QPushButton(gridGroupBox_3);
        stcr->setObjectName(QStringLiteral("stcr"));
        stcr->setCheckable(true);

        gridLayout->addWidget(stcr, 0, 1, 1, 1);

        sL0dckinv = new QPushButton(gridGroupBox_3);
        sL0dckinv->setObjectName(QStringLiteral("sL0dckinv"));
        sL0dckinv->setCheckable(true);

        gridLayout->addWidget(sL0dckinv, 2, 3, 1, 1);

        s32 = new QPushButton(gridGroupBox_3);
        s32->setObjectName(QStringLiteral("s32"));
        s32->setCheckable(true);

        gridLayout->addWidget(s32, 0, 3, 1, 1);

        stlc = new QPushButton(gridGroupBox_3);
        stlc->setObjectName(QStringLiteral("stlc"));
        stlc->setCheckable(true);

        gridLayout->addWidget(stlc, 1, 0, 1, 1);

        srec = new QPushButton(gridGroupBox_3);
        srec->setObjectName(QStringLiteral("srec"));
        srec->setCheckable(true);

        gridLayout->addWidget(srec, 1, 1, 1, 1);

        sbip = new QPushButton(gridGroupBox_3);
        sbip->setObjectName(QStringLiteral("sbip"));
        sbip->setCheckable(true);

        gridLayout->addWidget(sbip, 1, 2, 1, 1);

        sL0ckinv = new QPushButton(gridGroupBox_3);
        sL0ckinv->setObjectName(QStringLiteral("sL0ckinv"));
        sL0ckinv->setCheckable(true);

        gridLayout->addWidget(sL0ckinv, 2, 2, 1, 1);

        sfrst = new QPushButton(gridGroupBox_3);
        sfrst->setObjectName(QStringLiteral("sfrst"));
        sfrst->setCheckable(true);

        gridLayout->addWidget(sfrst, 1, 3, 1, 1);

        nskipm_i = new QPushButton(gridGroupBox_3);
        nskipm_i->setObjectName(QStringLiteral("nskipm_i"));
        nskipm_i->setCheckable(true);

        gridLayout->addWidget(nskipm_i, 2, 0, 1, 1);


        gridLayout_8->addWidget(gridGroupBox_3, 3, 0, 1, 1);

        groupBox_2 = new QGroupBox(tab);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setMaximumSize(QSize(400, 16777215));
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        slvsbc = new QPushButton(groupBox_2);
        slvsbc->setObjectName(QStringLiteral("slvsbc"));
        slvsbc->setCheckable(true);
        slvsbc->setChecked(false);

        gridLayout_2->addWidget(slvsbc, 0, 0, 1, 1);

        slvstp = new QPushButton(groupBox_2);
        slvstp->setObjectName(QStringLiteral("slvstp"));
        slvstp->setCheckable(true);

        gridLayout_2->addWidget(slvstp, 0, 1, 1, 1);

        slvstk = new QPushButton(groupBox_2);
        slvstk->setObjectName(QStringLiteral("slvstk"));
        slvstk->setCheckable(true);

        gridLayout_2->addWidget(slvstk, 0, 2, 1, 1);

        slvsdt = new QPushButton(groupBox_2);
        slvsdt->setObjectName(QStringLiteral("slvsdt"));
        slvsdt->setCheckable(true);

        gridLayout_2->addWidget(slvsdt, 0, 3, 1, 1);

        slvsart = new QPushButton(groupBox_2);
        slvsart->setObjectName(QStringLiteral("slvsart"));
        slvsart->setCheckable(true);

        gridLayout_2->addWidget(slvsart, 1, 0, 1, 1);

        slvstki = new QPushButton(groupBox_2);
        slvstki->setObjectName(QStringLiteral("slvstki"));
        slvstki->setCheckable(true);

        gridLayout_2->addWidget(slvstki, 1, 1, 1, 1);

        slvsena = new QPushButton(groupBox_2);
        slvsena->setObjectName(QStringLiteral("slvsena"));
        slvsena->setCheckable(true);

        gridLayout_2->addWidget(slvsena, 1, 2, 1, 1);

        slvs6b = new QPushButton(groupBox_2);
        slvs6b->setObjectName(QStringLiteral("slvs6b"));
        slvs6b->setCheckable(true);

        gridLayout_2->addWidget(slvs6b, 1, 3, 1, 1);


        gridLayout_8->addWidget(groupBox_2, 4, 0, 1, 1);

        gridLayout_12 = new QGridLayout();
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        ART = new QPushButton(tab);
        ART->setObjectName(QStringLiteral("ART"));
        ART->setCheckable(true);

        gridLayout_12->addWidget(ART, 1, 0, 1, 1);

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
        sbft = new QPushButton(tab);
        sbft->setObjectName(QStringLiteral("sbft"));
        sbft->setCheckable(true);

        gridLayout_5->addWidget(sbft, 0, 1, 1, 1);

        sfm = new QPushButton(tab);
        sfm->setObjectName(QStringLiteral("sfm"));
        sfm->setCheckable(true);

        gridLayout_5->addWidget(sfm, 3, 2, 1, 1);

        sbfp = new QPushButton(tab);
        sbfp->setObjectName(QStringLiteral("sbfp"));
        sbfp->setCheckable(true);
        sbfp->setAutoDefault(false);

        gridLayout_5->addWidget(sbfp, 0, 2, 1, 1);

        slg = new QPushButton(tab);
        slg->setObjectName(QStringLiteral("slg"));
        slg->setCheckable(true);

        gridLayout_5->addWidget(slg, 3, 1, 1, 1);

        sbfm = new QPushButton(tab);
        sbfm->setObjectName(QStringLiteral("sbfm"));
        sbfm->setCheckable(true);

        gridLayout_5->addWidget(sbfm, 1, 1, 1, 1);

        sbmx = new QPushButton(tab);
        sbmx->setObjectName(QStringLiteral("sbmx"));
        sbmx->setCheckable(true);

        gridLayout_5->addWidget(sbmx, 1, 2, 1, 1);


        gridLayout_8->addLayout(gridLayout_5, 1, 0, 1, 1);

        gridLayout_9 = new QGridLayout();
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        sttt = new QPushButton(tab);
        sttt->setObjectName(QStringLiteral("sttt"));
        sttt->setCheckable(true);

        gridLayout_9->addWidget(sttt, 1, 0, 1, 1);

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
        sp->setCurrentIndex(0);
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
        sng->setText(QApplication::translate("vmm_window", "Neighbor Trigger (sng)", 0));
        sdp->setText(QApplication::translate("vmm_window", "Disable At Peak", 0));
        sdrv->setText(QApplication::translate("vmm_window", "Analog tristates", 0));
        ssh->setText(QApplication::translate("vmm_window", "Sub Hysterisis ", 0));
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
        sp->clear();
        sp->insertItems(0, QStringList()
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
        srat->clear();
        srat->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Timing At Threshold", 0)
         << QApplication::translate("vmm_window", "Timing At Peak", 0)
        );
        sratLabel->setText(QApplication::translate("vmm_window", "SRAT Mode", 0));
        groupBox->setTitle(QApplication::translate("vmm_window", "ADC ", 0));
        s8b->setText(QApplication::translate("vmm_window", "8-bit Conv. Mode", 0));
        sc010b->clear();
        sc010b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "200ns", 0)
         << QApplication::translate("vmm_window", "+60ns", 0)
        );
        sc010bLabel->setText(QApplication::translate("vmm_window", "10b ADC", 0));
        adcs->setText(QApplication::translate("vmm_window", "ADCs", 0));
        s6b->setText(QApplication::translate("vmm_window", "6-bit", 0));
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
        sdcka->setText(QApplication::translate("vmm_window", "Dual Clock ART", 0));
        sdcks->setText(QApplication::translate("vmm_window", "Dual Clock Data", 0));
        sdck6b->setText(QApplication::translate("vmm_window", "Dual Clock 6-bit", 0));
        sdpLabel_2->setText(QApplication::translate("vmm_window", "Test Pulse DAC", 0));
        sdtLabel->setText(QApplication::translate("vmm_window", "Threshold DAC", 0));
        dacmvLabel->setText(QApplication::translate("vmm_window", "TextLabel", 0));
        dacmvLabel_TP->setText(QApplication::translate("vmm_window", "TextLabel", 0));
        tabWidget_4->setTabText(tabWidget_4->indexOf(tabWidget_4Page1), QApplication::translate("vmm_window", "General Settings", 0));
        ssart->setText(QApplication::translate("vmm_window", "ssart", 0));
        slvs->setText(QApplication::translate("vmm_window", "slvs", 0));
        stcr->setText(QApplication::translate("vmm_window", "stcr", 0));
        sL0dckinv->setText(QApplication::translate("vmm_window", "sL0dckinv", 0));
        s32->setText(QApplication::translate("vmm_window", "s32", 0));
        stlc->setText(QApplication::translate("vmm_window", "stlc", 0));
        srec->setText(QApplication::translate("vmm_window", "srec", 0));
        sbip->setText(QApplication::translate("vmm_window", "sbip", 0));
        sL0ckinv->setText(QApplication::translate("vmm_window", "sL0ckinv", 0));
        sfrst->setText(QApplication::translate("vmm_window", "sfrst", 0));
        nskipm_i->setText(QApplication::translate("vmm_window", "nskipm_i", 0));
        groupBox_2->setTitle(QApplication::translate("vmm_window", "SLVS 100 Ohm termination", 0));
        slvsbc->setText(QApplication::translate("vmm_window", "bc", 0));
        slvstp->setText(QApplication::translate("vmm_window", "tp", 0));
        slvstk->setText(QApplication::translate("vmm_window", "tk", 0));
        slvsdt->setText(QApplication::translate("vmm_window", "dt", 0));
        slvsart->setText(QApplication::translate("vmm_window", "art", 0));
        slvstki->setText(QApplication::translate("vmm_window", "tki", 0));
        slvsena->setText(QApplication::translate("vmm_window", "ena", 0));
        slvs6b->setText(QApplication::translate("vmm_window", "6b", 0));
        ART->setText(QApplication::translate("vmm_window", "ART", 0));
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
        sbft->setText(QApplication::translate("vmm_window", "SBFT", 0));
        sfm->setText(QApplication::translate("vmm_window", "Double Leak", 0));
        sbfp->setText(QApplication::translate("vmm_window", "SBFP", 0));
        slg->setText(QApplication::translate("vmm_window", "Leak. Curr.", 0));
        sbfm->setText(QApplication::translate("vmm_window", "SBFM", 0));
        sbmx->setText(QApplication::translate("vmm_window", "SBMX", 0));
        sttt->setText(QApplication::translate("vmm_window", "Direct Time", 0));
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
