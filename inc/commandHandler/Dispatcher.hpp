/* ************************************************************************** */
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

#include <cctype>
#include <string>
#include "commandHandler/DispatchResult.hpp"
#include "parser/IrcParser.hpp"
#include "core/UserRepository.hpp"
#include "core/ChannelRepository.hpp"
#include "core/ClientStateRepository.hpp"


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

	DispatchResult dispatch(int fd, const MessagePayload &payload);
};

