/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserRepository.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 23:10:59 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/12 23:48:11 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once


#include <string>
#include <map>
#include "parser/IrcParser.hpp"

struct User
{
	int fileDescriptor;
	std::string username;
	std::string password;
};

class UserRepository
{
	private:
		std::map<std::string, User> usersByUsername;
		std::map<int, User> usersByFileDescriptor;

	public:
		User findUserByUsername(std::string username);
		User findUserByFileDescriptor(int fd);
		bool createUser(int fd, std::string username, std::string password);
		bool removeUserByUsername(std::string username);
		bool removeUserByFileDescriptor(int fd);
};