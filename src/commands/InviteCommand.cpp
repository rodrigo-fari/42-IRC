#include "../../inc/commands/InviteCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void InviteCommand::execute(int fd, const MessagePayload& payload) {
	User* inviter = userRepository.findUserByFileDescriptor(fd);
	if (!inviter) return;

	ClientState& state = clientStateRepository.getClientStatus(fd);

	// needs to be registered
	if (!state.isRegistered) {
		sendTo(*inviter, ":" + serverName + " 451 " + inviter->username + " :You have not registered");
		return;
	}

	if (payload.params.size() < 2) {
		sendTo(*inviter, ":" + serverName + " 461 " + inviter->username + " INVITE :Not enough parameters");
		return;
	}

	std::string targetNick = payload.params[0];
	std::string channelName = payload.params[1];

	// Does the channel exist?
	Channel* ch = channelRepository.findChannelByChannelName(channelName);
	if (!ch) {
		sendTo(*inviter, ":" + serverName + " 403 " + inviter->username + " " + channelName + " :No such channel");
		return;
	}

	// The inviter must be on the channel.
	if (!ch->isUserInChannel(inviter->fileDescriptor)) {
		sendTo(*inviter, ":" + serverName + " 442 " + inviter->username + " " + channelName + " :You're not on that channel");
		return;
	}

	// only OP can invite
	if (!ch->isChannelOperator(inviter->fileDescriptor)) {
		sendTo(*inviter, ":" + serverName + " 482 " + inviter->username + " " + channelName + " :You're not channel operator");
		return;
	}

	// target exist ?
	User* target = userRepository.findUserByUsername(targetNick);
	if (!target) {
		sendTo(*inviter, ":" + serverName + " 401 " + inviter->username + " " + targetNick + " :No such nick");
		return;
	}

	// is the taget already in the channel ?
	if (ch->isUserInChannel(target->fileDescriptor)) {
		sendTo(*inviter, ":" + serverName + " 443 " + inviter->username + " " + targetNick + " " + channelName + " :is already on channel");
		return;
	}

	// add to guest list
	ch->inviteUser(target->fileDescriptor);

	// warns the target
	sendTo(*target, prefix(*inviter) + " INVITE " + targetNick + " " + channelName);

	// confirm for inviter
	sendTo(*inviter, ":" + serverName + " 341 " + inviter->username + " " + targetNick + " " + channelName);
}
