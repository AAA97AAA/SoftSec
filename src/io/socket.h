#pragma once

#include <sys/socket.h>

class Socket {
public:
	virtual ~Socket();

protected:
	Socket(int fd, int timeout);

private:
	Socket(const Socket &);
	Socket & operator=(const Socket &);

public:
	int fd() const;
	int timeout() const;

protected:
	int fd_;
	int timeout_;
};

class ListeningSocket : public Socket {
public:
	ListeningSocket(const sockaddr *local_ep, int accept_timeout = -1);

private:
	friend class InboundSocket;
	int accept_timeout_;
};

class InboundSocket : public Socket {
public:
	InboundSocket(ListeningSocket *listen_socket, int transfer_timeout);
};

class OutboundSocket : public Socket {
public:
	OutboundSocket(const sockaddr *remote_ep, int connect_timeout, int transfer_timeout);
};