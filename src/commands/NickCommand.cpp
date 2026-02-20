#include "commands/NickCommand.hpp"

NickCommand::NickCommand(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr, const std::string &srv)
    : BaseCommand(ur, cr), clientStateRepository(csr), serverName(srv) {}

void NickCommand::execute(int fd, const MessagePayload &payload)
{
    ClientState &state = clientStateRepository.getClientStatus(fd);
    (void)serverName;

    if (payload.params.empty())
        return;

    state.nickname = payload.params[0];
    state.hasNickname = true;
    if (!state.isRegistered && state.hasNickname && state.hasUsername)
    {
        bool created = userRepository.createUser(fd, state.nickname, "");
        if (created || userRepository.findUserByFileDescriptor(fd))
            state.isRegistered = true;
    }
}
