#include "HandleLogin.h"
#include "DatabaseManager.h"
#include "HandleImageUpload.h"
#include "HandleImageList.h"
#include "HandleImageRequest.h"



void HandleLogin(SOCKET clientSocket, DatabaseManager& db)
{
	char buffer[512] = { 0 };
	int recvLen = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
	if (recvLen <= 0)
	{
		cout << "接收数据失败或连接关闭" << endl;
		closesocket(clientSocket);
		WSACleanup();
		return;
	}

	buffer[recvLen] = '\0';
	string recvStr(buffer);

	// 简单用 ':' 分隔，格式：username:md5password
	size_t pos = recvStr.find(':');
	if (pos == string::npos)
	{
		const char* msg = "ERROR:格式错误";
		int cmd = htonl(1);  // 指令类型：1
		int payloadSize = htonl((int)strlen(msg));  // 负载大小（大端序）

		// 先发送 cmd
		send(clientSocket, (char*)&cmd, sizeof(cmd), 0);

		// 再发送 payloadSize
		send(clientSocket, (char*)&payloadSize, sizeof(payloadSize), 0);

		// 最后发送 payload 内容
		send(clientSocket, msg, (int)strlen(msg), 0);

		closesocket(clientSocket);
		WSACleanup();
		return;
	}

	string username = recvStr.substr(0, pos);
	string passwordHash = recvStr.substr(pos + 1);

	if (db.validateUser(username, passwordHash))
	{
		const char* msg = "SUCCESS:login success";
		int cmd = htonl(1);  // 指令类型：1
		int payloadSize = htonl((int)strlen(msg));  // 负载大小（大端序）

		// 先发送 cmd
		send(clientSocket, (char*)&cmd, sizeof(cmd), 0);

		// 再发送 payloadSize
		send(clientSocket, (char*)&payloadSize, sizeof(payloadSize), 0);

		// 最后发送 payload 内容
		send(clientSocket, msg, (int)strlen(msg), 0);

		cout << "用户登陆成功" << endl;
		while (true)
		{
			int commandType = 0;
			int recvLen = recv(clientSocket, (char*)&commandType, sizeof(int), 0);
			if (recvLen <= 0)
			{
				cout << "客户端断开连接或接收失败" << endl;
				break;
			}

			commandType = ntohl(commandType); // 网络转主机字节序

			if (commandType == 1)
			{
				// 处理图片上传
				HandleImageUpload(clientSocket, db, username);
			}

			if (commandType == 2)
			{
				HandleImageList(clientSocket, db);
			}

			if (commandType == 3)
			{
				HandleImageRequest(clientSocket, db);
			}

		}
		closesocket(clientSocket);
	}

	else
	{
		const char* msg = "ERROR:username or password error";
		int cmd = htonl(1);  // 指令类型：1
		int payloadSize = htonl((int)strlen(msg));  // 负载大小（大端序）

		// 先发送 cmd
		send(clientSocket, (char*)&cmd, sizeof(cmd), 0);

		// 再发送 payloadSize
		send(clientSocket, (char*)&payloadSize, sizeof(payloadSize), 0);

		// 最后发送 payload 内容
		send(clientSocket, msg, (int)strlen(msg), 0);

		cout << "账号或密码错误" << endl;
		closesocket(clientSocket);
	}

	return;
}