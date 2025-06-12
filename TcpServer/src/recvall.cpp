#include "recvall.h"

bool recvAll(SOCKET sock, char* buffer, int length)
{
	int received = 0;
	while (received < length)
	{
		int ret = recv(sock, buffer + received, length - received, 0);
		if (ret <= 0)
		{
			return false; // 接收失败或连接断开
		}
		received += ret;
	}
	return true;
}