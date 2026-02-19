# include "network/Socket.hpp"
# include "network/IrcMessageFramer.hpp"
#include "parser/IrcParser.hpp"
#include <poll.h>
#include <vector>
#include <map>


struct Connection
{
	Socket socket;
	std::string inBuffer;
	std::string outBuffer;

	IrcMessageFramer framer;

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

