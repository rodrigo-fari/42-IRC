#include "../../inc/network/Network.hpp"
// Helper: set POLLOUT for the correct pollfd given an fd
void set_pollout_for_fd(PollSet &pollset, int fd) {
	for (size_t j = 0; j < pollset.pfds.size(); ++j) {
		if (pollset.pfds[j].fd == fd) {
			pollset.pfds[j].events |= POLLOUT;
			std::cout << "[DEBUG] Set POLLOUT for fd=" << fd << std::endl;
			break;
		}
	}
}

Server::Server(const std::string &port) : port(port), listener(-1) {}

Server::~Server() { close_all(); }

int Server::set_nonblocking(int fd)
{

	const int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int Server::create_listener(const std::string &port)
{

	struct addrinfo hints;
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *res = 0;
	const int rv = getaddrinfo(0, port.c_str(), &hints, &res);
	if (rv != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
		return -1;
	}
	int listener = -1;
	for (struct addrinfo *p = res; p != 0; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
			continue;
		int yes = 1;
		// setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes);
		// bind(listener, p->ai_addr, p->ai_addrlen);
		// listen(listener, 10);
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
		{
			close(listener);
			listener = -1;
			continue;
		}
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener);
			listener = -1;
			continue;
		}
		if (listen(listener, 10) < 0)
		{
			close(listener);
			listener = -1;
			continue;
		}
		break;
	}
	freeaddrinfo(res);
	return listener;
}

void Server::close_all()
{
	for (std::size_t i = 0; i < pollset.pfds.size(); ++i)
		close(pollset.pfds[i].fd);
	pollset.pfds.clear();
	listener = -1;
}

static void del_pfd(std::vector<pollfd> &pfds, std::size_t i)
{
	pfds[i] = pfds.back();
	pfds.pop_back();
}

void Server::disconnect(std::vector<pollfd> &pfds, std::size_t i)
{
	const int fd = pfds[i].fd;
	close(fd);
	pfds.erase(pfds.begin() + i);
	std::cout << "[DISCONNECT] fd=" << fd << " disconnected (clients=" << pfds.size() - 1 << ")\n";
	close(fd);
	del_pfd(pfds, i);
}

void Server::init()
{

	listener = create_listener(port);
	if (listener < 0)
		return;

	set_nonblocking(listener);

	//* POLLFD MAP INITIALIZATION
	pollset.pfds.clear();
	pollfd p;
	p.fd = listener;
	p.events = POLLIN;
	p.revents = 0;
	pollset.pfds.push_back(p);
	run();

	std::cout << "[SERVER] listening on port " << port << "\n";
}

void Server::run()
{

	//*RECIVED BYTES MAP
	// std::map<int, std::string> receivedBytes;

	while (true)
	{
		const int n = poll(&pollset.pfds[0], pollset.pfds.size(), -1);
		if (n < 0)
		{
			if (errno == EINTR)
				continue;
			perror("poll");
			break;
		}
		//* EVENT HANDLING LOOP
		std::size_t i = 0;
		while (i < pollset.pfds.size())
		{
			const int fd = pollset.pfds[i].fd;
			const short re = pollset.pfds[i].revents;
			pollset.pfds[i].revents = 0;
			if (re == 0)
			{
				i++;
				continue;
			}
			//* NEW CONNECTION HANDLING
			if (fd == listener && (re & POLLIN))
			{
				while (true)
				{
					const int newfd = accept(listener, 0, 0);
					if (newfd < 0)
					{
						// if (errno == EAGAIN || errno == EWOULDBLOCK)
						// 	break;
						// perror("accept");
						break;
					}

					set_nonblocking(newfd);
					pollfd c;
					c.fd = newfd;
					c.events = POLLIN;
					c.revents = 0;
					pollset.pfds.push_back(c);
					//*ADD CONNECTION TO MAP
					connections[newfd] = Connection(newfd);

					std::cout << "[CONNECTION] accepted fd=" << newfd
							  << " (clients=" << (pollset.pfds.size() - 1) << ")\n";
				}
				i++;
				continue;
			}
			//* DATA RECEIVING HANDLING readAll
			if (re && POLLIN)
			{
				char buffer[4096];
				std::string line;

				while (true)
				{
					const ssize_t bitesRead = recv(fd, buffer, sizeof(buffer), 0);
					if (bitesRead > 0)
					{
						connections[fd].inBuffer.append(buffer, buffer + bitesRead);
						while (true)
						{
							//* Check for complete lines CRLF
							// TODO: handle \r\n
							size_t pos = connections[fd].inBuffer.find("\n");
							if (pos == std::string::npos)
								break;
							line = connections[fd].inBuffer.substr(0, pos);
							connections[fd].inBuffer.erase(0, pos + 1);
							std::cout << "[RECEIVED] fd=" << fd << " line: " << line << "\n";
							//* ECHO BACK THE RECEIVED LINE
							connections[fd].outBuffer.append("[REPLY]Message received from server\r\n");
							set_pollout_for_fd(pollset, fd);
						}
					}
					else
					{
						break;
					}
				}
				i++;
				continue;
			}
			//* HANDLING SEND FROM SERVER TO CLIENT
			if (re & POLLOUT)
			{
				std::string &out = connections[fd].outBuffer;
				if (!out.empty()) {
					std::cout << "[DEBUG] About to send: '" << out << "' to fd=" << fd << std::endl;
					ssize_t sent = send(fd, out.data(), out.size(), 0);
					std::cout << "[DEBUG] send() returned: " << sent << std::endl;
					if (sent < 0) {
						std::cerr << "[ERROR] send failed, errno=" << errno << std::endl;
						disconnect(pollset.pfds, i);
						continue;
					}
					out.erase(0, sent);
					if (out.empty()) {
						pollset.pfds[i].events &= ~POLLOUT;
					}
				}
			}
			i++;
		}
	}

	close_all();
}
