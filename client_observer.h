

#ifndef CLIENT_OBSERVER_H_
#define CLIENT_OBSERVER_H_

#include <string>
#include "tcp_ret.h"


typedef void (incoming_packet_func)(const char * msg, size_t size);
typedef incoming_packet_func* incoming_packet_func_t;

typedef void (disconnected_func)(const tcp_ret_t & ret);
typedef disconnected_func* disconnected_func_t;

struct client_observer_t {

	std::string wantedIp;
	incoming_packet_func_t incoming_packet_func;
	disconnected_func_t disconnected_func;

	client_observer_t() {
		wantedIp = "";
		incoming_packet_func = NULL;
		disconnected_func = NULL;
	}
};



#endif /* CLIENT_OBSERVER_H_ */
