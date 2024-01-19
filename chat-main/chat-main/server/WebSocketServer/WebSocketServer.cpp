#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <thread>
#include <conio.h>
#include <openssl/applink.c>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "websocket.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"libssl.lib")
#pragma comment(lib,"libcrypto.lib")

#define CRT "D:\\GitHub\\chat\\server\\test\\webserver\\letsencrypt\\PEM\\aiv.asuscomm.com-crt.pem"
#define KEY "D:\\GitHub\\chat\\server\\test\\webserver\\letsencrypt\\PEM\\aiv.asuscomm.com-key.pem"
//#define CRT "ca.crt"
//#define KEY "ca.key"

#pragma pack(push,1)
typedef struct _dataframe {
    char opcode;
    long long dataLen;
    char* data;
}DataFrame;
#pragma pack(pop)

char* FrameMessage(char, char*, long long);
void RecvThread(SSL*, char*);
DataFrame* UnframeMessage(char*);

int main()
{
    SOCKET servSock;
    SOCKET clntSock;

    WSADATA wsaData;
    SOCKADDR_IN servAddr;

    SSL_load_error_strings();
    SSLeay_add_ssl_algorithms();

    const SSL_METHOD* meth;
    SSL_CTX* ctx;

    int err;

    meth = TLS_server_method();
    ctx = SSL_CTX_new(meth);
    if (!ctx) {
        ERR_print_errors_fp(stderr);
        exit(2);
    }

    if (SSL_CTX_use_certificate_file(ctx, CRT, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, KEY, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(4);
    }
    if (!SSL_CTX_check_private_key(ctx)) {
        fprintf(stderr, "Private key does not match the certificate public key\n");
        exit(5);
    }

    SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

    WSAStartup(MAKEWORD(2, 2), &wsaData);
    servSock = socket(PF_INET, SOCK_STREAM, 0);

    if (!servSock) {
        printf("socket error\n");
        return 0;
    }

    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(2025);

    if (bind(servSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) {
        printf("bind error\n");
        return 0;
    }

    listen(servSock, 5);

    SOCKADDR_IN clntAddr;
    int addrLen = sizeof(clntAddr);
    clntSock = accept(servSock, (SOCKADDR*)&clntAddr, &addrLen);
    printf("connect: %s\n", inet_ntoa(clntAddr.sin_addr));
    SSL* ssl = SSL_new(ctx);

    if (!ssl) {
        printf("ssl is null\n");
        exit(1);
    }
    err=SSL_set_fd(ssl, clntSock);
    err = SSL_accept(ssl);
    if ((err) == -1) { ERR_print_errors_fp(stderr); exit(2); }
    char run = true;
    char buf[1024] = { 0 };
    SSL_read(ssl, buf, sizeof(buf));
    printf("recv data: %s\n", buf);
    if (WebSocketHandshake(ssl, buf)) {
        printf("web socket connect\n");
        
    }
    else {
        printf("web socket fail\n");
        closesocket(servSock);
        closesocket(clntSock);
        WSACleanup();

        return 1;
    }

    std::thread recvThread(&RecvThread, ssl, &run);
    char key;
    //send(clntSock, buf, strlen(buf), 0);
    while (run) {
        key = _getch();
        printf("key: %d\n", key);
        switch (key) {
        case 27: //esc
            run = false;
            //stop server
            break;
        case 'c':
            char msg[] = "hello web socket client!";
            char* framedMsg;
            framedMsg=FrameMessage(WS_OPCODE_TEXT, msg, sizeof(msg));

            SSL_write(ssl, framedMsg, _msize(framedMsg));
            break;
        }
    }

    recvThread.join();
    closesocket(servSock);
    closesocket(clntSock);
    WSACleanup();

    return 1;
}
void RecvThread(SSL* ssl,char* run) {
    char buf[1024] = { 0 };
    while (*run) {
        int recvSize= SSL_read(ssl, (char *)buf, sizeof(buf));
        printf("recvSize: %d\n", recvSize);
        if (recvSize == 0) {
            break;
        }
        UnframeMessage(buf);
        //printf("\n");
    }
    return;
}
char CopyLen8B(char* dest, char* orig) {
    memcpy(dest, orig, 8);
    return 8;
}
char CopyLen2B(char* dest, char* orig) {
    memcpy(dest, orig, 2);
    return 2;
}
char* FrameMessage(char opcode,char* data,long long dataSize) {
    char header = WS_FIN | opcode;
    int frameSize = dataSize + 2;
    char payloadLen;
    char (*CopyLen)(char*, char*)=NULL;
    switch (dataSize) {
    case 127:
        frameSize += 8;
        payloadLen = 127;
        CopyLen = CopyLen8B;
        break;
    case 126:
        frameSize += 2;
        payloadLen = 126;
        CopyLen = CopyLen2B;
        break;
    default:
        payloadLen = dataSize;
        break;
    }
    char* frame = new char[frameSize];
    int pos = 0;
    frame[pos++] = header;
    frame[pos++] = payloadLen;
    if (CopyLen) {
        pos+=CopyLen(&frame[pos], (char*)&dataSize);
    }
    memcpy(&frame[pos], data, dataSize);
    return frame;
}
DataFrame* UnframeMessage(char* msg) {
    if (!(msg[0] & WS_FIN)) {
        return 0;
    }
    DataFrame* dfrm = new DataFrame;
    memset(dfrm, 0, sizeof(DataFrame));
    int pos = 0;
    dfrm->opcode = msg[pos];
    printf("opcode: %x\n", msg[pos++] & 0xf);

    
    printf("%d\n", msg[pos] & 0x7f);
    switch (msg[pos] & 0x7f) {
    case 127:
        dfrm->dataLen = *((long long*)&msg[pos]);
        pos += 8;
        break;
    case 126:
        dfrm->dataLen = *((unsigned short*)&msg[pos]);
        pos += 2;
        break;
    default:
        dfrm->dataLen = msg[pos++] & 0x7f;
        break;
    }
    printf("data len: %d\n", dfrm->dataLen);
    int maskKey = 0;
    if (msg[1] & WS_MASK) {
        printf("masked pos: %d\n",pos);
        maskKey = *((int*)&msg[pos]);
        pos += 4;
    }
    else {
        printf("unmasked\n");
    }
    dfrm->data = new char[dfrm->dataLen];
    dfrm->data = &msg[pos];
    char* pMaskKey = (char*)&maskKey;
    for (int i = 0; i < dfrm->dataLen; i++) {
        dfrm->data[i] ^= pMaskKey[i % 4];
    }
    printf("%s\n", dfrm->data);

    return dfrm;
}