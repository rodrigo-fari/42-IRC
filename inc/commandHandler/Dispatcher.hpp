/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 21:40:52 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/20 18:58:33 by rerodrig         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <cctype>
#include "parser/IrcParser.hpp"
#include "core/UserRepository.hpp"
#include "core/ChannelRepository.hpp"
#include "core/ClientStateRepository.hpp"
#include <sstream>

/**
 * @brief Dispatcher handles routing and executing IRC commands
 *
 * Manages command dispatch to appropriate command handlers based on
 * the received message payload.
 */
class Dispatcher
{
private:
	UserRepository &userRepository;
	ChannelRepository &channelRepository;
	ClientStateRepository &clientStateRepository;
	std::string serverName;
	std::string serverPassword;

public:
	Dispatcher(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr,
		const std::string &srv, const std::string &password);
	~Dispatcher();

	/**
	 * @brief Dispatch a message to the appropriate command handler
	 *
	 * @param fd File descriptor of the client
	 * @param payload The parsed message payload
	 * @return The response string to send back to the client
	 */
	std::string dispatch(int fd, const MessagePayload &payload);
};
