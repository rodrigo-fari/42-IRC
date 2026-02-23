#include "../../inc/commands/TopicCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void TopicCommand::execute(int fd, const MessagePayload& payload) {
	User* user = userRepository.findUserByFileDescriptor(fd);
	if (!user) return;

	ClientState& state = clientStateRepository.getClientStatus(fd);

	// needs to be registered
	if (!state.isRegistered) {
		sendTo(*user, ":" + serverName + " 451 " + user->username + " :You have not registered");
		return;
	}

	// payload.params for TOPIC comes like this: "#chan" ou "#chan", "new topic..."
	if (payload.params.size() < 1) {
		sendTo(*user, ":" + serverName + " 461 " + user->username + " TOPIC :Not enough parameters");
		return;
	}

	std::string channelName = payload.params[0];

	Channel* ch = channelRepository.findChannelByChannelName(channelName);
	if (!ch) {
		sendTo(*user, ":" + serverName + " 403 " + user->username + " " + channelName + " :No such channel");
		return;
	}

	// have to be a channel member
	if (!ch->isUserInChannel(user->fileDescriptor)) {
		sendTo(*user, ":" + serverName + " 442 " + user->username + " " + channelName + " :You're not on that channel");
		return;
	}

	//  VIEW
	if (payload.params.size() == 1) {
		if (ch->getChannelTopic().empty())
			sendTo(*user, ":" + serverName + " 331 " + user->username + " " + channelName + " :No topic is set");
		else
			sendTo(*user, ":" + serverName + " 332 " + user->username + " " + channelName + " :" + ch->getChannelTopic());
		return;
	}

	// SET
	std::string newTopic = payload.params[1];

	// if +t active, only op can change
	if (ch->topicLockPolicy && !ch->isChannelOperator(user->fileDescriptor)) {
		sendTo(*user, ":" + serverName + " 482 " + user->username + " " + channelName + " :You're not channel operator");
		return;
	}

	ch->setTopic(newTopic);

	// broadcast for all channel
	broadcastToChannel(userRepository, *ch,
		prefix(*user) + " TOPIC " + channelName + " :" + newTopic);
}
