/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ParserAndDispatch.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 21:38:23 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/20 00:53:14 by rerodrig         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#pragma once

#include "commandHandler/Dispatcher.hpp"
#include "parser/IrcMessageFramer.hpp"
#include "parser/IrcParser.hpp"
#include "commandHandler/DispatchResult.hpp"

class ParserAndDispatcher
{
private:
	IrcMessageFramer _framer;
	Dispatcher& _dispatcher;

public:
	ParserAndDispatcher(Dispatcher& dispatcher);
	~ParserAndDispatcher();

	DispatchResult processData(int fd, const std::string& rawData);
	void clearClient(int fd);
};
