#ifndef CHANNELCOMMANDHELPERS_HPP
#define CHANNELCOMMANDHELPERS_HPP

#include <string>
#include <vector>
#include "core/UserRepository.hpp"

#include "Channel.hpp"


//!(from Rod) Hex chat by default ja manda tudo com o \r\n, nao precisa adicionar.
//! Mesmo que nao seja pelo hex, quando nos conectamos no servidor com 'nc -C etc.'
//! essa flag serve para que cada enter seja '\r\n' envez de '\n'.
static inline void sendTo(User& u, const std::string& line) {
	u.enqueue(line + "\r\n");
}

static inline std::string prefix(const User& u) {
	return ":" + u.username;
}

static inline void broadcastToChannel(UserRepository& userRepo, Channel& ch, const std::string& line) {
	std::vector<int> fds = ch.getUsersInChannelJoinOrder();
	for (size_t i = 0; i < fds.size(); ++i) {
		User* target = userRepo.findUserByFileDescriptor(fds[i]);
		if (target) sendTo(*target, line);
	}
}

// TOPIC + NAMES (pra cliente IRC não bugar)
static inline void replyTopicAndNames(UserRepository& userRepo, User& user, Channel& ch, const std::string& serverName) {
	if (ch.getChannelTopic().empty())
		sendTo(user, ":" + serverName + " 331 " + user.username + " " + ch.getChannelName() + " :No topic is set");
	else
		sendTo(user, ":" + serverName + " 332 " + user.username + " " + ch.getChannelName() + " :" + ch.getChannelTopic());

	std::vector<int> fds = ch.getUsersInChannelJoinOrder();
	std::string names;

	for (size_t i = 0; i < fds.size(); ++i) {
		User* m = userRepo.findUserByFileDescriptor(fds[i]);
		if (!m) continue;
		if (ch.isChannelOperator(m->fileDescriptor)) names += "@";
		names += m->username;
		if (i + 1 < fds.size()) names += " ";
	}

	sendTo(user, ":" + serverName + " 353 " + user.username + " = " + ch.getChannelName() + " :" + names);
	sendTo(user, ":" + serverName + " 366 " + user.username + " " + ch.getChannelName() + " :End of /NAMES list");
}

#endif