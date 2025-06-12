#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);

    // 初始化网络管理器
    networkManager = new QNetworkAccessManager(this);

    // 设置窗口标题
    this->setWindowTitle("图片远程存储客户端");
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_selectButton_clicked()
{
    QString filepath = QFileDialog::getOpenFileName(this, "请选择图片", QDir::homePath(),
                                                    "图片文件 (*.png *.jpg *.jpeg *.bmp);;所有文件 (*)");
    if(!filepath.isEmpty())
    {
        QFileInfo fileInfo(filepath);
        QString filename = fileInfo.fileName();
        QListWidgetItem *item = new QListWidgetItem(filename);
        ui->uploadList->addItem(item);
        item->setIcon(QIcon(filepath));
        item->setData(Qt::UserRole, filepath);
        qDebug() << "你选择的图片是：" << filename << "图片路径是：" << filepath;
    }
}

void Widget::on_uploadList_itemDoubleClicked(QListWidgetItem *item)
{
    QString fullPath = item->data(Qt::UserRole).toString();

    QPixmap pixmap(fullPath);
    if (!pixmap.isNull())
    {
        QLabel *label = new QLabel;
        label->setPixmap(pixmap.scaled(600, 400, Qt::KeepAspectRatio));
        label->setWindowTitle(item->text());
        label->setAttribute(Qt::WA_DeleteOnClose);
        label->resize(label->pixmap()->size());
        label->show();
    }
    else
    {
        QMessageBox::warning(this, "错误", "无法加载图片！");
    }
}

void Widget::on_registerButton_clicked()
{
    QString username = ui->userEdit->text().trimmed();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "警告", "账号或密码不能为空！");
        return;
    }

    // 计算MD5密码
    QString passwordHash = calculateMD5(password);

    // 构造请求数据
    QJsonObject requestData;
    requestData["username"] = username;
    requestData["password"] = passwordHash;

    // 发送HTTP请求
    QJsonObject response = sendHttpRequest("/api/register", "POST", requestData);
    handleRegisterResponse(response);
}

void Widget::on_loginButton_clicked()
{
    QString username = ui->userEdit->text().trimmed();
    QString password = ui->passwordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "警告", "账号或密码不能为空！");
        return;
    }

    // 计算MD5密码
    QString passwordHash = calculateMD5(password);

    // 构造请求数据
    QJsonObject requestData;
    requestData["username"] = username;
    requestData["password"] = passwordHash;

    // 发送HTTP请求
    QJsonObject response = sendHttpRequest("/api/login", "POST", requestData);
    handleLoginResponse(response);
}

void Widget::on_uploadButton_clicked()
{
    if (currentUsername.isEmpty())
    {
        QMessageBox::warning(this, "未登录", "请先登录！");
        return;
    }

    int count = ui->uploadList->count();
    if(count == 0)
    {
        QMessageBox::warning(this, "错误", "图片列表为空");
        return;
    }

    int successCount = 0;
    int failCount = 0;

    // 逐个上传图片
    for(int i = 0; i < count; i++)
    {
        QListWidgetItem* item = ui->uploadList->item(i);
        QString imagePath = item->data(Qt::UserRole).toString();
        QString imageName = QFileInfo(imagePath).fileName();

        // 创建multipart数据
        QHttpMultiPart* multiPart = createImageMultiPart(imagePath, imageName);
        if (!multiPart)
        {
            QMessageBox::warning(this, "错误", "无法读取图片文件: " + imagePath);
            failCount++;
            continue;
        }

        // 发送上传请求
        QJsonObject response = sendHttpRequest("/api/upload", "POST", QJsonObject(), multiPart);

        multiPart->deleteLater();

        if (response.contains("error"))
        {
            failCount++;
            qDebug() << "图片上传失败:" << imageName << response["error"].toString();
        }
        else if (response.contains("success"))
        {
            successCount++;
            qDebug() << "图片上传成功:" << imageName;
        }
        else
        {
            failCount++;
            qDebug() << "图片上传失败:" << imageName << "未知错误";
        }
    }

    QString message = QString("上传完成！成功: %1 个，失败: %2 个")
                     .arg(successCount).arg(failCount);
    QMessageBox::information(this, "上传结果", message);

    if (successCount > 0)
    {
        ui->uploadList->clear();
    }
}

