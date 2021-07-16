

#include "../include/client.h"


Client::~Client() {
    if (_threadHandler != nullptr) {
        _threadHandler->detach();
        delete _threadHandler;
        _threadHandler = nullptr;
    }
}

bool Client::operator ==(const Client & other) {
    if ((this->_sockfd == other._sockfd) &&
        (this->_ip == other._ip) ) {
        return true;
    }
    return false;
}
