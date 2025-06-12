/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.12.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QLabel *user;
    QLineEdit *userEdit;
    QLineEdit *passwordEdit;
    QLabel *password;
    QListWidget *uploadList;
    QListWidget *infoList;
    QPushButton *selectButton;
    QPushButton *uploadButton;
    QPushButton *refreshButton;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QPushButton *registerButton;
    QPushButton *loginButton;
    QPushButton *deleteButton;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(858, 572);
        user = new QLabel(Widget);
        user->setObjectName(QString::fromUtf8("user"));
        user->setGeometry(QRect(20, 40, 81, 41));
        userEdit = new QLineEdit(Widget);
        userEdit->setObjectName(QString::fromUtf8("userEdit"));
        userEdit->setGeometry(QRect(80, 30, 181, 41));
        passwordEdit = new QLineEdit(Widget);
        passwordEdit->setObjectName(QString::fromUtf8("passwordEdit"));
        passwordEdit->setGeometry(QRect(430, 30, 181, 41));
        password = new QLabel(Widget);
        password->setObjectName(QString::fromUtf8("password"));
        password->setGeometry(QRect(370, 40, 81, 41));
        uploadList = new QListWidget(Widget);
        uploadList->setObjectName(QString::fromUtf8("uploadList"));
        uploadList->setGeometry(QRect(20, 110, 301, 361));
        infoList = new QListWidget(Widget);
        infoList->setObjectName(QString::fromUtf8("infoList"));
        infoList->setGeometry(QRect(370, 110, 301, 361));
        selectButton = new QPushButton(Widget);
        selectButton->setObjectName(QString::fromUtf8("selectButton"));
        selectButton->setGeometry(QRect(20, 490, 101, 41));
        uploadButton = new QPushButton(Widget);
        uploadButton->setObjectName(QString::fromUtf8("uploadButton"));
        uploadButton->setGeometry(QRect(260, 490, 61, 41));
        refreshButton = new QPushButton(Widget);
        refreshButton->setObjectName(QString::fromUtf8("refreshButton"));
        refreshButton->setGeometry(QRect(580, 490, 61, 41));
        layoutWidget = new QWidget(Widget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(720, 10, 77, 83));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        registerButton = new QPushButton(layoutWidget);
        registerButton->setObjectName(QString::fromUtf8("registerButton"));

        verticalLayout->addWidget(registerButton);

        loginButton = new QPushButton(layoutWidget);
        loginButton->setObjectName(QString::fromUtf8("loginButton"));

        verticalLayout->addWidget(loginButton);

        deleteButton = new QPushButton(Widget);
        deleteButton->setObjectName(QString::fromUtf8("deleteButton"));
        deleteButton->setGeometry(QRect(160, 490, 61, 41));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QApplication::translate("Widget", "Widget", nullptr));
        user->setText(QApplication::translate("Widget", "<html><head/><body><p><span style=\" font-size:14pt;\">\350\264\246\345\217\267\357\274\232</span></p><p><br/></p></body></html>", nullptr));
        password->setText(QApplication::translate("Widget", "<html><head/><body><p><span style=\" font-size:14pt;\">\345\257\206\347\240\201\357\274\232</span></p><p><br/></p></body></html>", nullptr));
        selectButton->setText(QApplication::translate("Widget", "\350\257\267\351\200\211\346\213\251\346\234\254\345\234\260\346\226\207\344\273\266", nullptr));
        uploadButton->setText(QApplication::translate("Widget", "\344\270\212\344\274\240", nullptr));
        refreshButton->setText(QApplication::translate("Widget", "\345\210\267\346\226\260", nullptr));
        registerButton->setText(QApplication::translate("Widget", "\346\263\250\345\206\214", nullptr));
        loginButton->setText(QApplication::translate("Widget", "\347\231\273\345\275\225", nullptr));
        deleteButton->setText(QApplication::translate("Widget", "\346\270\205\351\231\244", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
