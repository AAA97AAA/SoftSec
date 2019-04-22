#pragma once

#include <mutex>
#include <condition_variable>

class Barrier {
public:
	explicit Barrier(std::size_t count);

	void wait();

private:
	std::mutex mx_;
    std::condition_variable cond_;
	std::size_t count_;
	std::size_t reset_;
	std::size_t round_;
};