#include "authman.h"
#include "runsys.h"
#include "process.h"
#include <unordered_set>
#include <string>

using namespace std;

void AuthManager::populate_users(std::unordered_set<std::string> &users)
{
	RuntimeSystem &sys = dynamic_cast<RuntimeSystem &>(*this);

	for (auto it = sys.proc_cbegin(); it != sys.proc_cend(); ++it) {
		auto proc = it->second;
		if (proc->env_.defined("USER") && proc->env_["USER"] != "") {
			users.insert(proc->env_["USER"]);
		}
	}
}