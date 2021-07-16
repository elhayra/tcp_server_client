
#include "../include/tcp_server.h"
#include "../include/common.h"

//todo: turn code into c++11 (use cpp 11 features)
//todo: allow running server and client examples together such that it is interactive (maybe use docker-compose?)
//todo: go over code, improve doc in code and in README

//todo: pr: fix race - code now thread safe, refactored, running

TcpServer::TcpServer() {
    _subscibers.reserve(10);
    _clients.reserve(10);
}

void TcpServer::subscribe(const server_observer_t & observer) {
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    _subscibers.push_back(observer);
}

void TcpServer::unsubscribeAll() {
    std::lock_guard<std::mutex> lock(_subscribersMtx);
    _subscibers.clear();
}

void TcpServer::printClients() {
    std::lock_guard<std::mutex> lock(_clientsMtx);
    for (const Client & client : _clients) {
        const std::string connected = client.isConnected() ? "True" : "False";
        std::cout << "-----------------\n" <<
                  "IP address: " << client.getIp() << std::endl <<
                  "Connected?: " << connected << std::endl <<
                  "Socket FD: " << client.getFileDescriptor() << std::endl <<
                  "Message: " << client.getInfoMessage().c_str() << std::endl;
    }
}

/*
 * Receive client packets, and notify user
 */
void TcpServer::receiveTask() {
    std::lock_guard<std::mutex> lock(_clientsMtx); // todo: this will cause deadlock

    Client * client = &_clients.back();

    while(client->isConnected()) {
        char msg[MAX_PACKET_SIZE];
        const int numOfBytesReceived = recv(client->getFileDescriptor(), msg, MAX_PACKET_SIZE, 0);
        if(numOfBytesReceived < 1) {
            client->setDisconnected();
            const bool clientClosedConnection = (numOfBytesReceived == 0);
            if (clientClosedConnection) {
                client->setErrorMessage("Client closed connection");
            } else {
                client->setErrorMessage(strerror(errno));
            }
            close(client->getFileDescriptor());
            publishClientDisconnected(*client);
            deleteClient(*client);
            break;
        } else {
            publishClientMsg(*client, msg, numOfBytesReceived);
        }
    }
}

/*
 * Erase client from clients vector.
 * If client isn't in the vector, return false. Return
 * true if it is.
 */
bool TcpServer::deleteClient(Client & client) {
    std::lock_guard<std::mutex> lock(_clientsMtx);

    int clientIndex = -1;
    for (uint i=0; i < _clients.size(); i++) {
        if (_clients[i] == client) {
            clientIndex = i;
            break;
        }
    }
    if (clientIndex > -1) {
        _clients.erase(_clients.begin() + clientIndex);
        return true;
    }
    return false;
}

/*
 * Publish incoming client message to observer.
 * Observers get only messages that originated
 * from clients with IP address identical to
 * the specific observer requested IP
 */
void TcpServer::publishClientMsg(const Client & client, const char * msg, size_t msgSize) {
    std::lock_guard<std::mutex> lock(_subscribersMtx);

    for (uint i=0; i < _subscibers.size(); i++) {
        if (_subscibers[i].wantedIp == client.getIp() || _subscibers[i].wantedIp.empty()) {
            if (_subscibers[i].incoming_packet_func != NULL) {
                (*_subscibers[i].incoming_packet_func)(client, msg, msgSize);
            }
        }
    }
}

/*
 * Publish client disconnection to observer.
 * Observers get only notify about clients
 * with IP address identical to the specific
 * observer requested IP
 */
void TcpServer::publishClientDisconnected(const Client & client) {
    std::lock_guard<std::mutex> lock(_subscribersMtx);

    for (uint i=0; i < _subscibers.size(); i++) {
        if (_subscibers[i].wantedIp == client.getIp()) {
            if (_subscibers[i].disconnected_func != NULL) {
                (*_subscibers[i].disconnected_func)(client);
            }
        }
    }
}

/*
 * Bind port and start listening
 * Return tcp_ret_t
 */
pipe_ret_t TcpServer::start(int port, int maxNumOfClients) {
    try {
        initializeSocket();
        bindAddress(port);
        listenToClients(maxNumOfClients);
    } catch (const std::runtime_error &error) {
        return pipe_ret_t::failure(error.what());
    }
    return pipe_ret_t::success();
}

void TcpServer::initializeSocket() {
    _sockfd = socket(AF_INET, SOCK_STREAM, 0);
    const bool socketFailed = (_sockfd == -1);
    if (socketFailed) {
        throw new std::runtime_error(strerror(errno));
    }

    // set socket for reuse (otherwise might have to wait 4 minutes every time socket is closed)
    const int option = 1;
    setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
}

