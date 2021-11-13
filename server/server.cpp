

/*#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>*/
#include <tchar.h>
#include <strsafe.h>
#include <thread>

// Need to link with Ws2_32.lib
// #pragma comment (lib, "Mswsock.lib")


#define DEFAULT_PORT "80"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include "HttpParser.h"
#include "SocketHandler.h"
using namespace std;
void ErrorHandler(LPTSTR lpszFunction);
DWORD WINAPI MyThreadFunction( LPVOID lpParam );
void thread_func(SOCKET clientSocke);
std::atomic<int> SocketHandler::curr_threads(0);
int  main(int argc, char** argv){
    printf("Starting...\r\n");

    WSADATA wsaData;
    int iResult;
    const char* port;
    if(argc > 1){
        port = argv[1];
    }else{
        port = DEFAULT_PORT;
    }
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    printf("Link complete\n");

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    printf("Listening on port ");
    printf(port);
    iResult = getaddrinfo(NULL, port, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    printf("\nListenning...\n");
    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return -1;
    }

    atomic<int> curr_threads(0);
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while(true) {

        printf("Waiting for incomming connection\n");
        // Accept a client socket

        cout << "Taken threads from begining of loop = " << curr_threads << endl;

        //only work if a thread is available
        if(curr_threads < SocketHandler::MAX_THREADS){
            ClientSocket = accept(ListenSocket, NULL, NULL);

            if (ClientSocket == INVALID_SOCKET) {
                printf("accept failed with error: %d\n", WSAGetLastError());
                closesocket(ListenSocket);
                continue;
            }
            cout << "New connection\n";
            SocketHandler::curr_threads++;
            thread t1(thread_func,ClientSocket);
            t1.detach();
        }else{
            cout << "Full thread pool used";
        }
    }
#pragma clang diagnostic pop

}
void thread_func(SOCKET clientSocket){
    cout<< "Inside thread func ,before handling, taken threads= " <<SocketHandler::curr_threads << endl;
    SocketHandler handler(clientSocket);
    handler.handle();
    SocketHandler::curr_threads--;
    cout<< "Inside thread func ,handler finished , taken threads= " << SocketHandler::curr_threads << endl;

}
