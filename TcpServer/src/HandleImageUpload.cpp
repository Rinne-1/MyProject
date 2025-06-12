#include "HandleImageUpload.h"
#include "recvall.h"

void HandleImageUpload(SOCKET clientSocket, DatabaseManager& db, const string& username)
{
	int nameLen = 0;
	if (!recvAll(clientSocket, (char*)&nameLen, sizeof(int)))
	{
		cout << "接收图片长度失败" << endl;
		return;
	}
	nameLen = ntohl(nameLen); // 转换字节序
	if (nameLen <= 0 || nameLen > 1024)
	{
		cout << "图片名长度非法: " << nameLen << endl;
		return;
	}

	char* imageName = new char[nameLen + 1];
	memset(imageName, 0, nameLen + 1);
	if (!recvAll(clientSocket, imageName, nameLen))
	{
		cout << "接收图片名失败" << endl;
		delete[] imageName;
		return;
	}
	imageName[nameLen] = '\0'; // 添加结尾符

	int dataLen = 0;
	if (!recvAll(clientSocket, (char*)&dataLen, sizeof(int)))
	{
		std::cout << "接收图片长度失败" << std::endl;
		delete[] imageName;
		return;
	}

	dataLen = ntohl(dataLen);
	if (dataLen <= 0 || dataLen > 100 * 1024 * 1024) // 限制100MB
	{
		cout << "图片数据长度非法: " << dataLen << endl;
		delete[] imageName;
		return;
	}

	char* imageData = new char[dataLen];
	if (!recvAll(clientSocket, imageData, dataLen))
	{
		cout << "接收图片数据失败" << endl;
		delete[] imageName;
		delete[] imageData;
		return;
	}

	// 保存图片到本地
	string savePath = "D:\\lrq\\Picture\\";
	string fullPath = savePath + imageName;

	ofstream out(fullPath.c_str(), ios::binary);
	if (!out) {
		cout << "文件打开失败: " << fullPath << endl;
		delete[] imageName;
		delete[] imageData;
		return;
	}
	out.write(imageData, dataLen);
	out.close();
	cout << "图片保存成功：" << fullPath << endl;

	// 插入数据库
	if (!db.insertImageInfo(imageName, dataLen, fullPath, username))
	{
		cout << "图片信息插入数据库失败！" << endl;
	}
	else
	{
		cout << "图片信息成功插入数据库。" << endl;
	}

	delete[] imageName;
	delete[] imageData;


}