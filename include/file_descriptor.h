#pragma once

class FileDescriptor {
private:
    int _sockfd = 0;

public:
    void set(int fd) { _sockfd = fd; }
    int get() const { return _sockfd; }
};