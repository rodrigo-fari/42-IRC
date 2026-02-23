/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserAndDispatch.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 21:41:16 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/20 02:14:08 by rerodrig         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "parser/ParserAndDispatch.hpp"

ParserAndDispatcher::ParserAndDispatcher(Dispatcher& dispatcher)
	: _dispatcher(dispatcher)
{
}

ParserAndDispatcher::~ParserAndDispatcher()
{
}

DispatchResult ParserAndDispatcher::processData(int fd, const std::string& rawData)
{
	std::vector<std::string> framedMessages;
	DispatchResult aggregated;

	framedMessages = _framer.processRawData(fd, rawData);
	for (size_t i = 0; i < framedMessages.size(); i++)
	{
		MessagePayload payload = parseMessage(framedMessages[i]);
		DispatchResult result = _dispatcher.dispatch(fd, payload);
		aggregated.wire += result.wire;
		aggregated.closeAfterFlush = aggregated.closeAfterFlush || result.closeAfterFlush;
		aggregated.fatalInternal = aggregated.fatalInternal || result.fatalInternal;
	}

	return aggregated;
}

void ParserAndDispatcher::clearClient(int fd)
{
	_framer.clearClient(fd);
}
