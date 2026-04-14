#include "../../inc/core/ChannelRepository.hpp"

// return pointer for real channel
Channel* ChannelRepository::findChannelByChannelName(const std::string& channelName) {
	std::map<std::string, Channel>::iterator it = channelsByName.find(channelName);
	if (it == channelsByName.end())
		return 0;
	return &it->second;
}

// check if this from map
bool ChannelRepository::doesChannelExists(const std::string& channelName) const {
	return channelsByName.find(channelName) != channelsByName.end();
}

// create enter from map with defaults
bool ChannelRepository::createChannel(const std::string& channelName) {
	if (doesChannelExists(channelName))
		return false;

	// uses Channel(channelName)
	channelsByName[channelName] = Channel(channelName);
	return true;
}

// remove from map
bool ChannelRepository::removeChannel(const std::string& channelName) {
	if (!doesChannelExists(channelName))
		return false;
	channelsByName.erase(channelName);
	return true;
}

std::vector<std::string> ChannelRepository::getChannelsForUser(int fd) const {
	std::vector<std::string> names;
	for (std::map<std::string, Channel>::const_iterator it = channelsByName.begin();
		 it != channelsByName.end(); ++it) {
		if (it->second.isUserInChannel(fd))
			names.push_back(it->first);
	}
	return names;
}

void ChannelRepository::removeUserFromAllChannels(int fd) {
	std::map<std::string, Channel>::iterator it = channelsByName.begin();
	while (it != channelsByName.end()) {
		Channel& channel = it->second;
		if (channel.isUserInChannel(fd)) {
			channel.removeUserFromChannel(fd);
			if (channel.empty()) {
				std::map<std::string, Channel>::iterator toErase = it++;
				channelsByName.erase(toErase);
				continue;
			}
			channel.ensureAtLeastOneOperator();
		}
		++it;
	}
}
