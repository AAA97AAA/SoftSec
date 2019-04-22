#include "programs.h"
#include "core/process.h"
#include "io/channel.h"
#include "core/runsys.h"
#include "core/path.h"
#include <iostream>
#include <string>
#include <exception>
#include <cstdlib>
#include <sstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <fstream>
#include <iterator>
#include <algorithm>

using namespace std;

static int GetFileDaemon(const string &args, Process &proc, Channel *io)
{
	istringstream dargs(args);

	unsigned short port;
	dargs >> port;
	string filename = get_args(dargs);

	ScopedPath *scoped = proc.sys_.resolve(proc, filename);
	filename = static_cast<const string &>(*scoped);
	ScopedPath local(*scoped); // so as not to worry about leaks
	delete scoped;

	struct sockaddr_in addr;

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0; //0x0100007F; // 127.0.0.1
	addr.sin_port = htons(port);

	ListeningSocket sock((sockaddr *)&addr, 10000);
	ostream &out = proc.resman_.create_inbound_channel(&sock, 5000)->out();
	istream &in = proc.resman_.create_readfile_channel(local, fstream::binary)->in();

	copy(istreambuf_iterator<char>(in), 
		istreambuf_iterator<char>(),
		ostreambuf_iterator<char>(out));
	out.flush();

	return 0;
}

int GetFile(const string &args, Process &proc, Channel *io)
{
	ostream &out = io->out();

	unsigned short port = (rand() % (0xffff - 0xc000)) + 0xc000;

	string filename = args;
	ScopedPath *scoped = proc.sys_.resolve(proc, filename);
	size_t size;
	try {
		FilePath file(static_cast<const string &>(*scoped));
		delete scoped;
		size = file.size();
	} catch (...) {
		delete scoped;
		throw;
	}

	ostringstream dargs;
	dargs << port << " " << args;

	pid_t pid = proc.sys_.create_daemon(&proc, proc.env_);
	Process *child = proc.sys_.get_process(pid);
	child->run(GetFileDaemon, dargs.str(), nullptr);

	out << "get port: " << port << " size: " << size << endl;

	return 0;
}