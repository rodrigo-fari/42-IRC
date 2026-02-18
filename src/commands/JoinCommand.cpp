#include "../../inc/commands/JoinCommand.hpp"
#include "../../inc/commands/CommandHelpers.hpp"
#include "../../inc/core/ClientStateRepository.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/core/ClientStateRepository.hpp" 

void JoinCommand::execute(int fd, const MessagePayload& payload) {
	User* user = userRepository.findUserByFileDescriptor(fd);
	if (!user) return;

	ClientState& state = clientStateRepository.getClientStatus(fd);

	// precisa estar registrado
	if (!state.isRegistered) {
		sendTo(*user, ":" + serverName + " 451 " + user->username + " :You have not registered");
		return;
	}

	// payload.params para JOIN vem assim: ["#chan", "key?"]
	if (payload.params.size() < 1) {
		sendTo(*user, ":" + serverName + " 461 " + user->username + " JOIN :Not enough parameters");
		return;
	}

	std::string channelName = payload.params[0];
	std::string providedKey = (payload.params.size() >= 2) ? payload.params[1] : "";

	if (channelName.empty() || channelName[0] != '#') {
		sendTo(*user, ":" + serverName + " 403 " + user->username + " " + channelName + " :No such channel");
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

	// +i
	if (ch->inviteOnlyPolicy && !ch->isUserInvited(user->fileDescriptor)) {
		sendTo(*user, ":" + serverName + " 473 " + user->username + " " + channelName + " :Cannot join channel (+i)");
		return;
	}

	// +k
	if (ch->hasChannelPassword && ch->channelPassword != providedKey) {
		sendTo(*user, ":" + serverName + " 475 " + user->username + " " + channelName + " :Cannot join channel (+k)");
		return;
	}

	// +l
	if (ch->hasMaxUsersAmount && ch->usersCount() >= static_cast<size_t>(ch->maxUsersAmount)) {
		sendTo(*user, ":" + serverName + " 471 " + user->username + " " + channelName + " :Cannot join channel (+l)");
		return;
	}

	// entra
	ch->addUserToChannel(user->fileDescriptor);
	ch->uninviteUser(user->fileDescriptor);

	// primeiro a entrar vira op
	if (created)
		ch->addChannelOperator(user->fileDescriptor);

	// broadcast JOIN
	broadcastToChannel(userRepository, *ch, prefix(*user) + " JOIN " + channelName);

	// topic + names
	replyTopicAndNames(userRepository, *user, *ch, serverName);
}
