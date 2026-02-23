#include "../../inc/commands/KickCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void KickCommand::execute(int fd, const MessagePayload& payload) {
	User* kicker = userRepository.findUserByFileDescriptor(fd);
	if (!kicker) return;

	ClientState& state = clientStateRepository.getClientStatus(fd);

	// registered?
	if (!state.isRegistered) {
		sendTo(*kicker, ":" + serverName + " 451 " + kicker->username + " :You have not registered");
		return;
	}

	// payload.params for KICK it comes like this: "#chan", "nick", "reason?"
	if (payload.params.size() < 2) {
		sendTo(*kicker, ":" + serverName + " 461 " + kicker->username + " KICK :Not enough parameters");
		return;
	}

	std::string channelName = payload.params[0];
	std::string targetNick  = payload.params[1];
	std::string reason      = (payload.params.size() >= 3) ? payload.params[2] : "";

	Channel* ch = channelRepository.findChannelByChannelName(channelName);
	if (!ch) {
		sendTo(*kicker, ":" + serverName + " 403 " + kicker->username + " " + channelName + " :No such channel");
		return;
	}

	// kicker needs to be on the channel
	if (!ch->isUserInChannel(kicker->fileDescriptor)) {
		sendTo(*kicker, ":" + serverName + " 442 " + kicker->username + " " + channelName + " :You're not on that channel");
		return;
	}

	// only OP can kick
	if (!ch->isChannelOperator(kicker->fileDescriptor)) {
		sendTo(*kicker, ":" + serverName + " 482 " + kicker->username + " " + channelName + " :You're not channel operator");
		return;
	}

	// target exist?
	User* target = userRepository.findUserByUsername(targetNick);
	if (!target) {
		sendTo(*kicker, ":" + serverName + " 401 " + kicker->username + " " + targetNick + " :No such nick");
		return;
	}

	// target is in the channel?
	if (!ch->isUserInChannel(target->fileDescriptor)) {
		sendTo(*kicker, ":" + serverName + " 441 " + kicker->username + " " + targetNick + " " + channelName + " :They aren't on that channel");
		return;
	}

	// create line a kick broadcast line
	std::string kickLine = prefix(*kicker) + " KICK " + channelName + " " + targetNick;
	if (!reason.empty()) kickLine += " :" + reason;

	// warns everyone, including the target
	broadcastToChannel(userRepository, *ch, kickLine);

	// remove target the channel
	ch->removeUserFromChannel(target->fileDescriptor);

	// if channel is void, delete repo
	if (ch->empty()) {
		channelRepository.removeChannel(channelName);
		return;
	}

	// at latest 1 OP
	ch->ensureAtLeastOneOperator();
}
