#pragma once

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include <fcntl.h>
#include <netdb.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <map>


struct Connection{
	int		fd;
	std::string	inBuffer;
	std::string	outBuffer;
	Connection() : fd(-1) {}
	Connection(int f) : fd(f) {}
};

struct PollSet
{
	std::vector<pollfd> pfds;
	std::map<int, int> indexByFd;
};


class Server {
public:
	Server(const std::string &port);
	~Server();

	void run();
	void init();
	int     getSize() const { return pollset.pfds.size(); }

private:
	std::string port;
	int listener;
	// std::vector<pollfd> pfds;
	std::map<int, Connection> connections;
	PollSet pollset;

	int		set_nonblocking(int fd);
	int		create_listener(const std::string &port);
	void	close_all();
	void	disconnect(std::vector<pollfd> &pfds, std::size_t i);
};

