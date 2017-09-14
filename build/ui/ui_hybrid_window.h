/********************************************************************************
** Form generated from reading UI file 'hybrid_window.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HYBRID_WINDOW_H
#define UI_HYBRID_WINDOW_H

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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_hybrid_window
{
public:
    QGridLayout *gridLayout_3;
    QWidget *widget;
    QGridLayout *gridLayout_6;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QLabel *cktkLabel_2;
    QComboBox *Xaxis;
    QLabel *cktkLabel_3;
    QSpinBox *position;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QCheckBox *Box_vmm1;
    QCheckBox *Box_vmm2;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout;
    QComboBox *ckbc_skew_s6;
    QSpinBox *fecPeriodReset;
    QLabel *ckbcLabel;
    QComboBox *cktk_s6;
    QLabel *ckbcLabel_2;
    QComboBox *ckbc_s6;
    QLabel *cktkLabel;
    QLabel *label_32;
    QLabel *label_34;
    QSpinBox *s6_tkPulses;
    QGroupBox *groupBox_3;
    QGridLayout *gridLayout_5;
    QComboBox *tpWidth;
    QLabel *tpWidthLabel;
    QComboBox *tpSkew;
    QLabel *tpSkewLabel;
    QLabel *tpPolarityLabel;
    QComboBox *tpPolarity;
    QTabWidget *tabWidget;

    void setupUi(QWidget *hybrid_window)
    {
        if (hybrid_window->objectName().isEmpty())
            hybrid_window->setObjectName(QStringLiteral("hybrid_window"));
        hybrid_window->resize(1191, 746);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(hybrid_window->sizePolicy().hasHeightForWidth());
        hybrid_window->setSizePolicy(sizePolicy);
        hybrid_window->setFocusPolicy(Qt::NoFocus);
        gridLayout_3 = new QGridLayout(hybrid_window);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        widget = new QWidget(hybrid_window);
        widget->setObjectName(QStringLiteral("widget"));
        gridLayout_6 = new QGridLayout(widget);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        groupBox_2 = new QGroupBox(widget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        cktkLabel_2 = new QLabel(groupBox_2);
        cktkLabel_2->setObjectName(QStringLiteral("cktkLabel_2"));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(8);
        cktkLabel_2->setFont(font);

        gridLayout_4->addWidget(cktkLabel_2, 0, 0, 1, 1);

        Xaxis = new QComboBox(groupBox_2);
        Xaxis->setObjectName(QStringLiteral("Xaxis"));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(9);
        Xaxis->setFont(font1);

        gridLayout_4->addWidget(Xaxis, 1, 0, 1, 1);

        cktkLabel_3 = new QLabel(groupBox_2);
        cktkLabel_3->setObjectName(QStringLiteral("cktkLabel_3"));
        cktkLabel_3->setFont(font);

        gridLayout_4->addWidget(cktkLabel_3, 2, 0, 1, 1);

        position = new QSpinBox(groupBox_2);
        position->setObjectName(QStringLiteral("position"));
        position->setFont(font1);
        position->setMinimum(1);
        position->setMaximum(65535);
        position->setSingleStep(1);
        position->setValue(1);

        gridLayout_4->addWidget(position, 3, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_2, 1, 0, 1, 1);

        groupBox = new QGroupBox(widget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        Box_vmm1 = new QCheckBox(groupBox);
        Box_vmm1->setObjectName(QStringLiteral("Box_vmm1"));

        gridLayout_2->addWidget(Box_vmm1, 0, 0, 1, 1);

        Box_vmm2 = new QCheckBox(groupBox);
        Box_vmm2->setObjectName(QStringLiteral("Box_vmm2"));

        gridLayout_2->addWidget(Box_vmm2, 1, 0, 1, 1);


        gridLayout_6->addWidget(groupBox, 0, 0, 1, 1);

        groupBox_8 = new QGroupBox(widget);
        groupBox_8->setObjectName(QStringLiteral("groupBox_8"));
        groupBox_8->setFont(font1);
        gridLayout = new QGridLayout(groupBox_8);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        ckbc_skew_s6 = new QComboBox(groupBox_8);
        ckbc_skew_s6->setObjectName(QStringLiteral("ckbc_skew_s6"));
        ckbc_skew_s6->setFont(font1);

        gridLayout->addWidget(ckbc_skew_s6, 5, 0, 1, 3);

        fecPeriodReset = new QSpinBox(groupBox_8);
        fecPeriodReset->setObjectName(QStringLiteral("fecPeriodReset"));
        fecPeriodReset->setFont(font1);
        fecPeriodReset->setMinimum(0);
        fecPeriodReset->setMaximum(4096);
        fecPeriodReset->setSingleStep(1);
        fecPeriodReset->setValue(4094);

        gridLayout->addWidget(fecPeriodReset, 7, 1, 1, 2);

        ckbcLabel = new QLabel(groupBox_8);
        ckbcLabel->setObjectName(QStringLiteral("ckbcLabel"));
        ckbcLabel->setFont(font);

        gridLayout->addWidget(ckbcLabel, 2, 0, 1, 1);

        cktk_s6 = new QComboBox(groupBox_8);
        cktk_s6->setObjectName(QStringLiteral("cktk_s6"));
        cktk_s6->setFont(font1);

        gridLayout->addWidget(cktk_s6, 1, 0, 1, 3);

        ckbcLabel_2 = new QLabel(groupBox_8);
        ckbcLabel_2->setObjectName(QStringLiteral("ckbcLabel_2"));
        ckbcLabel_2->setFont(font);

        gridLayout->addWidget(ckbcLabel_2, 4, 0, 1, 3);

        ckbc_s6 = new QComboBox(groupBox_8);
        ckbc_s6->setObjectName(QStringLiteral("ckbc_s6"));
        ckbc_s6->setFont(font1);

        gridLayout->addWidget(ckbc_s6, 3, 0, 1, 3);

        cktkLabel = new QLabel(groupBox_8);
        cktkLabel->setObjectName(QStringLiteral("cktkLabel"));
        cktkLabel->setFont(font);

        gridLayout->addWidget(cktkLabel, 0, 0, 1, 1);

        label_32 = new QLabel(groupBox_8);
        label_32->setObjectName(QStringLiteral("label_32"));
        QFont font2;
        font2.setPointSize(8);
        label_32->setFont(font2);

        gridLayout->addWidget(label_32, 6, 2, 1, 1);

        label_34 = new QLabel(groupBox_8);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setFont(font2);

        gridLayout->addWidget(label_34, 6, 0, 1, 2);

        s6_tkPulses = new QSpinBox(groupBox_8);
        s6_tkPulses->setObjectName(QStringLiteral("s6_tkPulses"));
        s6_tkPulses->setFont(font1);
        s6_tkPulses->setMinimum(0);
        s6_tkPulses->setMaximum(7);
        s6_tkPulses->setSingleStep(1);
        s6_tkPulses->setValue(2);

        gridLayout->addWidget(s6_tkPulses, 7, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_8, 2, 0, 1, 1);

        groupBox_3 = new QGroupBox(widget);
        groupBox_3->setObjectName(QStringLiteral("groupBox_3"));
        gridLayout_5 = new QGridLayout(groupBox_3);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        tpWidth = new QComboBox(groupBox_3);
        tpWidth->setObjectName(QStringLiteral("tpWidth"));
        tpWidth->setFont(font1);

        gridLayout_5->addWidget(tpWidth, 3, 0, 1, 1);

        tpWidthLabel = new QLabel(groupBox_3);
        tpWidthLabel->setObjectName(QStringLiteral("tpWidthLabel"));
        tpWidthLabel->setFont(font1);

        gridLayout_5->addWidget(tpWidthLabel, 2, 0, 1, 1);

        tpSkew = new QComboBox(groupBox_3);
        tpSkew->setObjectName(QStringLiteral("tpSkew"));
        tpSkew->setFont(font1);

        gridLayout_5->addWidget(tpSkew, 1, 0, 1, 1);

        tpSkewLabel = new QLabel(groupBox_3);
        tpSkewLabel->setObjectName(QStringLiteral("tpSkewLabel"));
        tpSkewLabel->setFont(font1);

        gridLayout_5->addWidget(tpSkewLabel, 0, 0, 1, 1);

        tpPolarityLabel = new QLabel(groupBox_3);
        tpPolarityLabel->setObjectName(QStringLiteral("tpPolarityLabel"));
        tpPolarityLabel->setFont(font1);

        gridLayout_5->addWidget(tpPolarityLabel, 4, 0, 1, 1);

        tpPolarity = new QComboBox(groupBox_3);
        tpPolarity->setObjectName(QStringLiteral("tpPolarity"));
        tpPolarity->setFont(font1);

        gridLayout_5->addWidget(tpPolarity, 5, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_3, 3, 0, 1, 1);


        gridLayout_3->addWidget(widget, 0, 0, 1, 1);

        tabWidget = new QTabWidget(hybrid_window);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        gridLayout_3->addWidget(tabWidget, 0, 1, 1, 1);


        retranslateUi(hybrid_window);

        Xaxis->setCurrentIndex(0);
        ckbc_skew_s6->setCurrentIndex(0);
        cktk_s6->setCurrentIndex(0);
        ckbc_s6->setCurrentIndex(2);
        tpWidth->setCurrentIndex(0);
        tpSkew->setCurrentIndex(0);
        tpPolarity->setCurrentIndex(0);
        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(hybrid_window);
    } // setupUi

    void retranslateUi(QWidget *hybrid_window)
    {
        hybrid_window->setWindowTitle(QApplication::translate("hybrid_window", "Form", 0));
        groupBox_2->setTitle(QApplication::translate("hybrid_window", "Position", 0));
        cktkLabel_2->setText(QApplication::translate("hybrid_window", "Axis", 0));
        Xaxis->clear();
        Xaxis->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "Y", 0)
         << QApplication::translate("hybrid_window", "X", 0)
        );
        cktkLabel_3->setText(QApplication::translate("hybrid_window", "Position", 0));
        groupBox->setTitle(QApplication::translate("hybrid_window", "VMM", 0));
        Box_vmm1->setText(QApplication::translate("hybrid_window", "1", 0));
        Box_vmm2->setText(QApplication::translate("hybrid_window", "2", 0));
        groupBox_8->setTitle(QApplication::translate("hybrid_window", "S6", 0));
        ckbc_skew_s6->clear();
        ckbc_skew_s6->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "0 ns", 0)
         << QApplication::translate("hybrid_window", "6.26 ns", 0)
         << QApplication::translate("hybrid_window", "12.52 ns", 0)
         << QApplication::translate("hybrid_window", "18.78 ns", 0)
        );
        ckbcLabel->setText(QApplication::translate("hybrid_window", "CKBC", 0));
        cktk_s6->clear();
        cktk_s6->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "0 ns", 0)
         << QApplication::translate("hybrid_window", "12.5 ns", 0)
         << QApplication::translate("hybrid_window", "25 ns", 0)
        );
        ckbcLabel_2->setText(QApplication::translate("hybrid_window", "CKBC skew", 0));
        ckbc_s6->clear();
        ckbc_s6->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "160MHz", 0)
         << QApplication::translate("hybrid_window", "160MHz inv", 0)
         << QApplication::translate("hybrid_window", "80MHz", 0)
         << QApplication::translate("hybrid_window", "40MHz", 0)
         << QApplication::translate("hybrid_window", "20MHz", 0)
         << QApplication::translate("hybrid_window", "10MHz", 0)
         << QApplication::translate("hybrid_window", "5MHz", 0)
         << QApplication::translate("hybrid_window", "2.5MHz", 0)
        );
        cktkLabel->setText(QApplication::translate("hybrid_window", "CKTK", 0));
        label_32->setText(QApplication::translate("hybrid_window", "  Period", 0));
        label_34->setText(QApplication::translate("hybrid_window", "TK Pulses", 0));
        groupBox_3->setTitle(QApplication::translate("hybrid_window", "Test Pulse", 0));
        tpWidth->clear();
        tpWidth->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "128x25ns", 0)
         << QApplication::translate("hybrid_window", "64x25ns", 0)
         << QApplication::translate("hybrid_window", "32x25ns", 0)
         << QApplication::translate("hybrid_window", "16x25ns", 0)
         << QApplication::translate("hybrid_window", "8x25ns", 0)
         << QApplication::translate("hybrid_window", "4x25ns", 0)
         << QApplication::translate("hybrid_window", "2x25ns", 0)
         << QApplication::translate("hybrid_window", "1x25ns", 0)
        );
        tpWidthLabel->setText(QApplication::translate("hybrid_window", "Width", 0));
        tpSkew->clear();
        tpSkew->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "0ns", 0)
         << QApplication::translate("hybrid_window", "3.125ns", 0)
         << QApplication::translate("hybrid_window", "6.25ns", 0)
         << QApplication::translate("hybrid_window", "9.375ns", 0)
         << QApplication::translate("hybrid_window", "12.5ns", 0)
         << QApplication::translate("hybrid_window", "15.625ns", 0)
         << QApplication::translate("hybrid_window", "18.75ns", 0)
         << QApplication::translate("hybrid_window", "21.875ns", 0)
        );
        tpSkewLabel->setText(QApplication::translate("hybrid_window", "Skew", 0));
        tpPolarityLabel->setText(QApplication::translate("hybrid_window", "Polarity", 0));
        tpPolarity->clear();
        tpPolarity->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "Positive", 0)
         << QApplication::translate("hybrid_window", "Negative", 0)
        );
    } // retranslateUi

};

namespace Ui {
    class hybrid_window: public Ui_hybrid_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HYBRID_WINDOW_H
