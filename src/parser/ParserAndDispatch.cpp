/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserAndDispatch.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 21:41:16 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/18 00:38:38 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/ParserAndDispatch.hpp"

ParserAndDispatcher::ParserAndDispatcher(Dispatcher& dispatcher)
	: _dispatcher(dispatcher)
{
}

ParserAndDispatcher::~ParserAndDispatcher()
{
}

std::string ParserAndDispatcher::processData(int fd, const std::string& rawData)
{
	std::vector<std::string> framedMessages;
	std::string response;
	
	// Step 1: Frame - extract complete messages from raw data
	framedMessages = _framer.processRawData(fd, rawData);
	
	// Step 2: Parse and Dispatch - process each complete message
	for (size_t i = 0; i < framedMessages.size(); i++)
	{
		MessagePayload payload = parseMessage(framedMessages[i]);
		response += _dispatcher.dispatch(fd, payload);
	}
	
	return response;
}

void ParserAndDispatcher::clearClient(int fd)
{
	_framer.clearClient(fd);
}