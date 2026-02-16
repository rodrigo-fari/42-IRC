#ifndef KICKCOMMAND_HPP
#define KICKCOMMAND_HPP

#include <string>
#include "BaseCommand.hpp"
#include "../../inc/core/ClientStateRepository.hpp"

class KickCommand : public BaseCommand {
private:
    std::string serverName;
    ClientStateRepository& clientStateRepository;

public:
    KickCommand(UserRepository& ur, ChannelRepository& cr, ClientStateRepository& csr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv), clientStateRepository(csr) {}

    void execute(int fd, const MessagePayload& payload);
};

#endif