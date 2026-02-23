/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 17:51:56 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/20 18:27:58 by rerodrig         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>
#include "../inc/core/Server.hpp"

static bool isValidPort(const char *arg)
{
	if (!arg || !*arg)
		return false;

	std::string s(arg);
	for (std::string::size_type i = 0; i < s.size(); ++i)
	{
		if (!std::isdigit(static_cast<unsigned char>(s[i])))
			return false;
	}

	long p = std::strtol(arg, 0, 10);
	return (p >= 1 && p <= 65535);
}

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: ./ircserv <port 1024-65535> <password>" << std::endl;
		return 1;
	}

	if (!isValidPort(argv[1]))
	{
		std::cerr << "Invalid port: " << argv[1] << std::endl;
		std::cerr << "Usage: ./ircserv <port 1024-65535> <password>" << std::endl;
		return 1;
	}

	std::string port = argv[1];
	std::string password = argv[2];

	try
	{
		Server server(port, password);
		server.init();
	}
	catch (const std::exception &e)
	{
		std::cerr << "Startup error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
