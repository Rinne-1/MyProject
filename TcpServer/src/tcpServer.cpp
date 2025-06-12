#include <winsock2.h>
#include <ws2tcpip.h>
#include "DatabaseManager.h"
#include "HandleRegister.h"
#include "HandleLogin.h"
#include "ThreadPool.h"

#pragma comment (lib, "ws2_32.lib")  //加载 ws2_32.dll
#pragma warning(disable:4996)

int main()
{
	//数据库连接
	DatabaseManager db("MySql_odbc", "root", "lrq030510");
	if (!db.connect())
	{
		cout << "连接数据库失败！" << endl;
		return 1;
	}

	//初始化Winsock
	WSAData wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0)
	{
		cout << "WSAStartup 初始化失败，错误码：" << ret << endl;
		return 1;
	}

	//创建socket
	SOCKET RegisterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKET LoginSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (RegisterSocket == INVALID_SOCKET)
	{
		cout << "socket创建失败1" << endl;
		WSACleanup();
		return 1;
	}
	if (LoginSocket == INVALID_SOCKET)
	{
		cout << "socket创建失败2" << endl;
		WSACleanup();
		return 1;
	}

	//绑定地址和端口
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8888);
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (::bind(RegisterSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
	{
		cout << "绑定失败1" << endl;
		closesocket(RegisterSocket);
		WSACleanup();
		return 1;
	}

	sockaddr_in serverLoginAddr{};
	serverLoginAddr.sin_family = AF_INET;
	serverLoginAddr.sin_port = htons(8890);
	serverLoginAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	if (::bind(LoginSocket, (sockaddr*)&serverLoginAddr, sizeof(serverLoginAddr)) == SOCKET_ERROR)
	{
		cout << "绑定失败2" << endl;
		closesocket(RegisterSocket);
		WSACleanup();
		return 1;
	}

	//开始监听
	if (listen(RegisterSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "监听失败1" << endl;
		closesocket(RegisterSocket);
		WSACleanup();
		return 1;
	}
	if (listen(LoginSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "监听失败2" << endl;
		closesocket(LoginSocket);
		WSACleanup();
		return 1;
	}

	cout << "开始监听模式！" << endl;
	ThreadPool pool(8); // 线程池

	//select监听两个socket
	while (true)
	{
		fd_set readfds;
		FD_ZERO(&readfds);
		FD_SET(RegisterSocket, &readfds);
		FD_SET(LoginSocket, &readfds);

		int maxSocket = max(RegisterSocket, LoginSocket) + 1;
		int activity = select(maxSocket, &readfds, nullptr, nullptr, nullptr);

		if (activity < 0)
		{
			cout << "select 错误" << endl;
			break;
		}

		if (FD_ISSET(RegisterSocket, &readfds))
		{
			SOCKET client = accept(RegisterSocket, nullptr, nullptr);
			if (client != INVALID_SOCKET)
			{
				cout << "[注册] 接收到客户端连接" << endl;
				pool.enqueue([client, &db]() { HandleRegister(client, db);});
			}
		}

		if (FD_ISSET(LoginSocket, &readfds))
		{
			SOCKET client = accept(LoginSocket, nullptr, nullptr);
			if (client != INVALID_SOCKET)
			{
				cout << "[登录] 接收到客户端连接" << endl;
				pool.enqueue([client, &db]() { HandleLogin(client, db);});
			}
		}
	}

	closesocket(LoginSocket);
	closesocket(RegisterSocket);

	WSACleanup();
	db.disconnect();

	return 0;

}