#pragma once
#include "sqlite3.h"
#include <windows.h>
#include "communication.h"

typedef struct _dbdata {
	int argc;
	char** argv;
	char** colName;
}DBData;

class DBCommand{
	private:
		sqlite3* db;
	public:
		DBCommand();

		void FreeDBData(DBData);
		static int DBCallback(void*, int, char**, char**);
		UINT32 CreateUserCode();

		UINT32 DoSignin(char*, char*);
		UINT32 DoSignup(SignupData);
		UserData* GetUserData(UINT32);
		bool CheckId(char*);
		UserData* AddFriend(UINT32,UINT32);

		static void ConvertUserCodeIPFormat(UINT32*, bool);
};