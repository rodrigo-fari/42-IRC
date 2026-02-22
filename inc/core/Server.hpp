#pragma once

# include <map>
# include <vector>
# include <cstdio>
# include "network/Socket.hpp"
# include "network/Network.hpp"
# include "parser/ParserAndDispatch.hpp"
# include "commandHandler/Dispatcher.hpp"
# include "core/UserRepository.hpp"
# include "core/ChannelRepository.hpp"
# include "core/ClientStateRepository.hpp"

#ifndef DEBUG
#define DEBUG 1 
#endif

class Server
{
private:
	std::string port;
	std::string serverPassword;
	Socket serverSocket;
	std::map<int, Connection> connections;
	PollSet pollset;
	
	UserRepository userRepository;
	ChannelRepository channelRepository;
	ClientStateRepository clientStateRepository;
	Dispatcher dispatcher;
	ParserAndDispatcher parserDispatcher;

		
	

public:
	Server(const std::string &port, const std::string &password);
	~Server();

	void run();
	void init();
	int getSize() const;

	int set_nonblocking(int fd);
	int create_listener(const std::string &port);
	void close_all();
	void disconnect(std::vector<pollfd> &pfds, std::size_t i);

	void handleConnection(Socket serverSocket, std::map<int, Connection> &connections);
	void handlePollIn(int fd, std::map<int, Connection> &connections, int i);
	void handlePollOut(int fd, std::map<int, Connection> &connections, int i);
};
