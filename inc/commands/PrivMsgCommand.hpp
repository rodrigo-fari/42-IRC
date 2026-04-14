#ifndef PRIVMSGCOMMAND_HPP
#define PRIVMSGCOMMAND_HPP

#include <string>
#include "BaseCommand.hpp"
#include "core/ClientStateRepository.hpp"

class PrivMsgCommand : public BaseCommand {
private:
	std::string serverName;
	ClientStateRepository& clientStateRepository;

public:
	PrivMsgCommand(UserRepository& ur, ChannelRepository& cr, ClientStateRepository& csr, const std::string& srv)
		: BaseCommand(ur, cr), serverName(srv), clientStateRepository(csr) {}

	void execute(int fd, const MessagePayload& payload, ReplyCollector &replies);
};

#endif
