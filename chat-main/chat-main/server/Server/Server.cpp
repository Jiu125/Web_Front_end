#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include "Server.h"
#include <iostream>
#include <thread>
#include <time.h>

using namespace std;

Server::Server(int port) {
	this->accepting = true;

	this->initFail = false;

	WSAStartup(MAKEWORD(2, 2), &this->wsaData);
	this->serverSock = socket(PF_INET, SOCK_STREAM, 0);
	if (this->serverSock == 0) {
		this->initFail = true;
		cout << "socket error" << endl;
		return;
	}
	memset(&this->servAddr, 0, sizeof(this->serverSock));
	this->servAddr.sin_family = AF_INET;
	this->servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	this->servAddr.sin_port = htons(port);

	if (bind(this->serverSock, (SOCKADDR*)&this->servAddr, sizeof(this->servAddr)) == SOCKET_ERROR) {
		this->initFail = true;
		cout << "bind error " << WSAGetLastError() << endl;
		return;
	}
	cout << "server start. port: " << port << endl;
	listen(this->serverSock, 5);

}
void Server::AcceptThread() {
	int addrLen;
	saddrin tempAddr;
	cout << "AcceptThread start" << endl;
	while (this->accepting) {
		addrLen = sizeof(tempAddr);
		SOCKET clntSock = accept(this->serverSock, (SOCKADDR*)&tempAddr, &addrLen);
		this->clntSocks.push_back(clntSock);
		cout << "client connect. ip: " << inet_ntoa(tempAddr.sin_addr) << endl; //network to address?
		new thread(&Server::CommunicationThread, this, clntSock, inet_ntoa(tempAddr.sin_addr));
	}
	cout << "AcceptThread done" << endl;
	for (int i = 0; i < this->clntSocks.size(); i++) {
		closesocket(this->clntSocks[i]);
	}
	WSACleanup();
	return;
}
void Server::Communication(SOCKET source, Message msg) {
	//printf("type: %d\nbodySize: %d\n", msg.header.type, msg.header.bodySize);
	switch (msg.header.type&TYPEMASK) {
		case MSGTYPE_SIGNIN: {
			SigninData data = *((SigninData*)msg.body);
			printf("==Communication signin==\n");
			//printf("id: %s\npwd: %s\n", data.id, data.pwd);
			MessageHeader resHeader = { MSGTYPE_SIGNIN, UCODE_SERVER,0 };
			UserData *userData;

			int userCode=this->dbCmd.DoSignin(data.id, data.pwd);
			if (userCode == 1) {
				send(source, (char*)&resHeader, sizeof(MessageHeader), 0);
				break;
			}

			

			userData=this->dbCmd.GetUserData(userCode);
			if (userData == NULL) {
				send(source, (char*)&resHeader, sizeof(MessageHeader), 0);
				break;
			}

			printf("signin success. socket: %d ", source);
			DBCommand::ConvertUserCodeIPFormat(&userData->code, true);

			resHeader.type |= SUCCESS;
			resHeader.bodySize = sizeof(UserData);

			send(source, (char*)&resHeader, sizeof(MessageHeader), 0);
			send(source, (char*)userData, sizeof(UserData), 0);

			memset(userData, 0, sizeof(UserData));
			delete userData;
			break;
		}
		case MSGTYPE_SIGNUP: {
			printf("==Communication signup==\n");
			SignupData data = *((SignupData*)msg.body);
			MessageHeader resHeader = { MSGTYPE_SIGNUP,UCODE_SERVER,0 };
			//printf("id: %s\npwd: %s\nname: %s\nlang: %d\n", data.id, data.pwd, data.name, data.lang);

			UINT32 code = this->dbCmd.DoSignup(data);
			if (code == UCODE_SERVER) {
				send(source, (char*)&resHeader, sizeof(MessageHeader), 0);
				break;
			}

			resHeader.type |= SUCCESS;
			resHeader.bodySize = sizeof(code);

			printf("signup success. socket: %d ", source);
			DBCommand::ConvertUserCodeIPFormat(&code, true);
			send(source, (char*)&resHeader, sizeof(MessageHeader), 0);
			send(source, (char*)&code, sizeof(code), 0);

			memset(&data, 0, sizeof(SignupData));

			break;
		}
		case MSGTYPE_CHECKID: {
			printf("==Communication check id==\n");
			//msg.body[msg.header.bodySize-1] = 0;
			//printf("id: %s\n", msg.body);
			MessageHeader resHeader = { MSGTYPE_CHECKID ,UCODE_SERVER,0 };
			if (this->dbCmd.CheckId(msg.body)) {
				resHeader.type |= SUCCESS;
			}
			send(source, (char*)&resHeader, sizeof(MessageHeader), 0);
			break;
		}
		case MSGTYPE_ADDFRIEND: {
			printf("==Communication add friend==\n");
			UINT32 sourceUserCode = msg.header.srcUserCode;
			UINT32 targetUserCode = *((UINT32*)msg.body);
			//printf("targetUserCode: %d\n", targetUserCode);
			DBCommand::ConvertUserCodeIPFormat(&targetUserCode, true);

			MessageHeader resHeader = { MSGTYPE_ADDFRIEND,UCODE_SERVER,0 };

			UserData* targetUserData=this->dbCmd.AddFriend(sourceUserCode, targetUserCode);
			if (!targetUserData) {
				send(source, (char*)&resHeader, sizeof(MessageHeader), 0);
				break;
			}

			printf("add friend. success\n");
			resHeader.type |= SUCCESS;
			resHeader.bodySize = sizeof(UserData);

			send(source, (char*)&resHeader, sizeof(MessageHeader), 0);
			send(source, (char*)targetUserData, sizeof(UserData), 0);

			memset(targetUserData, 0, sizeof(UserData));
			delete targetUserData;
			break;
		}
	}
	memset(msg.body, 0, msg.header.bodySize);
	delete msg.body;
}
void Server::CommunicationThread(SOCKET client,char* addr) {
	Message msg;
	int recvSize;
	//printf("communication thread client sock: %d\n", client);
	while (1) {
		memset(&msg, 0, sizeof(Message));
		recvSize = recv(client, (char*)&msg.header, sizeof(MessageHeader), 0);
		printf("======header recv(size: %d). ip: %s\n",recvSize, addr);
		if (recvSize == SOCKET_ERROR || recvSize == 0) {
			printf("header recv error. errno:%d\n", WSAGetLastError());
			if (recvSize == SOCKET_ERROR) {
				printf("socket close (sock: %d, ip: %s)\n", client, addr);
				closesocket(client);
			}
			return;
		}
		//printf("recv header %d\n", recvSize);
		printf("type: %d\ncode: %d\nbodySize: %d\n", msg.header.type,msg.header.srcUserCode, msg.header.bodySize);
		DBCommand::ConvertUserCodeIPFormat(&msg.header.srcUserCode, true);

		if (msg.header.bodySize) {
			msg.body = new char[msg.header.bodySize];
			memset(msg.body, 0, msg.header.bodySize);

			recvSize = recv(client, (char*)msg.body, msg.header.bodySize, 0);
			printf("body recv(size: %d). ip: %s\n", recvSize, addr);
			if (recvSize == -1 || recvSize == 0) {
				printf("body recv error | recv: %d errno:%d\n", recvSize, WSAGetLastError());
				if (recvSize == SOCKET_ERROR) {
					printf("socket close (sock: %d, ip: %s)\n", client, addr);
					closesocket(client);
				}
				return;
			}
			//printf("recv body %d\n", recvSize);
		}
		this->Communication(client, msg);
		printf("===================\n");
	}   
}
void Server::Stop() {
	this->accepting = false;
	closesocket(this->serverSock);
}
void Server::Broadcast(const char* msg) {

	for (int i = 0; i < this->clntSocks.size(); i++) {
		send(this->clntSocks[i], msg, strlen(msg), 0);
	}
}


bool Server::GetInitFail() {
	return this->initFail;
}

void Server::PrintUserCode() {
	UINT32 code = this->dbCmd.CreateUserCode();
	DBCommand::ConvertUserCodeIPFormat(&code, true);
	//printf("user code: %X\n", this->dbCmd.CreateUserCode());
}
