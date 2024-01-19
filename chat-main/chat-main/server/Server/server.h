#pragma once
#include <winsock2.h>
#include <vector>
#include <thread>
#include "Communication.h"
#include "sqlite3.h"
#include "DBCommand.h"

typedef struct sockaddr_in saddrin;

//typedef struct _dbdata {
//	int argc;
//	char** argv;
//	char** colName;
//}DBData;

//typedef struct _usercode {
//	char a, b, c, d;
//}UserCode;

class Server {
private:
	SOCKET serverSock;
	WSADATA wsaData;
	saddrin servAddr;

	DBCommand dbCmd{};

	std::vector<SOCKET> clntSocks;
	std::vector<saddrin> clntAddrs;

	bool accepting;
	bool initFail;

	sqlite3* db;
public:
	Server(int);
	bool GetInitFail();
	//bool ConnectDB();
	void AcceptThread();
	void CommunicationThread(SOCKET,char*);
	void Communication(SOCKET, Message);
	void Stop();
	void Broadcast(const char*);
	//unsigned int CreateUserCode();

	//static int DBCallback(void*, int, char**, char**);

	void CommunicationTest(int);

	void PrintUserCode();
};

