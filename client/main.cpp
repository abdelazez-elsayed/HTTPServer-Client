#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define DEFAULT_BUFLEN 2048
#define DEFAULT_PORT "80"
#include <fstream>
#include "file_parser.h"
#include <map>
using namespace std;
map<string,SOCKET> hosts;
int  my_close_socket(SOCKET ConnectSocket){
    int iResult;
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        return -1;
    }
    // cleanup
    closesocket(ConnectSocket);
}
int  my_close_socket(string host,SOCKET ConnectSocket){
    int iResult;
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);

        return -1;
    }
    // cleanup
    hosts.erase(host);
    closesocket(ConnectSocket);
}


void closeAll(){

    map<string,SOCKET>::iterator it;

    for (it = hosts.begin(); it != hosts.end(); it++)
    {
        my_close_socket(it->second);
    }
    hosts.clear();
}
bool isFinished(char *buff,int iResult,int recvbuflen) {
    if(iResult < recvbuflen)
        return true;
    if(iResult == recvbuflen && (buff[recvbuflen-1] == '\n' && buff[recvbuflen-2] == '\r' && buff[recvbuflen-3] == '\n' && buff[recvbuflen-4] == '\r' )) {
        return true;
    }
    return false;
}
bool isReadyToReceive(SOCKET socket) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    timeval tv;
    tv.tv_sec = 10; // Wait for portion of max current waiting seconds before time-out
    tv.tv_usec = 0;

    return (select(socket, &fds, 0, 0, &tv) == 1);
}

string recieve_data(SOCKET ConnectSocket){

    // Receive until the peer closes the connection
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;
    char recvbuf[DEFAULT_BUFLEN];
    stringstream ss;
    do {
        if(isReadyToReceive(ConnectSocket))
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        else{
            cout << "TIME OUT CLIENT \n";
            iResult = 0;
        }
        if ( iResult > 0 ) {
            printf("Bytes received: %d\n", iResult);
            ss.write(recvbuf,iResult);
            if(isFinished(recvbuf,iResult,recvbuflen)){
                break;
            }
        }
        else if ( iResult == 0 ) {
            printf("Connection closing..\n");
            my_close_socket(ConnectSocket);
        }
        else {
            printf("recv failed with error: %d\n", WSAGetLastError());
            my_close_socket(ConnectSocket);
        }

    } while( iResult > 0 );
    return ss.str();
}
int sendall(SOCKET ConnectSocket,const char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(ConnectSocket, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1? SOCKET_ERROR :0; // return -1 on failure, 0 on success
}
int send_data(string http_command,SOCKET ConnectSocket){
// Send an initial buffer
    int iResult;
    int len = http_command.size();
    iResult = sendall(ConnectSocket,http_command.c_str(),&len);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        my_close_socket(ConnectSocket);
        return -1;
    }

    printf("Bytes Sent: %ld\n", iResult);

}
SOCKET make_socket(string host,string port) {
    // Resolve the server address and port
    int iResult;
    SOCKET ConnectSocket;
    ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
            *ptr = NULL,
            hints;

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    iResult = getaddrinfo(host.c_str(), port.c_str(), &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        return -1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
                               ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            return INVALID_SOCKET;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }
    freeaddrinfo(result);
    return ConnectSocket;

}
SOCKET get_connection(string host,string port){
    string address = host+":"+port;
    auto iter = hosts.find(address);
    if(iter != hosts.end()){
        cout << "EXISTING SOCKET !\n";
        return iter->second;
    }else{
        SOCKET socket = make_socket(host,port);
        if(socket != INVALID_SOCKET) {
            cout << "NEW SOCKET\n";
            hosts[address] = socket;
        }
        return socket;
    }
}
int  main(int argc, char **argv)
{
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;


    int iResult;
    if (argc < 2) {
        printf("Error: NO path entered, Please enter a path of file of commands");
        return 1;
    }
    vector<vector<string>> commands = parse(argv[1]);


    // Validate the parameters


    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }


    for (auto & command : commands) {

            string http_command = create_http_command(command[0], command[1]);
                if(http_command != "NOT FOUND" && http_command != "INVALID") {
                ConnectSocket = get_connection(command[2], command[3]);
                if (ConnectSocket == INVALID_SOCKET) {
                    printf("Unable to connect to server!\n");
                    continue;
                }
                int iSendReult = send_data(http_command, ConnectSocket);
                if (iSendReult < 0) {
                    cout << "Error in sending datat";
                    continue;
                }
                string recv_data = recieve_data(ConnectSocket);
                cout << recv_data;
            }else{
                if(http_command == "INVALID"){
                    cout << "Error: Invalid Command line" << endl;
                } else
                cout << http_command << endl;
            }
    }
    closeAll();






    // shutdown the connection since no more data will be sent

    WSACleanup();

    return 0;
}
