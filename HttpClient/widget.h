#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QDebug>
#include <QPixmap>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QTimer>
#include <QFile>
#include <QFileInfo>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void on_selectButton_clicked();
    void on_uploadList_itemDoubleClicked(QListWidgetItem *item);
    void on_registerButton_clicked();
    void on_loginButton_clicked();
    void on_uploadButton_clicked();
    void on_refreshButton_clicked();
    void on_deleteButton_clicked();
    void on_infoList_itemDoubleClicked(QListWidgetItem *item);

private:
    // 网络请求相关
    QJsonObject sendHttpRequest(const QString& endpoint, const QString& method,
                               const QJsonObject& data = QJsonObject(),
                               QHttpMultiPart* multiPart = nullptr);

    // 响应处理
    void handleRegisterResponse(const QJsonObject& response);
    void handleLoginResponse(const QJsonObject& response);
    void handleUploadResponse(const QJsonObject& response);
    void handleImageListResponse(const QJsonObject& response);
    void handleImageDataResponse(QNetworkReply* reply);

    // 辅助方法
    QString calculateMD5(const QString& input);
    QHttpMultiPart* createImageMultiPart(const QString& imagePath, const QString& imageName);

private:
    Ui::Widget *ui;
    QNetworkAccessManager* networkManager;
    QString currentUsername;
    QString serverUrl = "http://localhost:8080";
    QLabel* dynamicImageLabel = nullptr;
};

#endif // WIDGET_H
