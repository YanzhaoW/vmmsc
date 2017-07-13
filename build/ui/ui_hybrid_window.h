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
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_hybrid_window
{
public:
    QHBoxLayout *horizontalLayout;
    QGroupBox *groupBox;
    QVBoxLayout *verticalLayout;
    QCheckBox *Box_vmm1;
    QCheckBox *Box_vmm2;
    QTabWidget *tabWidget;

    void setupUi(QWidget *hybrid_window)
    {
        if (hybrid_window->objectName().isEmpty())
            hybrid_window->setObjectName(QStringLiteral("hybrid_window"));
        hybrid_window->resize(400, 300);
        horizontalLayout = new QHBoxLayout(hybrid_window);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        groupBox = new QGroupBox(hybrid_window);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        verticalLayout = new QVBoxLayout(groupBox);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        Box_vmm1 = new QCheckBox(groupBox);
        Box_vmm1->setObjectName(QStringLiteral("Box_vmm1"));

        verticalLayout->addWidget(Box_vmm1);

        Box_vmm2 = new QCheckBox(groupBox);
        Box_vmm2->setObjectName(QStringLiteral("Box_vmm2"));

        verticalLayout->addWidget(Box_vmm2);


        horizontalLayout->addWidget(groupBox);

        tabWidget = new QTabWidget(hybrid_window);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));

        horizontalLayout->addWidget(tabWidget);


        retranslateUi(hybrid_window);

        tabWidget->setCurrentIndex(-1);


        QMetaObject::connectSlotsByName(hybrid_window);
    } // setupUi

    void retranslateUi(QWidget *hybrid_window)
    {
        hybrid_window->setWindowTitle(QApplication::translate("hybrid_window", "Form", 0));
        groupBox->setTitle(QApplication::translate("hybrid_window", "VMM", 0));
        Box_vmm1->setText(QApplication::translate("hybrid_window", "1", 0));
        Box_vmm2->setText(QApplication::translate("hybrid_window", "2", 0));
    } // retranslateUi

};

namespace Ui {
    class hybrid_window: public Ui_hybrid_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_HYBRID_WINDOW_H
