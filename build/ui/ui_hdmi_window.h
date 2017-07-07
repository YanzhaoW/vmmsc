/********************************************************************************
** Form generated from reading UI file 'hdmi_window.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_HDMI_WINDOW_H
#define UI_HDMI_WINDOW_H

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

class Ui_hdmi_window
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *Box_hybrid1;
    QTabWidget *tabWidget;

    void setupUi(QWidget *hdmi_window)
    {
        if (hdmi_window->objectName().isEmpty())
            hdmi_window->setObjectName(QStringLiteral("hdmi_window"));
        hdmi_window->resize(400, 300);
        horizontalLayout = new QHBoxLayout(hdmi_window);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(hdmi_window);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        Box_hybrid1 = new QCheckBox(groupBox);
        Box_hybrid1->setObjectName(QStringLiteral("Box_hybrid1"));

        verticalLayout->addWidget(Box_hybrid1);


        horizontalLayout->addWidget(groupBox);

        tabWidget = new QTabWidget(hdmi_window);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        horizontalLayout->addWidget(tabWidget);


        retranslateUi(hdmi_window);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(hdmi_window);
    } // setupUi

    void retranslateUi(QWidget *hdmi_window)
    {
        hdmi_window->setWindowTitle(QApplication::translate("hdmi_window", "Form", 0));
        groupBox->setTitle(QApplication::translate("hdmi_window", "Hybrid", 0));
        Box_hybrid1->setText(QApplication::translate("hdmi_window", "1", 0));
    } // retranslateUi

};

namespace Ui {
    class hdmi_window: public Ui_hdmi_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HDMI_WINDOW_H
