#include "commands/UserCommand.hpp"

UserCommand::UserCommand(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr, const std::string &srv)
    : BaseCommand(ur, cr), clientStateRepository(csr), serverName(srv) {}

void UserCommand::execute(int fd, const MessagePayload &payload)
{
    ClientState &state = clientStateRepository.getClientStatus(fd);
    (void)serverName;

    if (payload.params.size() < 4)
        return;

    state.username = payload.params[0];
    state.hasUsername = true;
    if (!state.isRegistered && state.hasNickname && state.hasUsername)
    {
        bool created = userRepository.createUser(fd, state.nickname, "");
        if (created || userRepository.findUserByFileDescriptor(fd))
            state.isRegistered = true;
    }
}
