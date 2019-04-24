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

using namespace std;

int Ls(const string &args, Process &proc, Channel *io)
{
	ostream &out = io->out();

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

		execl("/bin/ls", "/bin/ls", "-l",  static_cast<const string &>(proc.env_.get_wd()).c_str(), (char*) NULL);
		exit(0);
	} else if (pid > 0) {
		close(pipefd[1]);
		output = fdopen(pipefd[0], "r");

		while(fgets(line, sizeof(line), output)) {
			out << line;
		}

		waitpid(pid, &status, 0);
	}
	return 0;
}