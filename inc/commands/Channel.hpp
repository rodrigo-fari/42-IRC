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
        const std::string& getName() const;
        const std::string& getTopic() const;

        // members / ops
        bool isMember(int fd) const;
        bool isOp(int fd) const;

        bool addMember(int fd);
        bool removeMember(int fd);
        bool addOp(int fd);
        bool removeOp(int fd);

        // invites
        bool isInvited(int fd) const;
        void invite(int fd);
        void uninvite(int fd);

        // info util
        bool empty() const;
        size_t memberCount() const;
        size_t opCount() const;

        // return members
        std::vector<int> getMembersInJoinOrder() const;

        // promote member to operator
        int ensureLeastOneOp();

        // modes
        bool inviteOnly; // +i
        bool topicLock; // +t
        bool hasKey; // +k
        std::string key; 
        bool hasLimit; // +l
        int limit;

        void setTopic(const std::string& topic);

    private:
        std::string _name;
        std::string _topic;

        std::set<int> _members;
        std::set<int> _ops;
        std::set<int> _invited;

        std::vector<int> _joinOrder; // to maintain the order of members joining

        void _removeFromJoinOrder(int fd);
};

#endif