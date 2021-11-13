//
// Created by zezo on 11/8/2021.
//

#ifndef NETWORK_ASSIGNMENT1_SOCKETHANDLER_H
#define NETWORK_ASSIGNMENT1_SOCKETHANDLER_H
#define DEFAULT_BUFLEN 1024
#undef UNICODE

#define WIN32_LEAN_AND_MEAN
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>
#include <atomic>

#pragma comment (lib, "Ws2_32.lib")

using namespace std;
class SocketHandler {
public:
    SocketHandler(SOCKET &Socket);
    static atomic<int> curr_threads;
    static const int MAX_THREADS = 80;
    void handle();
    void closeConnection();
private:
    bool isReadyToReceive(SOCKET socket);
    SOCKET clientSocket;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;
    bool isFinished(char* buff,int iResult);
    int sendall(SOCKET ConnectSocket,const char *buf, int *len);
};


#endif //NETWORK_ASSIGNMENT1_SOCKETHANDLER_H
