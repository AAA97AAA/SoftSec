#pragma once

#include "procman.h"
#include "fileman.h"
#include "authman.h"
#include <pthread.h>
#include <string>

class RuntimeSystem : 
	public ProcessManager,
	public FileManager,
	public AuthManager
{
public:
	RuntimeSystem(const std::string &root_dir);
};