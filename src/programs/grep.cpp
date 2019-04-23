#include "programs.h"
#include "io/channel.h"
#include "core/process.h"
#include "core/runsys.h"
#include "regex/re.h"
#include <dirent.h>
#include <string>
#include <vector>
#include <exception>
#include <iterator>
#include <sstream>

using namespace std;

static
void process(const ScopedPath *prefix, vector<ScopedPath *> &files, Process &proc, DIR *dirp, vector<ScopedPath *> &dirstash)
{
	struct dirent *ent;
	while ((ent = readdir(dirp)) != nullptr) {
		if (ent->d_type == DT_DIR) {
			if(ent->d_name[0] == '.') {
				continue;
			}
			dirstash.push_back(proc.sys_.resolve(*prefix, ent->d_name));
		} else if (ent->d_type == DT_REG) {
			files.push_back(proc.sys_.resolve(*prefix, ent->d_name));
		}
	}
}

static
void walk(const string &prefix, vector<ScopedPath *> &files, Process &proc)
{
	vector<ScopedPath *> dirstash;
	dirstash.push_back(proc.sys_.resolve(proc, prefix));
	DIR *dirp;

	while (dirstash.size() != 0) {
		ScopedPath *current = dirstash.back();
		dirstash.pop_back();
		dirp = opendir(static_cast<const string &>(*current).c_str());
		process(current, files, proc, dirp, dirstash);
		closedir(dirp);
		delete current;
	}
}

int Grep(const string &args, Process &proc, Channel *io)
{
	ostream &out = io->out();
	const string &pattern = args;

	if (pattern.size() == 0) {
		throw std::runtime_error("Regex pattern not specified.");
	}

	const char *tmp = pattern.c_str();
	size_t len = pattern.size();
	int err = 0;
	RegularExpression *regexp = parse(&tmp, &len, 0, &err);
	if (err != 0) {
		throw std::runtime_error("Cannot parse regex pattern.");
	}
	SearchExpression se(regexp);

	vector<ScopedPath *> files;
	walk(".", files, proc);

	for (auto it = files.cbegin(); it != files.cend(); ++it) {
		ScopedPath *spath = *it;
		ScopedPath current(*spath);
		delete spath;

		Channel *fileio = proc.resman_.create_readfile_channel(current);
		istream &file = fileio->in();
		ostringstream sscontents;
		copy(istreambuf_iterator<char>(file),
			istreambuf_iterator<char>(),
			ostreambuf_iterator<char>(sscontents));

		string contents = sscontents.str();
		auto result = se.search(contents.c_str(), contents.size());
		if (std::get<1>(result) >= 0) {
			out << current.stripped() << endl;
		}
	}

	return 0;
}