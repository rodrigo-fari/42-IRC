#include <poll.h>
#include <vector>
#include <map>
#include "network/Socket.hpp"

struct Connection
{
	Socket socket;
	std::string outBuffer;
	std::string inBuffer;

	Connection();
	Connection(int fd);
	void clearInBuffer() { inBuffer.clear(); }
};

struct PollSet
{
	std::vector<pollfd> pfds;
	std::map<int, int> indexByFd;

	void add(int fd, short events);
};
