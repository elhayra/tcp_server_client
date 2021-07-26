///////////////////////////////////////////////////////////
/////////////////////SERVER EXAMPLE////////////////////////
///////////////////////////////////////////////////////////

#ifdef SERVER_EXAMPLE

#include <iostream>
#include <signal.h>

#include "../include/tcp_server.h"

// declare the server
TcpServer server;

// declare a server observer which will receive incomingPacketHandler messages.
// the server supports multiple observers
server_observer_t observer1, observer2;

// observer callback. will be called for every new message received by clients
// with the requested IP address
void onIncomingMsg1(const std::string &clientIP, const char * msg, size_t size) {
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
void onIncomingMsg2(const std::string &clientIP, const char * msg, size_t size) {
    std::string msgStr = msg;
    // print client message
    std::cout << "Observer2 got client msg: " << msgStr << std::endl;

    // reply back to client
    std::string replyMsg = "server got this msg: "+ msgStr;
    server.sendToClient(clientIP, msg, size);
}

// observer callback. will be called when client disconnects
void onClientDisconnected(const std::string &ip, const std::string &msg) {
    std::cout << "Client: " << ip << " disconnected. Reason: " << msg << std::endl;
}


void printMenu() {
    std::cout << "\n\nselect one of the following options: \n" <<
              "1. accept client\n" <<
              "2. send all clients a message ('hello clients')\n" <<
              "3. print list of accepted clients\n" <<
              "4. close server\n";
}

int getMenuSelection() {
    int selection = 0;
    std::cin >> selection;
    return selection;
}

void handleMenuSelection(int selection) {
    static const int minSelection = 1;
    static const int maxSelection = 4;
    if (selection < minSelection || selection > maxSelection) {
        std::cout << "invalid selection: " << selection <<
                  ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
    }
    switch (selection) {
        case 1: { // accept client
            try {
                std::string clientIP = server.acceptClient(0);
                std::cout << "\naccepted new client with IP: " << clientIP << "\n" <<
                          "== updated list of accepted clients ==" << "\n";
                server.printClients();
            } catch (const std::runtime_error &error) {
                std::cout << "Accepting client failed: " << error.what() << std::endl;
            }
            break;
        }
        case 2: { // send all clients a message
            const std::string msg = "hello clients";
            pipe_ret_t sendingResult = server.sendToAllClients(msg.c_str(), msg.size());
            if (sendingResult.isSuccessful()) {
                std::cout << "sent message to all clients successfully\n";
            } else {
                std::cout << "failed to sent message: " << sendingResult.message() << "\n";
            }
            break;
        }
        case 3: { // print list of accepted clients
            server.printClients();
            break;
        }
        case 4: { // close server
            pipe_ret_t sendingResult = server.close();
            if (sendingResult.isSuccessful()) {
                std::cout << "closed server successfully\n";
            } else {
                std::cout << "failed to close server: " << sendingResult.message() << "\n";
            }
            break;
        }
    }
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
        printMenu();
        int selection = getMenuSelection();
        handleMenuSelection(selection);
    }
}

#endif