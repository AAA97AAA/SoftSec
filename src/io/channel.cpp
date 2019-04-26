#include "channel.h"
#include "socket.h"
#include "socketstream.h"
#include "core/path.h"
#include <stdexcept>
#include <sys/stat.h>

std::istream & Channel::in()
{
	throw std::runtime_error("Input stream unavailable.");
}

std::ostream & Channel::out()
{
	throw std::runtime_error("Output stream unavailable.");
}

NetworkChannel::NetworkChannel(const Socket& socket) :
	socket_(socket),
	sin_buf_(socket_),
	sout_buf_(socket_),
	sin_(&sin_buf_),
	sout_(&sout_buf_)
{
}

std::istream & NetworkChannel::in()
{
	return sin_;
}

std::ostream & NetworkChannel::out()
{
	return sout_;
}

void NetworkChannel::shutdown()
{
	socket_.close();
}

InboundNetworkChannel::InboundNetworkChannel(ListeningSocket *listen_socket, int transfer_timeout) :
	NetworkChannel(in_socket_),
	in_socket_(listen_socket, transfer_timeout)
{
}

OutboundNetworkChannel::OutboundNetworkChannel(const sockaddr *remote, int connect_timeout, int transfer_timeout) :
	NetworkChannel(out_socket_),
	out_socket_(remote, connect_timeout, transfer_timeout)
{
}

ReadFileChannel::ReadFileChannel(const ScopedPath &path, std::fstream::openmode mode):
	filename_(static_cast<const std::string &>(path)),
	fin_(filename_, mode)
{
}

std::istream & ReadFileChannel::in()
{
	return fin_;
}

WriteFileChannel::WriteFileChannel(const ScopedPath &path, std::fstream::openmode mode) :
	filename_(static_cast<const std::string &>(path)),
	fout_(filename_, mode)
{
}

WriteFileChannel::~WriteFileChannel()
{
	fout_.flush();
	fout_.close();
	chmod(filename_.c_str(), 0x1ed);
}

std::ostream & WriteFileChannel::out()
{
	return fout_;
}

ExternalChannel::ExternalChannel(std::istream &sin, std::ostream &sout) :
	sin_(sin), sout_(sout)
{
}

std::istream & ExternalChannel::in()
{
	return sin_;
}

std::ostream & ExternalChannel::out()
{
	return sout_;
}