/********************************************************************************
** Form generated from reading UI file 'vmm_window.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
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
    QTabWidget *tabWidget_4;
    QWidget *tabWidget_4Page1;
    QGridLayout *gridLayout_7;
    QGroupBox *gridGroupBox_4;
    QGridLayout *gridLayout_6;
    QSpinBox *sdt;
    QSpinBox *sdp_2;
    QLabel *sdpLabel_2;
    QLabel *sdtLabel;
    QLabel *dacmvLabel;
    QLabel *dacmvLabel_TP;
    QFrame *frame;
    QGridLayout *gridLayout_15;
    QPushButton *sng;
    QPushButton *sdp;
    QPushButton *sdrv;
    QPushButton *ssh;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_10;
    QPushButton *s8b;
    QComboBox *sc010b;
    QLabel *sc010bLabel;
    QPushButton *adcs;
    QLabel *sc06bLabel;
    QComboBox *sc06b;
    QLabel *sc08bLabel;
    QComboBox *sc08b;
    QGridLayout *gridLayout_13;
    QComboBox *srat;
    QGridLayout *gridLayout_14;
    QLabel *sratLabel;
    QGroupBox *groupBox1;
    QGridLayout *gridLayout_11;
    QPushButton *sdcka;
    QPushButton *sdcks;
    QPushButton *sdck6b;
    QGridLayout *gridLayout_16;
    QLabel *sgLabel;
    QComboBox *sp;
    QLabel *sm5_sm0Label;
    QLabel *spgLabel;
    QComboBox *sg;
    QComboBox *sm5_sm0;
    QLabel *stcLabel;
    QComboBox *stc;
    QLabel *stLabel;
    QComboBox *st;
    QWidget *tab;
    QGridLayout *gridLayout_8;
    QGridLayout *gridLayout_12;
    QComboBox *sfam;
    QLabel *sfamLabel;
    QPushButton *sttt;
    QComboBox *stpp;
    QComboBox *stot;
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
    QLabel *stotLabel;
    QPushButton *s6b;
    QGridLayout *gridLayout_5;
    QPushButton *sbft;
    QPushButton *sfm;
    QPushButton *sbfp;
    QPushButton *slg;
    QPushButton *sbfm;
    QPushButton *sbmx;
    QWidget *widget;
    QGridLayout *gridLayout_3;
    QLabel *label;
    QStackedWidget *stackedWidget;
    QWidget *stackedWidgetPage1;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout_channels;
    QPushButton *vmm_reset;
    QPushButton *ApplyAll;

    void setupUi(QWidget *vmm_window)
    {
        if (vmm_window->objectName().isEmpty())
            vmm_window->setObjectName(QStringLiteral("vmm_window"));
        vmm_window->resize(900, 813);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(vmm_window->sizePolicy().hasHeightForWidth());
        vmm_window->setSizePolicy(sizePolicy);
        vmm_window->setMinimumSize(QSize(844, 0));
        vmm_window->setMaximumSize(QSize(900, 99999));
        gridLayout_4 = new QGridLayout(vmm_window);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        tabWidget_4 = new QTabWidget(vmm_window);
        tabWidget_4->setObjectName(QStringLiteral("tabWidget_4"));
        tabWidget_4->setMaximumSize(QSize(320, 16777215));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
        tabWidget_4->setFont(font);
        tabWidget_4Page1 = new QWidget();
        tabWidget_4Page1->setObjectName(QStringLiteral("tabWidget_4Page1"));
        gridLayout_7 = new QGridLayout(tabWidget_4Page1);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        gridGroupBox_4 = new QGroupBox(tabWidget_4Page1);
        gridGroupBox_4->setObjectName(QStringLiteral("gridGroupBox_4"));
        gridLayout_6 = new QGridLayout(gridGroupBox_4);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        sdt = new QSpinBox(gridGroupBox_4);
        sdt->setObjectName(QStringLiteral("sdt"));
        sdt->setFont(font);
        sdt->setMaximum(1023);
        sdt->setValue(300);

        gridLayout_6->addWidget(sdt, 0, 1, 1, 1);

        sdp_2 = new QSpinBox(gridGroupBox_4);
        sdp_2->setObjectName(QStringLiteral("sdp_2"));
        sdp_2->setFont(font);
        sdp_2->setMaximum(1023);
        sdp_2->setValue(300);

        gridLayout_6->addWidget(sdp_2, 2, 1, 1, 1);

        sdpLabel_2 = new QLabel(gridGroupBox_4);
        sdpLabel_2->setObjectName(QStringLiteral("sdpLabel_2"));
        sdpLabel_2->setFont(font);

        gridLayout_6->addWidget(sdpLabel_2, 2, 0, 1, 1);

        sdtLabel = new QLabel(gridGroupBox_4);
        sdtLabel->setObjectName(QStringLiteral("sdtLabel"));
        sdtLabel->setFont(font);

        gridLayout_6->addWidget(sdtLabel, 0, 0, 1, 1);

        dacmvLabel = new QLabel(gridGroupBox_4);
        dacmvLabel->setObjectName(QStringLiteral("dacmvLabel"));
        QFont font1;
        font1.setPointSize(9);
        dacmvLabel->setFont(font1);

        gridLayout_6->addWidget(dacmvLabel, 0, 2, 1, 1);

        dacmvLabel_TP = new QLabel(gridGroupBox_4);
        dacmvLabel_TP->setObjectName(QStringLiteral("dacmvLabel_TP"));
        dacmvLabel_TP->setFont(font1);

        gridLayout_6->addWidget(dacmvLabel_TP, 2, 2, 1, 1);


        gridLayout_7->addWidget(gridGroupBox_4, 7, 0, 1, 2);

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
        sc010b->setFont(font);

        gridLayout_10->addWidget(sc010b, 1, 1, 1, 1);

        sc010bLabel = new QLabel(groupBox);
        sc010bLabel->setObjectName(QStringLiteral("sc010bLabel"));
        sc010bLabel->setFont(font);

        gridLayout_10->addWidget(sc010bLabel, 1, 0, 1, 1);

        adcs = new QPushButton(groupBox);
        adcs->setObjectName(QStringLiteral("adcs"));
        adcs->setCheckable(true);

        gridLayout_10->addWidget(adcs, 0, 0, 1, 2);

        sc06bLabel = new QLabel(groupBox);
        sc06bLabel->setObjectName(QStringLiteral("sc06bLabel"));
        sc06bLabel->setFont(font);

        gridLayout_10->addWidget(sc06bLabel, 2, 0, 1, 1);

        sc06b = new QComboBox(groupBox);
        sc06b->setObjectName(QStringLiteral("sc06b"));
        sc06b->setFont(font);

        gridLayout_10->addWidget(sc06b, 2, 1, 1, 1);

        sc08bLabel = new QLabel(groupBox);
        sc08bLabel->setObjectName(QStringLiteral("sc08bLabel"));
        sc08bLabel->setFont(font);

        gridLayout_10->addWidget(sc08bLabel, 1, 2, 1, 1);

        sc08b = new QComboBox(groupBox);
        sc08b->setObjectName(QStringLiteral("sc08b"));
        sc08b->setFont(font);

        gridLayout_10->addWidget(sc08b, 1, 3, 1, 1);


        gridLayout_7->addWidget(groupBox, 3, 0, 1, 2);

        gridLayout_13 = new QGridLayout();
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        srat = new QComboBox(tabWidget_4Page1);
        srat->setObjectName(QStringLiteral("srat"));
        srat->setFont(font);

        gridLayout_13->addWidget(srat, 0, 1, 1, 1);

        gridLayout_14 = new QGridLayout();
        gridLayout_14->setObjectName(QStringLiteral("gridLayout_14"));
        sratLabel = new QLabel(tabWidget_4Page1);
        sratLabel->setObjectName(QStringLiteral("sratLabel"));
        sratLabel->setFont(font);

        gridLayout_14->addWidget(sratLabel, 0, 0, 1, 1);


        gridLayout_13->addLayout(gridLayout_14, 0, 0, 1, 1);


        gridLayout_7->addLayout(gridLayout_13, 1, 0, 1, 2);

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

        gridLayout_16 = new QGridLayout();
        gridLayout_16->setObjectName(QStringLiteral("gridLayout_16"));
        sgLabel = new QLabel(tabWidget_4Page1);
        sgLabel->setObjectName(QStringLiteral("sgLabel"));
        sgLabel->setFont(font);

        gridLayout_16->addWidget(sgLabel, 2, 0, 1, 1);

        sp = new QComboBox(tabWidget_4Page1);
        sp->setObjectName(QStringLiteral("sp"));
        sp->setFont(font);

        gridLayout_16->addWidget(sp, 0, 1, 1, 1);

        sm5_sm0Label = new QLabel(tabWidget_4Page1);
        sm5_sm0Label->setObjectName(QStringLiteral("sm5_sm0Label"));
        sm5_sm0Label->setFont(font);

        gridLayout_16->addWidget(sm5_sm0Label, 1, 0, 1, 1);

        spgLabel = new QLabel(tabWidget_4Page1);
        spgLabel->setObjectName(QStringLiteral("spgLabel"));
        spgLabel->setFont(font);

        gridLayout_16->addWidget(spgLabel, 0, 0, 1, 1);

        sg = new QComboBox(tabWidget_4Page1);
        sg->setObjectName(QStringLiteral("sg"));
        sg->setFont(font);

        gridLayout_16->addWidget(sg, 2, 1, 1, 1);

        sm5_sm0 = new QComboBox(tabWidget_4Page1);
        sm5_sm0->setObjectName(QStringLiteral("sm5_sm0"));
        sm5_sm0->setFont(font);

        gridLayout_16->addWidget(sm5_sm0, 1, 1, 1, 1);

        stcLabel = new QLabel(tabWidget_4Page1);
        stcLabel->setObjectName(QStringLiteral("stcLabel"));
        stcLabel->setFont(font);

        gridLayout_16->addWidget(stcLabel, 3, 0, 1, 1);

        stc = new QComboBox(tabWidget_4Page1);
        stc->setObjectName(QStringLiteral("stc"));
        stc->setFont(font);
        stc->setMaxVisibleItems(16);

        gridLayout_16->addWidget(stc, 3, 1, 1, 1);

        stLabel = new QLabel(tabWidget_4Page1);
        stLabel->setObjectName(QStringLiteral("stLabel"));
        stLabel->setFont(font);

        gridLayout_16->addWidget(stLabel, 4, 0, 1, 1);

        st = new QComboBox(tabWidget_4Page1);
        st->setObjectName(QStringLiteral("st"));
        st->setFont(font);

        gridLayout_16->addWidget(st, 4, 1, 1, 1);


        gridLayout_7->addLayout(gridLayout_16, 0, 0, 1, 2);

        tabWidget_4->addTab(tabWidget_4Page1, QString());
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_8 = new QGridLayout(tab);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        gridLayout_12 = new QGridLayout();
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        sfam = new QComboBox(tab);
        sfam->setObjectName(QStringLiteral("sfam"));
        sfam->setFont(font);

        gridLayout_12->addWidget(sfam, 1, 2, 1, 1);

        sfamLabel = new QLabel(tab);
        sfamLabel->setObjectName(QStringLiteral("sfamLabel"));
        sfamLabel->setFont(font);

        gridLayout_12->addWidget(sfamLabel, 1, 0, 1, 2);


        gridLayout_8->addLayout(gridLayout_12, 0, 0, 1, 5);

        sttt = new QPushButton(tab);
        sttt->setObjectName(QStringLiteral("sttt"));
        sttt->setMinimumSize(QSize(64, 0));
        sttt->setCheckable(true);

        gridLayout_8->addWidget(sttt, 3, 0, 1, 1);

        stpp = new QComboBox(tab);
        stpp->setObjectName(QStringLiteral("stpp"));
        stpp->setFont(font);

        gridLayout_8->addWidget(stpp, 3, 3, 1, 1);

        stot = new QComboBox(tab);
        stot->setObjectName(QStringLiteral("stot"));
        stot->setFont(font);

        gridLayout_8->addWidget(stot, 3, 4, 1, 1);

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


        gridLayout_8->addWidget(gridGroupBox_3, 4, 0, 1, 5);

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


        gridLayout_8->addWidget(groupBox_2, 5, 0, 1, 5);

        stotLabel = new QLabel(tab);
        stotLabel->setObjectName(QStringLiteral("stotLabel"));
        stotLabel->setMaximumSize(QSize(35, 16777215));
        stotLabel->setFont(font);

        gridLayout_8->addWidget(stotLabel, 3, 2, 1, 1);

        s6b = new QPushButton(tab);
        s6b->setObjectName(QStringLiteral("s6b"));
        s6b->setCheckable(true);

        gridLayout_8->addWidget(s6b, 3, 1, 1, 1);

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


        gridLayout_8->addLayout(gridLayout_5, 1, 0, 1, 5);

        tabWidget_4->addTab(tab, QString());

        gridLayout_4->addWidget(tabWidget_4, 0, 0, 1, 1);

        widget = new QWidget(vmm_window);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setMinimumSize(QSize(500, 740));
        gridLayout_3 = new QGridLayout(widget);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label = new QLabel(widget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout_3->addWidget(label, 0, 0, 1, 1);

        stackedWidget = new QStackedWidget(widget);
        stackedWidget->setObjectName(QStringLiteral("stackedWidget"));
        stackedWidget->setMinimumSize(QSize(480, 700));
        stackedWidget->setMaximumSize(QSize(480, 16777215));
        QFont font2;
        font2.setPointSize(10);
        stackedWidget->setFont(font2);
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


        gridLayout_4->addWidget(widget, 0, 1, 1, 1);

        vmm_reset = new QPushButton(vmm_window);
        vmm_reset->setObjectName(QStringLiteral("vmm_reset"));
        vmm_reset->setEnabled(true);
        vmm_reset->setFont(font);

        gridLayout_4->addWidget(vmm_reset, 3, 0, 1, 1);

        ApplyAll = new QPushButton(vmm_window);
        ApplyAll->setObjectName(QStringLiteral("ApplyAll"));

        gridLayout_4->addWidget(ApplyAll, 2, 0, 1, 1);


        retranslateUi(vmm_window);

        tabWidget_4->setCurrentIndex(0);
        sp->setCurrentIndex(0);
        sg->setCurrentIndex(2);
        stc->setCurrentIndex(0);
        st->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(vmm_window);
    } // setupUi

    void retranslateUi(QWidget *vmm_window)
    {
        vmm_window->setWindowTitle(QApplication::translate("vmm_window", "Form", Q_NULLPTR));
        sdpLabel_2->setText(QApplication::translate("vmm_window", "Test Pulse DAC", Q_NULLPTR));
        sdtLabel->setText(QApplication::translate("vmm_window", "Threshold DAC", Q_NULLPTR));
        dacmvLabel->setText(QApplication::translate("vmm_window", "TextLabel", Q_NULLPTR));
        dacmvLabel_TP->setText(QApplication::translate("vmm_window", "TextLabel", Q_NULLPTR));
        sng->setText(QApplication::translate("vmm_window", "Neighbor Trigger (sng)", Q_NULLPTR));
        sdp->setText(QApplication::translate("vmm_window", "Disable At Peak", Q_NULLPTR));
        sdrv->setText(QApplication::translate("vmm_window", "Analog tristates", Q_NULLPTR));
        ssh->setText(QApplication::translate("vmm_window", "Sub Hysterisis ", Q_NULLPTR));
        groupBox->setTitle(QApplication::translate("vmm_window", "ADC ", Q_NULLPTR));
        s8b->setText(QApplication::translate("vmm_window", "8-bit Conv. Mode", Q_NULLPTR));
        sc010b->clear();
        sc010b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "200ns", Q_NULLPTR)
         << QApplication::translate("vmm_window", "+60ns", Q_NULLPTR)
        );
        sc010bLabel->setText(QApplication::translate("vmm_window", "10b ADC", Q_NULLPTR));
        adcs->setText(QApplication::translate("vmm_window", "ADCs", Q_NULLPTR));
        sc06bLabel->setText(QApplication::translate("vmm_window", "6b ADC", Q_NULLPTR));
        sc06b->clear();
        sc06b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Low", Q_NULLPTR)
         << QApplication::translate("vmm_window", "Middle", Q_NULLPTR)
         << QApplication::translate("vmm_window", "Up", Q_NULLPTR)
        );
        sc08bLabel->setText(QApplication::translate("vmm_window", "8b ADC", Q_NULLPTR));
        sc08b->clear();
        sc08b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "100ns", Q_NULLPTR)
         << QApplication::translate("vmm_window", "+60ns", Q_NULLPTR)
        );
        srat->clear();
        srat->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Timing At Peak", Q_NULLPTR)
         << QApplication::translate("vmm_window", "Timing At Threshold", Q_NULLPTR)
        );
        sratLabel->setText(QApplication::translate("vmm_window", "SRAT Mode", Q_NULLPTR));
        groupBox1->setTitle(QApplication::translate("vmm_window", "Dual Clock", Q_NULLPTR));
        sdcka->setText(QApplication::translate("vmm_window", "Dual Clock ART", Q_NULLPTR));
        sdcks->setText(QApplication::translate("vmm_window", "Dual Clock Data", Q_NULLPTR));
        sdck6b->setText(QApplication::translate("vmm_window", "Dual Clock 6-bit", Q_NULLPTR));
        sgLabel->setText(QApplication::translate("vmm_window", "Gain (sg)", Q_NULLPTR));
        sp->clear();
        sp->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "negative", Q_NULLPTR)
         << QApplication::translate("vmm_window", "positive", Q_NULLPTR)
        );
        sm5_sm0Label->setText(QApplication::translate("vmm_window", "Analog (Channel) Monitor", Q_NULLPTR));
        spgLabel->setText(QApplication::translate("vmm_window", "Input charge polarity", Q_NULLPTR));
        sg->clear();
        sg->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "0.5 mV/fC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "1.0 mV/fC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "3.0 mV/fC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "4.5 mV/fC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "6.0 mV/fC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "9.0 mV/fC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "12.0 mV/fC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "16.0 mV/fC", Q_NULLPTR)
        );
        sm5_sm0->clear();
        sm5_sm0->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Pulser DAC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "Threshold DAC", Q_NULLPTR)
         << QApplication::translate("vmm_window", "Bandgap reference", Q_NULLPTR)
         << QApplication::translate("vmm_window", "Temperature sensor", Q_NULLPTR)
         << QApplication::translate("vmm_window", "1", Q_NULLPTR)
         << QApplication::translate("vmm_window", "2", Q_NULLPTR)
         << QApplication::translate("vmm_window", "3", Q_NULLPTR)
         << QApplication::translate("vmm_window", "4", Q_NULLPTR)
         << QApplication::translate("vmm_window", "5", Q_NULLPTR)
         << QApplication::translate("vmm_window", "6", Q_NULLPTR)
         << QApplication::translate("vmm_window", "7", Q_NULLPTR)
         << QApplication::translate("vmm_window", "8", Q_NULLPTR)
         << QApplication::translate("vmm_window", "9", Q_NULLPTR)
         << QApplication::translate("vmm_window", "10", Q_NULLPTR)
         << QApplication::translate("vmm_window", "11", Q_NULLPTR)
         << QApplication::translate("vmm_window", "12", Q_NULLPTR)
         << QApplication::translate("vmm_window", "13", Q_NULLPTR)
         << QApplication::translate("vmm_window", "14", Q_NULLPTR)
         << QApplication::translate("vmm_window", "15", Q_NULLPTR)
         << QApplication::translate("vmm_window", "16", Q_NULLPTR)
         << QApplication::translate("vmm_window", "17", Q_NULLPTR)
         << QApplication::translate("vmm_window", "18", Q_NULLPTR)
         << QApplication::translate("vmm_window", "19", Q_NULLPTR)
         << QApplication::translate("vmm_window", "20", Q_NULLPTR)
         << QApplication::translate("vmm_window", "21", Q_NULLPTR)
         << QApplication::translate("vmm_window", "22", Q_NULLPTR)
         << QApplication::translate("vmm_window", "23", Q_NULLPTR)
         << QApplication::translate("vmm_window", "24", Q_NULLPTR)
         << QApplication::translate("vmm_window", "25", Q_NULLPTR)
         << QApplication::translate("vmm_window", "26", Q_NULLPTR)
         << QApplication::translate("vmm_window", "27", Q_NULLPTR)
         << QApplication::translate("vmm_window", "28", Q_NULLPTR)
         << QApplication::translate("vmm_window", "29", Q_NULLPTR)
         << QApplication::translate("vmm_window", "30", Q_NULLPTR)
         << QApplication::translate("vmm_window", "31", Q_NULLPTR)
         << QApplication::translate("vmm_window", "32", Q_NULLPTR)
         << QApplication::translate("vmm_window", "33", Q_NULLPTR)
         << QApplication::translate("vmm_window", "34", Q_NULLPTR)
         << QApplication::translate("vmm_window", "35", Q_NULLPTR)
         << QApplication::translate("vmm_window", "36", Q_NULLPTR)
         << QApplication::translate("vmm_window", "37", Q_NULLPTR)
         << QApplication::translate("vmm_window", "38", Q_NULLPTR)
         << QApplication::translate("vmm_window", "39", Q_NULLPTR)
         << QApplication::translate("vmm_window", "40", Q_NULLPTR)
         << QApplication::translate("vmm_window", "41", Q_NULLPTR)
         << QApplication::translate("vmm_window", "42", Q_NULLPTR)
         << QApplication::translate("vmm_window", "43", Q_NULLPTR)
         << QApplication::translate("vmm_window", "44", Q_NULLPTR)
         << QApplication::translate("vmm_window", "45", Q_NULLPTR)
         << QApplication::translate("vmm_window", "46", Q_NULLPTR)
         << QApplication::translate("vmm_window", "47", Q_NULLPTR)
         << QApplication::translate("vmm_window", "48", Q_NULLPTR)
         << QApplication::translate("vmm_window", "49", Q_NULLPTR)
         << QApplication::translate("vmm_window", "50", Q_NULLPTR)
         << QApplication::translate("vmm_window", "51", Q_NULLPTR)
         << QApplication::translate("vmm_window", "52", Q_NULLPTR)
         << QApplication::translate("vmm_window", "53", Q_NULLPTR)
         << QApplication::translate("vmm_window", "54", Q_NULLPTR)
         << QApplication::translate("vmm_window", "55", Q_NULLPTR)
         << QApplication::translate("vmm_window", "56", Q_NULLPTR)
         << QApplication::translate("vmm_window", "57", Q_NULLPTR)
         << QApplication::translate("vmm_window", "58", Q_NULLPTR)
         << QApplication::translate("vmm_window", "59", Q_NULLPTR)
         << QApplication::translate("vmm_window", "60", Q_NULLPTR)
         << QApplication::translate("vmm_window", "61", Q_NULLPTR)
         << QApplication::translate("vmm_window", "62", Q_NULLPTR)
         << QApplication::translate("vmm_window", "63", Q_NULLPTR)
         << QApplication::translate("vmm_window", "64", Q_NULLPTR)
        );
        stcLabel->setText(QApplication::translate("vmm_window", "TAC Slop Adj (stc)", Q_NULLPTR));
        stc->clear();
        stc->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "60 ns", Q_NULLPTR)
         << QApplication::translate("vmm_window", "100 ns", Q_NULLPTR)
         << QApplication::translate("vmm_window", "350 ns", Q_NULLPTR)
         << QApplication::translate("vmm_window", "650 ns", Q_NULLPTR)
        );
        stLabel->setText(QApplication::translate("vmm_window", "Peak time (st)", Q_NULLPTR));
        st->clear();
        st->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "200 ns", Q_NULLPTR)
         << QApplication::translate("vmm_window", "100 ns", Q_NULLPTR)
         << QApplication::translate("vmm_window", "50 ns", Q_NULLPTR)
         << QApplication::translate("vmm_window", "25 ns", Q_NULLPTR)
        );
        tabWidget_4->setTabText(tabWidget_4->indexOf(tabWidget_4Page1), QApplication::translate("vmm_window", "General Settings", Q_NULLPTR));
        sfam->clear();
        sfam->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", Q_NULLPTR)
         << QApplication::translate("vmm_window", "Timing At Threshold", Q_NULLPTR)
         << QApplication::translate("vmm_window", "Timing At Peak", Q_NULLPTR)
        );
        sfamLabel->setText(QApplication::translate("vmm_window", "ART-Mode", Q_NULLPTR));
        sttt->setText(QApplication::translate("vmm_window", "Direct Time", Q_NULLPTR));
        stpp->clear();
        stpp->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "0", Q_NULLPTR)
         << QApplication::translate("vmm_window", "1", Q_NULLPTR)
        );
        stot->clear();
        stot->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "0", Q_NULLPTR)
         << QApplication::translate("vmm_window", "1", Q_NULLPTR)
        );
        ssart->setText(QApplication::translate("vmm_window", "ssart", Q_NULLPTR));
        slvs->setText(QApplication::translate("vmm_window", "slvs", Q_NULLPTR));
        stcr->setText(QApplication::translate("vmm_window", "stcr", Q_NULLPTR));
        sL0dckinv->setText(QApplication::translate("vmm_window", "sL0dckinv", Q_NULLPTR));
        s32->setText(QApplication::translate("vmm_window", "s32", Q_NULLPTR));
        stlc->setText(QApplication::translate("vmm_window", "stlc", Q_NULLPTR));
        srec->setText(QApplication::translate("vmm_window", "srec", Q_NULLPTR));
        sbip->setText(QApplication::translate("vmm_window", "sbip", Q_NULLPTR));
        sL0ckinv->setText(QApplication::translate("vmm_window", "sL0ckinv", Q_NULLPTR));
        sfrst->setText(QApplication::translate("vmm_window", "sfrst", Q_NULLPTR));
        nskipm_i->setText(QApplication::translate("vmm_window", "nskipm_i", Q_NULLPTR));
        groupBox_2->setTitle(QApplication::translate("vmm_window", "SLVS 100 Ohm termination", Q_NULLPTR));
        slvsbc->setText(QApplication::translate("vmm_window", "bc", Q_NULLPTR));
        slvstp->setText(QApplication::translate("vmm_window", "tp", Q_NULLPTR));
        slvstk->setText(QApplication::translate("vmm_window", "tk", Q_NULLPTR));
        slvsdt->setText(QApplication::translate("vmm_window", "dt", Q_NULLPTR));
        slvsart->setText(QApplication::translate("vmm_window", "art", Q_NULLPTR));
        slvstki->setText(QApplication::translate("vmm_window", "tki", Q_NULLPTR));
        slvsena->setText(QApplication::translate("vmm_window", "ena", Q_NULLPTR));
        slvs6b->setText(QApplication::translate("vmm_window", "6b", Q_NULLPTR));
        stotLabel->setText(QApplication::translate("vmm_window", "Mode", Q_NULLPTR));
        s6b->setText(QApplication::translate("vmm_window", "6-bit", Q_NULLPTR));
        sbft->setText(QApplication::translate("vmm_window", "SBFT", Q_NULLPTR));
        sfm->setText(QApplication::translate("vmm_window", "Double Leak", Q_NULLPTR));
        sbfp->setText(QApplication::translate("vmm_window", "SBFP", Q_NULLPTR));
        slg->setText(QApplication::translate("vmm_window", "Leak. Curr.", Q_NULLPTR));
        sbfm->setText(QApplication::translate("vmm_window", "SBFM", Q_NULLPTR));
        sbmx->setText(QApplication::translate("vmm_window", "SBMX", Q_NULLPTR));
        tabWidget_4->setTabText(tabWidget_4->indexOf(tab), QApplication::translate("vmm_window", "Advanced Settings", Q_NULLPTR));
        label->setText(QApplication::translate("vmm_window", "Channel Settings", Q_NULLPTR));
        vmm_reset->setText(QApplication::translate("vmm_window", "Hard Reset", Q_NULLPTR));
        ApplyAll->setText(QApplication::translate("vmm_window", "Apply to all VMMs", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class vmm_window: public Ui_vmm_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VMM_WINDOW_H
