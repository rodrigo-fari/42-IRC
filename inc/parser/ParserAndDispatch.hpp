/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserAndDispatch.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 21:38:23 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/18 00:38:23 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "parser/IrcMessageFramer.hpp"
#include "parser/IrcParser.hpp"
#include "commandHandler/Dispatcher.hpp"
#include <string>

/**
 * @brief ParserAndDispatcher encapsulates the complete message processing pipeline
 * 
 * Manages framing, parsing, and dispatching of IRC messages in one coherent unit.
 * This class handles:
 * - Raw data buffering and line extraction (via IrcMessageFramer)
 * - Message parsing (via IrcParser)
 * - Command dispatching (via Dispatcher)
 */
class ParserAndDispatcher
{
private:
	IrcMessageFramer _framer;
	Dispatcher& _dispatcher;

public:
	ParserAndDispatcher(Dispatcher& dispatcher);
	~ParserAndDispatcher();

	/**
	 * @brief Process raw data from a client and return the response
	 * 
	 * @param fd File descriptor of the client
	 * @param rawData Raw data received from the client
	 * @return The response string to send back to the client
	 */
	std::string processData(int fd, const std::string& rawData);

	/**
	 * @brief Clear buffers for a disconnected client
	 * 
	 * @param fd File descriptor of the client
	 */
	void clearClient(int fd);
};