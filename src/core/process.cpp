#include "process.h"
#include "program.h"
#include "runsys.h"
#include <exception>
#include <pthread.h>

using namespace std;

void default_ex_handler(Process &proc, exception_ptr ex, void *arg)
{
	if (proc.parent_ == nullptr || proc.parent_->ex_handler == nullptr) {
		return;
	}

	proc.parent_->ex_handler(*proc.parent_, ex, proc.parent_->ex_args);
}

struct process_start_info {
	Process *proc_;
	Program *p_;
	const string &args_;
	Channel *io_;

	process_start_info(Process *proc, Program *p, const string &args, Channel *io)
	:	proc_(proc), p_(p), args_(args), io_(io) {}
};

Process::Process(pid_t pid, Process *parent, const RuntimeEnvironment &env, RuntimeSystem &sys):
	pid_(pid),
	env_(env),
	resman_(*this),
	sys_(sys),
	parent_(parent),
	ex_handler(default_ex_handler),
	ex_args(nullptr)
{
}

void Process::set_ex_handler(ex_handler_fn fn, void *args)
{
	ex_handler = fn;
	ex_args = args;
}

void Process::exec(Program *p, const string &args, Channel *io)
{
	ecode_ = -256;
	try {
		ecode_ = (*p)(args, *this, io);
		ex_ = nullptr;
	} catch (exception const& e) {
		// TODO io->out() << "Exception: " << e.what() << endl;
		ex_ = current_exception(); // TODO look into std::future?
	}
	sys_.signal_exit(pid_);
}

void * Process::exec_helper(void *args)
{
	process_start_info *psinfo = static_cast<process_start_info *>(args);
	psinfo->proc_->exec(psinfo->p_, psinfo->args_, psinfo->io_);
	delete psinfo;
	return nullptr; // TODO anything useful to return?
}

int Application::run(Program *p, const std::string &args, Channel *io)
{
	process_start_info *psinfo = new process_start_info(this, p, args, io);
	pthread_create(&main_thread_, nullptr, exec_helper, psinfo);
	return sys_.wait_exit(pid_);
}

int Daemon::run(Program *p, const std::string &args, Channel *io)
{
	process_start_info *psinfo = new process_start_info(this, p, args, io);
	pthread_create(&main_thread_, nullptr, exec_helper, psinfo);
	return 0; // exit code not readily available
}