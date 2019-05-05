#include "programs.h"
#include "core/process.h"
#include "io/channel.h"
#include "core/runsys.h"
#include <iostream>
#include <string>
#include <exception>

using namespace std;

int MakeDir(const string &args, Process &proc, Channel *io)
{
	if (args.size() == 0) {
		throw std::runtime_error("Missing argument: directory name.");
	}

	proc.sys_.make_dir(proc, args);

	return 0;
}

int Remove(const string &args, Process &proc, Channel *io)
{
	if (args.size() == 0) {
		throw std::runtime_error("Missing argument: path name.");
	}

	proc.sys_.remove_path(proc, args);

	return 0;
}