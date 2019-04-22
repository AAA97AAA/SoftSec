#pragma once

#include <string>

// Forward declaration
struct Process;
class Channel;

int LoginShell(const std::string &args, Process &proc, Channel *io);
int Ping(const std::string &args, Process &proc, Channel *io);
int Who(const std::string &args, Process &proc, Channel *io);
int WhoAmI(const std::string &args, Process &proc, Channel *io);
int MakeDir(const std::string &args, Process &proc, Channel *io);
int Remove(const std::string &args, Process &proc, Channel *io);