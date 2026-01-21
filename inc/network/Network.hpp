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


class Server {
public:
	Server(const std::string &port);
	~Server();

	void run();

private:
	std::string port;
	int listener;
	std::vector<pollfd> pfds;

	int set_nonblocking(int fd);
	int create_listener(const std::string &port);
	void close_all();
};
