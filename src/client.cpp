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
#include <fstream>
#include <vector>
#include <thread>

using namespace std;

class sentinel {
public:
	explicit sentinel(const std::string &sentinel) :
		sentinel_(sentinel),
		bar_(2)
	{
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

	string readline()
	{
		bar_.wait();
		string line = line_.str();
		bar_.wait();
		return line;
	}

	void stop()
	{
		for (int i = 0; i < 8; ++i) {
			bar_.open();
		}
	}

private:
	std::string sentinel_;
	std::string view_;
	std::ostringstream line_;
	Barrier bar_;
};

struct sockaddr_in addr;
sentinel get_sentinel("get port:");
sentinel put_sentinel("put port:");
bool working = true;
string get_cmd, put_cmd;

void receiver(Channel *io);
void get_listener();
void put_listener();
void get_execute(string filename, size_t size, unsigned short port);
void put_execute(string filename, size_t size, unsigned short port);

int main(int argc, const char *argv[])
{
	if (argc < 3 || argc > 5) {
		throw std::runtime_error("Invalid number of arguments");
	}

	string remote_ip = argv[1];
	unsigned short port = atoi(argv[2]);

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	if (inet_pton(AF_INET, remote_ip.c_str(), &addr.sin_addr) != 1) {
		throw std::runtime_error("Invalid server IP address");
	}
	addr.sin_port = htons(port);

	istream *uin = nullptr;
	ostream *uout = nullptr;
	if (argc == 3) {
		cout.setf(std::ios::unitbuf);
		cin.setf(std::ios::unitbuf);

		uin = &cin;
		uout = &cout;
	} else if (argc == 4) {
		uin = new ifstream(argv[3]);
		if (uin->fail()) {
			delete uin;
			throw std::runtime_error("Invalid input file");
		}
		uout = &cout;
	} else {
		uin = new ifstream(argv[3]);
		if (uin->fail()) {
			delete uin;
			throw std::runtime_error("Invalid input file");
		}
		uout = new ofstream(argv[4]);
	}

	OutboundNetworkChannel io((sockaddr *)&addr, 5000, -1);

	ExternalChannel ciso(io.in(), *uout);
	ExternalChannel cosi(*uin, io.out());

	// Start the receiving thread
	thread recv_thread(receiver, &ciso);

	istream &in = cosi.in();
	ostream &out = cosi.out();

	string cmd;
	while (cmd != "exit" && in.peek() != EOF) {
		in >> cmd;
		string cmd_args = get_args(in);
		if (cmd == "get") {
			get_cmd = cmd_args;

			istringstream iss(get_cmd);
			string filename, path;
			iss >> path;
			filename = basename(path.c_str());

			ostringstream oss;
			oss << filename;
			get_cmd = oss.str();

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		} else if (cmd == "put") {
			put_cmd = cmd_args;

			istringstream iss(cmd_args);
			size_t size;
			string filename, path;
			iss >> path >> size;
			filename = basename(path.c_str());

			ostringstream oss;
			oss << filename << " " << size;
			cmd_args = oss.str();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		out << cmd << " " << cmd_args << endl; // send the command to the server
		if (cmd == "get" || cmd == "put") {
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	working = false;
	// io.shutdown();
	recv_thread.join();

	if (argc > 3) delete uin;
	if (argc > 4) delete uout;
}

void receiver(Channel *io)
{
	thread lget_thread(get_listener);
	thread lput_thread(put_listener);

	try {
		char c;
		while (io->in().read(&c, 1)) {
			io->out().write(&c, 1);
			io->out().flush();
			get_sentinel.feed(c);
			put_sentinel.feed(c);
		}
	} catch (...) {}

	get_sentinel.stop();
	put_sentinel.stop();

	lget_thread.join();
	lput_thread.join();
}

void get_listener()
{
	vector<thread> get_exs;
	while (working) {
		string line = get_sentinel.readline();
		if (!working) {
			break;
		}

		string tmp;
		istringstream iss(line);
		unsigned short port;
		size_t size;
		string filename = get_cmd;
		iss >> tmp >> tmp >> port >> tmp >> size; // get port: $port size: $size

		get_exs.emplace_back(get_execute, filename, size, port);
	}

	for (auto it = get_exs.begin(); it != get_exs.end(); ++it) {
		(*it).join();
	}
}

void put_listener()
{
	vector<thread> put_exs;
	while (working) {
		string line = put_sentinel.readline();
		if (!working) {
			break;
		}

		string tmp;
		istringstream iss(line);
		unsigned short port;
		iss >> tmp >> tmp >> port; // put port: $port

		iss.str(put_cmd);
		iss.clear();
		size_t size;
		string filename;
		iss >> filename >> size;

		put_exs.emplace_back(put_execute, filename, size, port);
	}

	for (auto it = put_exs.begin(); it != put_exs.end(); ++it) {
		(*it).join();
	}
}

void get_execute(string filename, size_t size, unsigned short port)
{
	struct sockaddr_in naddr = addr;
	naddr.sin_port = htons(port);

	OutboundNetworkChannel io((sockaddr *)&naddr, 5000, 5000);

	istream &in = io.in();

	ofstream out(filename, ios::binary);

	size_t i = 0; 
	auto oit = ostreambuf_iterator<char>(out);
	for (
			auto iit = istreambuf_iterator<char>(in);
			i < size && iit != istreambuf_iterator<char>();
			++i, ++iit
		)
	{
		oit = *iit;
	}

	out.flush();
}

void put_execute(string filename, size_t size, unsigned short port)
{
	struct sockaddr_in naddr = addr;
	naddr.sin_port = htons(port);

	OutboundNetworkChannel io((sockaddr *)&naddr, 5000, 5000);

	ostream &out = io.out();

	ifstream in(filename, ios::binary);

	size_t i = 0; 
	auto oit = ostreambuf_iterator<char>(out);
	for (
			auto iit = istreambuf_iterator<char>(in);
			i < size && iit != istreambuf_iterator<char>();
			++i, ++iit
		)
	{
		oit = *iit;
	}

	out.flush();
}