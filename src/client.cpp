#include "programs/programs.h"

#include "io/socket.h"
#include "io/channel.h"
#include "core/runsys.h"
#include "core/runenv.h"
#include "core/process.h"
#include "core/barrier.h"

#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <signal.h>
#include <arpa/inet.h>
#include <sstream>
#include <streambuf>
#include <chrono>
#include <thread>

#include <vector>

using namespace std;

int ClientShell(const std::string &args, Process &proc, Channel *io);
int ClientDaemon(const std::string &args, Process &proc, Channel *io);
int ClientGet(const std::string &args, Process &proc, Channel *io);
int ClientPut(const std::string &args, Process &proc, Channel *io);
int GetDaemon(const std::string &args, Process &proc, Channel *io);
int PutDaemon(const std::string &args, Process &proc, Channel *io);

class sentinel {
public:
	explicit sentinel(const std::string &sentinel, Barrier &bar) :
		sentinel_(sentinel),
		bar_(bar)
	{
	}

	std::ostringstream & get_line()
	{
		return line_;
	}

	void feed(char c)
	{
		if (view_.size() < sentinel_.size()) {
			view_ += c;
		} else if (view_ == sentinel_) {
			if (c == '\n') {
				string rest = line_.str();
				line_.seekp(0);
				line_ << sentinel_ << rest;
				bar_.wait(); // wait until receiver is notified
				bar_.wait(); // wait until receiver finishes processing
				line_.str("");
				view_.clear();
			} else {
				line_.write(&c, 1);
			}
		} else {
			view_ = view_.substr(1) + c;
		}
	}

private:
	std::string sentinel_;
	std::string view_;
	std::ostringstream line_;
	Barrier &bar_;
};

struct sockaddr_in addr;
Barrier get_bar(2);
sentinel get_sen("get port:", get_bar);
string get_cmd;
Barrier put_bar(2);
sentinel put_sen("put port:", put_bar);
string put_cmd;
bool working = true;

int main(int argc, const char *argv[])
{
	if (argc != 3 && argc != 5) {
		throw std::runtime_error("Invalid number of arguments");
	}

	string root_dir = "root_cli";
	string remote_ip = argv[1];
	unsigned short port = atoi(argv[2]);

	string args;
	if (argc == 5) {
		ostringstream ssargs;
		ssargs << argv[3] << " " << argv[4];
		args = ssargs.str();
	}

	srand(time(nullptr));
	signal(SIGPIPE, SIG_IGN); // THIS IS SUPER FUCKING IMPORTANT DONT REMOVE!!!!

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, remote_ip.c_str(), &addr.sin_addr) != 1) {
		throw std::runtime_error("Invalid server IP address");
	}
	addr.sin_port = htons(port);

	RuntimeSystem sys(root_dir);
	RuntimeEnvironment env(sys.root_dir());
	try {
		pid_t pid = sys.create_application(nullptr, env);
		Process *proc = sys.get_process(pid);
		Channel *io = proc->resman_.create_outbound_channel((sockaddr *)&addr, 5000);
		cout << "Connected to server!" << endl;
		proc->run(ClientShell, args, io);
	} catch(std::exception const& e) {
	    cout << "Exception: " << e.what() << endl;
	} catch(...) {
		cout << "wtf" << endl;
	}

	cout << "Leaving..." << endl;

	working = false;

	get_bar.open();
	get_bar.open();
	put_bar.open();
	put_bar.open();

	return 0;
}

