#pragma once

#include <unordered_set>
#include <string>

class AuthManager {
public:
	virtual ~AuthManager() {};

	void populate_users(std::unordered_set<std::string> &users);
};