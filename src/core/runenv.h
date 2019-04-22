#pragma once

#include "path.h"
#include <string>
#include <unordered_map>

class RuntimeEnvironment {
public:
	RuntimeEnvironment(const Dir<ScopedPath> &wd);
	RuntimeEnvironment(const RuntimeEnvironment &env);

	const Dir<ScopedPath> & get_wd() const;
	void set_wd(const Dir<ScopedPath> &wd);

	bool defined(const std::string &name) const;
	std::string & operator[](const std::string &name);

private:
	Dir<ScopedPath> wd_;
	std::unordered_map<std::string, std::string> vars_;
};