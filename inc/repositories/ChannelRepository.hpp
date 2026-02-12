/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChannelRepository.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 23:27:43 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/12 23:37:17 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <string>
#include <map>
#include "repositories/UserRepository.hpp"

struct Channel
{
	std::vector<User&> usersInChannel;
	std::string channelPassword;
	bool inviteOnlyPolicy;
	int maxUsersAmount;
};

class ChannelRepository
{
	private:
		std::map<std::string, Channel> channelsByName;

	public:
		Channel findChannelbyChannelName(std::string);
		bool createChannel(std::string channelName);
		bool doesChannelExists(std::string channelName);
		bool removeChannel(std::string channelName);
};
