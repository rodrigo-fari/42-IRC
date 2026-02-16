#include "../../inc/commands/TopicCommand.hpp"
#include "../../inc/commands/Commands.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/ChannelRepository.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void TopicCommand::execute(int fd, const MessagePayload& payload) {
    User* user = userRepository.findUserByFileDescriptor(fd);
    if (!user) return;

    // precisa estar registrado
    if (!user->isRegistered) {
        sendTo(*user, ":" + serverName + " 451 " + user->username + " :You have not registered");
        return;
    }

    // precisa de canal
    if (payload.tokenizedMessage.size() < 2) {
        sendTo(*user, ":" + serverName + " 461 " + user->username + " TOPIC :Not enough parameters");
        return;
    }

    std::string channelName = payload.tokenizedMessage[1];

    Channel* ch = channelRepository.findChannelByChannelName(channelName);
    if (!ch) {
        sendTo(*user, ":" + serverName + " 403 " + user->username + " " + channelName + " :No such channel");
        return;
    }

    // tem que ser membro do canal
    if (!ch->isUserInChannel(user->fileDescriptor)) {
        sendTo(*user, ":" + serverName + " 442 " + user->username + " " + channelName + " :You're not on that channel");
        return;
    }

    //  VIEW 
    // TOPIC 
    if (payload.tokenizedMessage.size() == 2) {
        if (ch->getChannelTopic().empty())
            sendTo(*user, ":" + serverName + " 331 " + user->username + " " + channelName + " :No topic is set");
        else
            sendTo(*user, ":" + serverName + " 332 " + user->username + " " + channelName + " :" + ch->getChannelTopic());
        return;
    }

    // SET 
    // TOPIC new topic
    std::string newTopic = payload.tokenizedMessage[2];

    // se +t estiver ativo, só op pode mudar
    if (ch->topicLockPolicy && !ch->isChannelOperator(user->fileDescriptor)) {
        sendTo(*user, ":" + serverName + " 482 " + user->username + " " + channelName + " :You're not channel operator");
        return;
    }

    // seta topic
    ch->setTopic(newTopic);

    // broadcast pro canal todo
    broadcastToChannel(userRepository, *ch,
        prefix(*user) + " TOPIC " + channelName + " :" + newTopic);
}
