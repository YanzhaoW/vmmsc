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
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_vmm_window
{
public:
    QPushButton *pushButton;
    QGroupBox *groupBox_4;
    QCheckBox *useMapping;
    QPushButton *loadConfigXMLFileButton;
    QPushButton *writeConfigXMLFileButton;
    QPushButton *loadDAQXMLFile;
    QPushButton *writeDAQXMLFile;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *spgLabel;
    QComboBox *spg;
    QLabel *sdrvLabel;
    QComboBox *sdrv;
    QHBoxLayout *horizontalLayout_2;
    QLabel *sgLabel;
    QComboBox *sg;
    QLabel *stcLabel;
    QComboBox *stc;
    QHBoxLayout *horizontalLayout_6;
    QLabel *sngLabel;
    QComboBox *sng;
    QLabel *sdpLabel;
    QComboBox *sdp;
    QHBoxLayout *horizontalLayout_3;
    QLabel *slgLabel;
    QComboBox *slg;
    QLabel *sfmLabel;
    QComboBox *sfm;
    QHBoxLayout *horizontalLayout_7;
    QLabel *stLabel;
    QComboBox *st;
    QLabel *sshLabel;
    QComboBox *ssh;
    QHBoxLayout *horizontalLayout_9;
    QLabel *sfaLabel;
    QComboBox *sfa;
    QLabel *sfamLabel;
    QComboBox *sfam;
    QLabel *sdckaLabel;
    QComboBox *sdcka;
    QHBoxLayout *horizontalLayout_5;
    QLabel *sbfmLabel;
    QComboBox *sbfm;
    QLabel *sbfpLabel;
    QComboBox *sbfp;
    QLabel *sbftLabel;
    QComboBox *sbft;
    QHBoxLayout *horizontalLayout_8;
    QLabel *sm5_sm0Label;
    QComboBox *sm5_sm0;
    QLabel *scmxLabel;
    QComboBox *scmx;
    QLabel *sbmxLabel;
    QComboBox *sbmx;
    QWidget *layoutWidget_2;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_14;
    QLabel *spdcLabel;
    QComboBox *spdc;
    QHBoxLayout *horizontalLayout_10;
    QLabel *stttLabel;
    QComboBox *sttt;
    QLabel *stotLabel;
    QComboBox *stpp;
    QComboBox *stot;
    QHBoxLayout *horizontalLayout_11;
    QLabel *s8bLabel;
    QComboBox *s8b;
    QLabel *s6bLabel;
    QComboBox *s6b;
    QHBoxLayout *horizontalLayout_12;
    QLabel *sc010bLabel;
    QComboBox *sc010b;
    QLabel *sc08bLabel;
    QComboBox *sc08b;
    QLabel *sc06bLabel;
    QComboBox *sc06b;
    QHBoxLayout *horizontalLayout_13;
    QLabel *sdcksLabel;
    QComboBox *sdcks;
    QLabel *sdck6bLabel;
    QComboBox *sdck6b;
    QHBoxLayout *horizontalLayout_16;
    QLabel *sdtLabel;
    QSpinBox *sdt;
    QLabel *dacmvLabel;
    QHBoxLayout *horizontalLayout_15;
    QLabel *sdpLabel_2;
    QSpinBox *sdp_2;
    QLabel *dacmvLabel_TP;
    QCheckBox *doMonitoring;
    QTabWidget *tabWidget;
    QWidget *tab_3;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents;
    QWidget *gridLayoutWidget;
    QGridLayout *gridLayout_channels;

    void setupUi(QWidget *vmm_window)
    {
        if (vmm_window->objectName().isEmpty())
            vmm_window->setObjectName(QStringLiteral("vmm_window"));
        vmm_window->resize(1474, 1114);
        pushButton = new QPushButton(vmm_window);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(1330, 520, 80, 22));
        groupBox_4 = new QGroupBox(vmm_window);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        groupBox_4->setGeometry(QRect(10, 20, 391, 571));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
        groupBox_4->setFont(font);
        useMapping = new QCheckBox(groupBox_4);
        useMapping->setObjectName(QStringLiteral("useMapping"));
        useMapping->setEnabled(true);
        useMapping->setGeometry(QRect(10, 520, 96, 20));
        QFont font1;
        font1.setPointSize(8);
        useMapping->setFont(font1);
        useMapping->setChecked(false);
        loadConfigXMLFileButton = new QPushButton(groupBox_4);
        loadConfigXMLFileButton->setObjectName(QStringLiteral("loadConfigXMLFileButton"));
        loadConfigXMLFileButton->setGeometry(QRect(120, 530, 81, 21));
        QFont font2;
        font2.setPointSize(9);
        loadConfigXMLFileButton->setFont(font2);
        writeConfigXMLFileButton = new QPushButton(groupBox_4);
        writeConfigXMLFileButton->setObjectName(QStringLiteral("writeConfigXMLFileButton"));
        writeConfigXMLFileButton->setGeometry(QRect(210, 530, 81, 21));
        writeConfigXMLFileButton->setFont(font2);
        loadDAQXMLFile = new QPushButton(groupBox_4);
        loadDAQXMLFile->setObjectName(QStringLiteral("loadDAQXMLFile"));
        loadDAQXMLFile->setGeometry(QRect(110, 500, 91, 21));
        loadDAQXMLFile->setFont(font1);
        writeDAQXMLFile = new QPushButton(groupBox_4);
        writeDAQXMLFile->setObjectName(QStringLiteral("writeDAQXMLFile"));
        writeDAQXMLFile->setGeometry(QRect(210, 500, 91, 21));
        writeDAQXMLFile->setFont(font1);
        layoutWidget = new QWidget(groupBox_4);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 33, 378, 241));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        spgLabel = new QLabel(layoutWidget);
        spgLabel->setObjectName(QStringLiteral("spgLabel"));
        spgLabel->setFont(font);

        horizontalLayout->addWidget(spgLabel);

        spg = new QComboBox(layoutWidget);
        spg->setObjectName(QStringLiteral("spg"));
        spg->setFont(font);

        horizontalLayout->addWidget(spg);

        sdrvLabel = new QLabel(layoutWidget);
        sdrvLabel->setObjectName(QStringLiteral("sdrvLabel"));
        sdrvLabel->setFont(font);

        horizontalLayout->addWidget(sdrvLabel);

        sdrv = new QComboBox(layoutWidget);
        sdrv->setObjectName(QStringLiteral("sdrv"));
        sdrv->setFont(font);

        horizontalLayout->addWidget(sdrv);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        sgLabel = new QLabel(layoutWidget);
        sgLabel->setObjectName(QStringLiteral("sgLabel"));
        sgLabel->setFont(font);

        horizontalLayout_2->addWidget(sgLabel);

        sg = new QComboBox(layoutWidget);
        sg->setObjectName(QStringLiteral("sg"));
        sg->setFont(font);

        horizontalLayout_2->addWidget(sg);

        stcLabel = new QLabel(layoutWidget);
        stcLabel->setObjectName(QStringLiteral("stcLabel"));
        stcLabel->setFont(font);

        horizontalLayout_2->addWidget(stcLabel);

        stc = new QComboBox(layoutWidget);
        stc->setObjectName(QStringLiteral("stc"));
        stc->setFont(font);
        stc->setMaxVisibleItems(16);

        horizontalLayout_2->addWidget(stc);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        sngLabel = new QLabel(layoutWidget);
        sngLabel->setObjectName(QStringLiteral("sngLabel"));
        sngLabel->setFont(font);

        horizontalLayout_6->addWidget(sngLabel);

        sng = new QComboBox(layoutWidget);
        sng->setObjectName(QStringLiteral("sng"));
        sng->setFont(font);

        horizontalLayout_6->addWidget(sng);

        sdpLabel = new QLabel(layoutWidget);
        sdpLabel->setObjectName(QStringLiteral("sdpLabel"));
        sdpLabel->setFont(font);

        horizontalLayout_6->addWidget(sdpLabel);

        sdp = new QComboBox(layoutWidget);
        sdp->setObjectName(QStringLiteral("sdp"));
        sdp->setFont(font);

        horizontalLayout_6->addWidget(sdp);


        verticalLayout->addLayout(horizontalLayout_6);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        slgLabel = new QLabel(layoutWidget);
        slgLabel->setObjectName(QStringLiteral("slgLabel"));
        slgLabel->setFont(font);

        horizontalLayout_3->addWidget(slgLabel);

        slg = new QComboBox(layoutWidget);
        slg->setObjectName(QStringLiteral("slg"));
        slg->setFont(font);

        horizontalLayout_3->addWidget(slg);

        sfmLabel = new QLabel(layoutWidget);
        sfmLabel->setObjectName(QStringLiteral("sfmLabel"));
        sfmLabel->setFont(font);

        horizontalLayout_3->addWidget(sfmLabel);

        sfm = new QComboBox(layoutWidget);
        sfm->setObjectName(QStringLiteral("sfm"));
        sfm->setFont(font);

        horizontalLayout_3->addWidget(sfm);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        stLabel = new QLabel(layoutWidget);
        stLabel->setObjectName(QStringLiteral("stLabel"));
        stLabel->setFont(font);

        horizontalLayout_7->addWidget(stLabel);

        st = new QComboBox(layoutWidget);
        st->setObjectName(QStringLiteral("st"));
        st->setFont(font);

        horizontalLayout_7->addWidget(st);

        sshLabel = new QLabel(layoutWidget);
        sshLabel->setObjectName(QStringLiteral("sshLabel"));
        sshLabel->setFont(font);

        horizontalLayout_7->addWidget(sshLabel);

        ssh = new QComboBox(layoutWidget);
        ssh->setObjectName(QStringLiteral("ssh"));
        ssh->setFont(font);

        horizontalLayout_7->addWidget(ssh);


        verticalLayout->addLayout(horizontalLayout_7);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        sfaLabel = new QLabel(layoutWidget);
        sfaLabel->setObjectName(QStringLiteral("sfaLabel"));
        sfaLabel->setFont(font);

        horizontalLayout_9->addWidget(sfaLabel);

        sfa = new QComboBox(layoutWidget);
        sfa->setObjectName(QStringLiteral("sfa"));
        sfa->setFont(font);

        horizontalLayout_9->addWidget(sfa);

        sfamLabel = new QLabel(layoutWidget);
        sfamLabel->setObjectName(QStringLiteral("sfamLabel"));
        sfamLabel->setFont(font);

        horizontalLayout_9->addWidget(sfamLabel);

        sfam = new QComboBox(layoutWidget);
        sfam->setObjectName(QStringLiteral("sfam"));
        sfam->setFont(font);

        horizontalLayout_9->addWidget(sfam);

        sdckaLabel = new QLabel(layoutWidget);
        sdckaLabel->setObjectName(QStringLiteral("sdckaLabel"));
        sdckaLabel->setFont(font);

        horizontalLayout_9->addWidget(sdckaLabel);

        sdcka = new QComboBox(layoutWidget);
        sdcka->setObjectName(QStringLiteral("sdcka"));
        sdcka->setFont(font);

        horizontalLayout_9->addWidget(sdcka);


        verticalLayout->addLayout(horizontalLayout_9);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        sbfmLabel = new QLabel(layoutWidget);
        sbfmLabel->setObjectName(QStringLiteral("sbfmLabel"));
        sbfmLabel->setFont(font);

        horizontalLayout_5->addWidget(sbfmLabel);

        sbfm = new QComboBox(layoutWidget);
        sbfm->setObjectName(QStringLiteral("sbfm"));
        sbfm->setFont(font);
        sbfm->setFocusPolicy(Qt::StrongFocus);

        horizontalLayout_5->addWidget(sbfm);

        sbfpLabel = new QLabel(layoutWidget);
        sbfpLabel->setObjectName(QStringLiteral("sbfpLabel"));
        sbfpLabel->setFont(font);

        horizontalLayout_5->addWidget(sbfpLabel);

        sbfp = new QComboBox(layoutWidget);
        sbfp->setObjectName(QStringLiteral("sbfp"));
        sbfp->setFont(font);

        horizontalLayout_5->addWidget(sbfp);

        sbftLabel = new QLabel(layoutWidget);
        sbftLabel->setObjectName(QStringLiteral("sbftLabel"));
        sbftLabel->setFont(font);

        horizontalLayout_5->addWidget(sbftLabel);

        sbft = new QComboBox(layoutWidget);
        sbft->setObjectName(QStringLiteral("sbft"));
        sbft->setFont(font);

        horizontalLayout_5->addWidget(sbft);


        verticalLayout->addLayout(horizontalLayout_5);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        sm5_sm0Label = new QLabel(layoutWidget);
        sm5_sm0Label->setObjectName(QStringLiteral("sm5_sm0Label"));
        sm5_sm0Label->setFont(font);

        horizontalLayout_8->addWidget(sm5_sm0Label);

        sm5_sm0 = new QComboBox(layoutWidget);
        sm5_sm0->setObjectName(QStringLiteral("sm5_sm0"));
        sm5_sm0->setFont(font);

        horizontalLayout_8->addWidget(sm5_sm0);

        scmxLabel = new QLabel(layoutWidget);
        scmxLabel->setObjectName(QStringLiteral("scmxLabel"));
        scmxLabel->setFont(font);

        horizontalLayout_8->addWidget(scmxLabel);

        scmx = new QComboBox(layoutWidget);
        scmx->setObjectName(QStringLiteral("scmx"));
        scmx->setFont(font);

        horizontalLayout_8->addWidget(scmx);

        sbmxLabel = new QLabel(layoutWidget);
        sbmxLabel->setObjectName(QStringLiteral("sbmxLabel"));
        sbmxLabel->setFont(font);

        horizontalLayout_8->addWidget(sbmxLabel);

        sbmx = new QComboBox(layoutWidget);
        sbmx->setObjectName(QStringLiteral("sbmx"));
        sbmx->setFont(font);

        horizontalLayout_8->addWidget(sbmx);


        verticalLayout->addLayout(horizontalLayout_8);

        layoutWidget_2 = new QWidget(groupBox_4);
        layoutWidget_2->setObjectName(QStringLiteral("layoutWidget_2"));
        layoutWidget_2->setGeometry(QRect(10, 280, 361, 211));
        verticalLayout_2 = new QVBoxLayout(layoutWidget_2);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        spdcLabel = new QLabel(layoutWidget_2);
        spdcLabel->setObjectName(QStringLiteral("spdcLabel"));
        spdcLabel->setFont(font);

        horizontalLayout_14->addWidget(spdcLabel);

        spdc = new QComboBox(layoutWidget_2);
        spdc->setObjectName(QStringLiteral("spdc"));
        spdc->setFont(font);

        horizontalLayout_14->addWidget(spdc);


        verticalLayout_2->addLayout(horizontalLayout_14);

        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        stttLabel = new QLabel(layoutWidget_2);
        stttLabel->setObjectName(QStringLiteral("stttLabel"));
        stttLabel->setFont(font);

        horizontalLayout_10->addWidget(stttLabel);

        sttt = new QComboBox(layoutWidget_2);
        sttt->setObjectName(QStringLiteral("sttt"));
        sttt->setFont(font);

        horizontalLayout_10->addWidget(sttt);

        stotLabel = new QLabel(layoutWidget_2);
        stotLabel->setObjectName(QStringLiteral("stotLabel"));
        stotLabel->setFont(font);

        horizontalLayout_10->addWidget(stotLabel);

        stpp = new QComboBox(layoutWidget_2);
        stpp->setObjectName(QStringLiteral("stpp"));
        stpp->setFont(font);

        horizontalLayout_10->addWidget(stpp);

        stot = new QComboBox(layoutWidget_2);
        stot->setObjectName(QStringLiteral("stot"));
        stot->setFont(font);

        horizontalLayout_10->addWidget(stot);


        verticalLayout_2->addLayout(horizontalLayout_10);

        horizontalLayout_11 = new QHBoxLayout();
        horizontalLayout_11->setObjectName(QStringLiteral("horizontalLayout_11"));
        s8bLabel = new QLabel(layoutWidget_2);
        s8bLabel->setObjectName(QStringLiteral("s8bLabel"));
        s8bLabel->setFont(font);

        horizontalLayout_11->addWidget(s8bLabel);

        s8b = new QComboBox(layoutWidget_2);
        s8b->setObjectName(QStringLiteral("s8b"));
        s8b->setFont(font);

        horizontalLayout_11->addWidget(s8b);

        s6bLabel = new QLabel(layoutWidget_2);
        s6bLabel->setObjectName(QStringLiteral("s6bLabel"));
        s6bLabel->setFont(font);

        horizontalLayout_11->addWidget(s6bLabel);

        s6b = new QComboBox(layoutWidget_2);
        s6b->setObjectName(QStringLiteral("s6b"));
        s6b->setFont(font);

        horizontalLayout_11->addWidget(s6b);


        verticalLayout_2->addLayout(horizontalLayout_11);

        horizontalLayout_12 = new QHBoxLayout();
        horizontalLayout_12->setObjectName(QStringLiteral("horizontalLayout_12"));
        sc010bLabel = new QLabel(layoutWidget_2);
        sc010bLabel->setObjectName(QStringLiteral("sc010bLabel"));
        sc010bLabel->setFont(font);

        horizontalLayout_12->addWidget(sc010bLabel);

        sc010b = new QComboBox(layoutWidget_2);
        sc010b->setObjectName(QStringLiteral("sc010b"));
        sc010b->setFont(font);

        horizontalLayout_12->addWidget(sc010b);

        sc08bLabel = new QLabel(layoutWidget_2);
        sc08bLabel->setObjectName(QStringLiteral("sc08bLabel"));
        sc08bLabel->setFont(font);

        horizontalLayout_12->addWidget(sc08bLabel);

        sc08b = new QComboBox(layoutWidget_2);
        sc08b->setObjectName(QStringLiteral("sc08b"));
        sc08b->setFont(font);

        horizontalLayout_12->addWidget(sc08b);

        sc06bLabel = new QLabel(layoutWidget_2);
        sc06bLabel->setObjectName(QStringLiteral("sc06bLabel"));
        sc06bLabel->setFont(font);

        horizontalLayout_12->addWidget(sc06bLabel);

        sc06b = new QComboBox(layoutWidget_2);
        sc06b->setObjectName(QStringLiteral("sc06b"));
        sc06b->setFont(font);

        horizontalLayout_12->addWidget(sc06b);


        verticalLayout_2->addLayout(horizontalLayout_12);

        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        sdcksLabel = new QLabel(layoutWidget_2);
        sdcksLabel->setObjectName(QStringLiteral("sdcksLabel"));
        sdcksLabel->setFont(font);

        horizontalLayout_13->addWidget(sdcksLabel);

        sdcks = new QComboBox(layoutWidget_2);
        sdcks->setObjectName(QStringLiteral("sdcks"));
        sdcks->setFont(font);

        horizontalLayout_13->addWidget(sdcks);

        sdck6bLabel = new QLabel(layoutWidget_2);
        sdck6bLabel->setObjectName(QStringLiteral("sdck6bLabel"));
        sdck6bLabel->setFont(font);

        horizontalLayout_13->addWidget(sdck6bLabel);

        sdck6b = new QComboBox(layoutWidget_2);
        sdck6b->setObjectName(QStringLiteral("sdck6b"));
        sdck6b->setFont(font);

        horizontalLayout_13->addWidget(sdck6b);


        verticalLayout_2->addLayout(horizontalLayout_13);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        sdtLabel = new QLabel(layoutWidget_2);
        sdtLabel->setObjectName(QStringLiteral("sdtLabel"));
        sdtLabel->setFont(font);

        horizontalLayout_16->addWidget(sdtLabel);

        sdt = new QSpinBox(layoutWidget_2);
        sdt->setObjectName(QStringLiteral("sdt"));
        sdt->setFont(font);
        sdt->setMaximum(1023);
        sdt->setValue(300);

        horizontalLayout_16->addWidget(sdt);

        dacmvLabel = new QLabel(layoutWidget_2);
        dacmvLabel->setObjectName(QStringLiteral("dacmvLabel"));
        dacmvLabel->setFont(font2);

        horizontalLayout_16->addWidget(dacmvLabel);


        verticalLayout_2->addLayout(horizontalLayout_16);

        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        sdpLabel_2 = new QLabel(layoutWidget_2);
        sdpLabel_2->setObjectName(QStringLiteral("sdpLabel_2"));
        sdpLabel_2->setFont(font);

        horizontalLayout_15->addWidget(sdpLabel_2);

        sdp_2 = new QSpinBox(layoutWidget_2);
        sdp_2->setObjectName(QStringLiteral("sdp_2"));
        sdp_2->setFont(font);
        sdp_2->setMaximum(1023);
        sdp_2->setValue(300);

        horizontalLayout_15->addWidget(sdp_2);

        dacmvLabel_TP = new QLabel(layoutWidget_2);
        dacmvLabel_TP->setObjectName(QStringLiteral("dacmvLabel_TP"));
        dacmvLabel_TP->setFont(font2);

        horizontalLayout_15->addWidget(dacmvLabel_TP);


        verticalLayout_2->addLayout(horizontalLayout_15);

        doMonitoring = new QCheckBox(groupBox_4);
        doMonitoring->setObjectName(QStringLiteral("doMonitoring"));
        doMonitoring->setEnabled(true);
        doMonitoring->setGeometry(QRect(10, 540, 96, 20));
        doMonitoring->setFont(font1);
        doMonitoring->setChecked(false);
        layoutWidget_2->raise();
        layoutWidget->raise();
        useMapping->raise();
        loadConfigXMLFileButton->raise();
        writeConfigXMLFileButton->raise();
        loadDAQXMLFile->raise();
        writeDAQXMLFile->raise();
        doMonitoring->raise();
        tabWidget = new QTabWidget(vmm_window);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        tabWidget->setGeometry(QRect(430, 20, 611, 701));
        QFont font3;
        font3.setPointSize(10);
        tabWidget->setFont(font3);
        tabWidget->setTabPosition(QTabWidget::North);
        tabWidget->setIconSize(QSize(16, 16));
        tabWidget->setElideMode(Qt::ElideRight);
        tabWidget->setDocumentMode(true);
        tabWidget->setMovable(false);
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        scrollArea = new QScrollArea(tab_3);
        scrollArea->setObjectName(QStringLiteral("scrollArea"));
        scrollArea->setGeometry(QRect(10, 34, 570, 621));
        scrollArea->setLineWidth(3);
        scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContentsOnFirstShow);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QStringLiteral("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 554, 619));
        scrollArea->setWidget(scrollAreaWidgetContents);
        gridLayoutWidget = new QWidget(tab_3);
        gridLayoutWidget->setObjectName(QStringLiteral("gridLayoutWidget"));
        gridLayoutWidget->setGeometry(QRect(10, 0, 571, 31));
        gridLayout_channels = new QGridLayout(gridLayoutWidget);
        gridLayout_channels->setObjectName(QStringLiteral("gridLayout_channels"));
        gridLayout_channels->setContentsMargins(0, 0, 0, 0);
        tabWidget->addTab(tab_3, QString());

        retranslateUi(vmm_window);

        spg->setCurrentIndex(0);
        sdrv->setCurrentIndex(0);
        sg->setCurrentIndex(2);
        stc->setCurrentIndex(0);
        sng->setCurrentIndex(0);
        slg->setCurrentIndex(0);
        sfm->setCurrentIndex(1);
        st->setCurrentIndex(0);
        sfa->setCurrentIndex(1);
        sdcka->setCurrentIndex(0);
        sbfm->setCurrentIndex(0);
        sbfp->setCurrentIndex(0);
        sbft->setCurrentIndex(0);
        spdc->setCurrentIndex(1);
        s8b->setCurrentIndex(1);
        sdcks->setCurrentIndex(0);
        sdck6b->setCurrentIndex(0);
        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(vmm_window);
    } // setupUi

    void retranslateUi(QWidget *vmm_window)
    {
        vmm_window->setWindowTitle(QApplication::translate("vmm_window", "Form", 0));
        pushButton->setText(QApplication::translate("vmm_window", "PushButton", 0));
        groupBox_4->setTitle(QApplication::translate("vmm_window", "Global Registers", 0));
        useMapping->setText(QApplication::translate("vmm_window", "Use Mapping", 0));
        loadConfigXMLFileButton->setText(QApplication::translate("vmm_window", "Load Config", 0));
        writeConfigXMLFileButton->setText(QApplication::translate("vmm_window", "Write Config", 0));
        loadDAQXMLFile->setText(QApplication::translate("vmm_window", "Load DAQ Config", 0));
        writeDAQXMLFile->setText(QApplication::translate("vmm_window", "Write DAQ Config", 0));
        spgLabel->setText(QApplication::translate("vmm_window", "Ch. polarity", 0));
        spg->clear();
        spg->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "negative", 0)
         << QApplication::translate("vmm_window", "positive", 0)
        );
        sdrvLabel->setText(QApplication::translate("vmm_window", "Analog tristates", 0));
        sdrv->clear();
        sdrv->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sgLabel->setText(QApplication::translate("vmm_window", "Gain (sg)", 0));
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
        stc->clear();
        stc->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "125 ns", 0)
         << QApplication::translate("vmm_window", "250 ns", 0)
         << QApplication::translate("vmm_window", "500 ns", 0)
         << QApplication::translate("vmm_window", "1000 ns", 0)
        );
        sngLabel->setText(QApplication::translate("vmm_window", "Neighbor Trigger (sng)", 0));
        sng->clear();
        sng->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sdpLabel->setText(QApplication::translate("vmm_window", "Disable At Peak", 0));
        sdp->clear();
        sdp->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        slgLabel->setText(QApplication::translate("vmm_window", "Leak. Curr.", 0));
        slg->clear();
        slg->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Enabled", 0)
         << QApplication::translate("vmm_window", "Disabled", 0)
        );
        sfmLabel->setText(QApplication::translate("vmm_window", "Double Leak", 0));
        sfm->clear();
        sfm->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        stLabel->setText(QApplication::translate("vmm_window", "Peak time (st)", 0));
        st->clear();
        st->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "200 ns", 0)
         << QApplication::translate("vmm_window", "100 ns", 0)
         << QApplication::translate("vmm_window", "50 ns", 0)
         << QApplication::translate("vmm_window", "25 ns", 0)
        );
        sshLabel->setText(QApplication::translate("vmm_window", "Sub Hysterisis ", 0));
        ssh->clear();
        ssh->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sfaLabel->setText(QApplication::translate("vmm_window", "ART", 0));
        sfa->clear();
        sfa->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sfamLabel->setText(QApplication::translate("vmm_window", "Mode", 0));
        sfam->clear();
        sfam->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Timing At Threshold", 0)
         << QApplication::translate("vmm_window", "Timing At Peak", 0)
        );
        sdckaLabel->setText(QApplication::translate("vmm_window", "Dual Clock", 0));
        sdcka->clear();
        sdcka->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sbfmLabel->setText(QApplication::translate("vmm_window", "sbfm", 0));
        sbfm->clear();
        sbfm->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sbfpLabel->setText(QApplication::translate("vmm_window", "sbfp", 0));
        sbfp->clear();
        sbfp->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sbftLabel->setText(QApplication::translate("vmm_window", "sbft", 0));
        sbft->clear();
        sbft->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sm5_sm0Label->setText(QApplication::translate("vmm_window", "Ch. Mon", 0));
        sm5_sm0->clear();
        sm5_sm0->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "1 (P DAC)", 0)
         << QApplication::translate("vmm_window", "2 (Th DAC)", 0)
         << QApplication::translate("vmm_window", "3 (BG V)", 0)
         << QApplication::translate("vmm_window", "4 (T sens)", 0)
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
        scmxLabel->setText(QApplication::translate("vmm_window", "SCMX", 0));
        scmx->clear();
        scmx->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On (ch)", 0)
        );
        sbmxLabel->setText(QApplication::translate("vmm_window", "SBMX", 0));
        sbmx->clear();
        sbmx->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        spdcLabel->setText(QApplication::translate("vmm_window", "ADCs", 0));
        spdc->clear();
        spdc->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Disable", 0)
         << QApplication::translate("vmm_window", "Enable", 0)
        );
        stttLabel->setText(QApplication::translate("vmm_window", "Direct Time", 0));
        sttt->clear();
        sttt->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        stotLabel->setText(QApplication::translate("vmm_window", "Mode", 0));
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
        s8bLabel->setText(QApplication::translate("vmm_window", "8-bit Conv. Mode", 0));
        s8b->clear();
        s8b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        s6bLabel->setText(QApplication::translate("vmm_window", "6-bit", 0));
        s6b->clear();
        s6b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sc010bLabel->setText(QApplication::translate("vmm_window", "10b ADC", 0));
        sc010b->clear();
        sc010b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "200ns", 0)
         << QApplication::translate("vmm_window", "+60ns", 0)
        );
        sc08bLabel->setText(QApplication::translate("vmm_window", "8b ADC", 0));
        sc08b->clear();
        sc08b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "100ns", 0)
         << QApplication::translate("vmm_window", "+60ns", 0)
        );
        sc06bLabel->setText(QApplication::translate("vmm_window", "6b ADC", 0));
        sc06b->clear();
        sc06b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Low", 0)
         << QApplication::translate("vmm_window", "Middle", 0)
         << QApplication::translate("vmm_window", "Up", 0)
        );
        sdcksLabel->setText(QApplication::translate("vmm_window", "Dual Clock Data", 0));
        sdcks->clear();
        sdcks->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sdck6bLabel->setText(QApplication::translate("vmm_window", "Dual Clock 6-bit", 0));
        sdck6b->clear();
        sdck6b->insertItems(0, QStringList()
         << QApplication::translate("vmm_window", "Off", 0)
         << QApplication::translate("vmm_window", "On", 0)
        );
        sdtLabel->setText(QApplication::translate("vmm_window", "Threshold DAC", 0));
        dacmvLabel->setText(QApplication::translate("vmm_window", "TextLabel", 0));
        sdpLabel_2->setText(QApplication::translate("vmm_window", "Test Pulse DAC", 0));
        dacmvLabel_TP->setText(QApplication::translate("vmm_window", "TextLabel", 0));
        doMonitoring->setText(QApplication::translate("vmm_window", "Monitoring", 0));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("vmm_window", "Page", 0));
    } // retranslateUi

};

namespace Ui {
    class vmm_window: public Ui_vmm_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VMM_WINDOW_H
