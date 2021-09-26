#pragma once

#include <string>
#include <functional>
#include "client.h"

struct server_observer_t {
	std::string wantedIP = "";
	std::function<void(const std::string &clientIP, const char * msg, size_t size)> incomingPacketHandler;
	std::function<void(const std::string &ip, const std::string &msg)> disconnectionHandler;
};

