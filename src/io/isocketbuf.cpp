#include "socketstream.h"
#include "socket.h"
#include <streambuf>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <stdexcept>
#include <errno.h>

using std::size_t;

isocketbuf::isocketbuf(const Socket &socket, std::size_t buff_sz) :
	socket_(socket),
	buffer_(buff_sz)
{
	char *end = &buffer_.front() + buffer_.size();
    setg(end, end, end);
}

std::streambuf::int_type isocketbuf::underflow()
{
    if (gptr() < egptr()) { // buffer not exhausted
        return traits_type::to_int_type(*gptr());
    }

    char *base = &buffer_.front();
    char *start = base;

    // start is now the start of the buffer.
    pollfd s_poll = {.fd = socket_.fd(), .events = POLLIN};
    int e_count = poll(&s_poll, 1, socket_.timeout());

    if (e_count < 0) {
    	// error occured (one of EFAULT, EINTR, EINVAL, ENOMEM)
    	// man poll(2)
        throw std::runtime_error(strerror(errno));
    } else if (e_count == 0) {
        throw std::runtime_error("Socket poll timeout (POLLIN).");
    } else if (((s_poll.revents & POLLIN) == 0)) {
        throw std::runtime_error("Socket read error.");
    }

    ssize_t n = 0;
    n = read(socket_.fd(), start, buffer_.size() - (start - base));
    if (n < 0) {
        throw std::runtime_error("Socket read error.");
    }

    if (n == 0) {
        return traits_type::eof();
    }

    // Set buffer pointers
    setg(base, start, start + n);

    return traits_type::to_int_type(*gptr());
}