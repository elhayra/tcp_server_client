///////////////////////////////////////////////////////////
/////////////////////SERVER EXAMPLE////////////////////////
///////////////////////////////////////////////////////////

#ifdef SERVER_EXAMPLE

#include <iostream>
#include <signal.h>

#include "include/tcp_server.h"

// declare the server
TcpServer server;

// declare a server observer which will receive incomingPacketHandler messages.
// the server supports multiple observers
server_observer_t observer1, observer2;

// observer callback. will be called for every new message received by clients
// with the requested IP address
void onIncomingMsg1(const Client & client, const char * msg, size_t size) {
    std::string msgStr = msg;
    // print the message content
    std::cout << "Observer1 got client msg: " << msgStr << std::endl;
    // if client sent the string "quit", close server
    // else if client sent "print" print the server clients
    // else just print the client message
    if (msgStr.find("quit") != std::string::npos) {
        std::cout << "Closing server..." << std::endl;
        pipe_ret_t finishRet = server.close();
        if (finishRet.isSuccessful()) {
            std::cout << "Server closed." << std::endl;
        } else {
            std::cout << "Failed closing server: " << finishRet.message() << std::endl;
        }
    } else if (msgStr.find("print") != std::string::npos){
        server.printClients();
    } else {
        std::string replyMsg = "server got this msg: "+ msgStr;
        server.sendToAllClients(replyMsg.c_str(), replyMsg.length());
    }
}

// observer callback. will be called for every new message received by clients
// with the requested IP address
void onIncomingMsg2(const Client & client, const char * msg, size_t size) {
    std::string msgStr = msg;
    // print client message
    std::cout << "Observer2 got client msg: " << msgStr << std::endl;

    // reply back to client
    std::string replyMsg = "server got this msg: "+ msgStr;
    server.sendToClient(client, msg, size);
}

// observer callback. will be called when client disconnects
void onClientDisconnected(const std::string &ip, const std::string &msg) {
    std::cout << "Client: " << ip << " disconnected. Reason: " << msg << std::endl;
}

int main(int argc, char *argv[])
{
    // start server on port 65123
    pipe_ret_t startRet = server.start(65123);
    if (startRet.isSuccessful()) {
        std::cout << "Server setup succeeded" << std::endl;
    } else {
        std::cout << "Server setup failed: " << startRet.message() << std::endl;
        return EXIT_FAILURE;
    }

    // configure and register observer1
    observer1.incomingPacketHandler = onIncomingMsg1;
    observer1.disconnectionHandler = onClientDisconnected;
    observer1.wantedIP = "127.0.0.1";
    server.subscribe(observer1);

    // configure and register observer2
    observer2.incomingPacketHandler = onIncomingMsg2;
    observer2.disconnectionHandler = nullptr; // nullptr or not setting this means we don't care about disconnection
    observer2.wantedIP = "10.88.0.11"; // use empty string instead to receive messages from any IP address
    server.subscribe(observer2);

    // receive clients
    while(1) {
        try {
            std::string clientIP = server.acceptClient(0);
            std::cout << "Got client with IP: " << clientIP << std::endl;
            server.printClients();
        } catch (const std::runtime_error &error) {
            std::cout << "Accepting client failed: " << error.what() << std::endl;
        }

        sleep(1);
    }
}

#endif