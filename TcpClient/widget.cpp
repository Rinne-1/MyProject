#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    registerSocket = new QTcpSocket(this);
    loginSocket = new QTcpSocket(this);

    connect(loginSocket, &QTcpSocket::readyRead, this, &Widget::onReadyRead);

    connect(registerSocket, &QTcpSocket::readyRead, this, [=]()
    {
        QByteArray replyData = registerSocket->readAll();
        QString replyStr = QString::fromUtf8(replyData);

        // 根据返回内容提示
        if (replyStr.startsWith("SUCCESS"))
        {
            QMessageBox::information(this, "提醒", replyData);
        }
        else
        {
            QMessageBox::warning(this, "提醒",replyData);
        }
    });

    //连接断开信号
    connect(registerSocket, &QTcpSocket::disconnected, this, [this]() {
        QMessageBox::warning(this, "连接提示", "连接已断开！");
    });

    //连接断开信号
    connect(loginSocket, &QTcpSocket::disconnected, this, [this]() {
        QMessageBox::warning(this, "连接提示", "连接已断开！");
    });

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
    //连接至服务端
    QString ip = "127.0.0.1"; // 服务端 IP
    quint16 port = 8888;      // 服务端端口

    registerSocket->connectToHost(ip, port);

    if (!registerSocket->waitForConnected(3000))
    {
        QMessageBox::warning(this, "连接失败", "无法连接到服务器！");
    }
    else
    {
        QMessageBox::information(this, "连接成功", "已连接到服务器！");
    }


    QString username = ui->userEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "警告", "账号或密码不能为空！");
        return;
    }

    //计算MD5密码
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();


    // 拼接发送数据
    QString dataToSend = username + ":" + passwordHash;
    registerSocket->write(dataToSend.toUtf8());
    registerSocket->flush();

}


void Widget::on_loginButton_clicked()
{
    //连接至服务端
    QString ip = "127.0.0.1"; // 替换为你的服务端 IP
    quint16 port = 8890;      // 替换为你的服务端端口

    loginSocket->connectToHost(ip, port);

    if (!loginSocket->waitForConnected(3000))
    {
        QMessageBox::warning(this, "连接失败", "无法连接到服务器！");
    }
    else
    {
        QMessageBox::information(this, "连接成功", "已连接到服务器！");
    }


    QString username = ui->userEdit->text().trimmed();
    QString password = ui->passwordEdit->text();
    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "警告", "账号或密码不能为空！");
        return;
    }

    //计算MD5密码
    QByteArray passwordHash = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Md5).toHex();


    // 拼接发送数据
    QString dataToSend = username + ":" + passwordHash;
    loginSocket->write(dataToSend.toUtf8());
    loginSocket->flush();

}

void Widget::on_uploadButton_clicked()
{
    if (!loginSocket || loginSocket->state() != QAbstractSocket::ConnectedState)
    {
        QMessageBox::warning(this, "未连接", "请先登录！");
        return;
    }

    int count = ui->uploadList->count();
    if(count == 0)
    {
        QMessageBox::warning(this, "错误", "图片列表为空");
        return;
    }

    for(int i = 0; i < count; i++)
    {
        QListWidgetItem* item = ui->uploadList->item(i);
        QString imagePath = item->data(Qt::UserRole).toString();
        qDebug()<<imagePath<<endl;
        QFile file(imagePath);

        if(!file.open(QIODevice::ReadOnly))
        {
            QMessageBox::warning(this, "错误", "无法打开图片文件: " + imagePath);
            continue;
        }

        QByteArray fileData = file.readAll();
        qDebug()<<"image size:"<<fileData.size();
        file.close();


        QString imageName = QFileInfo(imagePath).fileName();
        QByteArray nameBytes = imageName.toUtf8();
        qDebug()<<"name:"<<nameBytes;
        qint32 nameLen = nameBytes.size();
        qint32 dataLen = fileData.size();

        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setByteOrder(QDataStream::BigEndian);

        qint32 cmd = 1;

        //先写请求命令
        out << cmd;
        // 写nameLen
        out << nameLen;
        // 再写图片名（用 writeRawData 写原始字节）
        out.writeRawData(nameBytes.constData(), nameBytes.size());

        // 写dataLen
        out << dataLen;
        qDebug()<<"data:"<<data;
        // 写图片数据（也用 writeRawData）
        out.writeRawData(fileData.constData(), fileData.size());

        loginSocket->write(data);
        loginSocket->flush();
        loginSocket->waitForBytesWritten();

        qDebug() << "已发送图片:" << imageName;
    }

    QMessageBox::information(this, "完成", "图片上传完成");
    ui->uploadList->clear();
}



