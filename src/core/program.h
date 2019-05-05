#pragma once

#include <string>

// Forward declaration
class Channel;
struct Process;

typedef int (Program)(const std::string &args, Process &proc, Channel *io);