#pragma once

#include <string>
#include "commandHandler/ReplyCollector.hpp"
#include "commands/Channel.hpp"
#include "core/ChannelRepository.hpp"
#include "core/ClientStateRepository.hpp"
#include "core/UserRepository.hpp"

inline std::string resolveReplyTarget(const ClientState &state, const User *user)
{
	if (user && !user->username.empty())
		return user->username;
	if (!state.nickname.empty())
		return state.nickname;
	return "*";
}

inline bool requireRegistered(const ClientState &state, const std::string &target, ReplyCollector &replies)
{
	if (state.isRegistered)
		return true;
	replies.error(ErrorReply(ERR_NOTREGISTERED, target, "", "", "You have not registered"));
	return false;
}

inline bool requireParams(size_t have, size_t need, const std::string &target,
	const std::string &command, ReplyCollector &replies)
{
	if (have >= need)
		return true;
	replies.error(ErrorReply(ERR_NEEDMOREPARAMS, target, command, "", "Not enough parameters"));
	return false;
}

inline Channel *requireChannelExists(ChannelRepository &channelRepository, const std::string &channelName,
	const std::string &target, ReplyCollector &replies)
{
	Channel *channel = channelRepository.findChannelByChannelName(channelName);
	if (channel)
		return channel;
	replies.error(ErrorReply(ERR_NOSUCHCHANNEL, target, "", channelName, "No such channel"));
	return 0;
}

inline bool requireMembership(Channel &channel, int fd, const std::string &channelName,
	const std::string &target, ReplyCollector &replies)
{
	if (channel.isUserInChannel(fd))
		return true;
	replies.error(ErrorReply(ERR_NOTONCHANNEL, target, "", channelName, "You're not on that channel"));
	return false;
}

inline bool requireOperator(Channel &channel, int fd, const std::string &channelName,
	const std::string &target, ReplyCollector &replies)
{
	if (channel.isChannelOperator(fd))
		return true;
	replies.error(ErrorReply(ERR_CHANOPRIVSNEEDED, target, "", channelName, "You're not channel operator"));
	return false;
}
