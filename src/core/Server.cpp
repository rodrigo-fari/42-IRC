#include "../../inc/core/Server.hpp"

Server::Server(const std::string &port) 
	: port(port), 
	  serverSocket(-1),
	  dispatcher(userRepository, channelRepository, clientStateRepository, "42IRC"),
	  parserDispatcher(dispatcher)
{
}

Server::~Server() { close_all(); }

int Server::getSize() const { return pollset.pfds.size(); }


// Helper: set POLLOUT for the correct pollfd given an fd
void set_pollout_for_fd(PollSet &pollset, int fd)
{
	for (size_t j = 0; j < pollset.pfds.size(); ++j)
	{
		if (pollset.pfds[j].fd == fd)
		{
			pollset.pfds[j].events |= POLLOUT;
			std::cout << "[DEBUG POLLOUT] Set to fd=" << fd << std::endl;
			break;
		}
	}
}

int Server::set_nonblocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return (-1);
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
		if (DEBUG)
			std::cout << "[NETWORK] socket() returned: " << listener << std::endl;
		if (listener < 0)
		{
			if (DEBUG)
				std::cout << "[NETWORK] socket() failed: " << strerror(errno) << std::endl;
			continue;
		}
		Socket sock(listener);
		int yes = 1;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0)
		{

			if (DEBUG)
				std::cout << "[NETWORK] setsockopt() failed: " << strerror(errno) << std::endl;
			close(listener);
			listener = -1;
			continue;
		}
		if (DEBUG)
			std::cout << "[NETWORK] setsockopt() success" << std::endl;
		if (!sock.bindSocket(p))
		{
			std::cout << "[NETWORK] bind() failed: " << strerror(errno) << std::endl;
			close(listener);
			listener = -1;
			continue;
		}
		if (DEBUG)
			std::cout << "[NETWORK] bind() success" << std::endl;
		if (!sock.listenSocket(10))
		{
			std::cout << "[NETWORK] listen() failed: " << strerror(errno) << std::endl;
			close(listener);
			listener = -1;
			continue;
		}
		if (DEBUG)
			std::cout << "[NETWORK] listen() success" << std::endl;
		break;
	}
	freeaddrinfo(res);
	return listener;
}

void Server::close_all()
{
	for (std::size_t i = 0; i < pollset.pfds.size(); ++i)
	{
		int fd = pollset.pfds[i].fd;
		if (fd != serverSocket.getSocketFd() && connections.count(fd))
			connections[fd].socket.closeSocket();
	}
	pollset.pfds.clear();
	serverSocket.closeSocket();
}

static void del_pfd(std::vector<pollfd> &pfds, std::size_t i)
{
	pfds[i] = pfds.back();
	pfds.pop_back();
}

void Server::disconnect(std::vector<pollfd> &pfds, std::size_t i)
{
	const int fd = pfds[i].fd;
	pfds.erase(pfds.begin() + i);
	std::cout << "[DISCONNECT] fd=" << fd << " disconnected (clients=" << getSize() - 1 << ")"<< std::endl;
	if (fd != serverSocket.getSocketFd() && connections.count(fd))
	{
		connections[fd].socket.closeSocket();
		connections.erase(fd);
	}
	del_pfd(pfds, i);
}

void Server::init()
{
	int listener_fd = create_listener(port);
	if (DEBUG)
		std::cout << "[NETWORK] create_listener returned: " << listener_fd << std::endl;
	serverSocket.setSocketFd(listener_fd);
	if (DEBUG)
		std::cout << "[NETWORK] serverSocket.isSocketValid(): " << serverSocket.isSocketValid() << std::endl;
	if (!serverSocket.isSocketValid())
	{
		std::cout << "[NETWORK] Listener socket is not valid, aborting init." << std::endl;
		return;
	}
	serverSocket.setNonBlocking();
	//* ADD LISTENER TO POLLSET
	pollset.pfds.clear();
	pollset.add(serverSocket.getSocketFd(), POLLIN);
	run();
}