void TcpServer::bindAddress(int port) {
    memset(&_serverAddress, 0, sizeof(_serverAddress));
    _serverAddress.sin_family = AF_INET;
    _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
    _serverAddress.sin_port = htons(port);

    const int bindResult = bind(_sockfd, (struct sockaddr *)&_serverAddress, sizeof(_serverAddress));
    const bool bindFailed = (bindResult == -1);
    if (bindFailed) {
        throw new std::runtime_error(strerror(errno));
    }
}

void TcpServer::listenToClients(int maxNumOfClients) {
    const int clientsQueueSize = maxNumOfClients;
    int listenResult = listen(_sockfd, clientsQueueSize);
    const bool listenFailed = (listenResult == -1);
    if (listenFailed) {
        throw new std::runtime_error(strerror(errno));
    }
}

/*
 * Accept and handle new client socket. To handle multiple clients, user must
 * call this function in a loop to enable the acceptance of more than one.
 * If timeout argument equal 0, this function is executed in blocking mode.
 * If timeout argument is > 0 then this function is executed in non-blocking
 * mode (async) and will quit after timeout seconds if no client tried to connect.
 * Return accepted client
 */
Client TcpServer::acceptClient(uint timeout) {
    socklen_t sosize  = sizeof(_clientAddress);
    Client newClient;

    if (timeout > 0) {
        struct timeval tv;
        tv.tv_sec = timeout;
        tv.tv_usec = 0;
        FD_ZERO(&_fds);
        FD_SET(_sockfd, &_fds);
        int selectRet = select(_sockfd + 1, &_fds, NULL, NULL, &tv);

        const int SELECT_FAILED = -1;
        const int SELECT_TIMEOUT = 0;
        const bool noIncomingNewClient = (!FD_ISSET(_sockfd, &_fds));

        if (selectRet == SELECT_FAILED) {
            newClient.setErrorMessage(strerror(errno));
            return newClient;
        } else if (selectRet == SELECT_TIMEOUT) {
            newClient.setErrorMessage("Timeout waiting for client");
            return newClient;
        } else if (noIncomingNewClient) {
            newClient.setErrorMessage("File descriptor is not set");
            return newClient;
        }
    }

    const int fileDescriptor = accept(_sockfd, (struct sockaddr*)&_clientAddress, &sosize);
    const bool acceptFailed = (fileDescriptor == -1);
    if (acceptFailed) {
        newClient.setErrorMessage(strerror(errno));
        return newClient;
    }

    newClient.setFileDescriptor(fileDescriptor);
    newClient.setConnected();
    newClient.setIp(inet_ntoa(_clientAddress.sin_addr));
    {
        std::lock_guard<std::mutex> lock(_clientsMtx);
        _clients.push_back(newClient);
        _clients.back().setThreadHandler(std::bind(&TcpServer::receiveTask, this));
    }

    return newClient;
}

/*
 * Send message to all connected clients.
 * Return true if message was sent successfully to all clients
 */
pipe_ret_t TcpServer::sendToAllClients(const char * msg, size_t size) {
    std::lock_guard<std::mutex> lock(_clientsMtx);

    for (uint i=0; i < _clients.size(); i++) {
        pipe_ret_t ret = sendToClient(_clients[i], msg, size);
        if (!ret.successFlag) {
            return ret;
        }
    }
    return pipe_ret_t::success();
}

/*
 * Send message to specific client (determined by client IP address).
 * Return true if message was sent successfully
 */
pipe_ret_t TcpServer::sendToClient(const Client & client, const char * msg, size_t size){
    const int numBytesSent = send(client.getFileDescriptor(), (char *)msg, size, 0);
    const bool sendFailed = (numBytesSent < 0);
    if (sendFailed) {
        pipe_ret_t::failure(strerror(errno));
    }

    const bool notAllBytesWereSent = ((uint)numBytesSent < size);
    if (notAllBytesWereSent) {
        char errorMsg[100];
        sprintf(errorMsg, "Only %d bytes out of %lu was sent to client", numBytesSent, size);
        return pipe_ret_t::failure(errorMsg);
    }

    return pipe_ret_t::success();
}

/*
 * Close server and clients resources.
 * Return true is successFlag, false otherwise
 */
pipe_ret_t TcpServer::finish() {
    std::lock_guard<std::mutex> lock(_clientsMtx);

    for (uint i=0; i < _clients.size(); i++) {
        _clients[i].setDisconnected();
        const int closeClientResult = close(_clients[i].getFileDescriptor());
        const bool closeClientFailed = (closeClientResult == -1);
        if (closeClientFailed) {
            return pipe_ret_t::failure(strerror(errno));
        }
    }

    const int closeServerResult = close(_sockfd);
    const bool closeServerFailed = (closeServerResult == -1);
    if (closeServerFailed) {
        return pipe_ret_t::failure(strerror(errno));
    }
    _clients.clear();
    return pipe_ret_t::success();
}
