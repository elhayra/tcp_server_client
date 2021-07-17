
#include "../include/tcp_client.h"
#include "../include/common.h"

TcpClient::TcpClient() {
    _stop = false;
}

pipe_ret_t TcpClient::connectTo(const std::string & address, int port) {
    try {
        initializeSocket();
        setAddress(address, port);
    } catch (const std::runtime_error& error) {
        return pipe_ret_t::failure(error.what());
    }

    int connectResult = connect(_sockfd.get() , (struct sockaddr *)&_server , sizeof(_server));
    const bool connectionFailed = (connectResult == -1);
    if (connectionFailed) {
        return pipe_ret_t::failure(strerror(errno));
    }

    startReceivingMessages();

    return pipe_ret_t::success();
}

void TcpClient::startReceivingMessages() {
    std::lock_guard<std::mutex> lock(_receiveTaskMtx);
    _receiveTask = new std::thread(&TcpClient::receiveTask, this);
}

void TcpClient::initializeSocket() {
    pipe_ret_t ret;

    _sockfd.set(socket(AF_INET , SOCK_STREAM , 0));
    const bool socketFailed = (_sockfd.get() == -1);
    if (socketFailed) { //socket failed
        throw new std::runtime_error(strerror(errno));
    }
}

void TcpClient::setAddress(const std::string& address, int port) {
    int inetSuccess = inet_aton(address.c_str(), &_server.sin_addr);

    if(!inetSuccess) { // inet_addr failed to parse address
        // if hostname is not in IP strings and dots format, try resolve it
        struct hostent *host;
        struct in_addr **addrList;
        if ( (host = gethostbyname( address.c_str() ) ) == NULL){
            throw new std::runtime_error("Failed to resolve hostname");
        }
        addrList = (struct in_addr **) host->h_addr_list;
        _server.sin_addr = *addrList[0];
    }
    _server.sin_family = AF_INET;
    _server.sin_port = htons(port);
}


pipe_ret_t TcpClient::sendMsg(const char * msg, size_t size) {
    int numBytesSent = 0;
    {
        numBytesSent = send(_sockfd.get(), msg, size, 0);
    }
    if (numBytesSent < 0 ) { // send failed
        return pipe_ret_t::failure(strerror(errno));
    }
    if ((uint)numBytesSent < size) { // not all bytes were sent
        char errorMsg[100];
        sprintf(errorMsg, "Only %d bytes out of %lu was sent to client", numBytesSent, size);
        return pipe_ret_t::failure(errorMsg);
    }
    return pipe_ret_t::success();
}

void TcpClient::subscribe(const client_observer_t & observer) {
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    _subscibers.push_back(observer);
}

void TcpClient::unsubscribeAll() {
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    _subscibers.clear();
}

/*
 * Publish incomingPacketHandler client message to observer.
 * Observers get only messages that originated
 * from clients with IP address identical to
 * the specific observer requested IP
 */
void TcpClient::publishServerMsg(const char * msg, size_t msgSize) {
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    for (uint i=0; i < _subscibers.size(); i++) {
        if (_subscibers[i].incomingPacketHandler != NULL) {
            _subscibers[i].incomingPacketHandler(msg, msgSize);
        }
    }
}

/*
 * Publish client disconnection to observer.
 * Observers get only notify about clients
 * with IP address identical to the specific
 * observer requested IP
 */
void TcpClient::publishServerDisconnected(const pipe_ret_t & ret) {
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    for (uint i=0; i < _subscibers.size(); i++) {
        if (_subscibers[i].disconnectionHandler != NULL) {
            _subscibers[i].disconnectionHandler(ret);
        }
    }
}

/*
 * Receive server packets, and notify user
 */
void TcpClient::receiveTask() {
    while(!_stop) {
        char msg[MAX_PACKET_SIZE];
        int numOfBytesReceived = 0;
        {
            numOfBytesReceived = recv(_sockfd.get(), msg, MAX_PACKET_SIZE, 0);
        }
        if(numOfBytesReceived < 1) {
            _stop = true;
            std::string errorMsg;
            if (numOfBytesReceived == 0) { //server closed connection
                errorMsg = "Server closed connection";
            } else {
                errorMsg = strerror(errno);
            }
            publishServerDisconnected(pipe_ret_t::failure(errorMsg));
            close();
            break;
        } else {
            publishServerMsg(msg, numOfBytesReceived);
        }
    }
}

pipe_ret_t TcpClient::close(){
    _stop = true;
    terminateReceiveThread();
    pipe_ret_t ret;
    bool closeFailed = false;
    {
        closeFailed = (::close(_sockfd.get()) == -1);
    }
    if (closeFailed) {
        return pipe_ret_t::failure(strerror(errno));
    }
    return pipe_ret_t::success();
}

void TcpClient::terminateReceiveThread() {
    std::lock_guard<std::mutex> lock(_receiveTaskMtx);
    if (_receiveTask != nullptr) {
        _receiveTask->detach();
        delete _receiveTask;
        _receiveTask = nullptr;
    }
}

TcpClient::~TcpClient() {
    terminateReceiveThread();
}
