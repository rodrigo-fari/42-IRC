#include "../../inc/commands/KickCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandGuards.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void KickCommand::execute(int fd, const MessagePayload& payload, ReplyCollector &replies) {
	ClientState& state = clientStateRepository.getClientStatus(fd);
	User* kicker = userRepository.findUserByFileDescriptor(fd);
	const std::string target = resolveReplyTarget(state, kicker);

	if (!requireRegistered(state, target, replies))
		return;
	if (!kicker)
		return;
	if (!requireParams(payload.params.size(), 2, target, "KICK", replies))
		return;

	std::string channelName = payload.params[0];
	std::string targetNick  = payload.params[1];
	std::string reason      = (payload.params.size() >= 3) ? payload.params[2] : "";

	Channel* ch = requireChannelExists(channelRepository, channelName, target, replies);
	if (!ch)
		return;
	if (!requireMembership(*ch, kicker->fileDescriptor, channelName, target, replies))
		return;
	if (!requireOperator(*ch, kicker->fileDescriptor, channelName, target, replies))
		return;

	User* targetUser = userRepository.findUserByUsername(targetNick);
	if (!targetUser) {
		replies.error(ErrorReply(ERR_NOSUCHNICK, target, "", targetNick, "No such nick"));
		return;
	}

	if (!ch->isUserInChannel(targetUser->fileDescriptor)) {
		replies.error(ErrorReply(ERR_USERNOTINCHANNEL, target, channelName, targetNick, "They aren't on that channel"));
		return;
	}

	std::string kickLine = prefix(*kicker) + " KICK " + channelName + " " + targetNick;
	if (!reason.empty())
		kickLine += " :" + reason;

	broadcastToChannel(userRepository, *ch, kickLine);
	ch->removeUserFromChannel(targetUser->fileDescriptor);

	if (ch->empty()) {
		channelRepository.removeChannel(channelName);
		return;
	}

	ch->ensureAtLeastOneOperator();
}
