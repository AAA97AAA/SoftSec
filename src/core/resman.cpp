#include "resman.h"
#include "io/channel.h"
#include "fileman.h"
#include "process.h"
#include "path.h"
#include "runsys.h"

Resource::Resource()
:	owned(false)
{
}

ResourceManager::ResourceManager(Process &proc, size_t size)
:	proc_(proc)
{
	res_.reserve(size);
}

ResourceManager::~ResourceManager()
{
	for (auto it = res_.begin(); it != res_.end(); ++it) {
		delete (*it);
	}
}

inline
void ResourceManager::add_resource(Resource *res)
{
	res->owned = true;
	res_.push_back(res);
}

bool ResourceManager::adopt_resource(Resource *res)
{
	if (res->owned) {
		return false;
	} else {
		add_resource(res);
		return true;
	}
}

Channel * ResourceManager::create_inbound_channel(ListeningSocket *listen_socket, int transfer_timeout)
{
	Channel *res = new InboundNetworkChannel(listen_socket, transfer_timeout);
	add_resource(dynamic_cast<Resource *>(res));
	return res;
}

Channel * ResourceManager::create_outbound_channel(const sockaddr *remote, int connect_timeout, int transfer_timeout)
{
	Channel *res = new OutboundNetworkChannel(remote, connect_timeout, transfer_timeout);
	add_resource(dynamic_cast<Resource *>(res));
	return res;
}

Channel * ResourceManager::create_readfile_channel(const ScopedPath &path, std::fstream::openmode mode)
{
	Channel *res = new ReadFileChannel(path, mode);
	add_resource(dynamic_cast<Resource *>(res));
	return res;
}

Channel * ResourceManager::create_writefile_channel(const ScopedPath &path, std::fstream::openmode mode)
{
	Channel *res = new WriteFileChannel(path, mode);
	add_resource(dynamic_cast<Resource *>(res));
	return res;
}