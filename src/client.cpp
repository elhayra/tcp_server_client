

#include "../include/client.h"


Client::~Client() {
    if (m_threadHandler != nullptr) {
        m_threadHandler->detach();
        delete m_threadHandler;
        m_threadHandler = nullptr;
    }
}

bool Client::operator ==(const Client & other) {
    if ( (this->m_sockfd == other.m_sockfd) &&
         (this->m_ip == other.m_ip) ) {
        return true;
    }
    return false;
}
