#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <cstring>
#include <errno.h>
#include <iostream>
#include "client.h"
#include "server_observer.h"
#include "tcp_ret.h"


#define MAX_PACKET_SIZE 4096

class TcpServer
{
	private:

	int m_sockfd;
	struct sockaddr_in m_serverAddress;
	struct sockaddr_in m_clientAddress;
	fd_set m_fds;
	std::vector<client_t> m_clients;
	std::vector<server_observer_t> m_subscibers;

	void publishClientMsg(const client_t & client, const char * msg, size_t msgSize);
	void publishClientDisconnected(const client_t & client);
	static void * ReceiveTask(void * context);


	public:

	tcp_ret_t start(int port);
	client_t acceptClient(uint timeout);
	bool deleteClient(client_t & client);
	void subscribe(const server_observer_t & observer);
	void unsubscribeAll();
	tcp_ret_t sendToAllClients(const char * msg, size_t size);
	tcp_ret_t sendToClient(const client_t & client, const char * msg, size_t size);
	tcp_ret_t finish();
	void printClients();
};



#endif
