#pragma once

#include <string>
#include <thread>
#include <functional>
#include "pipe_ret_t.h"
#include "client_event.h"
//todo: the client need to get from the server all the info it needs to handle
//todo: incoming and outgoing data. the function receive task should be part
// todo: of this class.
//todo: also, it should get a list of it subs, and notify server when
//todo: it disconnects, or other events


class Client {

    using client_event_handler_t = std::function<void(const Client&, ClientEvent, const std::string&)>;

private:
    int _sockfd = 0;
    std::string _ip = "";
    bool _isConnected;
    std::thread * _threadHandler = nullptr;
    client_event_handler_t _eventHandlerCallback;


    void setDisconnected() { _isConnected = false; }
    void receiveTask();

public:
    ~Client();
    bool operator ==(const Client & other) const ;

    void setFileDescriptor(int sockfd) { _sockfd = sockfd; }

    void setIp(const std::string & ip) { _ip = ip; }
    std::string getIp() const { return _ip; }

    void setEventsHandler(const client_event_handler_t & eventHandler) { _eventHandlerCallback = eventHandler; }
    void publishEvent(ClientEvent clientEvent, const std::string &msg = "");

    void setConnected() { _isConnected = true; }

    bool isConnected() const { return _isConnected; }

    void startListen();

    void send(const char * msg, size_t size) const;

    void close();

    void print() const;

};


