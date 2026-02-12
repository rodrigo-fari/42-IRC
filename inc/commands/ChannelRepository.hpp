#ifndef CHANNELREPOSITORY_HPP
#define CHANNELREPOSITORY_HPP

#include <map>
#include <string>
#include "Channel.hpp"

class ChannelRepository {
public:
    std::map<std::string, Channel> channelsByChannelName;

    Channel* findChannelByChannelName(const std::string& channelName);
    bool createChannel(const std::string& channelName);
    bool exists(const std::string& channelName) const;
    bool removeChannel(const std::string& channelName);
};

#endif