void Widget::on_refreshButton_clicked()
{
    if (currentUsername.isEmpty())
    {
        QMessageBox::warning(this, "未登录", "请先登录！");
        return;
    }

    // 发送获取图片列表请求，添加用户名参数
    QString endpoint = QString("/api/images?username=%1").arg(currentUsername);
    QJsonObject response = sendHttpRequest(endpoint, "GET");
    handleImageListResponse(response);
}

void Widget::on_deleteButton_clicked()
{
    ui->uploadList->clear();
}

void Widget::on_infoList_itemDoubleClicked(QListWidgetItem *item)
{
    if (currentUsername.isEmpty())
    {
        QMessageBox::warning(this, "未登录", "请先登录！");
        return;
    }

    QString imageName = item->text();
    QString imageId = item->data(Qt::UserRole).toString();

    // 构造请求URL，添加用户名参数
    QString endpoint = QString("/api/image/%1?username=%2").arg(imageId).arg(currentUsername);

    // 创建请求
    QNetworkRequest request(QUrl(serverUrl + endpoint));

    // 发送请求
    QNetworkReply* reply = networkManager->get(request);

    // 等待响应
    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    handleImageDataResponse(reply);
    reply->deleteLater();
}

QJsonObject Widget::sendHttpRequest(const QString& endpoint, const QString& method,
                                   const QJsonObject& data, QHttpMultiPart* multiPart)
{
    QUrl url(serverUrl + endpoint);
    QNetworkRequest request(url);

    // 设置请求头
    if (!multiPart)
    {
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    }
    request.setRawHeader("User-Agent", "Qt Client");

    QNetworkReply* reply = nullptr;

    if (method == "POST")
    {
        if (multiPart)
        {
            reply = networkManager->post(request, multiPart);
        }
        else
        {
            QJsonDocument doc(data);
            QByteArray requestData = doc.toJson();
            qDebug() << "发送POST请求:" << endpoint;
            qDebug() << "请求数据:" << requestData;
            reply = networkManager->post(request, requestData);
        }
    }
    else if (method == "GET")
    {
        qDebug() << "发送GET请求:" << endpoint;
        reply = networkManager->get(request);
    }

    if (!reply)
    {
        QJsonObject errorResponse;
        errorResponse["error"] = "网络请求创建失败";
        return errorResponse;
    }

    // 等待响应
    QEventLoop loop;
    QTimer timer;
    timer.setSingleShot(true);
    timer.setInterval(10000); // 10秒超时

    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    QJsonObject response;

    if (timer.isActive())
    {
        timer.stop();

        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << "HTTP状态码:" << statusCode;

        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray responseData = reply->readAll();
            qDebug() << "原始响应:" << responseData;

            if (!responseData.isEmpty())
            {
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

                if (parseError.error != QJsonParseError::NoError)
                {
                    qDebug() << "JSON解析错误:" << parseError.errorString();
                    response["error"] = "JSON解析失败";
                }
                else
                {
                    response = doc.object();
                    qDebug() << "解析后的响应:" << doc.toJson();
                }
            }
            else
            {
                response["error"] = "空响应";
            }
        }
        else
        {
            qDebug() << "网络错误:" << reply->error() << reply->errorString();
            response["error"] = reply->errorString();
        }
    }
    else
    {
        response["error"] = "请求超时";
        reply->abort();
    }

    reply->deleteLater();
    return response;
}

void Widget::handleRegisterResponse(const QJsonObject& response)
{
    qDebug() << "处理注册响应:" << QJsonDocument(response).toJson();

    if (response.contains("error"))
    {
        QMessageBox::warning(this, "注册失败", response["error"].toString());
    }
    else if (response.contains("success"))
    {
        QMessageBox::information(this, "注册成功",
            response.contains("message") ? response["message"].toString() : "注册成功！");
    }
    else
    {
        QMessageBox::warning(this, "注册失败", "未知错误");
    }
}

