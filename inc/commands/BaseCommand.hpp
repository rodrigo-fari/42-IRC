/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 23:49:39 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/12 23:52:59 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "repositories/UserRepository.hpp"
#include "repositories/ChannelRepository.hpp"
#include "parser/IrcParser.hpp"

class BaseCommand
{
	private:
		UserRepository& userRepository;
		ChannelRepository& channelRepository;

	public:
		void execute(int fd, MessagePayload payload);

};