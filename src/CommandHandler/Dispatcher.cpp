/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 21:41:21 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/23 23:44:11 by rerodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commandHandler/Dispatcher.hpp"
#include <exception>
#include <iostream>
#include "commands/CapCommand.hpp"
#include "commands/CommandGuards.hpp"
#include "commands/InviteCommand.hpp"
#include "commands/JoinCommand.hpp"
#include "commands/KickCommand.hpp"
#include "commands/ModeCommand.hpp"
#include "commands/NickCommand.hpp"
#include "commands/PartCommand.hpp"
#include "commands/PassCommand.hpp"
#include "commands/PrivMsgCommand.hpp"
#include "commands/TopicCommand.hpp"
#include "commands/UserCommand.hpp"

Dispatcher::Dispatcher(
	UserRepository& ur,
	ChannelRepository& cr,
	ClientStateRepository& csr,
	const std::string& srv,
	const std::string& password
)
	: userRepository(ur), channelRepository(cr), clientStateRepository(csr), serverName(srv),
	  serverPassword(password) {
}

Dispatcher::~Dispatcher() {
}

DispatchResult Dispatcher::dispatch(int fd, const MessagePayload& payload) {
	DispatchResult result;
	ReplyCollector replies(serverName);

	try {
		std::string cmd = payload.command;
		for (size_t i = 0; i < cmd.length(); i++)
			cmd[i] = toupper(cmd[i]);

		ClientState& state = clientStateRepository.getClientStatus(fd);
		User* sender = userRepository.findUserByFileDescriptor(fd);
		const std::string target = resolveReplyTarget(state, sender);

		if (cmd.empty()) {
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "CAP") {
			CapCommand capCmd(userRepository, channelRepository, serverName);
			capCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "PING") {
			if (payload.params.size() >= 1)
				replies.raw(":" + serverName + " PONG " + serverName + " :" + payload.params[0]);
			else
				replies.raw(":" + serverName + " PONG " + serverName);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "JOIN") {
			JoinCommand joinCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			joinCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "KICK") {
			KickCommand kickCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			kickCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "PART") {
			PartCommand partCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			partCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "TOPIC") {
			TopicCommand topicCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			topicCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "INVITE") {
			InviteCommand inviteCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			inviteCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "MODE") {
			ModeCommand modeCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			modeCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "PRIVMSG") {
			PrivMsgCommand privMsgCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			privMsgCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "PASS") {
			PassCommand passCmd(
				userRepository, channelRepository, clientStateRepository, serverName, serverPassword
			);
			passCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "NICK") {
			NickCommand nickCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			nickCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		if (cmd == "USER") {
			UserCommand userCmd(
				userRepository, channelRepository, clientStateRepository, serverName
			);
			userCmd.execute(fd, payload, replies);
			result.wire = replies.flush();
			return result;
		}

		replies.error(ErrorReply(ERR_UNKNOWNCOMMAND, target, "", cmd, "Unknown command"));
	} catch (const std::exception& e) {
		std::cerr << "[DISPATCH ERROR] " << e.what() << std::endl;
		replies.raw(":" + serverName + " 451 * :Internal server error");
		result.fatalInternal = true;
	} catch (...) {
		std::cerr << "[DISPATCH ERROR] unknown" << std::endl;
		replies.raw(":" + serverName + " 451 * :Internal server error");
		result.fatalInternal = true;
	}

	result.wire = replies.flush();
	return result;
}
