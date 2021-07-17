///////////////////////////////////////////////////////////
/////////////////////CLIENT EXAMPLE////////////////////////
///////////////////////////////////////////////////////////

#ifdef CLIENT_EXAMPLE

#include <iostream>
#include <signal.h>
#include "../include/tcp_client.h"

TcpClient client;

// on sig_exit, close client
void sig_exit(int s)
{
	std::cout << "Closing client..." << std::endl;
	pipe_ret_t finishRet = client.close();
	if (finishRet.isSuccessful()) {
		std::cout << "Client closed." << std::endl;
	} else {
		std::cout << "Failed to close client." << std::endl;
	}
	exit(0);
}

// observer callback. will be called for every new message received by the server
void onIncomingMsg(const char * msg, size_t size) {
	std::cout << "Got msg from server (echo): " << msg << std::endl;
}

// observer callback. will be called when server disconnects
void onDisconnection(const pipe_ret_t & ret) {
	std::cout << "Server disconnected: " << ret.message() << std::endl;
	std::cout << "Closing client..." << std::endl;
    pipe_ret_t finishRet = client.close();
	if (finishRet.isSuccessful()) {
		std::cout << "Client closed." << std::endl;
	} else {
		std::cout << "Failed to close client: " << finishRet.message() << std::endl;
	}
}

void printMenu() {
    std::cout << "select one of the following options: \n" <<
                 "1. send message ('hello server') to server\n" <<
                 "2. close client\n";
}

int getMenuSelection() {
   int selection = 0;
   std::cin >> selection;
   return selection;
}

void handleMenuSelection(int selection) {
    static const int minSelection = 1;
    static const int maxSelection = 2;
    if (selection < minSelection || selection > maxSelection) {
        std::cout << "invalid selection: " << selection <<
                     ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
    }
    switch (selection) {
        case 1: { // send message to server
            std::string msg = "hello server\n";
            pipe_ret_t sendRet = client.sendMsg(msg.c_str(), msg.size());
            if (!sendRet.isSuccessful()) {
                std::cout << "Failed to send msg: " << sendRet.message() << std::endl;
                break;
            }
            break;
        }
        case 2: { // close client
            const pipe_ret_t closeResult = client.close();
            if (!closeResult.isSuccessful()) {
                std::cout << "closing client failed: " << closeResult.message() << "\n";
            } else {
                std::cout << "closed client successfully\n";
            }
            break;
        }
    }
}

int main() {
    //register to SIGINT to close client when user press ctrl+c
	signal(SIGINT, sig_exit);

    // configure and register observer
    client_observer_t observer;
	observer.wantedIP = "127.0.0.1";
	observer.incomingPacketHandler = onIncomingMsg;
	observer.disconnectionHandler = onDisconnection;
	client.subscribe(observer);

	// connect client to an open server
    pipe_ret_t connectRet = client.connectTo("127.0.0.1", 65123);
	if (connectRet.isSuccessful()) {
		std::cout << "Client connected successfully" << std::endl;
	} else {
		std::cout << "Client failed to connect: " << connectRet.message() << std::endl;
		return EXIT_FAILURE;
	}

	// send messages to server
	while(1)
	{
        printMenu();
        int selection = getMenuSelection();
        handleMenuSelection(selection);
	}
}

#endif