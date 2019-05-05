#pragma once

#include "messagebox.h"
#include <unordered_map>
#include <string>
#include <mutex>
#include <exception>

// Forward declaration
struct Process;
class RuntimeSystem;
class Channel;
class RuntimeEnvironment;

class ProcessManager {
public:
	ProcessManager(size_t size = 8);
	virtual ~ProcessManager();
	void shutdown();

	pid_t create_application(Process *parent, const RuntimeEnvironment &env);
	pid_t create_daemon(Process *parent, const RuntimeEnvironment &env);

	Process *get_process(pid_t pid);
	pid_t get_pid(Process *proc);
	void kill_process(pid_t pid);

	std::unordered_map<pid_t, Process *>::const_iterator proc_cbegin() const;
	std::unordered_map<pid_t, Process *>::const_iterator proc_cend() const;

	void signal_exit(pid_t pid);
	int wait_exit(pid_t pid);
	pid_t wait_exit_any();

protected:
	template <class T>
	pid_t create_process(Process *parent, const RuntimeEnvironment &env);

private:
	std::unordered_map<pid_t, Process *>::iterator process_helper(pid_t pid);
	void watch();
	static void * watch_helper(void *ctx);

private:
	pthread_t watch_thread_;
	std::unordered_map<pid_t, Process *> procs_;
	std::mutex mx_procs_;
	MessageBox<pid_t> sigs_;
	int rc_;
	bool working;
};