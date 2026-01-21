/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 17:51:56 by rde-fari          #+#    #+#             */
/*   Updated: 2026/01/21 17:43:42 by rerodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/core/Core.hpp"

int main(int argc, char **argv)
{
	const std::string port = (argc >= 2) ? argv[1] : "6667";
	Server server(port);
	server.run();
	return 0;
}
