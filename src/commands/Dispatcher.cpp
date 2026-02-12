/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 21:41:21 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/12 20:23:12 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands/Dispatcher.hpp"


//Debug function. (while commands are not ready yet).
static std::string payloadParamsToString(messagePayload payload)
{
	std::string singleParamsString;
	if (payload.command != "PING")
	singleParamsString.append(payload.command);
	singleParamsString.append(" ");
	for (size_t i = 0; i < payload.params.size(); i++)
	{
		singleParamsString.append(payload.params[i]);
		singleParamsString.append(" ");
	}
	return (singleParamsString);
}

static std::string itoa(int nbr)
{
	std::stringstream ss;
	ss << nbr;
	std::string str = ss.str();
	return (str);
}

std::string dispatch(int fd, const messagePayload& payload)
{
	std::string fdStr = "Solicitor: " + itoa(fd);
	std::string cmd = payload.command;
	for (size_t i = 0; i < cmd.length(); i++)
		cmd[i] = toupper(cmd[i]);
	if (cmd == "PING")
		return ("PONG " + fdStr);
	else
		cmd = payloadParamsToString(payload);
	return (cmd);
}

// Source - https://stackoverflow.com/q/5590381
// Posted by SAK, modified by community. See post 'Timeline' for change history
// Retrieved 2026-02-12, License - CC BY-SA 4.0



