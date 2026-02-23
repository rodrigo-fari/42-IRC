#ifndef PASSCOMMAND_HPP
#define PASSCOMMAND_HPP

#include <string>
#include "BaseCommand.hpp"
#include "core/ClientStateRepository.hpp"

class PassCommand : public BaseCommand {
private:
	std::string serverName;
	std::string serverPassword;
	ClientStateRepository& clientStateRepository;

public:
	PassCommand(UserRepository& ur, ChannelRepository& cr, ClientStateRepository& csr,
		const std::string& srv, const std::string& password)
		: BaseCommand(ur, cr), serverName(srv), serverPassword(password), clientStateRepository(csr) {}

	void execute(int fd, const MessagePayload& payload, ReplyCollector &replies);
};

#endif
