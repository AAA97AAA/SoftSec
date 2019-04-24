#include "programs/programs.h"

#include "io/socket.h"
#include "core/runsys.h"
#include "core/runenv.h"
#include "core/process.h"

#include <netinet/in.h>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <signal.h>

#include <vector>

using namespace std;

RuntimeSystem *gsys;

void signal_handler(int s)
{
	gsys->shutdown();
}

int main()
{
	vector<string> creds = {
		"admin admin",
		"user pass",
	};
	string root_dir = "root";
	unsigned short port = 55365;

	srand(time(nullptr));
	signal(SIGPIPE, SIG_IGN); // THIS IS SUPER FUCKING IMPORTANT DONT REMOVE!!!!

	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = signal_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	struct sockaddr_in addr;

	bzero((char *) &addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0; //0x0100007F; // 127.0.0.1
	addr.sin_port = htons(port);

	ListeningSocket sock((sockaddr *)&addr, -1);

	RuntimeSystem sys(root_dir); gsys = &sys;
	sys.load_credentials(creds);
	RuntimeEnvironment env(sys.root_dir());
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
