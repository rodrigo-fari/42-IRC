#include "../../inc/commands/PartCommand.hpp"
#include "../../inc/commands/CommandHelpers.hpp"
#include "../../inc/commands/Channel.hpp"

void PartCommand::execute(int fd, const MessagePayload& payload) {
	User* user = userRepository.findUserByFileDescriptor(fd);
	if (!user) return;

	ClientState& state = clientStateRepository.getClientStatus(fd);

	// needs to be registered
	if (!state.isRegistered) {
		sendTo(*user, ":" + serverName + " 451 " + user->username + " :You have not registered");
		return;
	}

	// payload.params
	if (payload.params.size() < 1) {
		sendTo(*user, ":" + serverName + " 461 " + user->username + " PART :Not enough parameters");
		return;
	}

	std::string channelName = payload.params[0];

	Channel* ch = channelRepository.findChannelByChannelName(channelName);
	if (!ch) {
		sendTo(*user, ":" + serverName + " 403 " + user->username + " " + channelName + " :No such channel");
		return;
	}

	// you have to be a channel member
	if (!ch->isUserInChannel(user->fileDescriptor)) {
		sendTo(*user, ":" + serverName + " 442 " + user->username + " " + channelName + " :You're not on that channel");
		return;
	}

	// reason, if there is one
	std::string trailing = "";
	if (payload.params.size() >= 2)
		trailing = payload.params[1];

	std::string msg = trailing.empty() ? "" : " :" + trailing;

	// warns all members before remove
	broadcastToChannel(userRepository, *ch, prefix(*user) + " PART " + channelName + msg);

	// remove user from channel
	ch->removeUserFromChannel(user->fileDescriptor);

	// void channel, remove repo
	if (ch->empty()) {
		channelRepository.removeChannel(channelName);
		return;
	}

	// ensures that some option still exists
	ch->ensureAtLeastOneOperator();
}