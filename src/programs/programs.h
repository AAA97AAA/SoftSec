#pragma once

#include <string>
#include <iostream>

// Forward declaration
struct Process;
class Channel;

std::string get_args(std::istream &in);

int LoginShell(const std::string &args, Process &proc, Channel *io);
int Ping(const std::string &args, Process &proc, Channel *io);
int Who(const std::string &args, Process &proc, Channel *io);
int WhoAmI(const std::string &args, Process &proc, Channel *io);
int MakeDir(const std::string &args, Process &proc, Channel *io);
int Remove(const std::string &args, Process &proc, Channel *io);
int GetFile(const std::string &args, Process &proc, Channel *io);
int PutFile(const std::string &args, Process &proc, Channel *io);
int Grep(const std::string &args, Process &proc, Channel *io);
int Date(const std::string &args, Process &proc, Channel *io);
int Ls(const std::string &args, Process &proc, Channel *io);
