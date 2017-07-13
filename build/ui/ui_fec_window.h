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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_fec_window
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *Box_hdmi1;
    QCheckBox *Box_hdmi2;
    QCheckBox *Box_hdmi3;
    QCheckBox *Box_hdmi4;
    QCheckBox *Box_hdmi5;
    QCheckBox *Box_hdmi6;
    QCheckBox *Box_hdmi7;
    QCheckBox *Box_hdmi8;
    QTabWidget *tabWidget;

    void setupUi(QWidget *fec_window)
    {
        if (fec_window->objectName().isEmpty())
            fec_window->setObjectName(QStringLiteral("fec_window"));
        fec_window->resize(1336, 906);
        horizontalLayout = new QHBoxLayout(fec_window);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(fec_window);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        Box_hdmi1 = new QCheckBox(groupBox);
        Box_hdmi1->setObjectName(QStringLiteral("Box_hdmi1"));

        verticalLayout->addWidget(Box_hdmi1);

        Box_hdmi2 = new QCheckBox(groupBox);
        Box_hdmi2->setObjectName(QStringLiteral("Box_hdmi2"));

        verticalLayout->addWidget(Box_hdmi2);

        Box_hdmi3 = new QCheckBox(groupBox);
        Box_hdmi3->setObjectName(QStringLiteral("Box_hdmi3"));

        verticalLayout->addWidget(Box_hdmi3);

        Box_hdmi4 = new QCheckBox(groupBox);
        Box_hdmi4->setObjectName(QStringLiteral("Box_hdmi4"));

        verticalLayout->addWidget(Box_hdmi4);

        Box_hdmi5 = new QCheckBox(groupBox);
        Box_hdmi5->setObjectName(QStringLiteral("Box_hdmi5"));

        verticalLayout->addWidget(Box_hdmi5);

        Box_hdmi6 = new QCheckBox(groupBox);
        Box_hdmi6->setObjectName(QStringLiteral("Box_hdmi6"));

        verticalLayout->addWidget(Box_hdmi6);

        Box_hdmi7 = new QCheckBox(groupBox);
        Box_hdmi7->setObjectName(QStringLiteral("Box_hdmi7"));

        verticalLayout->addWidget(Box_hdmi7);

        Box_hdmi8 = new QCheckBox(groupBox);
        Box_hdmi8->setObjectName(QStringLiteral("Box_hdmi8"));

        verticalLayout->addWidget(Box_hdmi8);


        horizontalLayout->addWidget(groupBox);

        tabWidget = new QTabWidget(fec_window);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        horizontalLayout->addWidget(tabWidget);


        retranslateUi(fec_window);

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
    } // retranslateUi

};

namespace Ui {
    class fec_window: public Ui_fec_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FEC_WINDOW_H
