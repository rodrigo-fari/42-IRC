#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

#include <string>
#include "BaseCommand.hpp"
#include "../../inc/core/ClientStateRepository.hpp"

class JoinCommand : public BaseCommand {
private:
    std::string serverName;
    ClientStateRepository& clientStateRepository;

public:
    JoinCommand(UserRepository& ur, ChannelRepository& cr, ClientStateRepository& csr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv), clientStateRepository(csr) {}

    void execute(int fd, const MessagePayload& payload, ReplyCollector &replies);
};

#endif