void Widget::on_refreshButton_clicked()
{
    if (!loginSocket || loginSocket->state() != QAbstractSocket::ConnectedState)
    {
        QMessageBox::warning(this, "未连接", "请先登录！");
        return;
    }

    // 构造请求：命令头 + 用户名长度 + 用户名
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    QString username = ui->userEdit->text().trimmed();  // 假设你在登录时保存了当前用户名
    QByteArray usernameBytes = username.toUtf8();

    qint32 cmd = 2; //2表示“获取图片列表”
    out << cmd;
    out << usernameBytes.size();
    out.writeRawData(usernameBytes.constData(), usernameBytes.size());

    loginSocket->write(request);
    loginSocket->flush();

    return;
}

void Widget::onReadyRead()
{
    recvBuffer.append(loginSocket->readAll());

    while (true)
    {
        // 至少要有 cmd + length = 8 字节
        if (recvBuffer.size() < 8)
            return;

        QDataStream stream(recvBuffer);
        stream.setByteOrder(QDataStream::BigEndian);

        qint32 cmd = 0;
        qint32 payloadSize = 0;

        stream >> cmd >> payloadSize;
        qDebug()<<recvBuffer;

        // 判断总数据是否够一个完整包
        if (recvBuffer.size() < 8 + payloadSize)
            return;

        // 取 payload
        QByteArray payload = recvBuffer.mid(8, payloadSize);
        qDebug()<<payload;
        qDebug()<<"-------------------------------";


        // 移除已处理的数据
        recvBuffer.remove(0, 8 + payloadSize);
        qDebug()<<recvBuffer;
        qDebug()<<"*******************************";

        // 处理指令
        switch (cmd)
        {
        case 1:
            loginResponse(payload);
            break;

        case 2:
            refreshList(payload);
            break;

        case 3:
            handleImageMessage(payload); // 解码显示图片
            break;

        case 4:
            QMessageBox::warning(this, "提醒", "获取图片失败");
            break;

        default:
            qDebug() << "未知命令: " << cmd;
            break;
        }
    }
}


void Widget::on_deleteButton_clicked()
{
    ui->uploadList->clear();
}

void Widget::loginResponse(const QByteArray& payload)
{
    QString replyStr = QString::fromUtf8(payload);
    // 根据返回内容提示
    if (replyStr.startsWith("SUCCESS"))
    {
        QMessageBox::information(this, "提醒", replyStr);
    }
    else
    {
        QMessageBox::warning(this, "提醒",replyStr);
    }
    return;
}


void Widget::refreshList(const QByteArray& payload)
{
    QDataStream in(payload);
    in.setByteOrder(QDataStream::BigEndian);

    qint32 count = 0;
    in >> count;

    ui->infoList->clear();

    for (int i = 0; i < count; i++)
    {
        qint32 nameLen = 0;
        in >> nameLen;

        QByteArray nameData(nameLen, 0);
        in.readRawData(nameData.data(), nameLen);  // 这里读取具体的图片名

        QString imageName = QString::fromUtf8(nameData);  // 从 UTF-8 字节流转 QString
        qDebug() << "收到图片名: " << imageName;

        QListWidgetItem* item = new QListWidgetItem(imageName);  // 设置名称
        item->setData(Qt::UserRole, imageName);
        ui->infoList->addItem(item);
    }
}



void Widget::on_infoList_itemDoubleClicked(QListWidgetItem *item)
{
    QString imageName = item->text();

    // 构造请求
    QByteArray request;
    QDataStream out(&request, QIODevice::WriteOnly);
    out.setByteOrder(QDataStream::BigEndian);

    qint32 cmd = 3;
    QByteArray imageNameBytes = imageName.toUtf8();

    out << cmd;
    out << imageNameBytes.size();
    out.writeRawData(imageNameBytes.constData(), imageNameBytes.size());

    loginSocket->write(request);
    loginSocket->flush();
}

void Widget::handleImageMessage(const QByteArray& imageData)
{
    QPixmap pixmap;
    if (!pixmap.loadFromData(imageData))
    {
        qDebug() << "图片加载失败";
        return;
    }

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
    dynamicImageLabel->setPixmap(pixmap.scaled(400, 400, Qt::KeepAspectRatio));
    dynamicImageLabel->resize(dynamicImageLabel->pixmap()->size());
    dynamicImageLabel->show();

    connect(dynamicImageLabel, &QWidget::destroyed, this, [this]() {
        dynamicImageLabel = nullptr;
    });
}

