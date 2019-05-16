#include "programs.h"
#include "core/process.h"
#include "io/channel.h"
#include "core/runsys.h"
#include <iostream>
#include <string>
#include <exception>
#include <cstring>

#include <unistd.h>

using namespace std;

int Date(const string &args, Process &proc, Channel *io) {
	int rpipe[2];
	ostream &out = io->out();

	if(pipe(rpipe) != 0) {
		throw std::runtime_error(strerror(errno));
	}

	pid_t date_pid = fork();
	if (date_pid == 0) {
		close(rpipe[0]);
		dup2(rpipe[1], STDOUT_FILENO);
		execl("/bin/date", "/bin/date", (char*) NULL);
		close(rpipe[1]);
		exit(0);
	} else if (date_pid > 0) {
		char ch;
		string date_out = "";
		close(rpipe[1]);

		while (read(rpipe[0],&ch,1) > 0) {
			if (ch != '\0') {
				string tmp_s(1, ch);
				date_out.append(tmp_s);
			}
		}

		out << date_out;
		date_out.clear();
		close(rpipe[0]);
	}
	return 0;
}
