#include "../../inc/commands/PartCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandGuards.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void PartCommand::execute(int fd, const MessagePayload& payload, ReplyCollector& replies) {
	ClientState& state = clientStateRepository.getClientStatus(fd);
	User* user = userRepository.findUserByFileDescriptor(fd);
	const std::string target = resolveReplyTarget(state, user);

	if (!requireRegistered(state, target, replies))
		return;
	if (!user)
		return;
	if (!requireParams(payload.params.size(), 1, target, "PART", replies))
		return;

	std::string channelName = payload.params[0];
	Channel* ch = requireChannelExists(channelRepository, channelName, target, replies);
	if (!ch)
		return;

	if (!requireMembership(*ch, user->fileDescriptor, channelName, target, replies))
		return;

	std::string trailing = "";
	if (payload.params.size() >= 2)
		trailing = payload.params[1];

	std::string msg = trailing.empty() ? "" : " :" + trailing;
	broadcastToChannel(userRepository, *ch, prefix(*user) + " PART " + channelName + msg);

	ch->removeUserFromChannel(user->fileDescriptor);

	if (ch->empty()) {
		channelRepository.removeChannel(channelName);
		return;
	}

	const int promotedFd = ch->ensureAtLeastOneOperator();
	if (promotedFd != -1) {
		User* promoted = userRepository.findUserByFileDescriptor(promotedFd);
		if (promoted) {
			broadcastToChannel(
				userRepository,
				*ch,
				":" + serverName + " MODE " + channelName + " +o " + promoted->username
			);
		}
	}
}
