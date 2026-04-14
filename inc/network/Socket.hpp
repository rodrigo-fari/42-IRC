#pragma once

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

class Socket
{
private:
	int _fd;

public:
	Socket();
	Socket(int fd);
	~Socket();

	int getSocketFd() const;
	void setSocketFd(int fd);

	bool isSocketValid() const;
	bool bindSocket(const struct addrinfo *addr);
	bool listenSocket(int backlog);
	void closeSocket();
	int acceptConnection() const;
	void setNonBlocking();

	ssize_t sendData(const std::string &data) const;
	ssize_t receiveData(char *buffer, size_t size) const;

};