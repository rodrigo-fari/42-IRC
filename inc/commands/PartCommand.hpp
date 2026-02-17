#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

#include <string>
#include "BaseCommand.hpp"
#include "../../inc/core/ClientStateRepository.hpp"

class PartCommand : public BaseCommand {
private:
    std::string serverName;
    ClientStateRepository& clientStateRepository;

public:
    PartCommand(UserRepository& ur, ChannelRepository& cr, ClientStateRepository& csr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv), clientStateRepository(csr) {}

    void execute(int fd, const MessagePayload& payload);
};

#endif