///////////////////////////////////////////////////////////
/////////////////////SERVER EXAMPLE////////////////////////
///////////////////////////////////////////////////////////



//#include <iostream>
//#include <signal.h>
//
//#include "include/tcp_server.h"
//
//TcpServer server;
//
//Client client;
//server_observer_t observer1, observer2;
//
//
//void onIncomingMsg1(const Client & client, const char * msg, size_t size) {
//    std::string msgStr = msg;
//    std::cout << "Observer1 got client msg: " << msgStr << std::endl;
//    if (msgStr.find("quit") != std::string::npos) {
//        std::cout << "Closing server..." << std::endl;
//        pipe_ret_t finishRet = server.finish();
//        if (finishRet.success) {
//            std::cout << "Server closed." << std::endl;
//        } else {
//            std::cout << "Failed closing server: " << finishRet.msg << std::endl;
//        }
//    } else if (msgStr.find("print") != std::string::npos){
//        server.printClients();
//    } else {
//        std::string replyMsg = "server got this msg: "+ msgStr;
//        server.sendToAllClients(replyMsg.c_str(), replyMsg.length());
//    }
//}
//
//void onIncomingMsg2(const Client & client, const char * msg, size_t size) {
//    std::string msgStr = msg;
//    std::cout << "Observer2 got client msg: " << msgStr << std::endl;
//
//    std::string replyMsg = "server got this msg: "+ msgStr;
//    server.sendToClient(client, msg, size);
//}
//
//void onClientDisconnected(const Client & client) {
//    std::cout << "Client: " << client.getIp() << " disconnected: " << client.getInfoMessage() << std::endl;
//}
//
//int main(int argc, char *argv[])
//{
//
//    pipe_ret_t startRet = server.start(65123);
//    if (startRet.success) {
//        std::cout << "Server setup succeeded" << std::endl;
//    } else {
//        std::cout << "Server setup failed: " << startRet.msg << std::endl;
//        return 1;
//    }
//    observer1.incoming_packet_func = onIncomingMsg1;
//    observer1.disconnected_func = onClientDisconnected;
//    observer1.wantedIp = "127.0.0.1";
//    server.subscribe(observer1);
//    observer1.incoming_packet_func = onIncomingMsg2;
//    observer1.wantedIp = "10.88.0.11";
//    server.subscribe(observer1);
//    std::cout << "waiting for clients..." << std::endl;
//
//    while(1) {
//        Client client = server.acceptClient(0);
//        if (client.isConnected()) {
//            std::cout << "Got client with IP: " << client.getIp() << std::endl;
//            server.printClients();
//        } else {
//            std::cout << "Accepting client failed: " << client.getInfoMessage() << std::endl;
//        }
//        sleep(1);
//    }
//
//    return 0;
//}



///////////////////////////////////////////////////////////
/////////////////////CLIENT EXAMPLE////////////////////////
///////////////////////////////////////////////////////////




#include <iostream>
#include <signal.h>
#include "include/tcp_client.h"

TcpClient client;

bool clientOpen = false;

void sig_exit(int s)
{
	std::cout << "Closing client..." << std::endl;
	pipe_ret_t finishRet = client.finish();
	if (finishRet.success) {
		std::cout << "Client closed." << std::endl;
	} else {
		std::cout << "Failed to close client." << std::endl;
	}
	exit(0);
}

void onIncomingMsg(const char * msg, size_t size) {
	std::cout << "Got msg from server: " << msg << std::endl;
}

void onDisconnection(const pipe_ret_t & ret) {
	std::cout << "Server disconnected: " << ret.msg << std::endl;
	std::cout << "Closing client..." << std::endl;
    pipe_ret_t finishRet = client.finish();
	if (finishRet.success) {
		std::cout << "Client closed." << std::endl;
	} else {
		std::cout << "Failed to close client: " << finishRet.msg << std::endl;
	}
}


int main() {

	signal(SIGINT, sig_exit);

	client_observer_t observer;
	observer.wantedIp = "127.0.0.1";
	observer.incoming_packet_func = onIncomingMsg;
	observer.disconnected_func = onDisconnection;
	client.subscribe(observer);

    pipe_ret_t connectRet = client.connectTo("127.0.0.1", 65123);
	if (connectRet.success) {
		std::cout << "Client connected successfully" << std::endl;
		clientOpen = true;
	} else {
		std::cout << "Client failed to connect: " << connectRet.msg << std::endl;
		return 1;
	}
	while(clientOpen)
	{
		std::string msg = "hello server\n";
        pipe_ret_t sendRet = client.sendMsg(msg.c_str(), msg.size());
		if (!sendRet.success) {
			std::cout << "Failed to send msg: " << sendRet.msg << std::endl;
			break;
		}
		sleep(1);
	}

	return 0;
}