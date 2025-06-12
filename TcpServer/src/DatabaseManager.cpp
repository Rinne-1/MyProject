#include "DatabaseManager.h"


DatabaseManager::DatabaseManager(const string& dsn, const string& user, const string& password)
	: dsn(dsn), user(user), password(password)
{
}

DatabaseManager::~DatabaseManager()
{
	disconnect();
}

bool DatabaseManager::connect()
{
	// 分配环境句柄
	if (SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv) != SQL_SUCCESS)
	{
		return false;
	}
	//设置环境变量
	if (SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0) != SQL_SUCCESS)
	{
		return false;
	}
	// 分配连接句柄
	if (SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc) != SQL_SUCCESS)
	{
		return false;
	}
	// 构造连接字符串
	string connStr = "DSN=" + dsn + ";UID=" + user + ";PWD=" + password + ";" + ";DATABASE=pic_server;";

	SQLCHAR outstr[1024];
	SQLSMALLINT outstrlen;

	SQLRETURN ret = SQLDriverConnectA(
		hDbc, NULL, (SQLCHAR*)connStr.c_str(), SQL_NTS,
		outstr, sizeof(outstr), &outstrlen, SQL_DRIVER_COMPLETE
	);

	if (SQL_SUCCEEDED(ret))
	{
		connected = true;
		cout << "数据库连接成功！" << endl;
		return true;
	}
	else
	{
		cout << "数据库连接失败！" << endl;
		return false;
	}
}

void DatabaseManager::disconnect()
{
	if (connected)
	{
		SQLDisconnect(hDbc);
		SQLFreeHandle(SQL_HANDLE_DBC, hDbc);
		SQLFreeHandle(SQL_HANDLE_ENV, hEnv);
		connected = false;
		cout << "数据库断开连接。" << endl;
	}
}

bool DatabaseManager::isConnected() const
{
	return connected;
}

bool DatabaseManager::checkUserExists(const string& username)
{
	if (!connected)
	{
		return false;
	}

	SQLHSTMT hStmt = nullptr;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS)
	{
		return false;
	}

	string sql = "SELECT COUNT(*) FROM user_info WHERE account = ?";
	if (SQLPrepareA(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)username.c_str(), 0, NULL);

	int count = 0;
	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		SQLBindCol(hStmt, 1, SQL_C_SLONG, &count, 0, NULL);
		SQLFetch(hStmt);
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return count > 0;
}

bool DatabaseManager::insertNewUser(const string& username, const string& password)
{
	if (!connected)
	{
		return false;
	}

	SQLHSTMT hStmt = nullptr;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS)
	{
		return false;
	}

	string sql = "INSERT INTO user_info (account, password) VALUES (?, ?)";
	if (SQLPrepareA(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)username.c_str(), 0, NULL);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)password.c_str(), 0, NULL);

	SQLRETURN ret = SQLExecute(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}

bool DatabaseManager::validateUser(const string& username, const string& password)
{
	if (!connected)
	{
		return false;
	}

	SQLHSTMT hStmt = nullptr;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS)
	{
		return false;
	}

	string sql = "SELECT COUNT(*) FROM user_info WHERE account = ? AND password = ?";
	if (SQLPrepareA(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	// 绑定参数：账号和密码（已加密）
	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)username.c_str(), 0, nullptr);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)password.c_str(), 0, nullptr);

	int count = 0;
	if (SQLExecute(hStmt) == SQL_SUCCESS)
	{
		SQLBindCol(hStmt, 1, SQL_C_SLONG, &count, 0, nullptr);
		SQLFetch(hStmt);
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return count > 0;
}


bool DatabaseManager::insertImageInfo(const string& filename, int size, const string& path, const string& account)
{

	if (!connected)
	{
		return false;
	}

	SQLHSTMT hStmt = nullptr;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS)
	{
		return false;
	}

	string sql = "INSERT INTO pic_info (filename, size, path, account) VALUES (?, ?, ?,?)";
	if (SQLPrepareA(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)filename.c_str(), 0, nullptr);
	SQLBindParameter(hStmt, 2, SQL_PARAM_INPUT, SQL_C_SLONG, SQL_INTEGER, 0, 0,
		(SQLPOINTER)&size, 0, nullptr);
	SQLBindParameter(hStmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)path.c_str(), 0, nullptr);
	SQLBindParameter(hStmt, 4, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)account.c_str(), 0, nullptr);

	SQLRETURN ret = SQLExecute(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

	return (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
}


bool DatabaseManager::getUserImageList(const string& username, vector<string>& imageNames)
{
	if (!connected)
	{
		return false;
	}

	SQLHSTMT hStmt = nullptr;
	if (SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt) != SQL_SUCCESS)
	{
		return false;
	}

	string sql = "SELECT filename FROM pic_info WHERE account = ?";
	if (SQLPrepareA(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	// 绑定参数，SQL_NTS 表示字符串以'\0'结尾
	if (SQLBindParameter(hStmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 0, 0,
		(SQLPOINTER)username.c_str(), 0, nullptr) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	// 执行查询
	if (SQLExecute(hStmt) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	char filename[256] = { 0 };
	SQLLEN indicator = 0;
	if (SQLBindCol(hStmt, 1, SQL_C_CHAR, filename, sizeof(filename), &indicator) != SQL_SUCCESS)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
		return false;
	}

	while (SQLFetch(hStmt) == SQL_SUCCESS)
	{
		if (indicator == SQL_NULL_DATA)
		{
			continue; // 忽略空值
		}
		imageNames.emplace_back(filename);
	}

	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return true;
}



