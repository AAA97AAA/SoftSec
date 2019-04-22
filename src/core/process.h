#pragma once

#include "runenv.h"
#include "resman.h"
#include "program.h"
#include <string>
#include <exception>
#include <pthread.h>

// Forward declaration
class RuntimeSystem;
class Channel;
class Barrier;

typedef void (*ex_handler_fn)(Process &proc, std::exception_ptr ex, void *arg);

void default_ex_handler(std::exception_ptr ex, Process &proc, void *arg);

struct Process {
	pid_t pid_;
	std::exception_ptr ex_;
	pthread_t main_thread_;
	RuntimeEnvironment env_;
	ResourceManager resman_;
	RuntimeSystem &sys_;
	Barrier *bar_;

	Process *parent_;
	ex_handler_fn ex_handler;
	void *ex_args;

	int ecode_;

public:
	Process(pid_t pid, Process *parent, const RuntimeEnvironment &env, RuntimeSystem &sys);
	virtual ~Process() {};

	void set_ex_handler(ex_handler_fn fn, void *args);

	virtual int run(Program *p, const std::string &args, Channel *io) = 0;

protected:
	virtual void exec(Program *p, const std::string &args, Channel *io);
	static void * exec_helper(void *args);
};

class Application : public Process {
public:
	using Process::Process;

	int run(Program *p, const std::string &args, Channel *io);
};

class Daemon : public Process {
public:
	using Process::Process;

	int run(Program *p, const std::string &args, Channel *io);
};