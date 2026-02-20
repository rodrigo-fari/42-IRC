/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 21:41:21 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/20 15:38:57 by rerodrig         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "commandHandler/Dispatcher.hpp"
#include "commands/JoinCommand.hpp"
#include "commands/KickCommand.hpp"
#include "commands/PartCommand.hpp"
#include "commands/TopicCommand.hpp"
#include "commands/InviteCommand.hpp"
#include "commands/NickCommand.hpp"
#include "commands/UserCommand.hpp"

Dispatcher::Dispatcher(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr, const std::string &srv)
	: userRepository(ur), channelRepository(cr), clientStateRepository(csr), serverName(srv)
{
}

Dispatcher::~Dispatcher()
{
}

std::string Dispatcher::dispatch(int fd, const MessagePayload &payload)
{
	std::string cmd = payload.command;
	ClientState &state = clientStateRepository.getClientStatus(fd);

	// Convert command to uppercase
	for (size_t i = 0; i < cmd.length(); i++)
		cmd[i] = toupper(cmd[i]);

	// PING command
	if (cmd == "PING")
	{
		if (payload.params.size() >= 1)
			return ("PONG :" + payload.params[0] + "\r\n");
		return ("PONG\r\n");
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

	// PASS command (simplified for now)
	if (cmd == "PASS")
	{
		return "";
	}

	// NICK command
	if (cmd == "NICK")
	{
		bool wasRegistered = state.isRegistered;
		NickCommand nickCmd(userRepository, channelRepository, clientStateRepository, serverName);
		nickCmd.execute(fd, payload);
		if (!wasRegistered && state.isRegistered)
			return ":" + serverName + " 001 " + state.nickname + " :Welcome to " + serverName + ", " + state.nickname + "\r\n";
		return "";
	}

	// USER command
	if (cmd == "USER")
	{
		bool wasRegistered = state.isRegistered;
		UserCommand userCmd(userRepository, channelRepository, clientStateRepository, serverName);
		userCmd.execute(fd, payload);
		if (!wasRegistered && state.isRegistered)
			return ":" + serverName + " 001 " + state.nickname + " :Welcome to " + serverName + ", " + state.nickname + "\r\n";
		return "";
	}

	// Unknown command - just ignore for now
	return "";
}
