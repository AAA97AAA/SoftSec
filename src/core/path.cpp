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
	scope_(static_cast<const std::string &>(scope), DirPath(static_cast<const string&>(scope)[0] == '/' ? "/" : "."))
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

std::string ScopedPath::stripped() const
{
	const string &strscope = static_cast<const std::string &>(scope_);
	if (strscope.size() > 1) {
		string s = static_cast<const string &>(*this).substr(strscope.size());
		if (s.size() == 0) {
			s = "/";
		}
		return s;
	} else {
		return static_cast<const string &>(*this);
	}
}

bool ScopedPath::is_scoped(const std::string &path) const
{
	const string &scope = static_cast<const std::string &>(scope_);
	if (path == scope || scope.size() == 1) { // '/' scope is always of size 1
		return true;
	} else {
		size_t pos = path.find(scope + "/"); // the trailing forward-slash is necessary
		return pos == 0;
	}
}

template <class T>
Dir<T>::Dir(const T& path) :
	T(path),
	DirPath(static_cast<const string&>(*this))
{
}

template <class T>
File<T>::File(const T& path) :
	T(path),
	FilePath(static_cast<const string&>(*this))
{
}

// Compiling the template for specific classes
template class Dir<ScopedPath>;
template class File<ScopedPath>;