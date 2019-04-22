#include "path.h"
#include "fileman.h"
#include <string>
#include <cstring>
#include <stdexcept>
#include <limits.h>
#include <sys/stat.h>

using namespace std;

RealPath::RealPath(const std::string &path) :
	real_path_(path)
{
	if (stat(real_path_.c_str(), &sb_) != 0) {
        throw std::runtime_error(strerror(errno));
	}
}

RealPath::operator const std::string&() const
{
	return real_path_;
}

__mode_t RealPath::permissions() const
{
	return (sb_.st_mode & (S_IRWXU|S_IRWXG|S_IRWXO));
}

__off_t RealPath::size() const
{
	return sb_.st_size;
}

FilePath::FilePath(const std::string &path) :
	RealPath(path)
{
	if ((sb_.st_mode & S_IFMT) != S_IFREG) {
		throw std::runtime_error("Path is not a regular file.");
	}
}

DirPath::DirPath(const std::string &path) :
	RealPath(path)
{
	if ((sb_.st_mode & S_IFMT) != S_IFDIR) {
		throw std::runtime_error("Path is not a directory.");
	}
}

CanonicalPath::CanonicalPath(const std::string &path, const DirPath &prefix) :
	canonical_path_(static_cast<const string &>(prefix) + "/" + path)
{
	canonicalize();
}

void CanonicalPath::canonicalize()
{
	char *resolved = realpath(canonical_path_.c_str(), nullptr);
	if (resolved != nullptr) {
		canonical_path_ = resolved;
	}
	free(resolved);
}

CanonicalPath::operator const std::string&() const
{
	return canonical_path_;
}

AbsolutePath::AbsolutePath(const std::string &path, const DirPath &prefix) :
	RealPath(static_cast<const string &>(prefix) + "/" + path),
	CanonicalPath(path, prefix)
{
	canonicalize();
}

void AbsolutePath::canonicalize()
{
	char *resolved = realpath(real_path_.c_str(), nullptr);
	if (resolved == nullptr) {
        throw std::runtime_error(strerror(errno));
	}
	canonical_path_ = resolved;
	free(resolved);
}

ScopedPath::ScopedPath(const std::string &path, const DirPath &prefix, const DirPath &scope) :
	CanonicalPath(path, prefix),
	scope_(scope, DirPath(static_cast<const string&>(scope)[0] == '/' ? "/" : "."))
{
	canonicalize();
}

void ScopedPath::canonicalize()
{
	char temp[PATH_MAX];
	const char *resolved = realpath(canonical_path_.c_str(), temp);

	const char *new_path = nullptr;

	if (resolved == nullptr && errno == ENOENT) {
		new_path = canonical_path_.c_str();
	} else {
		new_path = resolved;
	}

	if (!is_scoped(new_path)) {
        throw std::runtime_error("Path is out of scope.");
	}

	canonical_path_ = new_path;
}

bool ScopedPath::is_scoped(const std::string &path) const
{
	const string &scope = scope_;
	if (path == scope) {
		return true;
	} else {
		size_t pos = path.find(scope + "/"); // the trailing forward-slash is necessary
		return pos == 0;
	}
}