#pragma once

#define SUCCESS	1
#define FAIL	0

#define TYPEMASK	0xfe

#define MSGTYPE_SIGNIN			2
#define MSGTYPE_SIGNUP			4
#define MSGTYPE_DISCONNECT		8
#define MSGTYPE_CHECKID			12
#define MSGTYPE_ADDFRIEND		14

#define UCODE_SERVER			1

#pragma pack(push,1)
typedef struct _userdata {
	char name[64];
	char lang;
	UINT32 code;
}UserData;

typedef struct _signupdata {
	char id[16];
	char pwd[16];
	char name[64];
	char lang;
}SignupData;

typedef struct _signindata {
	char id[16];
	char pwd[16];
}SigninData;




typedef struct _messageheader {
	UINT8 type;
	//char srcUserCode[16];
	UINT32 srcUserCode;
	int bodySize;
}MessageHeader;

typedef struct _message {
	MessageHeader header;
	char* body;
}Message;

typedef struct _usercode {
	UINT8 a,b,c,d;
}UserCode;

