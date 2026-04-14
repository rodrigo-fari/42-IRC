#include "../../inc/commands/InviteCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandGuards.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void InviteCommand::execute(int fd, const MessagePayload& payload, ReplyCollector& replies) {
	ClientState& state = clientStateRepository.getClientStatus(fd);
	User* inviter = userRepository.findUserByFileDescriptor(fd);
	const std::string target = resolveReplyTarget(state, inviter);

	if (!requireRegistered(state, target, replies))
		return;
	if (!inviter)
		return;
	if (!requireParams(payload.params.size(), 2, target, "INVITE", replies))
		return;

	std::string targetNick = payload.params[0];
	std::string channelName = payload.params[1];

	Channel* ch = requireChannelExists(channelRepository, channelName, target, replies);
	if (!ch)
		return;
	if (!requireMembership(*ch, inviter->fileDescriptor, channelName, target, replies))
		return;
	if (!requireOperator(*ch, inviter->fileDescriptor, channelName, target, replies))
		return;

	User* invitedUser = userRepository.findUserByUsername(targetNick);
	if (!invitedUser) {
		replies.error(ErrorReply(ERR_NOSUCHNICK, target, "", targetNick, "No such nick"));
		return;
	}

	if (ch->isUserInChannel(invitedUser->fileDescriptor)) {
		replies.error(
			ErrorReply(ERR_USERONCHANNEL, target, channelName, targetNick, "is already on channel")
		);
		return;
	}

	ch->inviteUser(invitedUser->fileDescriptor);
	sendTo(*invitedUser, prefix(*inviter) + " INVITE " + targetNick + " " + channelName);
	sendTo(
		*inviter,
		":" + serverName + " 341 " + inviter->username + " " + targetNick + " " + channelName
	);
}
