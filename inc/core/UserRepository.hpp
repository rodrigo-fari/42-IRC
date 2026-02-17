/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserRepository.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/12 23:10:59 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/17 14:18:31 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <map>

#include <vector>

struct User
{
	int fileDescriptor;
	std::string username;
	std::string password;

	std::vector<std::string> outbox;

	void enqueue(const std::string& msg) {
		outbox.push_back(msg);
	}
};

class UserRepository
{
private:
	std::map<std::string, User> usersByUsername;
	std::map<int, User> usersByFileDescriptor;

public:
	User* findUserByUsername(const std::string& username);
	User* findUserByFileDescriptor(int fd);

	bool createUser(int fd, const std::string& username, const std::string& password);
	bool removeUserByUsername(const std::string& username);
	bool removeUserByFileDescriptor(int fd);
};