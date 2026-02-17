/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelRepository.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 23:27:43 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/17 14:18:31 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>
#include "commands/Channel.hpp"

class ChannelRepository
{
private:
    std::map<std::string, Channel> channelsByName;

public:
    Channel* findChannelByChannelName(const std::string& channelName);
    bool createChannel(const std::string& channelName);
    bool doesChannelExists(const std::string& channelName) const;
    bool removeChannel(const std::string& channelName);
};