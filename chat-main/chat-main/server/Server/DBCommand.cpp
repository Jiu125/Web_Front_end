#define _CRT_SECURE_NO_WARNINGS
#include "DBCommand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void DBCommand::ConvertUserCodeIPFormat(UINT32* code,bool newline) {
	UserCode userCode = *((UserCode*)code);
	if (newline) {
		printf("(code: %d.%d.%d.%d)\n", userCode.d, userCode.c, userCode.b, userCode.a);
	}
	else {
		printf("(code: %d.%d.%d.%d)", userCode.d, userCode.c, userCode.b, userCode.a);
	}
}

DBCommand::DBCommand() {
	int result = sqlite3_open("DataBase.db", &this->db);
	if (result == SQLITE_OK) {
		printf("database connected\n");
	}
	else {
		printf("database connect fail: %s\n", sqlite3_errmsg(this->db));
	}
}

UINT32 DBCommand::DoSignin(char* id, char* pwd) {
	char query[256] = { 0 };
	DBData data = { 0,0,0 };
	int queryResult;
	char* dbErrMsg;
	UINT32 code;

	sprintf(query, "SELECT id, pw, code FROM Account WHERE id=\"%s\" AND pw=\"%s\";", id, pwd);
	queryResult = sqlite3_exec(this->db, query, this->DBCallback, &data, &dbErrMsg);
	if (queryResult != SQLITE_OK || data.argc == 0) {
		printf("user not found (id: %s) (%s)\n",id, dbErrMsg);
		this->FreeDBData(data);
		return UCODE_SERVER;
	}
	code = atoi(data.argv[2]);
	this->FreeDBData(data);
	return code;
}
UserData* DBCommand::GetUserData(UINT32 code) {
	char query[256] = { 0 };
	DBData data = { 0,0,0 };
	int queryResult;
	char* dbErrMsg;

	sprintf(query, "SELECT code, name, lang FROM User WHERE code=%d AND friends is NULL;", code);
	queryResult = sqlite3_exec(this->db, query, this->DBCallback, &data, &dbErrMsg);
	if (queryResult != SQLITE_OK) {
		printf("can't get user data (%s)", dbErrMsg);
		ConvertUserCodeIPFormat(&code,true);
		this->FreeDBData(data);
		return NULL;
	}
	if (!data.argc) {
		printf("wrong target user code (%s)", dbErrMsg);
		ConvertUserCodeIPFormat(&code, true);
		return NULL;
	}

	UserData* result=new UserData;
	sprintf(result->name, "%s", data.argv[1]);
	result->code = code;
	result->lang = atoi(data.argv[2]);
	printf("get user data\n");
	this->FreeDBData(data);
	return result;
}
UINT32 DBCommand::DoSignup(SignupData data) {
	char query[256] = { 0 };
	int queryResult;
	char* dbErrMsg;

	UINT32 code = this->CreateUserCode();
	sprintf(query, "INSERT INTO Account VALUES(\"%s\", \"%s\", %d);", data.id, data.pwd, code);
	queryResult = sqlite3_exec(this->db, query, this->DBCallback, 0, &dbErrMsg);
	if (queryResult != SQLITE_OK) {
		printf("fail to insert into account (%s)\n", dbErrMsg);
		return UCODE_SERVER;
	}

	sprintf(query, "INSERT INTO User (code, name, lang) VALUES(%d, \"%s\", %d);", code, data.name, data.lang);
	queryResult = sqlite3_exec(this->db, query, this->DBCallback, 0, &dbErrMsg);
	if (queryResult != SQLITE_OK) {
		printf("fail to insert into user (%s)\n", dbErrMsg);
		return UCODE_SERVER;
	}
	return code;
}
bool DBCommand::CheckId(char* id) {
	char query[256] = { 0 };
	DBData data = { 0,0,0 };
	int queryResult;
	char* dbErrMsg;

	sprintf(query, "SELECT id FROM Account WHERE id=\"%s\";", id);
	queryResult = sqlite3_exec(this->db, query, &DBCommand::DBCallback, &data, &dbErrMsg);
	if (queryResult != SQLITE_OK) {
		printf("query error (%s)\n", dbErrMsg);
		this->FreeDBData(data);
		return false;
	}
	if (!data.argc) {
		//this->FreeDBData(&data);
		printf("useable id\n");
		return true;
	}
	printf("unuseable id\n");
	this->FreeDBData(data);
	return false;
}
UserData* DBCommand::AddFriend(UINT32 src, UINT32 trgt) {
	char query[256] = { 0 };
	DBData data = { 0,0,0 };
	int queryResult;
	char* dbErrMsg;

	UserData* targetUserData = this->GetUserData(trgt);
	if (targetUserData == NULL) {
		return NULL;
	}
	sprintf(query, "SELECT friends FROM User WHERE friends=%d;", trgt);
	queryResult = sqlite3_exec(this->db, query, &DBCommand::DBCallback, &data, &dbErrMsg);
	if (queryResult != SQLITE_OK) {
		printf("query error (%s)\n", dbErrMsg);
		this->FreeDBData(data);
		return NULL;
	}
	if (data.argc) {
		printf("already friend\n");
		this->FreeDBData(data);
		return NULL;
	}
	this->FreeDBData(data);

	sprintf(query, "INSERT INTO User (code, friends) VALUES (%d, %d);", src, trgt);
	queryResult = sqlite3_exec(this->db, query, this->DBCallback, 0, &dbErrMsg);
	if (queryResult != SQLITE_OK) {
		printf("fail to add friend (code:%d) (%s)\n",src, dbErrMsg);
		return NULL;
	}

	return targetUserData;
}
int DBCommand::DBCallback(void* data, int argc, char** argv, char** azColName) {
	DBData* pData = (DBData*)data;

	//printf("db argc: %d\n", argc);
	if (argc == 0) {
		return 0;
	}

	pData->argv = (char**)malloc(sizeof(char*) * argc);
	pData->colName = (char**)malloc(sizeof(char*) * argc);
	pData->argc = argc;
	for (int i = 0; i < pData->argc; i++) {
		//std::cout << pData->colName[i] << "=" << pData->argv[i] << std::endl;
		pData->colName[i] = (char*)malloc(sizeof(char) + (strlen(azColName[i]) + 1));
		sprintf(pData->colName[i], "%s", azColName[i]);

		pData->argv[i] = (char*)malloc(sizeof(char) + (strlen(argv[i]) + 1));
		sprintf(pData->argv[i], "%s", argv[i]);

		//printf("%s(%d)=%s(%d)\n", pData->colName[i], strlen(pData->colName[i]), pData->argv[i], strlen(pData->argv[i]));
	}
	return 0;
}
UINT32 DBCommand::CreateUserCode() {
	UINT32 code = ((time(NULL) * 1000) + (clock() % 1000)) >> 8;
	return code;
}
void DBCommand::FreeDBData(DBData data) {
	for (int i = 0; i < data.argc; i++) {
		free(data.colName[i]);
		free(data.argv[i]);
	}
	free(data.colName);
	free(data.argv);
	//free(data);
}