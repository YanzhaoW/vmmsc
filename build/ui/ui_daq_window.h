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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_daq_window
{
public:
    QWidget *centralwidget;
    QGridLayout *gridLayout_2;
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
    QTabWidget *tabWidget;
    QWidget *widget_2;
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
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *daq_window)
    {
        if (daq_window->objectName().isEmpty())
            daq_window->setObjectName(QStringLiteral("daq_window"));
        daq_window->resize(1235, 805);
        centralwidget = new QWidget(daq_window);
        centralwidget->setObjectName(QStringLiteral("centralwidget"));
        gridLayout_2 = new QGridLayout(centralwidget);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
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

        gridLayout_2->addWidget(groupBox, 0, 0, 1, 1);

        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        gridLayout_2->addWidget(tabWidget, 0, 1, 1, 1);

        widget_2 = new QWidget(centralwidget);
        widget_2->setObjectName(QStringLiteral("widget_2"));
        widget_2->setMinimumSize(QSize(1111, 140));
        runGroupbox = new QGroupBox(widget_2);
        runGroupbox->setObjectName(QStringLiteral("runGroupbox"));
        runGroupbox->setGeometry(QRect(10, 10, 911, 121));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
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
        QFont font1;
        font1.setPointSize(9);
        label_5->setFont(font1);
        label_11 = new QLabel(runGroupbox);
        label_11->setObjectName(QStringLiteral("label_11"));
        label_11->setGeometry(QRect(11, 86, 61, 16));
        label_11->setFont(font1);
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
        label_12->setFont(font1);
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
        label_14->setFont(font1);
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
        QFont font2;
        font2.setFamily(QStringLiteral("Arial"));
        font2.setPointSize(8);
        eventCountStopLabel_2->setFont(font2);
        label_15 = new QLabel(frame_5);
        label_15->setObjectName(QStringLiteral("label_15"));
        label_15->setGeometry(QRect(110, 0, 36, 16));
        label_15->setFont(font1);
        triggerCntLabel_fpga_2 = new QLabel(frame_5);
        triggerCntLabel_fpga_2->setObjectName(QStringLiteral("triggerCntLabel_fpga_2"));
        triggerCntLabel_fpga_2->setGeometry(QRect(154, 0, 31, 16));
        triggerCntLabel_fpga_2->setFont(font);
        triggerCntLabel_fpga_2->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        label_16 = new QLabel(frame_5);
        label_16->setObjectName(QStringLiteral("label_16"));
        label_16->setGeometry(QRect(110, 20, 36, 16));
        label_16->setFont(font1);
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
        openConnection->setFont(font2);
        groupBox_2 = new QGroupBox(widget_2);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        groupBox_2->setGeometry(QRect(930, 20, 141, 111));
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

        gridLayout_2->addWidget(widget_2, 1, 1, 1, 1);

        daq_window->setCentralWidget(centralwidget);
        menubar = new QMenuBar(daq_window);
        menubar->setObjectName(QStringLiteral("menubar"));
        menubar->setGeometry(QRect(0, 0, 1235, 19));
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
    } // retranslateUi

};

namespace Ui {
    class daq_window: public Ui_daq_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DAQ_WINDOW_H
