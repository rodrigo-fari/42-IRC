/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ClientStateRepository.cpp                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 13:18:32 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/20 17:44:02 by rerodrig         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "core/ClientStateRepository.hpp"

ClientState &ClientStateRepository::getClientStatus(int fd)
{
	std::map<int, ClientState>::iterator it = status.find(fd);
	if (it == status.end())
	{
		ClientState cs;
		cs.isRegistered = false;
		cs.hasUsername = false;
		cs.hasPassword = false;
		cs.hasNickname = false;
		cs.closeAfterFlush = false;
		cs.nickname = "";
		cs.username = "";
		status.insert(std::make_pair(fd, cs));
		it = status.find(fd);
	}
	return (it->second);
}

void ClientStateRepository::remove(int fd)
{
	status.erase(fd);
}
