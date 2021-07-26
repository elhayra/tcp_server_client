///////////////////////////////////////////////////////////
/////////////////////CLIENT EXAMPLE////////////////////////
///////////////////////////////////////////////////////////

#ifdef CLIENT_EXAMPLE

#include <iostream>
#include <csignal>
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

//todo: DOCUMENT: never call tcp_client or tcp_server functions in their subscribers CB functions,
// todo: this may cause dead lock (e.g. don't call client.close()) in this function. (client will be closed automatically in such case)
// todo: the CB functions should be called quickly and return, because they are called in
// todo: the context of the tcp_client / server-client

// observer callback. will be called when server disconnects
void onDisconnection(const pipe_ret_t & ret) {
	std::cout << "Server disconnected: " << ret.message() << std::endl;
}

void printMenu() {
    std::cout << "select one of the following options: \n" <<
                 "1. send message ('hello server') to server\n" <<
                 "2. close client and exit\n";
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

bool handleMenuSelection(int selection) {
    static const int minSelection = 1;
    static const int maxSelection = 2;
    if (selection < minSelection || selection > maxSelection) {
        std::cout << "invalid selection: " << selection <<
                     ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        return false;
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
            return true;
        }
        default: {
            std::cout << "invalid selection: " << selection <<
                      ". selection must be b/w " << minSelection << " and " << maxSelection << "\n";
        }
    }
    return false;
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
	bool shouldTerminate = false;
	while(!shouldTerminate)
	{
        printMenu();
        int selection = getMenuSelection();
        shouldTerminate = handleMenuSelection(selection);
	}

	return 0;
}

#endif