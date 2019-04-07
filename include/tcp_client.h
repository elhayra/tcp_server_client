//
// Created by erauper on 4/7/19.
//

#ifndef INTERCOM_TCP_CLIENT_H
#define INTERCOM_TCP_CLIENT_H


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
#include "client_observer.h"
#include "pipe_ret_t.h"

#define MAX_PACKET_SIZE 4096




//TODO: REMOVE ABOVE CODE, AND SHARE client.h FILE WITH SERVER AND CLIENT

class TcpClient
{
private:
    int m_sockfd;
    bool stop;
    struct sockaddr_in m_server;
    std::vector<client_observer_t> m_subscibers;

    void publishServerMsg(const char * msg, size_t msgSize);
    void publishServerDisconnected(const pipe_ret_t & ret);
    static void * ReceiveTask(void * context);

public:
    pipe_ret_t connectTo(const std::string & address, int port);
    pipe_ret_t sendMsg(const char * msg, size_t size);
    void subscribe(const client_observer_t & observer);
    void unsubscribeAll();
    void publish(const char * msg, size_t msgSize);
    pipe_ret_t finish();
};

#endif //INTERCOM_TCP_CLIENT_H
