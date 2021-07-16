#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <errno.h>
#include <unistd.h>
#include <stdexcept>
#include <sys/socket.h>
#include <iostream>

#include "../include/client.h"
#include "../include/common.h"

//todo: use mutexes

Client::~Client() {
    if (_threadHandler != nullptr) {
        _threadHandler->detach();
        delete _threadHandler;
        _threadHandler = nullptr;
    }
}

bool Client::operator ==(const Client & other) const {
    if ((this->_sockfd == other._sockfd) &&
        (this->_ip == other._ip) ) {
        return true;
    }
    return false;
}

void Client::startListen() {
    _threadHandler = new std::thread(&Client::receiveTask, this);
}

void Client::send(const char *msg, size_t size) const {
    const size_t numBytesSent = ::send(_sockfd, (char *)msg, size, 0);
    const bool sendFailed = (numBytesSent < 0);
    if (sendFailed) {
        throw std::runtime_error(strerror(errno));
    }

    const bool notAllBytesWereSent = ((uint)numBytesSent < size);
    if (notAllBytesWereSent) {
        char errorMsg[100];
        sprintf(errorMsg, "Only %d bytes out of %lu was sent to client", numBytesSent, size);
        throw std::runtime_error(errorMsg);
    }
}

/*
 * Receive client packets, and notify user
 */
void Client::receiveTask() {
    while(isConnected()) {
        char receivedMessage[MAX_PACKET_SIZE];
        const int numOfBytesReceived = recv(_sockfd, receivedMessage, MAX_PACKET_SIZE, 0);
        if(numOfBytesReceived < 1) {
            const bool clientClosedConnection = (numOfBytesReceived == 0);
            std::string disconnectionMessage;
            if (clientClosedConnection) {
                disconnectionMessage = "Client closed connection";
            } else {
                disconnectionMessage = strerror(errno);
            }
            close();
            setDisconnected();
            publishEvent(ClientEvent::DISCONNECTED, disconnectionMessage);
            break;
        } else {
            publishEvent(ClientEvent::INCOMING_MSG, receivedMessage);
        }
    }
}

void Client::publishEvent(ClientEvent clientEvent, const std::string &msg) {
    _eventHandlerCallback(*this, clientEvent, msg);
}

void Client::print() const {
    const std::string connected = isConnected() ? "True" : "False";
    std::cout << "-----------------\n" <<
              "IP address: " << getIp() << std::endl <<
              "Connected?: " << connected << std::endl <<
              "Socket FD: " << _sockfd << std::endl <<
              "Message: " << getInfoMessage().c_str() << std::endl;
}

void Client::close() {
   //todo: close thread and delete it. use mutex over connected flag

    const int closeClientResult = ::close(_sockfd);
    const bool closeClientFailed = (closeClientResult == -1);
    if (closeClientFailed) {
        throw new std::runtime_error(strerror(errno));
    }
}

