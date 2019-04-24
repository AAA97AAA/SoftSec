#include "programs/programs.h"

#include "io/socket.h"
#include "core/runsys.h"
#include "core/runenv.h"
#include "core/process.h"

#include "configs/loadconfig.h"

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
	ConfigManager conf("grass.conf");
	conf.loadConfigFile();
	vector<string> port_params = conf.getParams("port");
	unsigned short port = stoi(port_params[0]);
	vector<string> user_params = conf.getParams("user");
	vector<string> basedir_params = conf.getParams("base");
	vector<string> creds;
	for (auto i=0UL;i<user_params.size();i++) {
		creds.push_back(user_params[i]);
	}
	string root_dir = basedir_params[0];

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
