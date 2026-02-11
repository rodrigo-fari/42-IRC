#ifndef PARTCOMMAND_HPP
#define PARTCOMMAND_HPP

//#include "BaseCommand.hpp"

class PartCommand : public BaseCommand {
private:
    std::string serverName;

public:
    PartCommand(UserRepository& ur, ChannelRepository& cr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv) {}

    void execute(int fd, const MessagePayload& payload);
};

#endif