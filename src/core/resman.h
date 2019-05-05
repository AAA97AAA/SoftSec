#pragma once

#include <vector>
#include <string>
#include <fstream>

// Forward declaration
class Channel;
struct sockaddr;
class ListeningSocket;
class ScopedPath;
struct Process;

class Resource {
public:
	Resource();
	virtual ~Resource() {};

private:
	bool owned;

	friend class ResourceManager;
};

class ResourceManager {
public:
	ResourceManager(Process &proc, size_t size = 8);
	~ResourceManager();

	bool adopt_resource(Resource *res);

	Channel *create_inbound_channel(ListeningSocket *listen_socket, int transfer_timeout = -1);
	Channel *create_outbound_channel(const sockaddr *remote, int connect_timeout = -1, int transfer_timeout = -1);

	Channel *create_readfile_channel(const ScopedPath &path, std::fstream::openmode mode = std::fstream::in);
	Channel *create_writefile_channel(const ScopedPath &path, std::fstream::openmode mode = std::fstream::out);

private:
	void add_resource(Resource *res);

private:
	std::vector<Resource *> res_;
	Process &proc_;
};