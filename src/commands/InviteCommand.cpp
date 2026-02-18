#include "../../inc/commands/InviteCommand.hpp"
#include "../../inc/commands/Channel.hpp"
#include "../../inc/commands/CommandHelpers.hpp"

void InviteCommand::execute(int fd, const MessagePayload& payload) {
	User* inviter = userRepository.findUserByFileDescriptor(fd);
	if (!inviter) return;

	ClientState& state = clientStateRepository.getClientStatus(fd);

	// precisa estar registrado
	if (!state.isRegistered) {
		sendTo(*inviter, ":" + serverName + " 451 " + inviter->username + " :You have not registered");
		return;
	}

	if (payload.params.size() < 3) {
		sendTo(*inviter, ":" + serverName + " 461 " + inviter->username + " INVITE :Not enough parameters");
		return;
	}

	std::string targetNick = payload.params[1];
	std::string channelName = payload.params[2];

	// canal existe?
	Channel* ch = channelRepository.findChannelByChannelName(channelName);
	if (!ch) {
		sendTo(*inviter, ":" + serverName + " 403 " + inviter->username + " " + channelName + " :No such channel");
		return;
	}

	// inviter tem que estar no canal
	if (!ch->isUserInChannel(inviter->fileDescriptor)) {
		sendTo(*inviter, ":" + serverName + " 442 " + inviter->username + " " + channelName + " :You're not on that channel");
		return;
	}

	// so OP pode convidar
	if (!ch->isChannelOperator(inviter->fileDescriptor)) {
		sendTo(*inviter, ":" + serverName + " 482 " + inviter->username + " " + channelName + " :You're not channel operator");
		return;
	}

	// alvo existe?
	User* target = userRepository.findUserByUsername(targetNick);
	if (!target) {
		sendTo(*inviter, ":" + serverName + " 401 " + inviter->username + " " + targetNick + " :No such nick");
		return;
	}

	// alvo já está no canal?
	if (ch->isUserInChannel(target->fileDescriptor)) {
		sendTo(*inviter, ":" + serverName + " 443 " + inviter->username + " " + targetNick + " " + channelName + " :is already on channel");
		return;
	}

	// adiciona na lista de convidados
	ch->inviteUser(target->fileDescriptor);

	// avisa o alvo
	sendTo(*target, prefix(*inviter) + " INVITE " + targetNick + " " + channelName);

	// confirma pro inviter
	sendTo(*inviter, ":" + serverName + " 341 " + inviter->username + " " + targetNick + " " + channelName);
}
