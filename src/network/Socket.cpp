#include "../../inc/network/Socket.hpp"

    Socket::Socket() : _fd(-1) {}
	Socket::Socket(int fd) : _fd(fd) {}
	Socket::~Socket() {}

	int Socket::getSocketFd() const { 
		return _fd; 
	}
	void Socket::setSocketFd(int fd) {
		 _fd = fd; 
	}
	bool Socket::isSocketValid() const { 
		return _fd != -1; 
	}

	bool Socket::bindSocket(const struct addrinfo *addr)
	{
		if (::bind(_fd, addr->ai_addr, addr->ai_addrlen) == -1)
		{
			std::cerr << "bind error: " << strerror(errno) << std::endl;
			return false;
		}
		return true;
	}
	bool Socket::listenSocket(int backlog)
	{
		if (::listen(_fd, backlog) == -1)
		{
			std::cerr << "listen error: " << strerror(errno) << std::endl;
			return false;
		}
		return true;
	}
	void Socket::closeSocket()
	{
		if (_fd != -1)
		{
			close(_fd);
			_fd = -1;
		}
	}
	int Socket::acceptConnection() const
	{
		return ::accept(_fd, NULL, NULL);
	}
	void Socket::setNonBlocking()
	{
		int flags = fcntl(_fd, F_GETFL, 0);
		fcntl(_fd, F_SETFL, flags | O_NONBLOCK);
	}

	ssize_t Socket::sendData(const std::string &data) const
	{
		return send(_fd, data.c_str(), data.size(), 0);
	}
	ssize_t Socket::receiveData(char *buffer, size_t size) const
	{
		return recv(_fd, buffer, size, 0);
	}