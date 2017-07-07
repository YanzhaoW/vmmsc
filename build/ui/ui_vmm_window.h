/********************************************************************************
** Form generated from reading UI file 'vmm_window.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VMM_WINDOW_H
#define UI_VMM_WINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_vmm_window
{
public:
    QPushButton *pushButton;

    void setupUi(QWidget *vmm_window)
    {
        if (vmm_window->objectName().isEmpty())
            vmm_window->setObjectName(QStringLiteral("vmm_window"));
        vmm_window->resize(400, 300);
        pushButton = new QPushButton(vmm_window);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setGeometry(QRect(240, 120, 80, 22));

        retranslateUi(vmm_window);

        QMetaObject::connectSlotsByName(vmm_window);
    } // setupUi

    void retranslateUi(QWidget *vmm_window)
    {
        vmm_window->setWindowTitle(QApplication::translate("vmm_window", "Form", 0));
        pushButton->setText(QApplication::translate("vmm_window", "PushButton", 0));
    } // retranslateUi

};

namespace Ui {
    class vmm_window: public Ui_vmm_window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VMM_WINDOW_H
