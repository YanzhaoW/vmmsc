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
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_hybrid_window
{
public:
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox_8;
    QGridLayout *gridLayout;
    QSpinBox *s6_tkPulses;
    QComboBox *ckbc_s6;
    QComboBox *ckbc_skew_s6;
    QLabel *cktkLabel;
    QSpinBox *fecPeriodReset;
    QPushButton *setck_s6_resets;
    QPushButton *setck_s6;
    QCheckBox *s6_autoReset;
    QLabel *ckbcLabel_2;
    QCheckBox *s6_FECReset;
    QLabel *label_32;
    QLabel *ckbcLabel;
    QComboBox *cktk_s6;
    QLabel *label_34;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QCheckBox *Box_vmm1;
    QCheckBox *Box_vmm2;
    QTabWidget *tabWidget;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QLabel *cktkLabel_2;
    QComboBox *cktk_s6_2;
    QLabel *cktkLabel_3;
    QSpinBox *fecPeriodReset_2;

    void setupUi(QWidget *hybrid_window)
    {
        if (hybrid_window->objectName().isEmpty())
            hybrid_window->setObjectName(QStringLiteral("hybrid_window"));
        hybrid_window->resize(986, 643);
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(hybrid_window->sizePolicy().hasHeightForWidth());
        hybrid_window->setSizePolicy(sizePolicy);
        hybrid_window->setFocusPolicy(Qt::NoFocus);
        gridLayout_3 = new QGridLayout(hybrid_window);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        groupBox_8 = new QGroupBox(hybrid_window);
        groupBox_8->setObjectName(QStringLiteral("groupBox_8"));
        QFont font;
        font.setFamily(QStringLiteral("Arial"));
        font.setPointSize(9);
        groupBox_8->setFont(font);
        gridLayout = new QGridLayout(groupBox_8);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        s6_tkPulses = new QSpinBox(groupBox_8);
        s6_tkPulses->setObjectName(QStringLiteral("s6_tkPulses"));
        s6_tkPulses->setFont(font);
        s6_tkPulses->setMinimum(0);
        s6_tkPulses->setMaximum(7);
        s6_tkPulses->setSingleStep(1);
        s6_tkPulses->setValue(2);

        gridLayout->addWidget(s6_tkPulses, 10, 0, 1, 1);

        ckbc_s6 = new QComboBox(groupBox_8);
        ckbc_s6->setObjectName(QStringLiteral("ckbc_s6"));
        ckbc_s6->setFont(font);

        gridLayout->addWidget(ckbc_s6, 3, 0, 1, 3);

        ckbc_skew_s6 = new QComboBox(groupBox_8);
        ckbc_skew_s6->setObjectName(QStringLiteral("ckbc_skew_s6"));
        ckbc_skew_s6->setFont(font);

        gridLayout->addWidget(ckbc_skew_s6, 5, 0, 1, 3);

        cktkLabel = new QLabel(groupBox_8);
        cktkLabel->setObjectName(QStringLiteral("cktkLabel"));
        QFont font1;
        font1.setFamily(QStringLiteral("Arial"));
        font1.setPointSize(8);
        cktkLabel->setFont(font1);

        gridLayout->addWidget(cktkLabel, 0, 0, 1, 1);

        fecPeriodReset = new QSpinBox(groupBox_8);
        fecPeriodReset->setObjectName(QStringLiteral("fecPeriodReset"));
        fecPeriodReset->setFont(font);
        fecPeriodReset->setMinimum(0);
        fecPeriodReset->setMaximum(4096);
        fecPeriodReset->setSingleStep(1);
        fecPeriodReset->setValue(4094);

        gridLayout->addWidget(fecPeriodReset, 10, 1, 1, 2);

        setck_s6_resets = new QPushButton(groupBox_8);
        setck_s6_resets->setObjectName(QStringLiteral("setck_s6_resets"));
        QFont font2;
        font2.setPointSize(9);
        setck_s6_resets->setFont(font2);

        gridLayout->addWidget(setck_s6_resets, 11, 0, 1, 3);

        setck_s6 = new QPushButton(groupBox_8);
        setck_s6->setObjectName(QStringLiteral("setck_s6"));
        setck_s6->setFont(font2);

        gridLayout->addWidget(setck_s6, 6, 0, 1, 3);

        s6_autoReset = new QCheckBox(groupBox_8);
        s6_autoReset->setObjectName(QStringLiteral("s6_autoReset"));
        QFont font3;
        font3.setPointSize(8);
        s6_autoReset->setFont(font3);
        s6_autoReset->setChecked(true);

        gridLayout->addWidget(s6_autoReset, 7, 0, 1, 3);

        ckbcLabel_2 = new QLabel(groupBox_8);
        ckbcLabel_2->setObjectName(QStringLiteral("ckbcLabel_2"));
        ckbcLabel_2->setFont(font1);

        gridLayout->addWidget(ckbcLabel_2, 4, 0, 1, 3);

        s6_FECReset = new QCheckBox(groupBox_8);
        s6_FECReset->setObjectName(QStringLiteral("s6_FECReset"));
        s6_FECReset->setFont(font3);

        gridLayout->addWidget(s6_FECReset, 8, 0, 1, 3);

        label_32 = new QLabel(groupBox_8);
        label_32->setObjectName(QStringLiteral("label_32"));
        label_32->setFont(font3);

        gridLayout->addWidget(label_32, 9, 2, 1, 1);

        ckbcLabel = new QLabel(groupBox_8);
        ckbcLabel->setObjectName(QStringLiteral("ckbcLabel"));
        ckbcLabel->setFont(font1);

        gridLayout->addWidget(ckbcLabel, 2, 0, 1, 1);

        cktk_s6 = new QComboBox(groupBox_8);
        cktk_s6->setObjectName(QStringLiteral("cktk_s6"));
        cktk_s6->setFont(font);

        gridLayout->addWidget(cktk_s6, 1, 0, 1, 3);

        label_34 = new QLabel(groupBox_8);
        label_34->setObjectName(QStringLiteral("label_34"));
        label_34->setFont(font3);

        gridLayout->addWidget(label_34, 9, 0, 1, 2);


        gridLayout_3->addWidget(groupBox_8, 2, 0, 1, 1);

        groupBox = new QGroupBox(hybrid_window);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        Box_vmm1 = new QCheckBox(groupBox);
        Box_vmm1->setObjectName(QStringLiteral("Box_vmm1"));

        gridLayout_2->addWidget(Box_vmm1, 0, 0, 1, 1);

        Box_vmm2 = new QCheckBox(groupBox);
        Box_vmm2->setObjectName(QStringLiteral("Box_vmm2"));

        gridLayout_2->addWidget(Box_vmm2, 1, 0, 1, 1);


        gridLayout_3->addWidget(groupBox, 0, 0, 1, 1);

        tabWidget = new QTabWidget(hybrid_window);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        gridLayout_3->addWidget(tabWidget, 0, 1, 3, 1);

        groupBox_2 = new QGroupBox(hybrid_window);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        cktkLabel_2 = new QLabel(groupBox_2);
        cktkLabel_2->setObjectName(QStringLiteral("cktkLabel_2"));
        cktkLabel_2->setFont(font1);

        gridLayout_4->addWidget(cktkLabel_2, 0, 0, 1, 1);

        cktk_s6_2 = new QComboBox(groupBox_2);
        cktk_s6_2->setObjectName(QStringLiteral("cktk_s6_2"));
        cktk_s6_2->setFont(font);

        gridLayout_4->addWidget(cktk_s6_2, 1, 0, 1, 1);

        cktkLabel_3 = new QLabel(groupBox_2);
        cktkLabel_3->setObjectName(QStringLiteral("cktkLabel_3"));
        cktkLabel_3->setFont(font1);

        gridLayout_4->addWidget(cktkLabel_3, 2, 0, 1, 1);

        fecPeriodReset_2 = new QSpinBox(groupBox_2);
        fecPeriodReset_2->setObjectName(QStringLiteral("fecPeriodReset_2"));
        fecPeriodReset_2->setFont(font);
        fecPeriodReset_2->setMinimum(1);
        fecPeriodReset_2->setMaximum(65534);
        fecPeriodReset_2->setSingleStep(1);
        fecPeriodReset_2->setValue(1);

        gridLayout_4->addWidget(fecPeriodReset_2, 3, 0, 1, 1);


        gridLayout_3->addWidget(groupBox_2, 1, 0, 1, 1);


        retranslateUi(hybrid_window);

        ckbc_s6->setCurrentIndex(2);
        ckbc_skew_s6->setCurrentIndex(0);
        cktk_s6->setCurrentIndex(0);
        tabWidget->setCurrentIndex(-1);
        cktk_s6_2->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(hybrid_window);
    } // setupUi

    void retranslateUi(QWidget *hybrid_window)
    {
        hybrid_window->setWindowTitle(QApplication::translate("hybrid_window", "Form", 0));
        groupBox_8->setTitle(QApplication::translate("hybrid_window", "S6", 0));
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
        ckbc_skew_s6->clear();
        ckbc_skew_s6->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "0 ns", 0)
         << QApplication::translate("hybrid_window", "6.26 ns", 0)
         << QApplication::translate("hybrid_window", "12.52 ns", 0)
         << QApplication::translate("hybrid_window", "18.78 ns", 0)
        );
        cktkLabel->setText(QApplication::translate("hybrid_window", "CKTK", 0));
        setck_s6_resets->setText(QApplication::translate("hybrid_window", "Set", 0));
        setck_s6->setText(QApplication::translate("hybrid_window", "Set", 0));
        s6_autoReset->setText(QApplication::translate("hybrid_window", "VMM Auto Reset", 0));
        ckbcLabel_2->setText(QApplication::translate("hybrid_window", "CKBC skew", 0));
        s6_FECReset->setText(QApplication::translate("hybrid_window", "FEC Period. Reset", 0));
        label_32->setText(QApplication::translate("hybrid_window", "  Period", 0));
        ckbcLabel->setText(QApplication::translate("hybrid_window", "CKBC", 0));
        cktk_s6->clear();
        cktk_s6->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "0 ns", 0)
         << QApplication::translate("hybrid_window", "12.5 ns", 0)
         << QApplication::translate("hybrid_window", "25 ns", 0)
        );
        label_34->setText(QApplication::translate("hybrid_window", "TK Pulses", 0));
        groupBox->setTitle(QApplication::translate("hybrid_window", "VMM", 0));
        Box_vmm1->setText(QApplication::translate("hybrid_window", "1", 0));
        Box_vmm2->setText(QApplication::translate("hybrid_window", "2", 0));
        groupBox_2->setTitle(QApplication::translate("hybrid_window", "Position", 0));
        cktkLabel_2->setText(QApplication::translate("hybrid_window", "Axis", 0));
        cktk_s6_2->clear();
        cktk_s6_2->insertItems(0, QStringList()
         << QApplication::translate("hybrid_window", "X", 0)
         << QApplication::translate("hybrid_window", "Y", 0)
        );
        cktkLabel_3->setText(QApplication::translate("hybrid_window", "Position", 0));
    } // retranslateUi

};

namespace Ui {
    class hybrid_window: public Ui_hybrid_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HYBRID_WINDOW_H
