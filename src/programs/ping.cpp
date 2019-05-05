#include "programs.h"
#include "core/process.h"
#include "io/channel.h"
#include "core/runsys.h"
#include <iostream>
#include <string>
#include <exception>
#include <sstream> // to split
#include <cstring>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <sys/wait.h>
#include <unistd.h>
#include <iterator>

using namespace std;

int Ping(const string &args, Process &proc, Channel *io)
{

	struct {
		ostream &out;
		istringstream iss;
		vector<string> splitArgs;
		string host;
		pid_t pid;
		int pipefd[2];
		FILE* output;
		char line[256];
	} locals {.out = io->out(), .iss = istringstream(args), .pid = 0};

	locals.splitArgs.insert(locals.splitArgs.begin(), istream_iterator<string>(locals.iss), istream_iterator<string>());
	locals.host = locals.splitArgs[0];
	
	if(locals.splitArgs.size() < 1) {
		throw std::runtime_error("ping: Wrong usage");
	} else {
		if(pipe(locals.pipefd) != 0) {
			throw std::runtime_error(strerror(errno));
		}

		locals.pid = fork();
		if (locals.pid == 0) {
			close(locals.pipefd[0]);
			dup2(locals.pipefd[1], STDOUT_FILENO);
			dup2(locals.pipefd[1], STDERR_FILENO);
			execl("/bin/ping", "/bin/ping", "-c1", locals.host.c_str(), (char*) NULL);
			exit(0);
		} else if (locals.pid > 0) {
			close(locals.pipefd[1]);
			locals.output = fdopen(locals.pipefd[0], "r");

			while(fgets(locals.line, sizeof(locals.line), locals.output)) {
				locals.out << locals.line;
			}

			waitpid(locals.pid, nullptr, 0);
		}
	}
	return 0;
}