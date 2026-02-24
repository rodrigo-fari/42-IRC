#include "../../inc/core/Server.hpp"
#include "../../inc/commands/CommandHelpers.hpp"
#include <cctype>

Server::Server(const std::string& port, const std::string& password)
	: port(port), serverPassword(password), serverSocket(-1),
	  dispatcher(userRepository, channelRepository, clientStateRepository, "42IRC", password),
	  parserDispatcher(dispatcher) {
}

Server::~Server() {
	close_all();
}

int Server::getSize() const {
	return pollset.pfds.size();
}

// Helper: set POLLOUT for the correct pollfd given an fd
void set_pollout_for_fd(PollSet& pollset, int fd) {
	for (size_t j = 0; j < pollset.pfds.size(); ++j) {
		if (pollset.pfds[j].fd == fd) {
			pollset.pfds[j].events |= POLLOUT;
			// std::cout << "[DEBUG POLLOUT] Set to fd=" << fd << std::endl;
			break;
		}
	}
}

static void arm_pollout_for_pending_outboxes(
	PollSet& pollset,
	const std::map<int, Connection>& connections,
	UserRepository& userRepository)
{
	for (std::map<int, Connection>::const_iterator it = connections.begin(); it != connections.end(); ++it) {
		const int fd = it->first;
		User* user = userRepository.findUserByFileDescriptor(fd);
		if (user && !user->outbox.empty())
			set_pollout_for_fd(pollset, fd);
	}
}

int Server::set_nonblocking(int fd) {
	return fcntl(fd, F_SETFL, O_NONBLOCK);
}

int Server::create_listener(const std::string& port) {
	// Build a passive TCP address query so we can bind on this host/port.
	struct addrinfo hints;
	std::memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	struct addrinfo* res = 0;
	const int rv = getaddrinfo(0, port.c_str(), &hints, &res);
	if (rv != 0) {
		std::cerr << "getaddrinfo: " << gai_strerror(rv) << "\n";
		return -1;
	}
	int listener = -1;
	// Try each address candidate until one can be socket() + bind() + listen().
	for (struct addrinfo* p = res; p != 0; p = p->ai_next) {
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (DEBUG)
			std::cout << "[NETWORK] socket() returned: " << listener << std::endl;
		if (listener < 0) {
			if (DEBUG)
				std::cout << "[NETWORK] socket() failed: "<< std::endl;
			continue;
		}
		Socket sock(listener);
		// SO_REUSEADDR lets us restart quickly without waiting for TIME_WAIT.
		int yes = 1;
		if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) < 0) {
			if (DEBUG)
				std::cout << "[NETWORK] setsockopt() failed: "<< std::endl;
			close(listener);
			listener = -1;
			continue;
		}
		if (DEBUG)
			std::cout << "[NETWORK] setsockopt() success" << std::endl;
		if (!sock.bindSocket(p)) {
			std::cout << "[NETWORK] bind() failed: " << std::endl;
			close(listener);
			listener = -1;
			continue;
		}
		if (DEBUG)
			std::cout << "[NETWORK] bind() success" << std::endl;
		if (!sock.listenSocket(10)) {
			std::cout << "[NETWORK] listen() failed:"<< std::endl;
			close(listener);
			listener = -1;
			continue;
		}
		if (DEBUG)
			std::cout << "[NETWORK] listen() success" << std::endl;
		// Listener is ready; stop probing additional address candidates.
		break;
	}
	freeaddrinfo(res);
	return listener;
}

void Server::close_all() {
	for (std::size_t i = 0; i < pollset.pfds.size(); ++i) {
		int fd = pollset.pfds[i].fd;
		if (fd != serverSocket.getSocketFd() && connections.count(fd))
			connections[fd].socket.closeSocket();
	}
	pollset.pfds.clear();
	serverSocket.closeSocket();
}


