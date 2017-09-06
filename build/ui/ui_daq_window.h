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
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_daq_window
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
    QTabWidget *tabWidget_2;
    QWidget *tab;
    QGridLayout *gridLayout_5;
    QTabWidget *tabWidget;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_9;
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
    QWidget *widget_2;
    QGridLayout *gridLayout_8;
    QLabel *connectionLabel_3;
    QPushButton *Send;
    QPushButton *openConnection_2;
    QSpacerItem *verticalSpacer;
    QLabel *connectionLabel_2;
    QPushButton *reset_warnings;
    QWidget *tab_2;
    QGroupBox *runGroupbox;
    QLineEdit *userComments_2;
    QLabel *label_5;
    QLabel *label_11;
    QSpinBox *runNumber_2;
    QLabel *label_12;
    QLineEdit *runDirectoryField_2;
    QSpinBox *angle_2;
    QLabel *label_14;
    QPushButton *stopTriggerCnt_2;
    QPushButton *clearTriggerCnt_2;
    QPushButton *checkTriggers_2;
    QFrame *frame_5;
    QLineEdit *eventCountStop_2;
    QLabel *eventCountStopLabel_2;
    QLabel *label_15;
    QLabel *triggerCntLabel_fpga_2;
    QLabel *label_16;
    QLabel *triggerCntLabel_2;
    QFrame *frame_6;
    QLineEdit *runStatusField_2;
    QFrame *frame_7;
    QPushButton *selectDir_2;
    QFrame *frame_8;
    QCheckBox *ignore16_2;
    QCheckBox *enableDebugPB_2;
    QCheckBox *readTrigger_2;
    QCheckBox *writeData_2;
    QWidget *widget;
    QGridLayout *gridLayout;
    QPushButton *pushButton;
    QLabel *connectionLabel;
    QPushButton *openConnection;
    QGroupBox *groupBox_2;
    QWidget *horizontalLayoutWidget_5;
    QHBoxLayout *horizontalLayout_25;
    QRadioButton *readRB_3;
    QRadioButton *writeRB_3;
    QWidget *horizontalLayoutWidget_6;
    QGridLayout *gridLayout_4;
    QRadioButton *fecRB_3;
    QRadioButton *s6RB_3;
    QRadioButton *spiRB_3;
    QRadioButton *appRB_3;
    QComboBox *setVMMs_3;
    QPushButton *SendConfiguration_3;
    QLabel *cmdlabel_3;
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
    QWidget *tab_3;
    QGridLayout *gridLayout_10;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_11;
    QTextEdit *loggingScreen;
    QPushButton *logBottom;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_6;
    QPushButton *Button_load;
    QLineEdit *line_configFile;
    QPushButton *Button_save;
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
        tabWidget_2 = new QTabWidget(centralwidget);
        tabWidget_2->setObjectName(QStringLiteral("tabWidget_2"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_5 = new QGridLayout(tab);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        tabWidget = new QTabWidget(tab);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        gridLayout_5->addWidget(tabWidget, 0, 2, 1, 1);

        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setMinimumSize(QSize(100, 600));
        gridLayout_9 = new QGridLayout(groupBox);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        widget_3 = new QWidget(groupBox);
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


        gridLayout_9->addWidget(widget_3, 3, 0, 1, 1);

        widget_2 = new QWidget(groupBox);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setMaximumSize(QSize(16777215, 155));
        gridLayout_8 = new QGridLayout(widget_2);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        connectionLabel_3 = new QLabel(widget_2);
        connectionLabel_3->setObjectName(QStringLiteral("connectionLabel_3"));
        connectionLabel_3->setMinimumSize(QSize(0, 30));
        connectionLabel_3->setMaximumSize(QSize(16777215, 22));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
        connectionLabel_3->setFont(font);

        gridLayout_8->addWidget(connectionLabel_3, 3, 0, 1, 1);

        Send = new QPushButton(widget_2);
        Send->setObjectName(QStringLiteral("Send"));

        gridLayout_8->addWidget(Send, 2, 0, 1, 1);

        openConnection_2 = new QPushButton(widget_2);
        openConnection_2->setObjectName(QStringLiteral("openConnection_2"));
        openConnection_2->setMinimumSize(QSize(0, 22));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(8);
        openConnection_2->setFont(font1);

        gridLayout_8->addWidget(openConnection_2, 0, 0, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_8->addItem(verticalSpacer, 5, 0, 2, 1);

        connectionLabel_2 = new QLabel(widget_2);
        connectionLabel_2->setObjectName(QStringLiteral("connectionLabel_2"));
        connectionLabel_2->setMinimumSize(QSize(0, 20));
        connectionLabel_2->setMaximumSize(QSize(16777215, 16777215));
        connectionLabel_2->setFont(font);

        gridLayout_8->addWidget(connectionLabel_2, 1, 0, 1, 1);

        reset_warnings = new QPushButton(widget_2);
        reset_warnings->setObjectName(QStringLiteral("reset_warnings"));

        gridLayout_8->addWidget(reset_warnings, 4, 0, 1, 1);


        gridLayout_9->addWidget(widget_2, 0, 0, 1, 1);


        gridLayout_5->addWidget(groupBox, 0, 1, 1, 1);

        tabWidget_2->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        runGroupbox = new QGroupBox(tab_2);
        runGroupbox->setObjectName(QStringLiteral("runGroupbox"));
        runGroupbox->setGeometry(QRect(50, 630, 911, 121));
        runGroupbox->setFont(font);
        userComments_2 = new QLineEdit(runGroupbox);
        userComments_2->setObjectName(QStringLiteral("userComments_2"));
        userComments_2->setEnabled(true);
        userComments_2->setGeometry(QRect(79, 84, 151, 21));
        userComments_2->setFont(font);
        userComments_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_5 = new QLabel(runGroupbox);
        label_5->setObjectName(QStringLiteral("label_5"));
        label_5->setGeometry(QRect(11, 27, 81, 16));
        QFont font2;
        font2.setPointSize(9);
        label_5->setFont(font2);
        label_11 = new QLabel(runGroupbox);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(11, 86, 61, 16));
        label_11->setFont(font2);
        runNumber_2 = new QSpinBox(runGroupbox);
        runNumber_2->setObjectName(QStringLiteral("runNumber_2"));
        runNumber_2->setGeometry(QRect(85, 23, 62, 25));
        runNumber_2->setFont(font);
        runNumber_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        runNumber_2->setMaximum(1000000);
        runNumber_2->setValue(0);
        label_12 = new QLabel(runGroupbox);
        label_12->setObjectName(QStringLiteral("label_12"));
        label_12->setGeometry(QRect(11, 57, 61, 16));
        label_12->setFont(font2);
        runDirectoryField_2 = new QLineEdit(runGroupbox);
        runDirectoryField_2->setObjectName(QStringLiteral("runDirectoryField_2"));
        runDirectoryField_2->setEnabled(true);
        runDirectoryField_2->setGeometry(QRect(68, 55, 111, 21));
        runDirectoryField_2->setFont(font);
        runDirectoryField_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        angle_2 = new QSpinBox(runGroupbox);
        angle_2->setObjectName(QStringLiteral("angle_2"));
        angle_2->setGeometry(QRect(185, 23, 39, 25));
        angle_2->setFont(font);
        angle_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        angle_2->setMinimum(-90);
        angle_2->setMaximum(90);
        angle_2->setValue(0);
        label_14 = new QLabel(runGroupbox);
        label_14->setObjectName(QStringLiteral("label_14"));
        label_14->setGeometry(QRect(146, 27, 41, 16));
        label_14->setFont(font2);
        stopTriggerCnt_2 = new QPushButton(runGroupbox);
        stopTriggerCnt_2->setObjectName(QStringLiteral("stopTriggerCnt_2"));
        stopTriggerCnt_2->setGeometry(QRect(344, 62, 81, 21));
        stopTriggerCnt_2->setFont(font);
        clearTriggerCnt_2 = new QPushButton(runGroupbox);
        clearTriggerCnt_2->setObjectName(QStringLiteral("clearTriggerCnt_2"));
        clearTriggerCnt_2->setGeometry(QRect(242, 24, 101, 16));
        clearTriggerCnt_2->setFont(font);
        checkTriggers_2 = new QPushButton(runGroupbox);
        checkTriggers_2->setObjectName(QStringLiteral("checkTriggers_2"));
        checkTriggers_2->setGeometry(QRect(250, 62, 81, 21));
        checkTriggers_2->setFont(font);
        frame_5 = new QFrame(runGroupbox);
        frame_5->setObjectName(QStringLiteral("frame_5"));
        frame_5->setGeometry(QRect(240, 20, 191, 41));
        frame_5->setFrameShape(QFrame::StyledPanel);
        frame_5->setFrameShadow(QFrame::Raised);
        eventCountStop_2 = new QLineEdit(frame_5);
        eventCountStop_2->setObjectName(QStringLiteral("eventCountStop_2"));
        eventCountStop_2->setGeometry(QRect(53, 22, 51, 16));
        eventCountStop_2->setFont(font);
        eventCountStop_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        eventCountStopLabel_2 = new QLabel(frame_5);
        eventCountStopLabel_2->setObjectName(QStringLiteral("eventCountStopLabel_2"));
        eventCountStopLabel_2->setGeometry(QRect(0, 20, 51, 16));
        eventCountStopLabel_2->setFont(font1);
        label_15 = new QLabel(frame_5);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(110, 0, 36, 16));
        label_15->setFont(font2);
        triggerCntLabel_fpga_2 = new QLabel(frame_5);
        triggerCntLabel_fpga_2->setObjectName(QStringLiteral("triggerCntLabel_fpga_2"));
        triggerCntLabel_fpga_2->setGeometry(QRect(154, 0, 31, 16));
        triggerCntLabel_fpga_2->setFont(font);
        triggerCntLabel_fpga_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_16 = new QLabel(frame_5);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(110, 20, 36, 16));
        label_16->setFont(font2);
        triggerCntLabel_2 = new QLabel(frame_5);
        triggerCntLabel_2->setObjectName(QStringLiteral("triggerCntLabel_2"));
        triggerCntLabel_2->setGeometry(QRect(150, 20, 35, 16));
        triggerCntLabel_2->setFont(font);
        triggerCntLabel_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        frame_6 = new QFrame(runGroupbox);
        frame_6->setObjectName(QStringLiteral("frame_6"));
        frame_6->setGeometry(QRect(240, 60, 191, 51));
        frame_6->setFrameShape(QFrame::StyledPanel);
        frame_6->setFrameShadow(QFrame::Raised);
        runStatusField_2 = new QLineEdit(frame_6);
        runStatusField_2->setObjectName(QStringLiteral("runStatusField_2"));
        runStatusField_2->setEnabled(true);
        runStatusField_2->setGeometry(QRect(10, 24, 71, 21));
        runStatusField_2->setFont(font);
        runStatusField_2->setAlignment(Qt::AlignCenter);
        runStatusField_2->setReadOnly(true);
        frame_7 = new QFrame(runGroupbox);
        frame_7->setObjectName(QStringLiteral("frame_7"));
        frame_7->setGeometry(QRect(6, 20, 231, 91));
        frame_7->setFrameShape(QFrame::StyledPanel);
        frame_7->setFrameShadow(QFrame::Raised);
        selectDir_2 = new QPushButton(frame_7);
        selectDir_2->setObjectName(QStringLiteral("selectDir_2"));
        selectDir_2->setGeometry(QRect(178, 35, 45, 21));
        QIcon icon;
        icon.addFile(QStringLiteral(":/images/directory.png"), QSize(), QIcon::Normal, QIcon::Off);
        selectDir_2->setIcon(icon);
        selectDir_2->setIconSize(QSize(18, 20));
        frame_8 = new QFrame(runGroupbox);
        frame_8->setObjectName(QStringLiteral("frame_8"));
        frame_8->setGeometry(QRect(436, 70, 141, 41));
        frame_8->setFrameShape(QFrame::StyledPanel);
        frame_8->setFrameShadow(QFrame::Raised);
        ignore16_2 = new QCheckBox(frame_8);
        ignore16_2->setObjectName(QStringLiteral("ignore16_2"));
        ignore16_2->setGeometry(QRect(0, 0, 111, 18));
        ignore16_2->setFont(font);
        enableDebugPB_2 = new QCheckBox(frame_8);
        enableDebugPB_2->setObjectName(QStringLiteral("enableDebugPB_2"));
        enableDebugPB_2->setGeometry(QRect(0, 20, 121, 18));
        enableDebugPB_2->setFont(font);
        readTrigger_2 = new QCheckBox(runGroupbox);
        readTrigger_2->setObjectName(QStringLiteral("readTrigger_2"));
        readTrigger_2->setGeometry(QRect(335, 80, 91, 18));
        readTrigger_2->setFont(font);
        writeData_2 = new QCheckBox(runGroupbox);
        writeData_2->setObjectName(QStringLiteral("writeData_2"));
        writeData_2->setGeometry(QRect(335, 94, 91, 18));
        writeData_2->setFont(font);
        writeData_2->setChecked(true);
        widget = new QWidget(runGroupbox);
        widget->setObjectName(QStringLiteral("widget"));
        widget->setGeometry(QRect(600, 20, 100, 40));
        gridLayout = new QGridLayout(widget);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pushButton = new QPushButton(widget);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setCheckable(true);

        gridLayout->addWidget(pushButton, 0, 0, 1, 1);

        connectionLabel = new QLabel(runGroupbox);
        connectionLabel->setObjectName(QStringLiteral("connectionLabel"));
        connectionLabel->setGeometry(QRect(880, 30, 131, 16));
        connectionLabel->setFont(font);
        openConnection = new QPushButton(runGroupbox);
        openConnection->setObjectName(QStringLiteral("openConnection"));
        openConnection->setGeometry(QRect(750, 30, 121, 21));
        openConnection->setFont(font1);
        groupBox_2 = new QGroupBox(tab_2);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(630, 410, 141, 111));
        groupBox_2->setFont(font);
        horizontalLayoutWidget_5 = new QWidget(groupBox_2);
        horizontalLayoutWidget_5->setObjectName(QStringLiteral("horizontalLayoutWidget_5"));
        horizontalLayoutWidget_5->setGeometry(QRect(10, 60, 122, 22));
        horizontalLayout_25 = new QHBoxLayout(horizontalLayoutWidget_5);
        horizontalLayout_25->setObjectName(QStringLiteral("horizontalLayout_25"));
        horizontalLayout_25->setContentsMargins(0, 0, 0, 0);
        readRB_3 = new QRadioButton(horizontalLayoutWidget_5);
        readRB_3->setObjectName(QStringLiteral("readRB_3"));
        readRB_3->setFont(font);
        readRB_3->setAutoExclusive(true);

        horizontalLayout_25->addWidget(readRB_3);

        writeRB_3 = new QRadioButton(horizontalLayoutWidget_5);
        writeRB_3->setObjectName(QStringLiteral("writeRB_3"));
        writeRB_3->setEnabled(true);
        writeRB_3->setFont(font);

        horizontalLayout_25->addWidget(writeRB_3);

        horizontalLayoutWidget_6 = new QWidget(groupBox_2);
        horizontalLayoutWidget_6->setObjectName(QStringLiteral("horizontalLayoutWidget_6"));
        horizontalLayoutWidget_6->setGeometry(QRect(10, 20, 112, 64));
        gridLayout_4 = new QGridLayout(horizontalLayoutWidget_6);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        gridLayout_4->setContentsMargins(0, 0, 0, 17);
        fecRB_3 = new QRadioButton(horizontalLayoutWidget_6);
        fecRB_3->setObjectName(QStringLiteral("fecRB_3"));
        fecRB_3->setFont(font);

        gridLayout_4->addWidget(fecRB_3, 0, 3, 1, 1);

        s6RB_3 = new QRadioButton(horizontalLayoutWidget_6);
        s6RB_3->setObjectName(QStringLiteral("s6RB_3"));
        s6RB_3->setFont(font);

        gridLayout_4->addWidget(s6RB_3, 1, 0, 1, 1);

        spiRB_3 = new QRadioButton(horizontalLayoutWidget_6);
        spiRB_3->setObjectName(QStringLiteral("spiRB_3"));
        spiRB_3->setFont(font);

        gridLayout_4->addWidget(spiRB_3, 1, 3, 1, 1);

        appRB_3 = new QRadioButton(horizontalLayoutWidget_6);
        appRB_3->setObjectName(QStringLiteral("appRB_3"));
        appRB_3->setFont(font);
        appRB_3->setLayoutDirection(Qt::LeftToRight);

        gridLayout_4->addWidget(appRB_3, 0, 0, 1, 1);

        setVMMs_3 = new QComboBox(groupBox_2);
        setVMMs_3->setObjectName(QStringLiteral("setVMMs_3"));
        setVMMs_3->setGeometry(QRect(5, 80, 41, 24));
        setVMMs_3->setFont(font);
        setVMMs_3->setMinimumContentsLength(1);
        SendConfiguration_3 = new QPushButton(groupBox_2);
        SendConfiguration_3->setObjectName(QStringLiteral("SendConfiguration_3"));
        SendConfiguration_3->setEnabled(true);
        SendConfiguration_3->setGeometry(QRect(50, 80, 41, 21));
        SendConfiguration_3->setFont(font);
        cmdlabel_3 = new QLabel(groupBox_2);
        cmdlabel_3->setObjectName(QStringLiteral("cmdlabel_3"));
        cmdlabel_3->setEnabled(false);
        cmdlabel_3->setGeometry(QRect(100, 84, 31, 16));
        cmdlabel_3->setFont(font);
        cmdlabel_3->setAlignment(Qt::AlignCenter);
        cmdlabel_3->setWordWrap(true);
        groupBox_12 = new QGroupBox(tab_2);
        groupBox_12->setObjectName(QStringLiteral("groupBox_12"));
        groupBox_12->setGeometry(QRect(310, 310, 190, 244));
        groupBox_12->setFont(font);
        groupBox_12->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignVCenter);
        gridLayout_3 = new QGridLayout(groupBox_12);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_7 = new QLabel(groupBox_12);
        label_7->setObjectName(QStringLiteral("label_7"));
        QFont font3;
        font3.setPointSize(8);
        label_7->setFont(font3);

        gridLayout_3->addWidget(label_7, 0, 0, 1, 1);

        label_37 = new QLabel(groupBox_12);
        label_37->setObjectName(QStringLiteral("label_37"));
        label_37->setFont(font3);
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
        label_22->setFont(font3);
        label_22->setWordWrap(true);

        gridLayout_3->addWidget(label_22, 2, 0, 1, 1);

        label_20 = new QLabel(groupBox_12);
        label_20->setObjectName(QStringLiteral("label_20"));
        label_20->setFont(font3);

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
        label_21->setFont(font3);
        label_21->setWordWrap(true);

        gridLayout_3->addWidget(label_21, 4, 0, 1, 1);

        holdOffCheckBox_3 = new QCheckBox(groupBox_12);
        holdOffCheckBox_3->setObjectName(QStringLiteral("holdOffCheckBox_3"));
        holdOffCheckBox_3->setFont(font3);

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
        QFont font4;
        font4.setPointSize(14);
        setTrgAcqConst_3->setFont(font4);

        gridLayout_3->addWidget(setTrgAcqConst_3, 6, 1, 1, 1);

        trgPulser_3 = new QPushButton(groupBox_12);
        trgPulser_3->setObjectName(QStringLiteral("trgPulser_3"));
        trgPulser_3->setFont(font2);
        trgPulser_3->setFocusPolicy(Qt::NoFocus);

        gridLayout_3->addWidget(trgPulser_3, 7, 0, 1, 1);

        onACQ_3 = new QPushButton(groupBox_12);
        onACQ_3->setObjectName(QStringLiteral("onACQ_3"));
        onACQ_3->setFont(font2);
        onACQ_3->setFocusPolicy(Qt::NoFocus);

        gridLayout_3->addWidget(onACQ_3, 7, 1, 1, 1);

        trgExternal_3 = new QPushButton(groupBox_12);
        trgExternal_3->setObjectName(QStringLiteral("trgExternal_3"));
        trgExternal_3->setFont(font2);
        trgExternal_3->setFocusPolicy(Qt::NoFocus);
        trgExternal_3->setAutoFillBackground(false);
        trgExternal_3->setAutoDefault(false);

        gridLayout_3->addWidget(trgExternal_3, 8, 0, 1, 1);

        offACQ_3 = new QPushButton(groupBox_12);
        offACQ_3->setObjectName(QStringLiteral("offACQ_3"));
        offACQ_3->setFont(font2);
        offACQ_3->setFocusPolicy(Qt::NoFocus);

        gridLayout_3->addWidget(offACQ_3, 8, 1, 1, 1);

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
        QFont font5;
        font5.setFamily(QStringLiteral("Courier"));
        font5.setPointSize(9);
        loggingScreen->setFont(font5);
        loggingScreen->setLineWrapMode(QTextEdit::NoWrap);
        loggingScreen->setLineWrapColumnOrWidth(150);
        loggingScreen->setTabStopWidth(0);

        gridLayout_11->addWidget(loggingScreen, 0, 0, 1, 1);

        logBottom = new QPushButton(groupBox_5);
        logBottom->setObjectName(QStringLiteral("logBottom"));
        logBottom->setFont(font);

        gridLayout_11->addWidget(logBottom, 1, 0, 1, 1);


        gridLayout_10->addWidget(groupBox_5, 0, 0, 1, 1);

        tabWidget_2->addTab(tab_3, QString());

        gridLayout_2->addWidget(tabWidget_2, 0, 1, 2, 2);

        groupBox_3 = new QGroupBox(centralwidget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        groupBox_3->setMaximumSize(QSize(300, 70));
        gridLayout_6 = new QGridLayout(groupBox_3);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        Button_load = new QPushButton(groupBox_3);
        Button_load->setObjectName(QStringLiteral("Button_load"));

        gridLayout_6->addWidget(Button_load, 0, 1, 1, 1);

        line_configFile = new QLineEdit(groupBox_3);
        line_configFile->setObjectName(QStringLiteral("line_configFile"));

        gridLayout_6->addWidget(line_configFile, 0, 0, 1, 1);

        Button_save = new QPushButton(groupBox_3);
        Button_save->setObjectName(QStringLiteral("Button_save"));

        gridLayout_6->addWidget(Button_save, 0, 2, 1, 1);


        gridLayout_2->addWidget(groupBox_3, 2, 1, 1, 1);

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
        groupBox->setTitle(QApplication::translate("daq_window", "FEC", 0));
        Box_fec1->setText(QApplication::translate("daq_window", "1", 0));
        Box_fec2->setText(QApplication::translate("daq_window", "2", 0));
        Box_fec3->setText(QApplication::translate("daq_window", "3", 0));
        Box_fec4->setText(QApplication::translate("daq_window", "4", 0));
        Box_fec5->setText(QApplication::translate("daq_window", "5", 0));
        Box_fec6->setText(QApplication::translate("daq_window", "6", 0));
        Box_fec7->setText(QApplication::translate("daq_window", "7", 0));
        Box_fec8->setText(QApplication::translate("daq_window", "8", 0));
        connectionLabel_3->setText(QString());
        Send->setText(QApplication::translate("daq_window", "Send", 0));
        openConnection_2->setText(QApplication::translate("daq_window", "Open Communication", 0));
        connectionLabel_2->setText(QApplication::translate("daq_window", "  N/A", 0));
        reset_warnings->setText(QApplication::translate("daq_window", "Reset Warnings", 0));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab), QApplication::translate("daq_window", "VMM Slow Control", 0));
        runGroupbox->setTitle(QApplication::translate("daq_window", "Run Control", 0));
        label_5->setText(QApplication::translate("daq_window", "Run Number", 0));
        label_11->setText(QApplication::translate("daq_window", "Comments", 0));
        label_12->setText(QApplication::translate("daq_window", "Directory", 0));
        label_14->setText(QApplication::translate("daq_window", "  Angle", 0));
        stopTriggerCnt_2->setText(QApplication::translate("daq_window", "Stop Run", 0));
        clearTriggerCnt_2->setText(QApplication::translate("daq_window", "Clear Counters", 0));
        checkTriggers_2->setText(QApplication::translate("daq_window", "Start Run", 0));
        eventCountStop_2->setText(QApplication::translate("daq_window", "-1", 0));
        eventCountStopLabel_2->setText(QApplication::translate("daq_window", " Event Stop", 0));
        label_15->setText(QApplication::translate("daq_window", "Triggers", 0));
        triggerCntLabel_fpga_2->setText(QApplication::translate("daq_window", "0", 0));
        label_16->setText(QApplication::translate("daq_window", "Hits", 0));
        triggerCntLabel_2->setText(QApplication::translate("daq_window", "0", 0));
        runStatusField_2->setText(QApplication::translate("daq_window", "N/A", 0));
        selectDir_2->setText(QString());
        ignore16_2->setText(QApplication::translate("daq_window", "Ignore 16", 0));
        enableDebugPB_2->setText(QApplication::translate("daq_window", "Enable Debug", 0));
        readTrigger_2->setText(QApplication::translate("daq_window", "Trigger Data", 0));
        writeData_2->setText(QApplication::translate("daq_window", "Write Data", 0));
        pushButton->setText(QApplication::translate("daq_window", "Event Display", 0));
        connectionLabel->setText(QApplication::translate("daq_window", "  N/A", 0));
        openConnection->setText(QApplication::translate("daq_window", "Open Communication", 0));
        groupBox_2->setTitle(QApplication::translate("daq_window", "Command", 0));
        readRB_3->setText(QApplication::translate("daq_window", "Read", 0));
        writeRB_3->setText(QApplication::translate("daq_window", "Write", 0));
        fecRB_3->setText(QApplication::translate("daq_window", "FEC", 0));
        s6RB_3->setText(QApplication::translate("daq_window", "S6", 0));
        spiRB_3->setText(QApplication::translate("daq_window", "SPI", 0));
        appRB_3->setText(QApplication::translate("daq_window", "APP", 0));
        SendConfiguration_3->setText(QApplication::translate("daq_window", "Send", 0));
        cmdlabel_3->setText(QApplication::translate("daq_window", "count", 0));
        groupBox_12->setTitle(QApplication::translate("daq_window", "Trigger & Acquisition", 0));
        label_7->setText(QApplication::translate("daq_window", "TP Delay", 0));
        label_37->setText(QApplication::translate("daq_window", "BCID Reset", 0));
        label_22->setText(QApplication::translate("daq_window", "Trg Per", 0));
        label_20->setText(QApplication::translate("daq_window", "ACQ Sync", 0));
        trgPeriod_3->setText(QApplication::translate("daq_window", "61A80", 0));
        label_21->setText(QApplication::translate("daq_window", "ACQ Win", 0));
        holdOffCheckBox_3->setText(QApplication::translate("daq_window", "Hold Off", 0));
        setTrgAcqConst_3->setText(QApplication::translate("daq_window", "Set", 0));
        trgPulser_3->setText(QApplication::translate("daq_window", "Pulser", 0));
        onACQ_3->setText(QApplication::translate("daq_window", "ACQ On", 0));
        trgExternal_3->setText(QApplication::translate("daq_window", "External", 0));
        offACQ_3->setText(QApplication::translate("daq_window", "ACQ Off", 0));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_2), QApplication::translate("daq_window", "VMM Data Acquisition", 0));
        groupBox_5->setTitle(QApplication::translate("daq_window", "Logging", 0));
        logBottom->setText(QApplication::translate("daq_window", "Bottom", 0));
        tabWidget_2->setTabText(tabWidget_2->indexOf(tab_3), QApplication::translate("daq_window", "Logging", 0));
        groupBox_3->setTitle(QApplication::translate("daq_window", "Config file", 0));
        Button_load->setText(QApplication::translate("daq_window", "Load", 0));
        Button_save->setText(QApplication::translate("daq_window", "Save", 0));
    } // retranslateUi

};

namespace Ui {
    class daq_window: public Ui_daq_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAQ_WINDOW_H
