#include "HandleImageList.h"
#include "recvall.h"



void HandleImageList(SOCKET clientSocket, DatabaseManager& db)
{
	vector<string> imageNames;

	// 先接收用户名长度和用户名（你原代码一样）
	int32_t nameLenNet = 0;
	if (!recvAll(clientSocket, (char*)&nameLenNet, sizeof(int32_t)))
	{
		return;
	}

	int nameLen = ntohl(nameLenNet);

	string username(nameLen, '\0');
	if (!recvAll(clientSocket, &username[0], nameLen))
	{
		return;
	}

	if (!db.getUserImageList(username, imageNames))
	{
		return;
	}

	// 构造payload
	string payload;
	int32_t countNet = htonl(static_cast<int32_t>(imageNames.size()));
	payload.append(reinterpret_cast<char*>(&countNet), sizeof(int32_t));
	for (const auto& name : imageNames)
	{
		int32_t lenNet = htonl(static_cast<int32_t>(name.size()));
		payload.append(reinterpret_cast<const char*>(&lenNet), sizeof(int32_t));
		payload.append(name);
	}

	int32_t cmdNet = htonl(2);
	int32_t payloadSizeNet = htonl(static_cast<int32_t>(payload.size()));

	// 一次性发送完整包： cmd + payloadSize + payload
	send(clientSocket, (char*)&cmdNet, sizeof(int32_t), 0);
	send(clientSocket, (char*)&payloadSizeNet, sizeof(int32_t), 0);
	send(clientSocket, payload.data(), static_cast<int>(payload.size()), 0);

	cout << "发送图片列表完毕，共 " << imageNames.size() << " 张图片" << endl;
}



