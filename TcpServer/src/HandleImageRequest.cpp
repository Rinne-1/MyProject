#include "HandleImageRequest.h"
#include "recvall.h"


void HandleImageRequest(SOCKET clientSocket, DatabaseManager& db)
{
	// 1. 接收图片名长度 + 图片名字符串（大端）
	int32_t nameLenNet = 0;
	if (!recvAll(clientSocket, (char*)&nameLenNet, sizeof(int32_t)))
	{
		return;
	}

	int nameLen = ntohl(nameLenNet);

	char* imageName = new char[nameLen + 1];
	if (!recvAll(clientSocket, imageName, nameLen))
	{
		delete[] imageName;
		return;
	}
	imageName[nameLen] = '\0';

	// 2. 拼接图片文件完整路径，比如 D:\lrq\Picture\ + imageName
	string imagePath = "D:\\lrq\\Picture\\" + string(imageName);
	delete[] imageName;

	// 3. 打开图片文件，读取内容（用 fopen_s）
	FILE* fp = nullptr;
	errno_t err = fopen_s(&fp, imagePath.c_str(), "rb");
	if (err != 0 || !fp)
	{
		// 发送空数据或错误信息
		int32_t cmd = htonl(4);  // 假设4是返回图片失败
		send(clientSocket, (char*)&cmd, sizeof(int32_t), 0);
		return;
	}

	fseek(fp, 0, SEEK_END);
	int fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (fileSize <= 0)
	{
		fclose(fp);
		int32_t cmd = htonl(4);
		send(clientSocket, (char*)&cmd, sizeof(int32_t), 0);
		return;
	}

	char* fileBuffer = new char[fileSize];
	size_t readSize = fread(fileBuffer, 1, fileSize, fp);
	fclose(fp);

	if (readSize != (size_t)fileSize)
	{
		delete[] fileBuffer;
		int32_t cmd = htonl(4);
		send(clientSocket, (char*)&cmd, sizeof(int32_t), 0);
		return;
	}

	// 4. 发送命令字段 cmd=3 表示发送图片数据
	int32_t cmd = htonl(3);
	send(clientSocket, (char*)&cmd, sizeof(int32_t), 0);

	// 5. 发送图片数据大小
	int32_t sizeNet = htonl(fileSize);
	send(clientSocket, (char*)&sizeNet, sizeof(int32_t), 0);

	// 6. 发送图片二进制数据
	send(clientSocket, fileBuffer, fileSize, 0);

	delete[] fileBuffer;
}