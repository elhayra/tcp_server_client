

#include <iostream>
#include <signal.h>

#include "tcp_server.h"

TcpServer server;

client_t client;
server_observer_t observer1, observer2;

void onIncomingMsg1(const client_t & client, const char * msg, size_t size) {
	std::string msgStr = msg;
	std::cout << "Observer1 got client msg: " << msgStr << std::endl;
	if (msgStr.find("quit") != std::string::npos) {
		std::cout << "Closing server..." << std::endl;
		tcp_ret_t finishRet = server.finish();
		if (finishRet.success) {
			std::cout << "Server closed." << std::endl;
		} else {
			std::cout << "Failed closing server: " << finishRet.msg << std::endl;
		}
	} else {
		std::string replyMsg = "server got this msg: "+ msgStr;
		server.sendToAllClients(replyMsg.c_str(), replyMsg.length());
	}
}

void onIncomingMsg2(const client_t & client, const char * msg, size_t size) {
	std::string msgStr = msg;
	std::cout << "Observer2 got client msg: " << msgStr << std::endl;

	std::string replyMsg = "server got this msg: "+ msgStr;
	server.sendToClient(client, msg, size);
}

void onClientDisconnected(const client_t & client) {
	std::cout << "Client: " << client.ip << " disconnected: " << client.msg << std::endl;
}

int main(int argc, char *argv[])
{
	tcp_ret_t startRet = server.start(65123);
	if (startRet.success) {
		std::cout << "Server setup succeeded" << std::endl;
	} else {
		std::cout << "Server setup failed: " << startRet.msg << std::endl;
		return 1;
	}
	observer1.incoming_packet_func = onIncomingMsg1;
	observer1.disconnected_func = onClientDisconnected;
	observer1.wantedIp = "127.0.0.1";
	server.subscribe(observer1);
	observer1.incoming_packet_func = onIncomingMsg2;
	observer1.wantedIp = "147.234.144.23";
	server.subscribe(observer1);
	std::cout << "waiting for clients..." << std::endl;

	while(1) {
		client_t client = server.acceptClient(0);
		if (client.connected) {
			std::cout << "Got client with IP: " << client.ip << std::endl;
			server.printClients();
		} else {
			std::cout << "Accepting client failed: " << client.msg << std::endl;
		}
		sleep(1);
	}

	return 0;
}
