#include "../../inc/commands/Channel.hpp"
#include <iostream>
#include <algorithm>

Channel::Channel()
	: inviteOnlyPolicy(false), topicLockPolicy(false), hasChannelPassword(false),
	  channelPassword(""), hasMaxUsersAmount(false), maxUsersAmount(0), _channelName(""),
	  _channelTopic("") {
}

Channel::Channel(const std::string& channelName)
	: inviteOnlyPolicy(false), topicLockPolicy(false), hasChannelPassword(false),
	  channelPassword(""), hasMaxUsersAmount(false), maxUsersAmount(0), _channelName(channelName),
	  _channelTopic("") {
}

const std::string& Channel::getChannelName() const {
	return _channelName;
}

const std::string& Channel::getChannelTopic() const {
	return _channelTopic;
}
void Channel::setTopic(const std::string& topic) {
	_channelTopic = topic;
}

// checks if the member was found in the container
bool Channel::isUserInChannel(int fd) const {
	return _usersInChannel.find(fd) != _usersInChannel.end();
}

// checks if the OP was found in the container.
bool Channel::isChannelOperator(int fd) const {
	return _channelOperators.find(fd) != _channelOperators.end();
}

// Try adding it to the members container; if it's new, register it
// If it already existed, say it didn't add it
bool Channel::addUserToChannel(int fd) {
	std::pair<std::set<int>::iterator, bool> res = _usersInChannel.insert(fd);
	if (res.second) {
		_usersJoinOrder.push_back(fd);
		return true;
	}
	return false;
}

// if it's not a member, return false
// if it is, remove it from the container and join order
bool Channel::removeUserFromChannel(int fd) {
	if (!isUserInChannel(fd))
		return false;

	_usersInChannel.erase(fd);
	_channelOperators.erase(fd);
	_invitedUsers.erase(fd);
	_removeFromJoinOrder(fd);
	return true;
}

// check if they are a memeber and attempts to promote to op
// if they were already an op or not member, it return false
bool Channel::addChannelOperator(int fd) {
	if (!isUserInChannel(fd))
		return false;
	std::pair<std::set<int>::iterator, bool> res = _channelOperators.insert(fd);
	return res.second;
}

// check if it is an op and tries to remove the op
bool Channel::removeChannelOperator(int fd) {
	if (!isChannelOperator(fd))
		return false;
	_channelOperators.erase(fd);
	return true;
}

// check if it member is on the invite list
bool Channel::isUserInvited(int fd) const {
	return _invitedUsers.find(fd) != _invitedUsers.end();
}

void Channel::inviteUser(int fd) {
	_invitedUsers.insert(fd);
}
void Channel::uninviteUser(int fd) {
	_invitedUsers.erase(fd);
}

bool Channel::empty() const {
	return _usersInChannel.empty();
}
size_t Channel::usersCount() const {
	return _usersInChannel.size();
}
size_t Channel::operatorsCount() const {
	return _channelOperators.size();
}

std::vector<int> Channel::getUsersInChannelJoinOrder() const {
	std::vector<int> out;
	for (size_t i = 0; i < _usersJoinOrder.size(); ++i) {
		int fd = _usersJoinOrder[i];
		if (isUserInChannel(fd))
			out.push_back(fd);
	}
	return out;
}

// ir ensures there is at least one op on te channel. promoting the first member found
int Channel::ensureAtLeastOneOperator() {
	for (std::set<int>::iterator it = _channelOperators.begin(); it != _channelOperators.end();) {
		if (!isUserInChannel(*it)) {
			std::set<int>::iterator toErase = it++;
			_channelOperators.erase(toErase);
			continue;
		}
		++it;
	}

	std::cout << "OPERATOR " << operatorsCount() << std::endl;

	for (size_t i = 0; i < _usersJoinOrder.size(); ++i) {
		int fd = _usersJoinOrder[i];
		if (isUserInChannel(fd)) {
			addChannelOperator(fd);
			return fd;
		}
	}
	return -1;
}

void Channel::_removeFromJoinOrder(int fd) {
	_usersJoinOrder.erase(
		std::remove(_usersJoinOrder.begin(), _usersJoinOrder.end(), fd), _usersJoinOrder.end()
	);
}
