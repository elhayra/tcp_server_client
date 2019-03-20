/*
 * client.h
 *
 *  Created on: Mar 14, 2019
 *      Author: erauper
 */



#ifndef CLIENT_H_
#define CLIENT_H_

#include <string>

struct client_t {

	int sockfd;
	std::string ip;
	std::string msg;
	bool stop;
	bool connected;

	client_t() {
		sockfd = 0;
		ip = "";
		msg = "";
		stop = false;
		connected = false;
	}

	bool operator ==(const client_t & other) {
		if ( (this->sockfd == other.sockfd) &&
			 (this->ip == other.ip) ) {
			return true;
		}
		return false;
	}
};


#endif /* CLIENT_H_ */
