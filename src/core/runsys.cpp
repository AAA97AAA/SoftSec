#include "runsys.h"
#include "process.h"
#include "barrier.h"
#include <pthread.h>

RuntimeSystem::RuntimeSystem(const std::string &root_dir):
	ProcessManager(),
	FileManager(root_dir),
	AuthManager()
{
}