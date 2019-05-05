#include "procman.h"
#include "process.h"
#include "barrier.h"
#include "runsys.h"
#include <stdexcept>
#include <mutex>
#include <algorithm>

using namespace std;

ProcessManager::ProcessManager(size_t size)
:	procs_(size), rc_(1), working(true)
{
	pthread_create(&watch_thread_, nullptr, &ProcessManager::watch_helper, this);
}

ProcessManager::~ProcessManager()
{
	shutdown();

	for (auto it = procs_.begin(); it != procs_.end(); ++it) {
		delete it->second;
	}
}

void ProcessManager::shutdown()
{
	working = false;
	signal_exit(-1); // hacky fix to terminate watch thread
	void *ret;
	pthread_join(watch_thread_, &ret);
}

std::unordered_map<pid_t, Process *>::iterator ProcessManager::process_helper(pid_t pid)
{
	auto it = procs_.find(pid);
	if (it == procs_.end()) {
		throw std::runtime_error("Invalid pid.");
	}

	return it;
}

void ProcessManager::watch()
{
	while (working) {
		pid_t pid = wait_exit_any(); // wait until a process exits
		if (pid < 0) {
			continue;
		}

		Process *proc = get_process(pid);
		proc->bar_->wait(); // wait until all listeners are notified of exit (e.g: application)
		if (proc->ex_) {
			proc->ex_handler(*proc, proc->ex_, proc->ex_args);
		}
		proc->bar_->wait(); // wait until exception is handled and listeners receive the exit code

		void *ret;
		pthread_join(proc->main_thread_, &ret);

		// TODO kill child processes? nah
		
		kill_process(pid); // destroys resources and removes the pid
	}
}

void * ProcessManager::watch_helper(void *ctx)
{
	((ProcessManager *)ctx)->watch();
	return nullptr;
}

template <class T>
pid_t ProcessManager::create_process(Process *parent, const RuntimeEnvironment &env)
{
	lock_guard<mutex> lk(mx_procs_);

	pid_t pid = rc_++;
	T *proc = new T(pid, parent, env, dynamic_cast<RuntimeSystem&>(*this));

	procs_[pid] = dynamic_cast<Process *>(proc);

	return pid;
}

pid_t ProcessManager::create_application(Process *parent, const RuntimeEnvironment &env)
{
	pid_t pid = create_process<Application>(parent, env);
	Process *proc = get_process(pid);
	proc->bar_ = new Barrier(2); // both the runtime system and the parent wait
	return pid;
}

pid_t ProcessManager::create_daemon(Process *parent, const RuntimeEnvironment &env)
{
	pid_t pid = create_process<Daemon>(parent, env);
	Process *proc = get_process(pid);
	proc->bar_ = new Barrier(1); // only the runtime system waits
	return pid;
}

Process * ProcessManager::get_process(pid_t pid)
{
	lock_guard<mutex> lk(mx_procs_);

	auto it = process_helper(pid);
	return it->second;
}

pid_t ProcessManager::get_pid(Process *proc)
{
	auto it = find_if(std::begin(procs_), std::end(procs_),
                           [proc](std::pair<const int, Process*> &p)
                           {return p.second == proc;});
	if (it == std::end(procs_)) {
		return 0;
	} else {
		return it->first;
	}
}

void ProcessManager::kill_process(pid_t pid)
{
	lock_guard<mutex> lk(mx_procs_);

	auto it = process_helper(pid);
	delete it->second; // destroys the process and its allocated resources
	procs_.erase(it);
}

std::unordered_map<pid_t, Process *>::const_iterator ProcessManager::proc_cbegin() const
{
	return procs_.cbegin();
}

std::unordered_map<pid_t, Process *>::const_iterator ProcessManager::proc_cend() const
{
	return procs_.cend();
}

void ProcessManager::signal_exit(pid_t pid)
{
	sigs_.push(pid);
}

int ProcessManager::wait_exit(pid_t pid)
{
	Process *proc = get_process(pid);
	proc->bar_->wait();
	int exit_code = proc->ecode_;
	proc->bar_->wait();
	return exit_code;
}

pid_t ProcessManager::wait_exit_any()
{
	return sigs_.pop();
}

// Compiling the template for specific classes
template pid_t ProcessManager::create_process<Application>(Process *parent, const RuntimeEnvironment &env);
template pid_t ProcessManager::create_process<Daemon>(Process *parent, const RuntimeEnvironment &env);