void Server::run()
{
	std::cout << "[SERVER] listening on port " << port << "\n";
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
			// NEW CONNECTION HANDLING
			if (fd == serverSocket.getSocketFd() && (re & POLLIN))
			{
				if (DEBUG)
					std::cout << "[NETWORK] POLLIN on listener fd=" << fd << std::endl;
				while (true)
				{
					int client_fd = serverSocket.acceptConnection();
					if (client_fd < 0)
					{
						if (errno == EAGAIN || errno == EWOULDBLOCK)
							break;
						perror("accept");
						break;
					}
					if (set_nonblocking(client_fd) < 0)
					{
						if (DEBUG)
							std::cout << "[NETWORK] set_nonblocking failed for fd=" << client_fd << " errno=" << errno << std::endl;
					}
					pollset.add(client_fd, POLLIN);
					connections[client_fd] = Connection(client_fd);
					std::cout << "[CONNECTION] accepted fd=" << client_fd
							  << " (clients=" << (pollset.pfds.size() - 1) << ")\n";
				}
				i++;
				continue;
			}
			// DATA RECEIVING HANDLING readAll
			if (re & POLLIN)
			{
				if (DEBUG)
					std::cout << "[NETWORK] POLLIN event for client fd=" << fd << std::endl;
				char buffer[4096];
				Connection &reader = connections[fd];
				while (true)
				{
					const ssize_t bytesRead = reader.socket.receiveData(buffer, sizeof(buffer));
					if (bytesRead > 0)
					{
						// std::string rawData(buffer, bytesRead);
						// reader.outBuffer = parserDispatcher.processData(fd, rawData);
						// set_pollout_for_fd(pollset, fd);

						reader.inBuffer.append(buffer, buffer + bitesRead);
						std::vector<std::string> lines = framer.processRawData(fd, reader.inBuffer);
						for (size_t j = 0; j < lines.size(); ++j)
						{
							const std::string& normalizedLine = lines[j];
							std::cout << "[RECEIVED] fd=" << fd << " line: " << normalizedLine << "\n";

							payload = parseMessage(normalizedLine);
							std::string responseLines = dispatch(fd, payload);
							if (!responseLines.empty())
							{
								std::cout << "[RESPONSE] fd=" << fd << " line: " << responseLines << "\n";
								bool wasEmpty = reader.outBuffer.empty();
								reader.outBuffer += responseLines;
								if (wasEmpty)
									set_pollout_for_fd(pollset, fd);
								
								
							}
						}
						continue;
						// while (true)
						// {
						// 	std::string line;
						// 	size_t pos = reader.inBuffer.find("\r\n");
						// 	if (pos != std::string::npos)
						// 	{
						// 		line = reader.inBuffer.substr(0, pos);
						// 		reader.inBuffer.erase(0, pos + 2);
						// 	}
						// 	else
						// 	{
						// 		pos = reader.inBuffer.find('\n');
						// 		if (pos == std::string::npos)
						// 			break;
						// 		line = reader.inBuffer.substr(0, pos);
						// 		reader.inBuffer.erase(0, pos + 1);
						// 	}
						// 	std::cout << "[RECEIVED] fd=" << fd << " line: " << line << "\n";
						// 	std::string normalizedLine;
						// 	if (line.size() >= 2 && line.substr(line.size() - 2) == "\r\n")
						// 		normalizedLine = line;
						// 	else if (!line.empty() && line[line.size() - 1] == '\n')
						// 		normalizedLine = line.substr(0, line.size() - 1) + "\r\n";
						// 	else
						// 		normalizedLine = line + "\r\n";
						// 	reader.outBuffer += "ECHO: " + normalizedLine;
						// 	set_pollout_for_fd(pollset, fd);
						// }
						// continue;
					}
					if (bytesRead == 0)
					{
						std::cout << "[NETWORK] Client fd=" << fd << " disconnected (recv returned 0)" << std::endl;
						parserDispatcher.clearClient(fd);
						disconnect(pollset.pfds, i);
						break;
					}
					if (errno == EAGAIN || errno == EWOULDBLOCK)
						break;
					
				}
			}
			// HANDLING SEND FROM SERVER TO CLIENT
			if (re & POLLOUT)
			{

				if (DEBUG)
					std::cout << "[NETWORK] POLLOUT event for fd=" << fd << std::endl;
				Connection &sender = connections[fd];
				std::string &out = sender.outBuffer;
				while (!out.empty())
				{
					if (DEBUG)
						std::cout << "[NETWORK] POLLOUT sending to fd=" << fd << " contents='" << out << "' size=" << out.size() << std::endl;
					ssize_t sent = sender.socket.sendData(out);
					std::cout << "[DEBUG] send() returned: " << sent << std::endl;
					if (sent > 0)
					{
						out.erase(0, sent);
						continue;
					}
					if (sent < 0 && (errno == EAGAIN))
					{
						std::cerr << "[ERROR] send failed, errno=" << errno << std::endl;
						break;
					}
				}
				if (out.empty())
					pollset.pfds[i].events &= ~POLLOUT;
			}
			i++;
		}
	}
	if (DEBUG)
		std::cout << "[NETWORK] Exiting event loop, calling close_all()" << std::endl;
	close_all();
}
