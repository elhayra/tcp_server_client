#pragma once

#include <string>
#include <thread>
#include <functional>
#include <mutex>
#include <atomic>

#include "pipe_ret_t.h"
#include "client_event.h"


class Client {

    using client_event_handler_t = std::function<void(const Client&, ClientEvent, const std::string&)>;

private:
    int _sockfd = 0;
    std::string _ip = "";
    std::atomic<bool> _isConnected;
    std::thread * _threadHandler = nullptr;
    client_event_handler_t _eventHandlerCallback;

    mutable std::mutex _sockfdMtx;

    void setConnected(bool flag) { _isConnected = flag; }

    void receiveTask();

public:
    Client();
    ~Client();

    bool operator ==(const Client & other) const ;

    void setFileDescriptor(int sockfd) { _sockfd = sockfd; }

    void setIp(const std::string & ip) { _ip = ip; }
    std::string getIp() const { return _ip; }

    void setEventsHandler(const client_event_handler_t & eventHandler) { _eventHandlerCallback = eventHandler; }
    void publishEvent(ClientEvent clientEvent, const std::string &msg = "");


    bool isConnected() const { return _isConnected; }

    void startListen();

    void send(const char * msg, size_t size) const;

    void close();

    void print() const;

};


