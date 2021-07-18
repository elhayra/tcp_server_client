#pragma once

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netdb.h>
#include <vector>
#include <errno.h>
#include <thread>
#include <mutex>
#include <atomic>
#include "client_observer.h"
#include "pipe_ret_t.h"
#include "file_descriptor.h"

//todo: fix closing server (and also check closing client) - there is a deadlock when running the example
//todo: document: 'connection refused' - make sure server is running
//todo: document: play with code - close server while clients are alive, and watch how they receive notification about the server closing
//todo: document: how to play with the examples
//todo: document: how to compile the code, how to run multiple clients
//todo: document: thread safety
//todo: document: how you can customise this for your needs
//todo: add compilation bash script
//todo: for each function, go over it and check what if it is called from more than one thread at the same time?

class TcpClient
{
private:
    FileDescriptor _sockfd;
    std::atomic<bool> _stop;
    struct sockaddr_in _server;
    std::vector<client_observer_t> _subscibers;
    std::thread * _receiveTask = nullptr;
    std::mutex _subscribersMtx;

    void initializeSocket();
    void startReceivingMessages();
    void setAddress(const std::string& address, int port);
    void publishServerMsg(const char * msg, size_t msgSize);
    void publishServerDisconnected(const pipe_ret_t & ret);
    void receiveTask();
    void terminateReceiveThread();

public:
    TcpClient();
    ~TcpClient();
    pipe_ret_t connectTo(const std::string & address, int port);
    pipe_ret_t sendMsg(const char * msg, size_t size);

    void subscribe(const client_observer_t & observer);
    void unsubscribeAll();

    pipe_ret_t close();
};

