/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientStateRepository.hpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/13 00:07:30 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/13 00:13:27 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>

struct ClientState
{
	bool isRegistered;
	bool hasNickname;
	bool hasUsername;
	bool hasPassword;
};

class ClientStateRepository
{
	private:
		std::map<int, ClientState> status;

	public:
		ClientState& getClientStatus(int fd);
		void remove(int fd);
};

