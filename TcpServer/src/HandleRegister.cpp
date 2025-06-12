#include "HandleRegister.h"
#include "DatabaseManager.h"


void HandleRegister(SOCKET clientSocket, DatabaseManager& db)
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
		send(clientSocket, msg, (int)strlen(msg), 0);
		closesocket(clientSocket);
		WSACleanup();
		return;
	}

	string username = recvStr.substr(0, pos);
	string passwordHash = recvStr.substr(pos + 1);

	if (username.empty() || passwordHash.empty())
	{
		const char* msg = "ERROR:account or password empty";
		send(clientSocket, msg, (int)strlen(msg), 0);
		closesocket(clientSocket);
		WSACleanup();
		return;
	}

	// 检查用户是否存在
	if (db.checkUserExists(username))
	{
		const char* msg = "ERROR:user already exists";
		send(clientSocket, msg, (int)strlen(msg), 0);
	}
	else
	{
		if (db.insertNewUser(username, passwordHash))
		{
			const char* msg = "SUCCESS:register susscess";
			send(clientSocket, msg, (int)strlen(msg), 0);
		}
		else
		{
			const char* msg = "ERROR:register failed";
			send(clientSocket, msg, (int)strlen(msg), 0);
		}
	}
	closesocket(clientSocket);
	return;

}