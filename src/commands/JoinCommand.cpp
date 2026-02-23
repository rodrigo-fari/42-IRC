#include "../../inc/commands/JoinCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandGuards.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void JoinCommand::execute(int fd, const MessagePayload& payload, ReplyCollector &replies) {
	ClientState& state = clientStateRepository.getClientStatus(fd);
	User* user = userRepository.findUserByFileDescriptor(fd);
	const std::string target = resolveReplyTarget(state, user);

	if (!requireRegistered(state, target, replies))
		return;
	if (!user)
		return;
	if (!requireParams(payload.params.size(), 1, target, "JOIN", replies))
		return;

	std::string channelName = payload.params[0];
	std::string providedKey = (payload.params.size() >= 2) ? payload.params[1] : "";

	if (channelName.empty() || channelName[0] != '#') {
		replies.error(ErrorReply(ERR_NOSUCHCHANNEL, target, "", channelName, "No such channel"));
		return;
	}

	bool created = false;

	Channel* ch = channelRepository.findChannelByChannelName(channelName);
	if (!ch) {
		channelRepository.createChannel(channelName);
		created = true;
		ch = channelRepository.findChannelByChannelName(channelName);
	}
	if (!ch) return;

	if (ch->isUserInChannel(user->fileDescriptor))
		return;

	if (ch->inviteOnlyPolicy && !ch->isUserInvited(user->fileDescriptor)) {
		replies.error(ErrorReply(ERR_INVITEONLYCHAN, target, "", channelName, "Cannot join channel (+i)"));
		return;
	}

	if (ch->hasChannelPassword && ch->channelPassword != providedKey) {
		replies.error(ErrorReply(ERR_BADCHANNELKEY, target, "", channelName, "Cannot join channel (+k)"));
		return;
	}

	if (ch->hasMaxUsersAmount && ch->usersCount() >= static_cast<size_t>(ch->maxUsersAmount)) {
		replies.error(ErrorReply(ERR_CHANNELISFULL, target, "", channelName, "Cannot join channel (+l)"));
		return;
	}

	ch->addUserToChannel(user->fileDescriptor);
	ch->uninviteUser(user->fileDescriptor);

	if (created)
		ch->addChannelOperator(user->fileDescriptor);

	broadcastToChannel(userRepository, *ch, prefix(*user) + " JOIN " + channelName);
	replyTopicAndNames(userRepository, *user, *ch, serverName);
}
