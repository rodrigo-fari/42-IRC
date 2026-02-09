/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcMessageFramer.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 19:11:23 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/09 19:52:36 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "inc/network/IrcMessageFramer.hpp"

std::vector<std::string> IrcMessageFramer::processRawData(int fd, const std::string& rawData)
{
	size_t pos;
	std::vector<std::string> extractedLines;
	
	_buffers[fd] += rawData;

	while ((pos = _buffers[fd].find("\r\n")) != std::string::npos)
	{
		std::string line = _buffers[fd].substr(0, pos);
		extractedLines
	}
} 	