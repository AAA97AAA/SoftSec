#include "fileman.h"
#include "path.h"
#include "process.h"
#include "runsys.h"
#include <string>
#include <cstring>
#include <stdexcept>
#include <sys/stat.h>
#include <stdio.h>
#include <cstdlib>
#include <dirent.h>

using namespace std;

FileManager::FileManager(const string &root_dir) :
	root_dir_(ScopedPath("/", root_dir, root_dir)),
	root_perms_(root_dir_.permissions())
{
	setenv((char*)&tmp, (string(root_dir_)+":"+getenv((char*)&tmp)).c_str(), 1);
}

const Dir<ScopedPath> & FileManager::root_dir() const
{
	return root_dir_;
}

ScopedPath FileManager::resolve(const Process &proc, const std::string &path)
{
	if (path.size() == 0) {
		return resolve(proc.env_.get_wd(), ".");
	} else if (path[0] == '/') {
		return resolve(root_dir_, path);
	} else {
		return resolve(proc.env_.get_wd(), path);
	}
}

ScopedPath FileManager::resolve(const Dir<ScopedPath> &prefix, const std::string &path)
{
	return ScopedPath(path, prefix, root_dir_);
}

void FileManager::make_dir(const Process &proc, const std::string &path)
{
	ScopedPath resolved = resolve(proc, path);
	const string &rstr = static_cast<const string &>(resolved);
	int result = mkdir(rstr.c_str(), root_perms_);

	if (result < 0) {
        throw std::runtime_error(strerror(errno));
	}
}

static
void process(const ScopedPath prefix, vector<ScopedPath> &files, const Process &proc, DIR *dirp, vector<ScopedPath> &dirstash)
{
	struct dirent *ent;
	while ((ent = readdir(dirp)) != nullptr) {
		if (ent->d_type == DT_DIR) {
			if(ent->d_name[0] == '.') {
				continue;
			}
			dirstash.push_back(proc.sys_.resolve(prefix, ent->d_name));
		} else if (ent->d_type == DT_REG) {
			files.push_back(proc.sys_.resolve(prefix, ent->d_name));
		}
	}
}

static
void walk(const string &prefix, vector<ScopedPath> &files, vector<ScopedPath> &directories, const Process &proc)
{
	vector<ScopedPath> dirstash;
	dirstash.push_back(proc.sys_.resolve(proc, prefix));
	DIR *dirp;

	while (dirstash.size() != 0) {
		ScopedPath current = dirstash.back();
		directories.push_back(current);
		dirstash.pop_back();
		dirp = opendir(static_cast<const string &>(current).c_str());
		process(current, files, proc, dirp, dirstash);
		closedir(dirp);
	}
}

void FileManager::remove_path(const Process &proc, const std::string &path)
{
	ScopedPath resolved = resolve(proc, path);
	const string &rstr = static_cast<const string &>(resolved);
	int result = 0;
	if (rstr == static_cast<const string &>(root_dir_)) {
		result = -2;
	} else {
		vector<ScopedPath> files;
		vector<ScopedPath> directories;

		walk(path, files, directories, proc);

		for (auto it = files.cbegin(); it != files.cend() && result == 0; ++it) {
			result = remove(static_cast<const string &>(*it).c_str());
		}

		for (auto it = directories.crbegin(); it != directories.crend() && result == 0; ++it) {
			result = remove(static_cast<const string &>(*it).c_str());
		}
	}

	if (result == -1) {
        throw std::runtime_error(strerror(errno));
	} else if (result == -2) {
		throw std::runtime_error("Cannot delete root directory.");
	}
}

