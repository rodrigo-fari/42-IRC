#ifndef JOINCOMMAND_HPP
#define JOINCOMMAND_HPP

// #include "BaseCommand.hpp"

class JoinCommand : public BaseCommand {
private:
    std::string serverName;

public:
    JoinCommand(UserRepository& ur, ChannelRepository& cr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv) {}

    void execute(int fd, const MessagePayload& payload);
};

#endif