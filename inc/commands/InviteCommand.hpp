#ifndef INVITECOMMAND_HPP
#define INVITECOMMAND_HPP

// #include "BaseCommand.hpp"

class InviteCommand : public BaseCommand {
private:
    std::string serverName;

public:
    InviteCommand(UserRepository& ur, ChannelRepository& cr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv) {}

    void execute(int fd, const MessagePayload& payload);
};

#endif
