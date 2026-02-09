#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <vector>

class Channel {
    public:
        Channel();
        Channel(const std::string& name);

        // getters
        const std::string& getChannelName() const;
        const std::string& getChannelTopic() const;

        // members / ops
        bool isUserInChannel(int fileDescriptor) const;
        bool isChannelOperator(int fileDescriptor) const;

        bool addUserToChannel(int fileDescriptor);
        bool removeUserFromChannel(int fileDescriptor);
        bool addChannelOperator(int fileDescriptor);
        bool removeChannelOperator(int fileDescriptor);

        // invites
        bool isUserInvited(int fileDescriptor) const;
        void inviteUser(int fileDescriptor);
        void uninviteUser(int fileDescriptor);

        // info util
        bool empty() const;
        size_t usersCount() const;
        size_t operatorsCount() const;

        // return members
        std::vector<int> getUsersInChannelJoinOrder() const;

        // promote member to operator
        int ensureAtLeastOneOperator();

        // modes
        bool inviteOnlyPolicy; // +i
        bool topicLockPolicy; // +t
        bool hasChannelPassword; // +k
        std::string channelPassword;
        bool hasMaxUsersAmount; // +l
        int maxUsersAmount;

        void setTopic(const std::string& topic);

    private:
        std::string _channelName;
        std::string _channelTopic;

        std::set<int> _usersInChannel;
        std::set<int> _channelOperators;
        std::set<int> _invitedUsers;

        std::vector<int> _usersJoinOrder; // to maintain the order of members joining

        void _removeFromUsersJoinOrder(int fileDescriptor);
};

#endif