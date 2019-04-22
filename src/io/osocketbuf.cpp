#include "socketstream.h"
#include "socket.h"
#include <streambuf>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <assert.h>
#include <stdexcept>
#include <errno.h>

osocketbuf::osocketbuf(const Socket &socket, size_t buff_sz) :
	socket_(socket),
	buffer_(buff_sz + 1)
{
	char *base = &buffer_.front();
    setp(base, base + buffer_.size() - 1);
}

std::streambuf::int_type osocketbuf::overflow(int_type ch)
{
    if (ch != traits_type::eof()) {
        assert(std::less_equal<char *>()(pptr(), epptr()));
        *pptr() = ch;
        pbump(1);
        if (write_and_flush()) {
            return ch;
        }
	}

    return traits_type::eof();
}

int osocketbuf::sync()
{
    return write_and_flush() ? 0 : -1;
}

bool osocketbuf::write_and_flush()
{
    pollfd s_poll = {.fd = socket_.fd(), .events = POLLOUT};
    int e_count = poll(&s_poll, 1, socket_.timeout());

    if (e_count < 0) {
        // error occured (one of EFAULT, EINTR, EINVAL, ENOMEM)
        // man poll(2)
        throw std::runtime_error(strerror(errno));
    } else if (e_count == 0) {
        throw std::runtime_error("Socket poll timeout (POLLOUT).");
    } else if ((s_poll.revents & POLLOUT) == 0) {
        throw std::runtime_error("Socket poll error (POLLOUT).");
    }

    char *end = pptr();
    std::ptrdiff_t len = pptr() - pbase();
    pbump(-len);

    ssize_t n;
    size_t rem = len;
    while (rem > 0) {
        n = write(socket_.fd(), pptr(), rem);
        if (n <= 0) {
            throw std::runtime_error("Socket write error.");
        }
        rem -= n;
        pbump(n);
    }

    pbump(-len);
    assert(std::equal_to<char *>()(pptr(), pbase()));
    return true;
}