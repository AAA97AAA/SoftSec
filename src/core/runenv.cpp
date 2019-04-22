#include "runenv.h"
#include "path.h"
#include <string>

using std::string;

RuntimeEnvironment::RuntimeEnvironment(const DirPath &wd)
:	wd_(wd)
{
}

RuntimeEnvironment::RuntimeEnvironment(const RuntimeEnvironment &env)
:	wd_(env.wd_),
	vars_(env.vars_)
{
}

const DirPath & RuntimeEnvironment::get_wd() const
{
	return wd_;
}

void RuntimeEnvironment::set_wd(const DirPath &wd)
{
	wd_ = wd;
}

bool RuntimeEnvironment::defined(const std::string &name) const
{
	auto it = vars_.find(name);
	return (it != vars_.end());
}

std::string & RuntimeEnvironment::operator[](const std::string &name)
{
	return vars_[name];
}