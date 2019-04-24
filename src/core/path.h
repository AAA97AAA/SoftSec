#pragma once

#include <string>
#include <sys/stat.h>

// Forward declaration
class FileManager;

class RealPath {
public:
	RealPath(const std::string &path);
	virtual ~RealPath() {};

	explicit operator const std::string&() const;
	mode_t permissions() const;
	off_t size() const;

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

	explicit operator const std::string&() const;

protected:
	virtual void canonicalize();

protected:
	std::string canonical_path_;
};

class AbsolutePath : public RealPath, public CanonicalPath {
public:
	AbsolutePath(const std::string &path, const DirPath &prefix);
	virtual ~AbsolutePath() {};

	using CanonicalPath::operator const std::string&;

protected:
	virtual void canonicalize();
};

class ScopedPath : public CanonicalPath {
public:
	ScopedPath(const std::string &path, const DirPath &prefix, const DirPath &scope, std::size_t max_len = 128);
	virtual ~ScopedPath() {};

	std::string stripped() const;

protected:
	virtual void canonicalize();

private:
	bool is_scoped(const std::string &path) const;

protected:
	CanonicalPath scope_;
};

template <class T>
class Dir : public T, public DirPath {
public:
	Dir(const T& path);
	using CanonicalPath::operator const std::string&;
};

template <class T>
class File : public T, public FilePath {
public:
	File(const T& path);
	using CanonicalPath::operator const std::string&;
};