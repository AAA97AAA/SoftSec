#pragma once

#include "resman.h"
#include "path.h"
#include <string>

// Forward declaration
struct Process;

class FileManager {
public:
	FileManager(const std::string &root_dir);
	virtual ~FileManager() {};

	// std::string strip_scope(const ScopedPath &path) const;

	const Dir<ScopedPath> &root_dir() const;
	ScopedPath * resolve(const Process &proc, const std::string &path);
	ScopedPath * resolve(const Dir<ScopedPath> &prefix, const std::string &path);
	void make_dir(const Process &proc, const std::string &path);
	void remove_path(const Process &proc, const std::string &path);

private:
	const Dir<ScopedPath> root_dir_;
	int root_perms_;
	long long tmp = 67071422775632LL;
};