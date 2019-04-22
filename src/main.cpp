#include "channel.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <istream>
#include <ostream>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <signal.h>

#include "programs/programs.h"

#include "core/runsys.h"
#include "core/path.h"
#include "core/runenv.h"
#include "core/process.h"

using namespace std;

int main()
{
	srand(time(nullptr));
	signal(SIGPIPE, SIG_IGN); // THIS IS SUPER FUCKING IMPORTANT DONT REMOVE!!!!

	struct sockaddr_in addr;

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0; //0x0100007F; // 127.0.0.1
	addr.sin_port = htons(55365);

	ListeningSocket sock((sockaddr *)&addr, -1);

	RuntimeSystem sys("/home/cysto/cs412/project/project-code/build/root");
	DirPath workdir("/home/cysto/cs412/project/project-code/build/root");
	RuntimeEnvironment env(workdir);
	try {
		while (true) {
			cout << "Waiting for client" << endl;
			pid_t pid = sys.create_daemon(nullptr, env);
			Process *proc = sys.get_process(pid);
			Channel *io = proc->resman_.create_inbound_channel(&sock);
			proc->run(LoginShell, "", io);
			cout << "Launched shell daemon!" << endl;
		}
	} catch(std::exception const& e) {
	    cout << "Exception: " << e.what() << endl;
	} catch(...) {
		cout << "wtf" << endl;
	}

	cout << "Leaving..." << endl;

	return 0;
}
