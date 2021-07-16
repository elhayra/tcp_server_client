#pragma once

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <functional>
#include <cstring>
#include <errno.h>
#include <iostream>
#include <mutex>
#include "client.h"
#include "server_observer.h"
#include "pipe_ret_t.h"

class TcpServer
{
private:

    int _sockfd;
    struct sockaddr_in _serverAddress;
    struct sockaddr_in _clientAddress;
    fd_set _fds;
    std::vector<Client> _clients;
    std::vector<server_observer_t> _subscibers;

    std::mutex _subscribersMtx;
    std::mutex _clientsMtx;

    void publishClientMsg(const Client & client, const char * msg, size_t msgSize);
    void publishClientDisconnected(const Client & client);
    void receiveTask();

public:
    TcpServer();
    pipe_ret_t start(int port, int maxNumOfClients = 5);
    void initializeSocket();
    void bindAddress(int port);
    void listenToClients(int maxNumOfClients);
    Client acceptClient(uint timeout);
    bool deleteClient(Client & client);
    void subscribe(const server_observer_t & observer);
    void unsubscribeAll();
    pipe_ret_t sendToAllClients(const char * msg, size_t size);
    pipe_ret_t sendToClient(const Client & client, const char * msg, size_t size);
    pipe_ret_t finish();
    void printClients();
};

