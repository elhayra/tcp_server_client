///////////////////////////////////////////////////////////
/////////////////////SERVER EXAMPLE////////////////////////
///////////////////////////////////////////////////////////

#ifdef SERVER_EXAMPLE

#include <iostream>
#include <csignal>

#include "../include/tcp_server.h"

//todo: bug: closing server when client is connected, close server nicely, but tcp_client code crahses
//todo: bug: closing client with exception / killing it, cause server dead lock when trying to remove dead client
//todo: document 'removing dead client' message, when closing tcp_client_example

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

void acceptClient() { // todo: accept in a loop
    try {
        std::cout << "waiting for incoming client...\n";
        std::string clientIP = server.acceptClient(0);
        std::cout << "accepted new client with IP: " << clientIP << "\n" <<
                  "== updated list of accepted clients ==" << "\n";
        server.printClients();
    } catch (const std::runtime_error &error) {
        std::cout << "Accepting client failed: " << error.what() << std::endl;
    }
}

void printMenu() {
    std::cout << "\n\nselect one of the following options: \n" <<
              "1. send all clients a message\n" <<
              "2. print list of accepted clients\n" <<
              "3. close server and exit\n";
}

int getMenuSelection() {
    int selection = 0;
    std::cin >> selection;
    if (!std::cin) {
        throw std::runtime_error("invalid menu input. expected a number, but got something else");
    }
    std::cin.ignore (std::numeric_limits<std::streamsize>::max(), '\n');
    return selection;
}

/**
 * handle menu selection and return true in case program should terminate
 * after handling selection
 */
bool handleMenuSelection(int selection) {
    static const int minSelection = 1;
    static const int maxSelection = 3;
    if (selection < minSelection || selection > maxSelection) {
        std::cout << "invalid selection: " << selection <<
                  ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        return false;
    }
    switch (selection) {
        case 1: { // send all clients a message
            std::string msg;
            std::cout << "type message to send to all connected clients:\n";
            getline(std::cin, msg);
            pipe_ret_t sendingResult = server.sendToAllClients(msg.c_str(), msg.size());
            if (sendingResult.isSuccessful()) {
                std::cout << "sent message to all clients successfully\n";
            } else {
                std::cout << "failed to sent message: " << sendingResult.message() << "\n";
            }
            break;
        }
        case 2: { // print list of accepted clients
            server.printClients();
            break;
        }
        case 3: { // close server
            pipe_ret_t sendingResult = server.close();
            if (sendingResult.isSuccessful()) {
                std::cout << "closed server successfully\n";
            } else {
                std::cout << "failed to close server: " << sendingResult.message() << "\n";
            }
            return true;
        }
        default: {
            std::cout << "invalid selection: " << selection <<
                      ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        }
    }
    return false;
}

int main()
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

    // accept a single client. call this function
    // in a loop to accept multiple clients.
    acceptClient();

    bool shouldTerminate = false;
    while(!shouldTerminate) {
        printMenu();
        int selection = getMenuSelection();
        shouldTerminate = handleMenuSelection(selection);
    }

    return 0;
}

#endif