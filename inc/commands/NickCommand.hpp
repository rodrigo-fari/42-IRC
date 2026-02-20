#pragma once
#include "core/ClientStateRepository.hpp"
#include "commands/BaseCommand.hpp"
#include <string>

class NickCommand : public BaseCommand
{
public:
    NickCommand(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr, const std::string &srv);
    void execute(int fd, const MessagePayload &payload);

private:
    ClientStateRepository &clientStateRepository;
    std::string serverName;
};
