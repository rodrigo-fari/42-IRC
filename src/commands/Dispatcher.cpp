/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 21:41:21 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/15 18:10:19 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands/Dispatcher.hpp"

std::string dispatch(int fd, const MessagePayload& payload)
{
	(void)fd;
	std::string cmd = payload.command;
	for (size_t i = 0; i < cmd.length(); i++)
		cmd[i] = toupper(cmd[i]);
	if (cmd == "PING")
	{
		if (payload.params.size() >= 1)
			return ("PONG :" + payload.params[0] + "\r\n");
		return ("PONG\r\n");
	}
	return ("");
}

std::string handlePingCommand()
{
	
}