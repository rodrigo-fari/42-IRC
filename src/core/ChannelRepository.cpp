#include "../../inc/commands/ChannelRepository.hpp"

Channel* ChannelRepository::findChannelByChannelName(const std::string& channelName) {
	std::map<std::string, Channel>::iterator it = channelsByChannelName.find(channelName);
	if (it == channelsByChannelName.end()) return 0;
	return &it->second;
}

bool ChannelRepository::exists(const std::string& channelName) const {
	return channelsByChannelName.find(channelName) != channelsByChannelName.end();
}

bool ChannelRepository::createChannel(const std::string& channelName) {
	if (exists(channelName)) return false;
	channelsByChannelName[channelName] = Channel(channelName);
	return true;
}

bool ChannelRepository::removeChannel(const std::string& channelName) {
	if (!exists(channelName)) return false;
	channelsByChannelName.erase(channelName);
	return true;
}