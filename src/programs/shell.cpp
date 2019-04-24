#include "programs.h"
#include "core/process.h"
#include "io/channel.h"
#include "core/runsys.h"
#include <iostream>
#include <string>
#include <exception>
#include <sstream>
#include <unordered_map>

using namespace std;

static unordered_map<string, Program *> programs
{
	{"ping", Ping},
	{"whoami", WhoAmI},
	{"w", Who},
	{"mkdir", MakeDir},
	{"rm", Remove},
	{"get", GetFile},
	{"put", PutFile},
	{"grep", Grep},
};

static void ShellExceptionHandler(Process &proc, exception_ptr ex, void *arg)
{
	Channel *io = reinterpret_cast<Channel *>(arg);
	try {
		rethrow_exception(ex);
	} catch (const std::exception& e) {
		try {
			io->out() << "Error: " << e.what() << endl;
		} catch (...) {
			cout << "Fatal error: " << e.what() << endl;
		}
	}
}

static int PrivilegedShell(const string &args, Process &proc, Channel *io);

string get_args(istream &in)
{
	string args;
	string line;
	getline(in, line);
	istringstream liness(line);
	getline(liness >> ws, args);
	return args;
}

static int launch_application(Process *parent, Program *p, const string &args, Channel *io)
{
	pid_t pid = parent->sys_.create_application(parent, parent->env_);
	Process *child = parent->sys_.get_process(pid);
	return child->run(p, args, io);
}

int LoginShell(const string &args, Process &proc, Channel *io)
{
	proc.set_ex_handler(&ShellExceptionHandler, io);

	istream &in = io->in();
	ostream &out = io->out();

	string tmp;
	out << "Welcome to GraaS" << endl;	
	out << "> " << flush;
	while (in >> tmp) {
		if (tmp == "login") {
			string username = get_args(in);

			out << "> " << flush;
			in >> tmp;
			if (tmp == "pass") {
				string password = get_args(in);

				if (proc.sys_.verify_credentials(username, password)) {
					pid_t pid = proc.sys_.create_application(&proc, proc.env_);
					Process *child = proc.sys_.get_process(pid);
					child->env_["USER"] = username;
					if (child->run(PrivilegedShell, "", io) == -1) {
						break; // exit when priv. shell exits
					}
				} else {
					out << "Error: invalid login credentials." << endl;
				}
			} else {
				out << "Error: Illegal command during login." << endl;
			}
		} else if (tmp == "ping") {
			string child_args = get_args(in);
			launch_application(&proc, Ping, child_args, io);
		} else if (tmp == "exit") {
			break;
		}
		out << "> " << flush;
	}

	out << "Goodbye!" << endl;
	return 0;
}

static int PrivilegedShell(const string &args, Process &proc, Channel *io)
{
	istream &in = io->in();
	ostream &out = io->out();

	string user = proc.env_["USER"];

	out << "Welcome, " << user << "!" << endl;
	out << user << "@grass:" << proc.env_.get_wd().stripped() << "$ " << flush;

	string cmd;
	while (in >> cmd) {
		string cmd_args = get_args(in);
		if (cmd == "exit") {
			return -1;
		} else if (cmd == "logout") {
			return 0;
		} else if (cmd == "cd") {
			try {
				if (cmd_args.size() == 0) {
					out << proc.env_.get_wd().stripped() << endl;
				} else {
					ScopedPath nwd = proc.sys_.resolve(proc, cmd_args);
					proc.env_.set_wd(nwd);
				}
			} catch (const std::exception &e) {
				ShellExceptionHandler(proc, current_exception(), io);
			}
		} else {
			auto it = programs.find(cmd);
			if (it != programs.end()) {
				launch_application(&proc, it->second, cmd_args, io);
			} else {
				out << "Unrecognized command \"" << cmd << "\"" << endl;
			}
		}
		out << user << "@grass:" << proc.env_.get_wd().stripped() << "$ " << flush;
	}

	return 0;
}