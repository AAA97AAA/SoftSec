#pragma once

#include <string>
#include <sys/stat.h>

// Forward declaration
class FileManager;

class RealPath {
public:
	RealPath(const std::string &path);
	virtual ~RealPath() {};

	virtual operator const std::string&() const;
	__mode_t permissions() const;

protected:
	std::string real_path_;
	struct stat sb_;
};

class FilePath : public RealPath {
public:
	FilePath(const std::string &path);
};

class DirPath : public RealPath {
public:
	DirPath(const std::string &path);
};

class CanonicalPath {
public:
	CanonicalPath(const std::string &path, const DirPath &prefix);
	virtual ~CanonicalPath() {};

	virtual operator const std::string&() const;

protected:
	virtual void canonicalize();

protected:
	std::string canonical_path_;
};

class AbsolutePath : public RealPath, public CanonicalPath {
public:
	AbsolutePath(const std::string &path, const DirPath &prefix);

	using CanonicalPath::operator const std::string&;

protected:
	virtual void canonicalize();
};

class ScopedPath : public CanonicalPath {
public:
	ScopedPath(const std::string &path, const DirPath &prefix, const DirPath &scope);

protected:
	virtual void canonicalize();

private:
	bool is_scoped(const std::string &path) const;

protected:
	const CanonicalPath scope_;
};