//
// Created by zezo on 11/8/2021.
//

#include <string>
#include <sstream>
#include "SocketHandler.h"
#include "HttpParser.h"
#include "HttpBuilder.h"
#ifndef TCP_USER_TIMEOUT
#define TCP_USER_TIMEOUT 18  // how long for loss retry before timeout [ms]
#endif
#ifndef SOL_TCP
#define SOL_TCP 6  // socket options TCP level
#endif
#define MAX_SEC 15
using namespace std;

bool SocketHandler::isReadyToReceive(SOCKET socket) {
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    timeval tv;
    tv.tv_sec = double(1- (curr_threads-1)/MAX_THREADS)*MAX_SEC; // Wait for portion of max current waiting seconds before time-out
    tv.tv_usec = 0;

    return (select(socket, &fds, 0, 0, &tv) == 1);
}
void SocketHandler::handle() {
    int iResult;
    int iSendResult;
    vector<char> input;

    //Check if the client will send data
    if(isReadyToReceive(clientSocket)){
        cout<<"Ready\n";
    }else{
        cout << "Time out at beginning of transmission...\n";
        closeConnection();
        return;
    }

    do{
        //Wait for data to be transmitted
        if(!isReadyToReceive(clientSocket)){
            cout << "Time out while waiting for data \n";
            closeConnection();
            return;
        }

        //Receive Data
        iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
        if (iResult > 0) { //Receive ok
            printf("Bytes received: %d\n", iResult);
            if(isFinished(recvbuf,iResult)){ //The sender has send all the request
                printf("client sent finished\n");
                input.insert(input.end(),recvbuf,recvbuf+iResult); //Add to end on input
                stringstream ss;
                do {
                    HttpParser parser;
                    input = parser.parse(input); //Parse input
                    HttpBuilder builder(parser); //Build response using parsed request
                    builder.build();
                    string data = builder.to_string();//Get response as string
                    ss.write(data.c_str(),data.size());
                } while (input.size()>0);
                cout << "Sending Data to server : \n" ;//<< data << std::endl << "Data size = "<<data.size()<< std::endl;
                int len = ss.str().size();
                iSendResult = sendall(clientSocket,ss.str().c_str(),&len); //Send all response to client

                if (iSendResult == SOCKET_ERROR) {
                    //if error terminate connection
                    printf("send failed: %d\n", WSAGetLastError());
                    closesocket(clientSocket);
                   // WSACleanup();
                    return;
                }
                printf("Bytes sent: %d\n", len);
            }
            else {
                //There's more to be sent from client
                printf("Still waiting for more\n");
                input.insert(input.end(),recvbuf,recvbuf+recvbuflen);
            }


        } else if (iResult == 0){ //The client closed the connection
            printf("Client Disconnected, Connection closing...\n");
            closeConnection();
            return;
        }
        else if(iResult < 0){ //Error
            printf("recv failed with error: %d\n", WSAGetLastError());
            closesocket(clientSocket);
            //WSACleanup();
            return ;
        }


    }while (iResult>0); //Keep looping as there's no error or client didn't disconnect
    cout << "Ending connection \n " << std::endl;
    closeConnection();
}

bool SocketHandler::isFinished(char *buff,int iResult) {
    if(iResult < recvbuflen)
             return true;
    if(iResult == recvbuflen && (buff[recvbuflen-1] == '\n' && buff[recvbuflen-2] == '\r' && buff[recvbuflen-3] == '\n' && buff[recvbuflen-4] == '\r' )) {
        return true;
    }
        return false;
}

void SocketHandler::closeConnection() {
    int iResult;
    iResult = shutdown(clientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(clientSocket);
        //WSACleanup();
        return ;
    }
    // cleanup
    closesocket(clientSocket);
}

SocketHandler::SocketHandler(SOCKET &Socket) {
    clientSocket = Socket;
}

int SocketHandler::sendall(SOCKET ConnectSocket, const char *buf, int *len) {
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


