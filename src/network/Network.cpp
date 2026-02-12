#include "../../inc/network/Network.hpp"


Connection::Connection() : socket(-1) {}
Connection::Connection(int fd) : socket(fd) {}



void PollSet::add(int fd, short events)
{
    pollfd poll_fd;

    poll_fd.fd = fd;
    poll_fd.events = events;
    poll_fd.revents = 0;
    pfds.push_back(poll_fd);
    indexByFd[fd] = pfds.size() - 1;
}
