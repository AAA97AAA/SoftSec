#include "authman.h"
#include "runsys.h"
#include "process.h"
#include <unordered_set>
#include <string>
#include <sstream>

using namespace std;

void AuthManager::add_credentials(const std::string &username, const std::string &password)
{
	creds_[username] = password;
}

void AuthManager::load_credentials(const std::vector<std::string> &values)
{
	for (auto it = values.cbegin(); it != values.cend(); ++it) {
		string username, password;
		istringstream ss(*it);
		ss >> username >> password;
		add_credentials(username, password);
	}
}

bool AuthManager::verify_credentials(const std::string &username, const std::string &password) const
{
	auto it = creds_.find(username);
	if (it == creds_.end()) {
		return false;
	} else {
		return (it->second == password);
	}
}

void AuthManager::get_active_users(std::unordered_set<std::string> &out_users)
{
	RuntimeSystem &sys = dynamic_cast<RuntimeSystem &>(*this);

	for (auto it = sys.proc_cbegin(); it != sys.proc_cend(); ++it) {
		auto proc = it->second;
		if (proc->env_.defined("USER") && proc->env_["USER"] != "") {
			out_users.insert(proc->env_["USER"]);
		}
	}
}