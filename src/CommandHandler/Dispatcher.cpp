/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 21:41:21 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/21 00:50:41 by rerodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commandHandler/Dispatcher.hpp"
#include "commands/JoinCommand.hpp"
#include "commands/KickCommand.hpp"
#include "commands/PartCommand.hpp"
#include "commands/TopicCommand.hpp"
#include "commands/InviteCommand.hpp"
#include "commands/NickCommand.hpp"
#include "commands/UserCommand.hpp"
#include "commands/CommandHelpers.hpp"

Dispatcher::Dispatcher(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr,
					   const std::string &srv, const std::string &password)
	: userRepository(ur), channelRepository(cr), clientStateRepository(csr),
	  serverName(srv), serverPassword(password)
{
}

Dispatcher::~Dispatcher()
{
}

std::string Dispatcher::dispatch(int fd, const MessagePayload &payload)
{
	std::string cmd = payload.command;
	ClientState &state = clientStateRepository.getClientStatus(fd);
	const bool wasRegistered = state.isRegistered;
	std::string response;

	// Convert command to uppercase
	for (size_t i = 0; i < cmd.length(); i++)
		cmd[i] = toupper(cmd[i]);

	// CAP negotiation
	if (cmd == "CAP")
	{
		std::string subcmd;
		if (subcmd == "LS")
			return ":" + serverName + " CAP * LS :\r\n";
		if (subcmd == "REQ")
		{
			std::string requested = payload.params.size() >= 2 ? payload.params[1] : "";
			return ":" + serverName + " CAP * NAK :" + requested + "\r\n";
		}
		return "";
	}

	// PING command
	if (cmd == "PING")
	{
		if (payload.params.size() >= 1)
			return (":" + serverName + " PONG " + serverName + " :" + payload.params[0] + "\r\n");
		return (":" + serverName + " PONG " + serverName + "\r\n");
	}

	// JOIN command
	if (cmd == "JOIN")
	{
		JoinCommand joinCmd(userRepository, channelRepository, clientStateRepository, serverName);
		joinCmd.execute(fd, payload);
		return "";
	}

	// KICK command
	if (cmd == "KICK")
	{
		KickCommand kickCmd(userRepository, channelRepository, clientStateRepository, serverName);
		kickCmd.execute(fd, payload);
		return "";
	}

	// PART command
	if (cmd == "PART")
	{
		PartCommand partCmd(userRepository, channelRepository, clientStateRepository, serverName);
		partCmd.execute(fd, payload);
		return "";
	}

	// TOPIC command
	if (cmd == "TOPIC")
	{
		TopicCommand topicCmd(userRepository, channelRepository, clientStateRepository, serverName);
		topicCmd.execute(fd, payload);
		return "";
	}

	// INVITE command
	if (cmd == "INVITE")
	{
		InviteCommand inviteCmd(userRepository, channelRepository, clientStateRepository, serverName);
		inviteCmd.execute(fd, payload);
		return "";
	}

	// PRIVMSG command
	if (cmd == "PRIVMSG")
	{
		User *sender = userRepository.findUserByFileDescriptor(fd);
		const std::string &target = payload.params[0];
		const std::string &message = payload.params[1];
		const std::string line = prefix(*sender) + " PRIVMSG " + target + " :" + message;

		if (target[0] == '#')
		{
			Channel *channel = channelRepository.findChannelByChannelName(target);
			std::vector<int> fds = channel->getUsersInChannelJoinOrder();
			for (size_t i = 0; i < fds.size(); ++i)
			{
				if (fds[i] == fd)
					continue;
				User *member = userRepository.findUserByFileDescriptor(fds[i]);
				if (member)
					sendTo(*member, line);
			}
			return "";
		}

		User *targetUser = userRepository.findUserByUsername(target);
		sendTo(*targetUser, line);
		return "";
	}

	// PASS command
	if (cmd == "PASS")
	{
		if (state.isRegistered)
			return ":" + serverName + " 462 * :You may not reregister\r\n";
		state.hasPassword = true;
		if (!wasRegistered && state.hasNickname && state.hasUsername)
		{
			bool created = userRepository.createUser(fd, state.nickname, "");
			if (created || userRepository.findUserByFileDescriptor(fd))
			{
				state.isRegistered = true;
				return ":" + serverName + " 001 " + state.nickname +
					   " :Welcome to the Internet Relay Network " + state.nickname + "\r\n";
			}
		}
		return "";
	}

	// NICK command
	if (cmd == "NICK")
	{
		NickCommand nickCmd(userRepository, channelRepository, clientStateRepository, serverName);
		nickCmd.execute(fd, payload);
		if (!wasRegistered && state.isRegistered)
		{
			response += ":" + serverName + " 001 " + state.nickname +
						" :Welcome to the Internet Relay Network " + state.nickname + "\r\n";
		}
		return response;
	}

	// USER command
	if (cmd == "USER")
	{

		UserCommand userCmd(userRepository, channelRepository, clientStateRepository, serverName);
		userCmd.execute(fd, payload);
		if (!wasRegistered && state.isRegistered)
		{
			response += ":" + serverName + " 001 " + state.nickname +
						" :Welcome to the Internet Relay Network " + state.nickname + "\r\n";
		}
		return response;
	}

	// Unknown command - just ignore for now
	return "";
}