void Server::disconnect(std::vector<pollfd>& pfds, std::size_t i) {
	const int fd = pfds[i].fd;
	pfds.erase(pfds.begin() + i);
	std::cout << "[DISCONNECT] fd=" << fd << " disconnected (clients=" << pollset.pfds.size() - 1
	<< ")" << std::endl;
	if (fd != serverSocket.getSocketFd() && connections.count(fd)) {
		User* leavingUser = userRepository.findUserByFileDescriptor(fd);
		if (leavingUser) {
			const std::vector<std::string> channelNames = channelRepository.getChannelsForUser(fd);
			for (size_t c = 0; c < channelNames.size(); ++c) {
				Channel* channel = channelRepository.findChannelByChannelName(channelNames[c]);
				if (!channel)
					continue;
				broadcastToChannel(userRepository, *channel, ":" + leavingUser->username + " PART " + channel->getChannelName());
			}
			arm_pollout_for_pending_outboxes(pollset, connections, userRepository);
		}
		connections[fd].clearInBuffer();
		parserDispatcher.clearClient(fd);
		clientStateRepository.remove(fd);
		channelRepository.removeUserFromAllChannels(fd);
		userRepository.removeUserByFileDescriptor(fd);
		connections[fd].socket.closeSocket();
		connections.erase(fd);
	}
}

void Server::init() {
	// Create and validate the listening socket for this server instance.
	int listener_fd = create_listener(port);
	if (DEBUG)
		std::cout << "[NETWORK] create_listener returned: " << listener_fd << std::endl;
	serverSocket.setSocketFd(listener_fd);
	if (DEBUG)
		std::cout << "[NETWORK] serverSocket.isSocketValid(): " << serverSocket.isSocketValid()
				  << std::endl;
	if (!serverSocket.isSocketValid()) {
		std::cout << "[NETWORK] Listener socket is not valid, aborting init." << std::endl;
		return;
	}
	// Listener must be non-blocking because the server is poll-driven.
	serverSocket.setNonBlocking();
	// Start pollset with only the listener; clients are added as they connect.
	pollset.pfds.clear();
	pollset.add(serverSocket.getSocketFd(), POLLIN);
	// Enter the event loop (blocks until server shutdown/error).
	run();
}

