#pragma once
#include "../core/Core.hpp"
# include "../network/Socket.hpp"


#ifndef DEBUG
#define DEBUG 1
#endif



struct Connection
{
	Socket socket;
	std::string inBuffer;
	std::string outBuffer;

	Connection();
	Connection(int fd);
	// Connection(Socket &s) : socket(std::move(s)) {}
};

struct PollSet
{
	std::vector<pollfd> pfds;
	std::map<int, int> indexByFd;

	void add(int fd, short events);
};

