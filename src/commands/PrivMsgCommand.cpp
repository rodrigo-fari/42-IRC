#include "commands/PrivMsgCommand.hpp"
#include "commands/Channel.hpp"
#include "commands/CommandGuards.hpp"
#include "commands/CommandHelpers.hpp"

void PrivMsgCommand::execute(int fd, const MessagePayload& payload, ReplyCollector& replies) {
	ClientState& state = clientStateRepository.getClientStatus(fd);
	User* sender = userRepository.findUserByFileDescriptor(fd);
	const std::string target = resolveReplyTarget(state, sender);

	if (!requireRegistered(state, target, replies))
		return;
	if (!requireParams(payload.params.size(), 2, target, "PRIVMSG", replies))
		return;
	if (!sender) {
		replies.error(ErrorReply(ERR_NOTREGISTERED, target, "", "", "You have not registered"));
		return;
	}

	const std::string& messageTarget = payload.params[0];
	const std::string& message = payload.params[1];
	if (messageTarget.empty()) {
		replies.error(
			ErrorReply(ERR_NEEDMOREPARAMS, target, "PRIVMSG", "", "Not enough parameters")
		);
		return;
	}

	const std::string line = prefix(*sender) + " PRIVMSG " + messageTarget + " :" + message;
	if (messageTarget[0] == '#') {
		Channel* channel = channelRepository.findChannelByChannelName(messageTarget);
		if (!channel) {
			replies.error(
				ErrorReply(ERR_NOSUCHCHANNEL, target, "", messageTarget, "No such channel")
			);
			return;
		}

		std::vector<int> fds = channel->getUsersInChannelJoinOrder();
		for (size_t i = 0; i < fds.size(); ++i) {
			if (fds[i] == fd)
				continue;
			User* member = userRepository.findUserByFileDescriptor(fds[i]);
			if (member)
				sendTo(*member, line);
		}
		return;
	}

	User* targetUser = userRepository.findUserByUsername(messageTarget);
	if (!targetUser) {
		replies.error(ErrorReply(ERR_NOSUCHNICK, target, "", messageTarget, "No such nick"));
		return;
	}

	sendTo(*targetUser, line);
}
