#include "barrier.h"
#include <mutex>
#include <condition_variable>

using namespace std;

Barrier::Barrier(size_t count)
:	count_(count),
	reset_(count),
	round_(0)
{
}

void Barrier::wait() {
    unique_lock<mutex> lk{mx_};
    auto local_round = round_;
    if (--count_ == 0) {
    	++round_;
    	count_ = reset_;
        cond_.notify_all();
    } else {
        cond_.wait(lk, [this, local_round] { return local_round != round_; });
    }
}

void Barrier::open()
{
    ++round_;
    cond_.notify_all();
}