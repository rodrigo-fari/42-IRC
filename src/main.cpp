/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 17:51:56 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/12 23:19:55 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/Core.hpp"
#include "network/IrcMessageFramer.hpp"
#include "parser/IrcParser.hpp"

int main(int argc, char **argv)
{
	{
		(void)argc;
		(void)argv;

		std::string a = "PRIVMSG #43 :Hello world!";
		std::string b = "    :nick!user@host PRIVMSG #a :hello world";
		std::string c = "PING :server";
		std::string d = "USER drigo 0 * :Rodrigo Silva";

		MessagePayload MP;

		//* TEST MessagePayload ver.1
		std::cout << "[Input]= " << a << "\n";
		MP = parseMessage(a);
		std::cout << "[Command]= " << MP.command << "\n";
		for(size_t i = 0; i < MP.params.size(); i++)
			std::cout << "[Param: " << i << "]= " << MP.params.at(i) << "\n";

		std::cout << "\n";

		//* TEST MessagePayload ver.2
		std::cout << "[Input]= " << b << "\n";
		MP = parseMessage(b);
		std::cout << "[Command]= " << MP.command << "\n";
		for(size_t i = 0; i < MP.params.size(); i++)
			std::cout << "[Param: " << i << "]= " << MP.params.at(i) << "\n";

		std::cout << "\n";

		//* TEST MessagePayload ver.3
		std::cout << "[Input]= " << c << "\n";
		MP = parseMessage(c);
		std::cout << "[Command]= " << MP.command << "\n";
		for(size_t i = 0; i < MP.params.size(); i++)
			std::cout << "[Param: " << i << "]= " << MP.params.at(i) << "\n";

		std::cout << "\n";

		//* TEST MessagePayload ver.4
		std::cout << "[Input]= " << d << "\n";
		MP = parseMessage(d);
		std::cout << "[Command]= " << MP.command << "\n";
		for(size_t i = 0; i < MP.params.size(); i++)
			std::cout << "[Param: " << i << "]= " << MP.params.at(i) << "\n";
	}
	return (0);
}
