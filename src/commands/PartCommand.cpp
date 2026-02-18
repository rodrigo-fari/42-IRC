#include "../../inc/commands/PartCommand.hpp"
#include "../../inc/commands/CommandHelpers.hpp"
#include "../../inc/commands/Channel.hpp"

void PartCommand::execute(int fd, const MessagePayload& payload) {
	User* user = userRepository.findUserByFileDescriptor(fd);
	if (!user) return;

	ClientState& state = clientStateRepository.getClientStatus(fd);

	// precisa estar registrado
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

	// tem que ser membro do canal
	if (!ch->isUserInChannel(user->fileDescriptor)) {
		sendTo(*user, ":" + serverName + " 442 " + user->username + " " + channelName + " :You're not on that channel");
		return;
	}

	// motivo, se existir
	std::string trailing = "";
	if (payload.params.size() >= 2)
		trailing = payload.params[1];

	std::string msg = trailing.empty() ? "" : " :" + trailing;

	// avisa geral antes de remover
	broadcastToChannel(userRepository, *ch, prefix(*user) + " PART " + channelName + msg);

	// remove do canal
	ch->removeUserFromChannel(user->fileDescriptor);

	// canal vazio, remove do repo
	if (ch->empty()) {
		channelRepository.removeChannel(channelName);
		return;
	}

	// garante que ainda exista algum op
	ch->ensureAtLeastOneOperator();
}