#pragma once

#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>

class AuthManager {
public:
	virtual ~AuthManager() {};

	void add_credentials(const std::string &username, const std::string &password);
	void load_credentials(const std::vector<std::string> &values);
	bool verify_credentials(const std::string &username, const std::string &password) const;
	void get_active_users(std::unordered_set<std::string> &out_users);

private:
	std::unordered_map<std::string, std::string> creds_;
};