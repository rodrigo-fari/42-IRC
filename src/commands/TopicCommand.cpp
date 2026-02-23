#include "../../inc/commands/TopicCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandGuards.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void TopicCommand::execute(int fd, const MessagePayload& payload, ReplyCollector &replies) {
	ClientState& state = clientStateRepository.getClientStatus(fd);
	User* user = userRepository.findUserByFileDescriptor(fd);
	const std::string target = resolveReplyTarget(state, user);

	if (!requireRegistered(state, target, replies))
		return;
	if (!user)
		return;
	if (!requireParams(payload.params.size(), 1, target, "TOPIC", replies))
		return;

	std::string channelName = payload.params[0];
	Channel* ch = requireChannelExists(channelRepository, channelName, target, replies);
	if (!ch)
		return;

	if (!requireMembership(*ch, user->fileDescriptor, channelName, target, replies))
		return;

	if (payload.params.size() == 1) {
		if (ch->getChannelTopic().empty())
			sendTo(*user, ":" + serverName + " 331 " + user->username + " " + channelName + " :No topic is set");
		else
			sendTo(*user, ":" + serverName + " 332 " + user->username + " " + channelName + " :" + ch->getChannelTopic());
		return;
	}

	std::string newTopic = payload.params[1];
	if (ch->topicLockPolicy && !requireOperator(*ch, user->fileDescriptor, channelName, target, replies))
		return;

	ch->setTopic(newTopic);
	broadcastToChannel(userRepository, *ch, prefix(*user) + " TOPIC " + channelName + " :" + newTopic);
}
