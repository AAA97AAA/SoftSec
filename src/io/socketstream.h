#pragma once

#include <streambuf>
#include <vector>

class Socket;

class isocketbuf : public std::streambuf
{
public:
	explicit isocketbuf(const Socket &socket, std::size_t buff_sz = 256);

private:
	int_type underflow();

	isocketbuf(const isocketbuf &);
	isocketbuf & operator=(const isocketbuf &);

private:
	const Socket &socket_;
	std::vector<char> buffer_;
};

class osocketbuf : public std::streambuf
{
public:
	explicit osocketbuf(const Socket &socket, std::size_t buff_sz = 256);

protected:
	bool write_and_flush();

private:
	int_type overflow(int_type ch);
	int sync();

	osocketbuf(const osocketbuf &);
	osocketbuf & operator=(const osocketbuf &);

private:
	const Socket &socket_;
	std::vector<char> buffer_;
};