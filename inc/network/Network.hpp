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

#include <set>
#include <map>

#ifndef DEBUG
#define DEBUG 0
#endif


class Socket
{
private:
	int _fd;

public:
	Socket() : _fd(-1) {}
	Socket(int fd) : _fd(fd) {}
	~Socket() {}

	int getSocketFd() const { return _fd; }
	void setSocketFd(int fd) { _fd = fd; }

	bool isSocketValid() const { return _fd != -1; }

	bool bindSocket(const struct addrinfo *addr)
	{
		if (::bind(_fd, addr->ai_addr, addr->ai_addrlen) == -1)
		{
			std::cerr << "bind error: " << strerror(errno) << std::endl;
			return false;
		}
		return true;
	}
	bool listenSocket(int backlog)
	{
		if (::listen(_fd, backlog) == -1)
		{
			std::cerr << "listen error: " << strerror(errno) << std::endl;
			return false;
		}
		return true;
	}
	void closeSocket()
	{
		if (_fd != -1)
		{
			close(_fd);
			_fd = -1;
		}
	}
	int acceptConnection() const
	{
		return ::accept(_fd, NULL, NULL);
	}
	void setNonBlocking()
	{
		int flags = fcntl(_fd, F_GETFL, 0);
		fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
	}

	ssize_t sendData(const std::string &data) const
	{
		return send(_fd, data.c_str(), data.size(), 0);
	}
	ssize_t receiveData(char *buffer, size_t size) const
	{
		return recv(_fd, buffer, size, 0);
	}
};
struct Connection
{
	// int		fd;
	Socket socket;
	std::string inBuffer;
	std::string outBuffer;
	Connection() : socket(-1) {}
	Connection(int fd) : socket(fd) {}
	// Connection(Socket &s) : socket(std::move(s)) {}
};

struct PollSet
{
	std::vector<pollfd> pfds;
	std::map<int, int> indexByFd;

	void add(int fd, short events)
	{
		pollfd poll_fd;

		poll_fd.fd = fd;
		poll_fd.events = events;
		poll_fd.revents = 0;
		pfds.push_back(poll_fd);
		indexByFd[fd] = pfds.size() - 1;
	}
};

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
	int getSize() const { return pollset.pfds.size(); }

	int set_nonblocking(int fd);
	int create_listener(const std::string &port);
	void close_all();
	void disconnect(std::vector<pollfd> &pfds, std::size_t i);
};
