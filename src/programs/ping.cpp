#include "programs.h"
#include "core/process.h"
#include "io/channel.h"
#include "core/runsys.h"
#include <iostream>
#include <string>
#include <exception>

using namespace std;

int Ping(const string &args, Process &proc, Channel *io)
{
	throw runtime_error("Test exception");
	return 0;
}