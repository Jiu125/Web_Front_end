// server.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include <thread>
#include <iostream>
#include "Server.h"
#include <conio.h>
#include <cstdio>

#pragma comment(lib,"sqlite3.lib")

using namespace std;

const char* is_little_endian()
{
    int a = 0x01234567;

    if (*((char*)&a) == 0x67)
    {
        return "little endian"; // little endian
    }
    return "big endian";
}
int main()
{
    Server server(2024);
    //void (Server:: * pAcceptThread)() = &Server::AcceptThread;
    if (server.GetInitFail()) {
        return 0;
    }
    thread servThread{ &Server::AcceptThread,&server };
    //thread servThread = server.StartThread();
    char key;
    bool loop = true;
    printf("%s\n", is_little_endian());
    while (loop) {
        key = _getch();
        printf("key: %d\n", key);
        switch (key) {
        case 'b':
            //broadcast
            server.Broadcast("hello");
            break;
        case 27: //esc
            server.Stop();
            loop = false;
            //stop server
            break;
        case 't':
            //server.CommunicationTest(MSGTYPE_CHECKID);
            server.PrintUserCode();
            break;
        }
    }
    cout << "메인루프 종료" << endl;
    servThread.join();
    cout << "서버스레드 종료" << endl;
    return 0;
}