void Server::handleConnection(Socket serverSocket, std::map<int, Connection>& connections) {
	if (DEBUG)
		std::cout << "[NETWORK] POLLIN on listener fd=" << serverSocket.getSocketFd() << std::endl;
	// Drain accept queue in one shot so we do not leave pending clients waiting.
	while (true) {
		int client_fd = serverSocket.acceptConnection();
		if (client_fd < 0) {
			// perror("accept");
			break;
		}
		// Clients are non-blocking to fit the poll event loop model.
		if (set_nonblocking(client_fd) < 0) {
			if (DEBUG)
				std::cout << "[NETWORK] set_nonblocking failed for fd=" << client_fd
						  << std::endl;
		}
		// Track new client in pollset + connection repository.
		pollset.add(client_fd, POLLIN);
		connections[client_fd] = Connection(client_fd);
		std::cout << "[CONNECTION] accepted fd=" << client_fd
				  << " (clients=" << (pollset.pfds.size() - 1) << ")\n";
	}
}
void Server::handlePollIn(int fd, std::map<int, Connection>& connections, int i) {
	// One recv per POLLIN event. A second immediate recv on non-blocking sockets can
	// fail transiently and should not force disconnect in this strict-flow version.
	char buffer[4096];
	const ssize_t bitesRead = connections[fd].socket.receiveData(buffer, sizeof(buffer));
	if (bitesRead > 0) {
		// Forward raw bytes to parser/dispatcher and queue generated wire replies.
		std::string rawData(buffer, bitesRead);
		DispatchResult dispatchResult = parserDispatcher.processData(fd, rawData);
		connections[fd].outBuffer += dispatchResult.wire;
		// Some commands (e.g. QUIT/error) request "close after pending writes are sent".
		if (dispatchResult.closeAfterFlush) {
			ClientState& state = clientStateRepository.getClientStatus(fd);
			state.closeAfterFlush = true;
		}
		if (dispatchResult.fatalInternal) {
			std::cerr << "[DISPATCH ERROR] fatalInternal on fd=" << fd << std::endl;
		}
		if (DEBUG)
			std::cout << "[handlePollIn] Client fd=" << fd << "says:  " << rawData << std::endl;
		User* user = userRepository.findUserByFileDescriptor(fd);
		// Arm POLLOUT when we have anything queued to write.
		if (!dispatchResult.wire.empty() || (user && !user->outbox.empty()))
			set_pollout_for_fd(pollset, fd);
		// Also arm POLLOUT for any other clients that got data queued indirectly.
		arm_pollout_for_pending_outboxes(pollset, connections, userRepository);
		return;
	}

	if (bitesRead == 0) {
		if (DEBUG)
			std::cout << "[NETWORK] Client fd=" << fd << " disconnected (recv returned 0)"
					  << std::endl;
		disconnect(pollset.pfds, i);
		return;
	}

	// Transient recv failure on non-blocking socket: ignore and wait for next poll cycle.
}
void Server::handlePollOut(int fd, std::map<int, Connection>& connections, int i) {
	if (DEBUG)
		std::cout << "[NETWORK] POLLOUT event for fd=" << fd << std::endl;

	// Move logical messages from user outbox into the raw socket outBuffer.
	User* user = userRepository.findUserByFileDescriptor(fd);
	if (user) {
		while (!user->outbox.empty()) {
			connections[fd].outBuffer += user->outbox.front();
			user->outbox.erase(user->outbox.begin());
		}
	}

	// One send per POLLOUT event to avoid forcing disconnect on transient failures.
	if (!connections[fd].outBuffer.empty()) {
		if (DEBUG)
			std::cout << "[NETWORK] POLLOUT sending to fd=" << fd << " contents='"
					  << connections[fd].outBuffer << "' size=" << connections[fd].outBuffer.size()
					  << std::endl;
		ssize_t sent = connections[fd].socket.sendData(connections[fd].outBuffer);
		if (DEBUG)
			std::cout << "[DEBUG] send() returned: " << sent << std::endl;
		if (sent > 0)
			connections[fd].outBuffer.erase(0, sent);
		else if (sent == 0)
			disconnect(pollset.pfds, i);
	}

	if (connections[fd].outBuffer.empty()) {
		// Nothing left to write: stop listening for POLLOUT to avoid busy wakeups.
		pollset.pfds[i].events &= ~POLLOUT;
		ClientState& state = clientStateRepository.getClientStatus(fd);
		// If a command requested graceful close, disconnect once flush is complete.
		if (state.closeAfterFlush) {
			disconnect(pollset.pfds, i);
		}
	}
}
// Main event loop:
// - wait for socket activity with poll()
// - dispatch each ready fd to accept/read/write handlers
// - keep running until a fatal poll error occurs
void Server::run() {
	std::cout << "[SERVER] listening on port " << port << "\n";
	while (true) {
		// Block indefinitely until at least one fd is ready.
		const int n = poll(&pollset.pfds[0], pollset.pfds.size(), -1);
		if (n < 0) {
			break;
		}
		std::size_t i = 0;
		while (i < pollset.pfds.size()) {
			const int fd = pollset.pfds[i].fd;
			const short re = pollset.pfds[i].revents;
			pollset.pfds[i].revents = 0;
			if (re == 0) {
				i++;
				continue;
			}
			// ACCEPT NEW CONNECTIONS => accept as many pending clients as possible.
			if (fd == serverSocket.getSocketFd() && (re & POLLIN)) {
				handleConnection(serverSocket, connections);
				i++;
				continue;
			}
			// READ- RECIVE FROM CLIENT => receive and parse inbound data.
			if (re & POLLIN) {
				if (connections.count(fd) == 0) {
					i++;
					continue;
				}
				handlePollIn(fd,  connections, i);
			}
			// WRITE - SENT TO CLIENT => flush queued outbound data.
			if (re & POLLOUT) {
				if (connections.count(fd) == 0) {
					i++;
					continue;
				}
				handlePollOut(fd, connections, i);
			}
			i++;
		}
	}
	if (DEBUG)
		std::cout << "[NETWORK] Exiting event loop, calling close_all()" << std::endl;
	close_all();
}
