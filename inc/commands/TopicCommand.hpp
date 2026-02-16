#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

#include <string>
#include "BaseCommand.hpp"
#include "../../inc/core/ClientStateRepository.hpp"

class TopicCommand : public BaseCommand {
private:
    std::string serverName;
    ClientStateRepository& clientStateRepository;

public:
    TopicCommand(UserRepository& ur, ChannelRepository& cr, ClientStateRepository& csr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv), clientStateRepository(csr) {}

    void execute(int fd, const MessagePayload& payload);
};

#endif
