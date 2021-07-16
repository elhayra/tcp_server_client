#pragma once

#include <string>
#include <thread>
#include <functional>

//todo: the client need to get from the server all the info it needs to handle
//todo: incoming and outgoing data. the function receive task should be part
// todo: of this class.
//todo: also, it should get a list of it subs, and notify server when
//todo: it disconnects, or other events


class Client {

private:
    int _sockfd = 0;
    std::string _ip = "";
    std::string _errorMsg = "";
    bool _isConnected;
    std::thread * _threadHandler = nullptr;

public:
    ~Client();
    bool operator ==(const Client & other) const ;

    void setFileDescriptor(int sockfd) { _sockfd = sockfd; }
    int getFileDescriptor() const { return _sockfd; }

    void setIp(const std::string & ip) { _ip = ip; }
    std::string getIp() const { return _ip; }

    void setErrorMessage(const std::string & msg) { _errorMsg = msg; }
    std::string getInfoMessage() const { return _errorMsg; }

    void setConnected() { _isConnected = true; }

    void setDisconnected() { _isConnected = false; }
    bool isConnected() const { return _isConnected; }

    void setThreadHandler(std::function<void(void)> func) { _threadHandler = new std::thread(func);}

};


