#include <poll.h>
#include <vector>
#include <map>
#include "network/Socket.hpp"

struct Connection
{
	Socket socket;
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
