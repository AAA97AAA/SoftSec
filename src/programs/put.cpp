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

static int PutFileDaemon(const string &args, Process &proc, Channel *io)
{
	istringstream dargs(args);

	unsigned short port;
	size_t filesize;
	dargs >> port >> filesize;
	string filename = get_args(dargs);

	ScopedPath scoped = proc.sys_.resolve(proc, filename);
	filename = static_cast<const string &>(scoped);

	struct sockaddr_in addr;

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0; //0x0100007F; // 127.0.0.1
	addr.sin_port = htons(port);

	ListeningSocket sock((sockaddr *)&addr, 10000);
	istream &in = proc.resman_.create_inbound_channel(&sock, 5000)->in();
	ostream &out = proc.resman_.create_writefile_channel(scoped, fstream::binary)->out();

	size_t i = 0; 
	auto oit = ostreambuf_iterator<char>(out);
	for (
			auto iit = istreambuf_iterator<char>(in);
			i < filesize && iit != istreambuf_iterator<char>();
			++i, ++iit
		)
	{
		oit = *iit;
	}

	out.flush();

	return 0;
}

int PutFile(const string &args, Process &proc, Channel *io)
{
	ostream &out = io->out();

	unsigned short port = (rand() % (0xffff - 0xc000)) + 0xc000;

	istringstream aargs(args);
	string filename;
	ssize_t filesize = 0;
	aargs >> filename >> filesize;

	if (filesize <= 0) {
		throw std::runtime_error("Invalid file size");
	}

	ScopedPath scoped = proc.sys_.resolve(proc, filename); // sanitization step

	ostringstream ssdargs;
	ssdargs << port << " " << filesize << " " << filename << flush;
	string dargs = ssdargs.str();

	pid_t pid = proc.sys_.create_daemon(&proc, proc.env_);
	Process *child = proc.sys_.get_process(pid);
	child->run(PutFileDaemon, dargs, nullptr);

	out << "put port: " << port << endl;

	return 0;
}