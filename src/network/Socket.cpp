#include "../../inc/network/Network.hpp"

Server::Server(const std::string &port)
	: port(port), listener(-1) {
}

Server::~Server() {
	close_all();
}


int	Server::set_nonblocking(int fd) {

	const int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int	Server::create_listener(const std::string &port){

	struct addrinfo hints;
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo *res = 0;
	const int rv = getaddrinfo(0, port.c_str(), &hints, &res);
	if (rv != 0){
		std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
		return -1;
	}
	int listener = -1;
	for (struct addrinfo *p = res; p != 0; p = p->ai_next){
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0)
			continue;
		int yes = 1;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0){
			close(listener);
			listener = -1;
			continue;
		}
		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0){
			close(listener);
			listener = -1;
			continue;
		}
		if (listen(listener, 10) < 0){
			close(listener);
			listener = -1;
			continue;
		}
		break;
	}
	freeaddrinfo(res);
	return listener;
}


void	Server::close_all() {
	for (std::size_t i = 0; i < pfds.size(); ++i)
		close(pfds[i].fd);
	pfds.clear();
	listener = -1;
}

void	Server::run() {
	listener = create_listener(port);
	if (listener < 0)
		return;
	if (set_nonblocking(listener) < 0) {
		close(listener);
		listener = -1;
		return;
	}

	pfds.clear();
	pollfd p;
	p.fd = listener;
	p.events = POLLIN;
	p.revents = 0;
	pfds.push_back(p);

	std::cout << "[SERVER] listening on port " << port << "\n";

	while (true){
		const int n = poll(&pfds[0], pfds.size(), -1);
		if (n < 0){
			if (errno == EINTR)
				continue;
			perror("poll");
			break;
		}
		for (std::size_t i = 0; i < pfds.size(); ++i){
			const int fd = pfds[i].fd;
			const short re = pfds[i].revents;
			pfds[i].revents = 0;
			if (re == 0)
				continue;
			if (fd == listener && (re & POLLIN))
			{
				while (true){
					const int newfd = accept(listener, 0, 0);
					if (newfd < 0){
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break;
						perror("accept");
						break;
					}

					set_nonblocking(newfd);
					pollfd c;
					c.fd = newfd;
					c.events = POLLIN;
					c.revents = 0;
					pfds.push_back(c);

					std::cout << "[CONNECTION] accepted fd=" << newfd
							  << " (clients=" << (pfds.size() - 1) << ")\n";
				}
			}
		}
	}

	close_all();
}
