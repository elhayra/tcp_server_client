/*
 * tcp_ret.h
 *
 *  Created on: Mar 17, 2019
 *      Author: erauper
 */

#ifndef TCP_RET_H_
#define TCP_RET_H_


struct tcp_ret_t {
	bool success;
	std::string msg;
	tcp_ret_t() {
		success = false;
		msg = "";
	}
};


#endif /* TCP_RET_H_ */