void Widget::handleLoginResponse(const QJsonObject& response)
{
    qDebug() << "处理登录响应:" << QJsonDocument(response).toJson();

    if (response.contains("error"))
    {
        QMessageBox::warning(this, "登录失败", response["error"].toString());
        currentUsername.clear();
    }
    else if (response.contains("success"))
    {
        currentUsername = ui->userEdit->text().trimmed();
        QMessageBox::information(this, "登录成功", "欢迎回来，" + currentUsername + "！");
        // 登录成功后自动刷新图片列表
        on_refreshButton_clicked();
    }
    else
    {
        QMessageBox::warning(this, "登录失败", "未知错误");
        currentUsername.clear();
    }
}

void Widget::handleUploadResponse(const QJsonObject& response)
{
    if (response.contains("error"))
    {
        QMessageBox::warning(this, "上传失败", response["error"].toString());
    }
    else
    {
        qDebug() << "上传成功";
    }
}

void Widget::handleImageListResponse(const QJsonObject& response)
{
    qDebug() << "处理图片列表响应:" << QJsonDocument(response).toJson();

    if (response.contains("error"))
    {
        QMessageBox::warning(this, "获取列表失败", response["error"].toString());
        return;
    }

    ui->infoList->clear();

    if (response.contains("images"))
    {
        QJsonArray images = response["images"].toArray();

        for (const auto& imageValue : images)
        {
            QJsonObject imageObj = imageValue.toObject();
            QString imageName = imageObj["name"].toString();
            QString imageId = imageObj["id"].toString();
            QListWidgetItem* item = new QListWidgetItem(imageName);
            item->setData(Qt::UserRole, imageId);
            ui->infoList->addItem(item);
        }

        qDebug() << "获取到" << images.size() << "张图片";
    }
}

void Widget::handleImageDataResponse(QNetworkReply* reply)
{
    if (reply->error() != QNetworkReply::NoError)
    {
        QMessageBox::warning(this, "获取图片失败", reply->errorString());
        return;
    }

    QByteArray imageData = reply->readAll();
    if (imageData.isEmpty())
    {
        QMessageBox::warning(this, "错误", "图片数据为空");
        return;
    }

    QPixmap pixmap;
    if (!pixmap.loadFromData(imageData))
    {
        QMessageBox::warning(this, "错误", "图片数据损坏");
        return;
    }

    // 显示图片
    if (dynamicImageLabel)
    {
        dynamicImageLabel->close();
        dynamicImageLabel->deleteLater();
        dynamicImageLabel = nullptr;
    }

    dynamicImageLabel = new QLabel();
    dynamicImageLabel->setAttribute(Qt::WA_DeleteOnClose);
    dynamicImageLabel->setWindowFlags(Qt::Window);
    dynamicImageLabel->setWindowTitle("查看图片");

    // 限制最大显示尺寸
    QPixmap scaledPixmap = pixmap;
    if (pixmap.width() > 800 || pixmap.height() > 600)
    {
        scaledPixmap = pixmap.scaled(800, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    dynamicImageLabel->setPixmap(scaledPixmap);
    dynamicImageLabel->resize(dynamicImageLabel->pixmap()->size());
    dynamicImageLabel->show();

    connect(dynamicImageLabel, &QWidget::destroyed, this, [this]() {
        dynamicImageLabel = nullptr;
    });
}

QString Widget::calculateMD5(const QString& input)
{
    return QCryptographicHash::hash(input.toUtf8(), QCryptographicHash::Md5).toHex();
}

QHttpMultiPart* Widget::createImageMultiPart(const QString& imagePath, const QString& imageName)
{
    QFile* file = new QFile(imagePath);
    if (!file->open(QIODevice::ReadOnly))
    {
        delete file;
        return nullptr;
    }

    QHttpMultiPart* multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    // 添加用户名字段
    QHttpPart usernamePart;
    usernamePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                          QVariant("form-data; name=\"username\""));
    usernamePart.setBody(currentUsername.toUtf8());
    multiPart->append(usernamePart);

    // 添加图片文件
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                       QVariant(QString("form-data; name=\"image\"; filename=\"%1\"").arg(imageName)));
    imagePart.setBodyDevice(file);
    file->setParent(multiPart); // 确保multiPart被删除时file也被删除
    multiPart->append(imagePart);

    return multiPart;
}
