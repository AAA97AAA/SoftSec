#pragma once

#include "path.h"
#include <string>
#include <unordered_map>

class RuntimeEnvironment {
public:
	RuntimeEnvironment(const DirPath &wd);
	RuntimeEnvironment(const RuntimeEnvironment &env);

	const DirPath & get_wd() const;
	void set_wd(const DirPath &wd);

	bool defined(const std::string &name) const;
	std::string & operator[](const std::string &name);

	// TODO add variables to get program name, etc...

private:
	DirPath wd_;
	std::unordered_map<std::string, std::string> vars_;
};