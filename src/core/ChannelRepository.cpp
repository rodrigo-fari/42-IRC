#include "../../inc/core/ChannelRepository.hpp"

// retorna ponteiro pro canal real
Channel* ChannelRepository::findChannelByChannelName(const std::string& channelName)
{
    std::map<std::string, Channel>::iterator it = channelsByName.find(channelName);
    if (it == channelsByName.end()) return 0;
    return &it->second;
}

// checa se tem no map
bool ChannelRepository::doesChannelExists(const std::string& channelName) const
{
    return channelsByName.find(channelName) != channelsByName.end();
}

// cria entrada no map com defaults
bool ChannelRepository::createChannel(const std::string& channelName)
{
    if (doesChannelExists(channelName)) return false;

    // usa construtor Channel(channelName)
    channelsByName[channelName] = Channel(channelName);
    return true;
}

// apaga do map
bool ChannelRepository::removeChannel(const std::string& channelName)
{
    if (!doesChannelExists(channelName)) return false;
    channelsByName.erase(channelName);
    return true;
}