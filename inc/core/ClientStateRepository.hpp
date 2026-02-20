/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientStateRepository.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 00:07:30 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/17 14:18:31 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

#include <utility>
#include <iostream>

struct ClientState
{
	bool isRegistered;
	bool hasNickname;
	bool hasUsername;
	bool hasPassword;
	std::string nickname;
	std::string username;
};

class ClientStateRepository
{
	private:
		std::map<int, ClientState> status;

	public:
		ClientState& getClientStatus(int fd);
		void remove(int fd);
};
