#ifndef TOPICCOMMAND_HPP
#define TOPICCOMMAND_HPP

//#include "BaseCommand.hpp"

class TopicCommand : public BaseCommand {
private:
    std::string serverName;

public:
    TopicCommand(UserRepository& ur, ChannelRepository& cr, const std::string& srv)
        : BaseCommand(ur, cr), serverName(srv) {}

    void execute(int fd, const MessagePayload& payload);
};

#endif
