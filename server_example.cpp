///////////////////////////////////////////////////////////
/////////////////////SERVER EXAMPLE////////////////////////
///////////////////////////////////////////////////////////

#ifdef SERVER_EXAMPLE

#include <iostream>
#include <signal.h>

#include "include/tcp_server.h"

// declare the server
TcpServer server;

// declare a server observer which will receive incoming messages.
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
        pipe_ret_t finishRet = server.finish();
        if (finishRet.success) {
            std::cout << "Server closed." << std::endl;
        } else {
            std::cout << "Failed closing server: " << finishRet.msg << std::endl;
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
void onClientDisconnected(const Client & client) {
    std::cout << "Client: " << client.getIp() << " disconnected: " << client.getInfoMessage() << std::endl;
}

int main(int argc, char *argv[])
{
    // start server on port 65123
    pipe_ret_t startRet = server.start(65123);
    if (startRet.success) {
        std::cout << "Server setup succeeded" << std::endl;
    } else {
        std::cout << "Server setup failed: " << startRet.msg << std::endl;
        return EXIT_FAILURE;
    }

    // configure and register observer1
    observer1.incoming_packet_func = onIncomingMsg1;
    observer1.disconnected_func = onClientDisconnected;
    observer1.wantedIp = "127.0.0.1";
    server.subscribe(observer1);

    // configure and register observer2
    observer2.incoming_packet_func = onIncomingMsg2;
    observer1.disconnected_func = nullptr; //don't care about disconnection
    observer2.wantedIp = "10.88.0.11"; // use empty string instead to receive messages from any IP address
    server.subscribe(observer2);

    // receive clients
    while(1) {
        Client client = server.acceptClient(0);
        if (client.isConnected()) {
            std::cout << "Got client with IP: " << client.getIp() << std::endl;
            server.printClients();
        } else {
            std::cout << "Accepting client failed: " << client.getInfoMessage() << std::endl;
        }
        sleep(1);
    }

    return 0;
}

#endif