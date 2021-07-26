#pragma once

#include <cstdio>

#define MAX_PACKET_SIZE 4096

namespace socket_waiter {
    enum Result {
        FAILURE,
        TIMEOUT,
        SUCCESS
    };

    Result waitFor(const FileDescriptor &fileDescriptor, size_t timeoutSeconds = 1);
};




