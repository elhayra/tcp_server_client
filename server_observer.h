/*
 * server_observer.h
 *
 *  Created on: Mar 17, 2019
 *      Author: erauper
 */

#ifndef SERVER_OBSERVER_H_
#define SERVER_OBSERVER_H_


#include <string>
#include "client.h"

typedef void (incoming_packet_func)(const client_t & client, const char * msg, size_t size);
typedef incoming_packet_func* incoming_packet_func_t;

typedef void (disconnected_func)(const client_t & client);
typedef disconnected_func* disconnected_func_t;

struct server_observer_t {

	std::string wantedIp;
	incoming_packet_func_t incoming_packet_func;
	disconnected_func_t disconnected_func;

	server_observer_t() {
		wantedIp = "";
		incoming_packet_func = NULL;
		disconnected_func = NULL;
	}
};

#endif /* SERVER_OBSERVER_H_ */
