/******************************************************************************/
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   UserRepository.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/18 00:35:00 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/19 22:52:19 by rerodrig         ###   ########.fr       */
/*                                                                            */
/******************************************************************************/

#include "../../inc/core/UserRepository.hpp"

User* UserRepository::findUserByUsername(const std::string& username)
{
	std::map<std::string, User>::iterator it = usersByUsername.find(username);
	if (it == usersByUsername.end())
		return 0;
	return &it->second;
}

User* UserRepository::findUserByFileDescriptor(int fd)
{
	std::map<int, User>::iterator it = usersByFileDescriptor.find(fd);
	if (it == usersByFileDescriptor.end())
		return 0;
	return &it->second;
}

bool UserRepository::createUser(int fd, const std::string& username, const std::string& password)
{
	// Check if user already exists by username
	if (usersByUsername.find(username) != usersByUsername.end())
		return false;
	
	// Check if fd is already in use
	if (usersByFileDescriptor.find(fd) != usersByFileDescriptor.end())
		return false;
	
	// Create new user
	User newUser;
	newUser.fileDescriptor = fd;
	newUser.username = username;
	newUser.password = password;
	
	// Store in both maps
	usersByUsername[username] = newUser;
	usersByFileDescriptor[fd] = newUser;
	
	return true;
}

bool UserRepository::removeUserByUsername(const std::string& username)
{
	std::map<std::string, User>::iterator it = usersByUsername.find(username);
	if (it == usersByUsername.end())
		return false;
	
	int fd = it->second.fileDescriptor;
	
	// Remove from both maps
	usersByUsername.erase(it);
	usersByFileDescriptor.erase(fd);
	
	return true;
}

bool UserRepository::removeUserByFileDescriptor(int fd)
{
	std::map<int, User>::iterator it = usersByFileDescriptor.find(fd);
	if (it == usersByFileDescriptor.end())
		return false;
	
	std::string username = it->second.username;
	
	// Remove from both maps
	usersByFileDescriptor.erase(it);
	usersByUsername.erase(username);
	
	return true;
}
