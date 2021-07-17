#pragma once

#include <string>
#include <functional>
#include "pipe_ret_t.h"

struct client_observer_t {
    std::string wantedIP = "";
    std::function<void(const char * msg, size_t size)> incomingPacketHandler = nullptr;
    std::function<void(const pipe_ret_t & ret)> disconnectionHandler = nullptr;
};


