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
	ostream &out = io->out();
	istringstream iss(args);
	vector<string> splitArgs((istream_iterator<string>(iss)), istream_iterator<string>());

	if(splitArgs.size() < 1) {
		throw std::runtime_error("ping: Wrong usage");
	} else {
		string host = splitArgs[0];
		pid_t pid = 0;
		int pipefd[2];
		FILE* output;
		char line[256];
		int status;

		if(pipe(pipefd) != 0) {
			throw std::runtime_error(strerror(errno));
		}

		pid = fork();
		if (pid == 0) {
			close(pipefd[0]);
			dup2(pipefd[1], STDOUT_FILENO);
			dup2(pipefd[1], STDERR_FILENO);
			execl("/bin/ping", "/bin/ping", "-c1", host.c_str(), (char*) NULL);
			exit(0);
		} else if (pid > 0) {
			close(pipefd[1]);
			output = fdopen(pipefd[0], "r");

			while(fgets(line, sizeof(line), output)) {
				out << line;
			}

			waitpid(pid, &status, 0);
		}
	}
	return 0;
}