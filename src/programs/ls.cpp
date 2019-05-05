#include "programs.h"
#include "core/process.h"
#include "io/channel.h"
#include "core/runsys.h"
#include <iostream>
#include <string>
#include <exception>
#include <cstring>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <array>
#include <fstream>

using namespace std;

std::string exec(const char * cmd) {
	std::array < char, 128 > buffer;
	std::string result;
	std::unique_ptr < FILE, decltype( & pclose) > pipe(popen(cmd, "r"), pclose);
	if (!pipe) {
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
		result += buffer.data();
	}
	return result;
}

int Ls(const string & args, Process & proc, Channel * io) {
	ostream & out = io->out();

	string path = static_cast<const string &>(proc.env_.get_wd()).c_str();
	string cmdstr = "/bin/ls -l " + path;

	const char * cmd = cmdstr.c_str();
	auto pipe = popen(cmd, "r");

	char c;
	while ((c = fgetc(pipe)) != EOF) {
		out << c;
	}

	pclose(pipe);

	return 0;
}

char *fgets(char *s, int size, FILE *stream)
{
	size_t i;
	int c = 0;
	for (i = 0; c != '\n' && (c = fgetc(stream)) != EOF; ++i) {
		s[i] = c;
	}
	s[i] = 0;
	if (c == EOF) {
		return nullptr;
	} else {
		return s;
	}
}
