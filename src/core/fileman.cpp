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
	root_dir_(root_dir),
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

ScopedPath * FileManager::resolve(const Process &proc, const std::string &path)
{
	if (path.size() == 0) {
		return new ScopedPath(".", proc.env_.get_wd(), root_dir_);
	} else if (path[0] == '/') {
		return new ScopedPath(path, root_dir_, root_dir_);
	} else {
		return new ScopedPath(path, proc.env_.get_wd(), root_dir_);
	}
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

