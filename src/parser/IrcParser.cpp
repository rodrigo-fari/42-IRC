/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcParser.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 14:46:20 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/12 23:19:55 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/IrcParser.hpp"


static std::string getCommand(const std::string& line, size_t& i)
{
	size_t pos = i;
	while (i < line.size() && line[i] != ' ')
		i++;
	return (line.substr(pos, i - pos));
}

static std::vector<std::string> getArgs(const std::string& line, size_t& i)
{
	std::vector<std::string> args;

	while (i < line.size())
	{
		while (i < line.size() && line[i] == ' ')
			i++;

		if (i >= line.size())
			break;
		if (line[i] == ':')
		{
			args.push_back(line.substr(i + 1));
			break;
		}
		size_t pos = i;
		while (i < line.size() && line[i] != ' ')
			i++;
		args.push_back(line.substr(pos, i - pos));
	}
	return (args);
}

MessagePayload parseMessage(const std::string& line)
{
	MessagePayload payload;
	size_t i = 0;

	if (line.empty())
		return payload;

	while (i < line.size() && line[i] == ' ')
		i++;
	if (i < line.size() && line[i] == ':')
	{
		while (i < line.size() && line[i] != ' ')
			i++;
	}
	if (i < line.size() && line[i] == ' ')
		i += 1;
	payload.command = getCommand(line, i);
	payload.params = getArgs(line, i);
	return (payload);
}
