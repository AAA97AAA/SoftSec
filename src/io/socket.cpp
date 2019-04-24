#include "socket.h"
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdexcept>
#include <poll.h>
#include <cstring>
#include <unistd.h>

Socket::Socket(int fd, int timeout)
	: fd_(fd), timeout_(timeout)
{
}

Socket::~Socket()
{
	// disregard any errors that might arise, std::fstream::openmode mode
	shutdown(fd_, SHUT_RDWR);
	close(fd_);
}

int Socket::fd() const
{
	return fd_;
}

int Socket::timeout() const
{
	return timeout_;
}

InboundSocket::InboundSocket(ListeningSocket *listen_socket, int transfer_timeout)
:	Socket(-1, transfer_timeout)
{
	int listen_fd = listen_socket->fd();
	pollfd s_poll = {.fd = listen_fd, .events = POLLIN};
    int e_count = poll(&s_poll, 1, listen_socket->accept_timeout_);

    if (e_count < 0) {
    	// error occured (one of EFAULT, EINTR, EINVAL, ENOMEM)
    	// man poll(2)
        throw std::runtime_error(strerror(errno));
    } else if (e_count == 0) {
        throw std::runtime_error("Socket poll timeout (POLLIN).");
    } else if ((s_poll.revents & POLLIN) == 0) {
        throw std::runtime_error("Socket poll error (POLLIN).");
    }

    sockaddr_in cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    fd_ = accept(listen_fd, (sockaddr *)&cli_addr, &cli_len);

    if (fd_ < 0) {
    	// error occured (one of EBADF, EFAULT, ENOTSOCK, EOPNOTSUPP, EWOULDBLOCK)
    	// man accept(2)
        throw std::runtime_error(strerror(errno));
    }
}

OutboundSocket::OutboundSocket(const sockaddr *remote_ep, int connect_timeout, int transfer_timeout)
	: Socket(-1, transfer_timeout)
{
	fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (fd_ < 0) {
		throw std::runtime_error("Failed to create outbound socket");
	}

	if (connect(fd_, remote_ep, sizeof(*remote_ep)) < 0
			&& errno != EINPROGRESS) {
        throw std::runtime_error(strerror(errno));
	}

	pollfd s_poll = {.fd = fd_, .events = POLLOUT};
	if (poll(&s_poll, 1, connect_timeout) <= 0 || (s_poll.revents & POLLHUP) != 0) {
		throw std::runtime_error("Failed to connect outbound socket");
	}
}

ListeningSocket::ListeningSocket(const sockaddr *local_ep, int accept_timeout)
:	Socket(-1, -1),
	accept_timeout_(accept_timeout)
{
	fd_ = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	int reuse = 1;
	if
	(
		fd_ < 0 ||
		setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0 ||
		setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0
	) {
		throw std::runtime_error("Failed to create inbound socket");
	}

	if (bind(fd_, local_ep, sizeof(*local_ep)) < 0) {
        throw std::runtime_error(strerror(errno));
	}

	listen(fd_, 1); // TODO make backlog configurable
}