#include "tcp_server.h"



void TcpServer::subscribe(const server_observer_t & observer) {
	m_subscibers.push_back(observer);
}

void TcpServer::unsubscribeAll() {
	m_subscibers.clear();
}

void TcpServer::printClients() {
	for (uint i=0; i<m_clients.size(); i++) {
		std::string connected = m_clients[i].connected ? "True" : "False";
		std::string stopped = m_clients[i].stop ? "True" : "False";
		std::cout << "-----------------\n" <<
				"IP address: " << m_clients[i].ip << std::endl <<
				"Connected?: " << connected << std::endl <<
				"Stopped?: " << stopped << std::endl <<
 				"Socket FD: " << m_clients[i].sockfd << std::endl <<
				"Message: " << m_clients[i].msg.c_str() << std::endl;
 	}
}

/*
 * Receive client packets, and notify user
 */
void* TcpServer::ReceiveTask(void *context) {

	TcpServer * serverInstance = (TcpServer *)context;
	client_t * client = &serverInstance->m_clients.back();

	while(!client->stop) {
		char msg[MAX_PACKET_SIZE];
		int numOfBytesReceived = recv(client->sockfd, msg, MAX_PACKET_SIZE, 0);
		if(numOfBytesReceived < 1) {
			client->stop = true;
			client->connected = false;
			if (numOfBytesReceived == 0) { //client closed connection
				client->msg = "Client closed connection";
			} else {
				client->msg = strerror(errno);
			}
			close(client->sockfd);
			serverInstance->publishClientDisconnected(*client);
			serverInstance->deleteClient(*client);
			break;
		} else {
			serverInstance->publishClientMsg(*client, msg, numOfBytesReceived);
		}
	}

	return NULL;
}

/*
 * Erase client from clients vector.
 * If client isn't in the vector, return false. Return
 * true if it is.
 */
bool TcpServer::deleteClient(client_t & client) {
	int clientIndex = -1;
	for (uint i=0; i<m_clients.size(); i++) {
		if (m_clients[i] == client) {
			clientIndex = i;
			break;
		}
	}
	if (clientIndex > -1) {
		m_clients.erase(m_clients.begin() + clientIndex);
		return true;
	}
	return false;
}

/*
 * Publish incoming client message to observer.
 * Observers get only messages that originated
 * from clients with IP address identical to
 * the specific observer requested IP
 */
void TcpServer::publishClientMsg(const client_t & client, const char * msg, size_t msgSize) {
	for (uint i=0; i<m_subscibers.size(); i++) {
		if (m_subscibers[i].wantedIp == client.ip) {
			if (m_subscibers[i].incoming_packet_func != NULL) {
				(*m_subscibers[i].incoming_packet_func)(client, msg, msgSize);
			}
		}
	}
}

/*
 * Publish client disconnection to observer.
 * Observers get only notify about clients
 * with IP address identical to the specific
 * observer requested IP
 */
void TcpServer::publishClientDisconnected(const client_t & client) {
	for (uint i=0; i<m_subscibers.size(); i++) {
		if (m_subscibers[i].wantedIp == client.ip) {
			if (m_subscibers[i].disconnected_func != NULL) {
				(*m_subscibers[i].disconnected_func)(client);
			}
		}
	}
}

/*
 * Bind port and start listening
 * Return tcp_ret_t
 */
tcp_ret_t TcpServer::start(int port) {
	m_sockfd = 0;
	m_clients.reserve(10);
	m_subscibers.reserve(10);
	tcp_ret_t ret;

	m_sockfd = socket(AF_INET,SOCK_STREAM,0);
	if (m_sockfd == -1) { //socket failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
 	memset(&m_serverAddress, 0, sizeof(m_serverAddress));
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	m_serverAddress.sin_port = htons(port);

	int bindSuccess = bind(m_sockfd, (struct sockaddr *)&m_serverAddress, sizeof(m_serverAddress));
	if (bindSuccess == -1) { // bind failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
	const int clientsQueueSize = 5;
 	int listenSuccess = listen(m_sockfd, clientsQueueSize);
 	if (listenSuccess == -1) { // listen failed
 		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
 	}
 	ret.success = true;
 	return ret;
}

/*
 * Accept and handle new client socket. To handle multiple clients, user must
 * call this function in a loop to enable the acceptance of more than one.
 * If timeout argument equal 0, this function is executed in blocking mode.
 * If timeout argument is > 0 then this function is executed in non-blocking
 * mode and will quit after timeout seconds if no client tried to connect.
 * Return accepted client
 */
client_t TcpServer::acceptClient(uint timeout) {
	socklen_t sosize  = sizeof(m_clientAddress);
	client_t newClient;

	if (timeout > 0) {
		struct timeval tv;
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		FD_ZERO(&m_fds);
		FD_SET(m_sockfd, &m_fds);
		int selectRet = select(m_sockfd + 1, &m_fds, NULL, NULL, &tv);
		if (selectRet == -1) { // select failed
			newClient.msg = strerror(errno);
			return newClient;
		} else if (selectRet == 0) { // timeout
			newClient.msg = "Timeout waiting for client";
			return newClient;
		} else if (!FD_ISSET(m_sockfd, &m_fds)) { // no new client
			newClient.msg = "File descriptor is not set";
			return newClient;
		}
	}

	newClient.sockfd = accept(m_sockfd, (struct sockaddr*)&m_clientAddress, &sosize);
	if (newClient.sockfd == -1) { // accept failed
		newClient.msg = strerror(errno);
		return newClient;
	}
	newClient.connected = true;
	newClient.ip = inet_ntoa(m_clientAddress.sin_addr);
	m_clients.push_back(newClient);
	pthread_t threadhandle;
	pthread_create(&threadhandle, NULL, &TcpServer::ReceiveTask, this);
	return newClient;
}

/*
 * Send message to all connected clients.
 * Return true if message was sent successfully to all clients
 */
tcp_ret_t TcpServer::sendToAllClients(const char * msg, size_t size) {
	tcp_ret_t ret;
	for (uint i=0; i<m_clients.size(); i++) {
		ret = sendToClient(m_clients[i], msg, size);
		if (!ret.success) {
			return ret;
		}
	}
	ret.success = true;
	return ret;
}

/*
 * Send message to specific client (determined by client IP address).
 * Return true if message was sent successfully
 */
tcp_ret_t TcpServer::sendToClient(const client_t & client, const char * msg, size_t size){
	tcp_ret_t ret;
	int numBytesSent = send(client.sockfd, (char *)msg, size, 0);
	if (numBytesSent < 0) { // send failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
	if ((uint)numBytesSent < size) { // not all bytes were sent
		ret.success = false;
		char msg[100];
		sprintf(msg, "Only %d bytes out of %lu was sent to client", numBytesSent, size);
		ret.msg = msg;
		return ret;
	}
	ret.success = true;
	return ret;
}

/*
 * Close server and clients resources.
 * Return true is success, false otherwise
 */
tcp_ret_t TcpServer::finish() {
	tcp_ret_t ret;
	for (uint i=0; i<m_clients.size(); i++) {
		m_clients[i].stop = true;
		m_clients[i].connected = false;
		if (close(m_clients[i].sockfd) == -1) { // close failed
			ret.success = false;
			ret.msg = strerror(errno);
			return ret;
		}
	}
	if (close(m_sockfd) == -1) { // close failed
		ret.success = false;
		ret.msg = strerror(errno);
		return ret;
	}
	m_clients.clear();
	ret.success = true;
	return ret;
}
