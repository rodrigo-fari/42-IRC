#pragma once
#include "core/ClientStateRepository.hpp"
#include "commands/BaseCommand.hpp"
#include <string>

class UserCommand : public BaseCommand
{
public:
    UserCommand(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr, const std::string &srv);
    void execute(int fd, const MessagePayload &payload);

private:
    ClientStateRepository &clientStateRepository;
    std::string serverName;
};
