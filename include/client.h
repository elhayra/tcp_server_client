
#ifndef INTERCOM_CLIENT_H
#define INTERCOM_CLIENT_H


#include <string>
#include <thread>
#include <functional>

class Client {

private:
    int m_sockfd = 0;
    std::string m_ip = "";
    std::string m_errorMsg = "";
    bool m_isConnected;
    std::thread * m_threadHandler = nullptr;

public:
    ~Client();
    bool operator ==(const Client & other);

    void setFileDescriptor(int sockfd) { m_sockfd = sockfd; }
    int getFileDescriptor() const { return m_sockfd; }

    void setIp(const std::string & ip) { m_ip = ip; }
    std::string getIp() const { return m_ip; }

    void setErrorMessage(const std::string & msg) { m_errorMsg = msg; }
    std::string getInfoMessage() const { return m_errorMsg; }

    void setConnected() { m_isConnected = true; }

    void setDisconnected() { m_isConnected = false; }
    bool isConnected() { m_isConnected = true; }

    void setThreadHandler(std::function<void(void)> func) { m_threadHandler = new std::thread(func);}

};


#endif //INTERCOM_CLIENT_H
