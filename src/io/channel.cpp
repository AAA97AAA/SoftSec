#include "channel.h"
#include "socket.h"
#include "socketstream.h"
#include "core/path.h"
#include <stdexcept>

std::istream & Channel::in()
{
	throw std::runtime_error("Input stream unavailable.");
}

std::ostream & Channel::out()
{
	throw std::runtime_error("Output stream unavailable.");
}

NetworkChannel::NetworkChannel(const Socket& socket)
:	socket_(socket),
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

InboundNetworkChannel::InboundNetworkChannel(ListeningSocket *listen_socket, int transfer_timeout)
:	NetworkChannel(in_socket_),
	in_socket_(listen_socket, transfer_timeout)
{
}

OutboundNetworkChannel::OutboundNetworkChannel(const sockaddr *remote, int connect_timeout, int transfer_timeout)
:	NetworkChannel(out_socket_),
	out_socket_(remote, connect_timeout, transfer_timeout)
{
}

ReadFileChannel::ReadFileChannel(const ScopedPath &path, std::fstream::openmode mode)
:	fin_(path, mode)
{
}

std::istream & ReadFileChannel::in()
{
	return fin_;
}

WriteFileChannel::WriteFileChannel(const ScopedPath &path, std::fstream::openmode mode)
:	fout_(path, mode)
{
}

std::ostream & WriteFileChannel::out()
{
	return fout_;
}