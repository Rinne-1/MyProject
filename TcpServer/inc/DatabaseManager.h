#pragma once
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>

using namespace std;


class DatabaseManager
{
public:
	DatabaseManager(const string& dsn, const string& user, const string& password);
	~DatabaseManager();
	bool connect();
	void disconnect();
	bool isConnected() const;
	bool checkUserExists(const string& username);
	bool insertNewUser(const string& username, const string& password);
	bool validateUser(const string& username, const string& password);
	bool insertImageInfo(const string& filename, int size, const string& path, const string& account);
	bool getUserImageList(const string& username, std::vector<std::string>& imageNames);




private:
	string dsn;
	string user;
	string password;
	SQLHENV hEnv = nullptr;
	SQLHDBC hDbc = nullptr;
	bool connected = false;
};
