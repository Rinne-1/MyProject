#pragma once
#include "DatabaseManager.h"


void HandleImageUpload(SOCKET clientSocket, DatabaseManager& db, const string& username);
