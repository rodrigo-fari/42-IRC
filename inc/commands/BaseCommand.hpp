/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   BaseCommand.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 23:49:39 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/13 00:06:19 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core/UserRepository.hpp"
#include "core/ChannelRepository.hpp"
#include "parser/IrcParser.hpp"

class BaseCommand
{
protected:
    UserRepository& userRepository;
    ChannelRepository& channelRepository;

public:
    BaseCommand(UserRepository& ur, ChannelRepository& cr)
        : userRepository(ur), channelRepository(cr) {}

    virtual ~BaseCommand() {}

    virtual void execute(int fd, const MessagePayload& payload) = 0;
};