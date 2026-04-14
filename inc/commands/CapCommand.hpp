#ifndef CAPCOMMAND_HPP
#define CAPCOMMAND_HPP

#include <string>
#include "BaseCommand.hpp"

class CapCommand : public BaseCommand {
private:
	std::string serverName;

public:
	CapCommand(UserRepository& ur, ChannelRepository& cr, const std::string& srv)
		: BaseCommand(ur, cr), serverName(srv) {}

	void execute(int fd, const MessagePayload& payload, ReplyCollector &replies);
};

#endif
