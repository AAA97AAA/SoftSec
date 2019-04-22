#pragma once

#include <vector>
#include <mutex>
#include <condition_variable>

template <class T>
class MessageBox {
public:
	MessageBox(size_t size = 8);

	void push(T msg);
	T pop();

private:
	std::vector<T> msgs_;
	std::condition_variable cond_;
	std::mutex mx_cond_;
};