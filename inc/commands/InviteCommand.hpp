#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

#include <string>
#include "BaseCommand.hpp"
#include "../../inc/core/ClientStateRepository.hpp"

class InviteCommand : public BaseCommand {
private:
    std::string serverName;
    ClientStateRepository& clientStateRepository;

public:
    InviteCommand(UserRepository& ur, ChannelRepository& cr, ClientStateRepository& csr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv), clientStateRepository(csr) {}

    void execute(int fd, const MessagePayload& payload);
};

#endif
