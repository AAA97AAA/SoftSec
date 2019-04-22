#include "fileman.h"
#include "path.h"
#include "process.h"
#include <string>
#include <cstring>
#include <stdexcept>
#include <sys/stat.h>
#include <stdio.h>

using namespace std;

FileManager::FileManager(const string &root_dir) :
	root_dir_(ScopedPath("/", root_dir, root_dir)),
	root_perms_(root_dir_.permissions())
{
}

// string FileManager::strip_scope(const ScopedPath &path) const
// {
// 	size_t scope_len = root_dir_.absolute_path().size();
// 	string stripped = path.absolute_path().substr(scope_len);
// 	if (stripped.size() == 0) {
// 		stripped = "/";
// 	}
// 	return stripped;
// }

const Dir<ScopedPath> & FileManager::root_dir() const
{
	return root_dir_;
}

ScopedPath * FileManager::resolve(const Process &proc, const std::string &path)
{
	if (path.size() == 0) {
		return resolve(proc.env_.get_wd(), ".");
	} else if (path[0] == '/') {
		return resolve(root_dir_, path);
	} else {
		return resolve(proc.env_.get_wd(), path);
	}
}

ScopedPath * FileManager::resolve(const Dir<ScopedPath> &prefix, const std::string &path)
{
	return new ScopedPath(path, prefix, root_dir_);
}

void FileManager::make_dir(const Process &proc, const std::string &path)
{
	ScopedPath *resolved = resolve(proc, path);
	const string &rstr = static_cast<const string &>(*resolved);
	int result = mkdir(rstr.c_str(), root_perms_);
	delete resolved;

	if (result < 0) {
        throw std::runtime_error(strerror(errno));
	}
}

void FileManager::remove_path(const Process &proc, const std::string &path)
{
	ScopedPath *resolved = resolve(proc, path);
	const string &rstr = static_cast<const string &>(*resolved);
	int result;
	if (rstr == static_cast<const string &>(root_dir_)) {
		result = -2;
	} else {
		result = remove(rstr.c_str());
	}
	delete resolved;

	if (result == -1) {
        throw std::runtime_error(strerror(errno));
	} else if (result == -2) {
		throw std::runtime_error("Cannot delete root directory.");
	}
}

