#include "programs.h"
#include "core/process.h"
#include "io/channel.h"
#include "core/runsys.h"
#include "core/authman.h"
#include <iostream>
#include <string>
#include <exception>
#include <unordered_set>

using namespace std;

int Who(const string &args, Process &proc, Channel *io)
{
	ostream &out = io->out();

	unordered_set<std::string> users;
	proc.sys_.populate_users(users);

	for (auto it = users.cbegin(); it != users.cend(); ++it) {
		out << *it << endl;
	}

	return 0;
}

int WhoAmI(const string &args, Process &proc, Channel *io)
{
	ostream &out = io->out();

	out << proc.env_["USER"] << endl;

	return 0;
}