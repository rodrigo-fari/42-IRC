#pragma once

#include <map>
#include <vector>
#include <cstdio>
# include "../network/Socket.hpp"
# include "../network/Network.hpp"


class Server
{
private:
	std::string port;
	Socket serverSocket;
	std::map<int, Connection> connections;
	PollSet pollset;

public:
	Server(const std::string &port);
	~Server();

	void run();
	void init();
	int getSize() const;

	int set_nonblocking(int fd);
	int create_listener(const std::string &port);
	void close_all();
	void disconnect(std::vector<pollfd> &pfds, std::size_t i);
};
