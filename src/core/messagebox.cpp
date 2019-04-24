#include "messagebox.h"
#include <vector>
#include <mutex>
#include <condition_variable>

using namespace std;

template <class T>
MessageBox<T>::MessageBox(size_t size)
{
	msgs_.reserve(size);
}

template <class T>
void MessageBox<T>::push(T msg)
{
	{
		lock_guard<mutex> lk(mx_cond_);
		msgs_.insert(msgs_.begin(), msg);
	}

	cond_.notify_one();
}

template <class T>
T MessageBox<T>::pop()
{
	unique_lock<mutex> lk(mx_cond_);
	cond_.wait(lk, [this]{ return msgs_.size() != 0; });

	T msg = msgs_.back();
	msgs_.pop_back();

	return msg;
}

// Compiling the template for specific classes
template class MessageBox<pid_t>;