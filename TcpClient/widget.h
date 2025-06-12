#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QDir>
#include <QDebug>
#include <QListWidgetItem>
#include  <QMessageBox>
#include <QTcpSocket>
#include <QCryptographicHash>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();
    void loginResponse(const QByteArray& payload);
    void refreshList(const QByteArray& payload);
    void handleImageMessage(const QByteArray& imageData);

private slots:
    void on_selectButton_clicked();

    void on_uploadList_itemDoubleClicked(QListWidgetItem *item);

    void on_registerButton_clicked();

    void on_loginButton_clicked();

    void on_uploadButton_clicked();

    void on_refreshButton_clicked();

    void onReadyRead();


    void on_deleteButton_clicked();

    void on_infoList_itemDoubleClicked(QListWidgetItem *item);

private:
    Ui::Widget *ui;
    QTcpSocket *registerSocket;
    QTcpSocket *loginSocket;
    QLabel* dynamicImageLabel = nullptr;
    QByteArray recvBuffer;
};

#endif // WIDGET_H
