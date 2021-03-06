#pragma once

#include <istream>
#include <ostream>
#include <sys/socket.h>
#include "socket.h"
#include "socketstream.h"
#include "core/resman.h"
#include <fstream>

// Forward declaration
class ScopedPath;

class Channel : public Resource {
public:
	virtual std::istream &in();
	virtual std::ostream &out();
	virtual ~Channel() {};

	virtual void shutdown() = 0;

protected:
	Channel() {};

private:
	Channel(const Channel &);
	Channel & operator=(const Channel &);
};

class NetworkChannel : public Channel {
public:
	NetworkChannel(const Socket& socket);
	virtual std::istream &in();
	virtual std::ostream &out();

	virtual void shutdown();

private:
	const Socket& socket_;
	isocketbuf sin_buf_;
	osocketbuf sout_buf_;
	std::istream sin_;
	std::ostream sout_;
};

class InboundNetworkChannel: public NetworkChannel {
public:
	InboundNetworkChannel(ListeningSocket *listen_socket, int transfer_timeout);

private:
	InboundSocket in_socket_;
};

class OutboundNetworkChannel: public NetworkChannel {
public:
	OutboundNetworkChannel(const sockaddr *remote, int connect_timeout, int transfer_timeout);

private:
	OutboundSocket out_socket_;
};

class ReadFileChannel : public Channel {
public:
	ReadFileChannel(const ScopedPath &path, std::fstream::openmode mode);
	virtual std::istream &in();

	virtual void shutdown() { fin_.close(); };

private:
	std::string filename_;
	std::ifstream fin_;
};

class WriteFileChannel : public Channel {
public:
	WriteFileChannel(const ScopedPath &path, std::fstream::openmode mode);
	~WriteFileChannel();
	virtual std::ostream &out();

	virtual void shutdown() { fout_.close(); };

private:
	std::string filename_;
	std::ofstream fout_;
};

class ExternalChannel : public Channel {
public:
	ExternalChannel(std::istream &sin, std::ostream &sout);
	virtual std::istream &in();
	virtual std::ostream &out();

	virtual void shutdown() {};

private:
	std::istream &sin_;
	std::ostream &sout_;
};