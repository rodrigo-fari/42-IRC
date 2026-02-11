#include "../../inc/commands/Commands.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/ChannelRepository.hpp"
#include "../../inc/commands/PartCommand.hpp"
#include "../../inc/commands/CommandHelpers.hpp"
// falta include core.hpp "server e client"
#include <string>
#include <vector>

void PartCommand::execute(int fd, const MessagePayload& payload) {
    User* user = userRepository.findUserByFileDescriptor(fd);
    if (!user) return;

    if (!user->isRegistered) {
        sendTo(*user, ":" + serverName + " 451 " + user->username + " :You have not registered");
        return;
    }

    // tokenizedMessage: "PART", "#chan" já sem ':'"
    if (payload.tokenizedMessage.size() < 2) {
        sendTo(*user, ":" + serverName + " 461 " + user->username + " PART :Not enough parameters");
        return;
    }

    std::string channelName = payload.tokenizedMessage[1];

    Channel* ch = channelRepository.findChannelByChannelName(channelName);
    if (!ch) {
        sendTo(*user, ":" + serverName + " 403 " + user->username + " " + channelName + " :No such channel");
        return;
    }

    if (!ch->isUserInChannel(user->fileDescriptor)) {
        sendTo(*user, ":" + serverName + " 442 " + user->username + " " + channelName + " :You're not on that channel");
        return;
    }

    std::string trailing = "";
    if (payload.tokenizedMessage.size() >= 3)
        trailing = payload.tokenizedMessage[2];

    std::string msg = trailing.empty() ? "" : " :" + trailing;

    // avisa geral antes de remover
    broadcastToChannel(userRepository, *ch, prefix(*user) + " PART " + channelName + msg);

    // remove
    ch->removeUserFromChannel(user->fileDescriptor);

    // canal vazio, remove do repo
    if (ch->empty()) {
        channelRepository.removeChannel(channelName);
        return;
    }

    // garante op
    ch->ensureAtLeastOneOperator();
}