int ClientShell(const std::string &args, Process &proc, Channel *io)
{
	istream *uin;
	ostream *uout;
	if (args.size() == 0) {
		cout.setf(std::ios::unitbuf);
		cin.setf(std::ios::unitbuf);

		uin = &cin;
		uout = &cout;
	} else {
		istringstream ssargs(args);
		string ifname, ofname;
		ssargs >> ifname >> ofname;

		ScopedPath sifname = proc.sys_.resolve(proc, ifname);
		uin = &proc.resman_.create_readfile_channel(sifname)->in();

		ScopedPath sofname = proc.sys_.resolve(proc, ofname);
		uout = &proc.resman_.create_writefile_channel(sofname)->out();
	}

	ExternalChannel ciso(io->in(), *uout);
	ExternalChannel cosi(*uin, io->out());

	// Launch the client daemons
	pid_t pid = proc.sys_.create_daemon(&proc, proc.env_);
	Process *child = proc.sys_.get_process(pid);
	child->run(ClientDaemon, "", &ciso);

	pid = proc.sys_.create_daemon(&proc, proc.env_);
	child = proc.sys_.get_process(pid);
	child->run(GetDaemon, "", nullptr);

	pid = proc.sys_.create_daemon(&proc, proc.env_);
	child = proc.sys_.get_process(pid);
	child->run(PutDaemon, "", nullptr);

	istream &in = cosi.in();
	ostream &out = cosi.out();

	string cmd;
	while ((in.peek() != EOF) && io->in()) {
		in >> cmd;
		string cmd_args = get_args(in);
		out << cmd << " " << cmd_args << endl; // send the command to the server
		if (cmd == "get") {
			get_cmd = cmd_args;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		} else if (cmd == "put") {
			put_cmd = cmd_args;
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	return 0;
}


int ClientDaemon(const std::string &args, Process &proc, Channel *io)
{
	char c;
	while (io->in().read(&c, 1)) {
		io->out().write(&c, 1);
		io->out().flush();
		get_sen.feed(c);
		put_sen.feed(c);
	}
	return 0;
}

int GetDaemon(const std::string &args, Process &proc, Channel *io)
{
	while (working) {
		get_bar.wait();
		string line = get_sen.get_line().str();
		get_bar.wait();
		if (!working) {
			break;
		}

		string tmp;
		istringstream iss(line);
		unsigned short port;
		size_t size;
		iss >> tmp >> tmp >> port >> tmp >> size; // get port: $port size: $size

		ostringstream oss;
		oss << get_cmd << " " << size << " " << port; // filename size port

		pid_t pid = proc.sys_.create_daemon(&proc, proc.env_);
		Process *child = proc.sys_.get_process(pid);
		child->run(ClientGet, oss.str(), nullptr);
	}
	return 0;
}

int PutDaemon(const std::string &args, Process &proc, Channel *io)
{
	while (working) {
		put_bar.wait();
		string line = put_sen.get_line().str();
		put_bar.wait();
		if (!working) {
			break;
		}

		string tmp;
		istringstream iss(line);
		unsigned short port;
		iss >> tmp >> tmp >> port; // put port: $port

		ostringstream oss;
		oss << put_cmd << " " << port; // filename size port

		pid_t pid = proc.sys_.create_daemon(&proc, proc.env_);
		Process *child = proc.sys_.get_process(pid);
		child->run(ClientPut, oss.str(), nullptr);
	}
	return 0;
}

int ClientGet(const std::string &args, Process &proc, Channel *io)
{
	istringstream ssargs(args);
	string filename;
	size_t filesize;
	unsigned short port;
	ssargs >> filename >> filesize >> port;

	struct sockaddr_in naddr = addr;
	naddr.sin_port = htons(port);

	istream &in = proc.resman_.create_outbound_channel((sockaddr *)&naddr)->in();

	ScopedPath outfile = proc.sys_.resolve(proc, filename);
	ostream &out = proc.resman_.create_writefile_channel(outfile, ios::binary)->out();

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

int ClientPut(const std::string &args, Process &proc, Channel *io)
{
	istringstream ssargs(args);
	string filename;
	size_t filesize;
	unsigned short port;
	ssargs >> filename >> filesize >> port;

	struct sockaddr_in naddr = addr;
	naddr.sin_port = htons(port);

	ostream &out = proc.resman_.create_outbound_channel((sockaddr *)&naddr)->out();

	File<ScopedPath> infile(proc.sys_.resolve(proc, filename)); // sanitization step
	istream &in = proc.resman_.create_readfile_channel(infile, ios::binary)->in();

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