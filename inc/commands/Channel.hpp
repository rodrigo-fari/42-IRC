#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include <vector>

class Channel {
    public:
        Channel();
        Channel(const std::string& channelName);

    // Members / Ops / Invited
    bool isUserInChannel(int fd) const;
    bool addUserToChannel(int fd);       // true se entrou agora
    bool removeUserFromChannel(int fd);  // true se removeu

    bool isChannelOperator(int fd) const;
    bool addChannelOperator(int fd);     // só se for membro
    bool removeChannelOperator(int fd);

    bool isUserInvited(int fd) const;
    void inviteUser(int fd);
    void uninviteUser(int fd);

    // Topic
    const std::string& getChannelTopic() const;
    void setTopic(const std::string& topic);

    // Info
    const std::string& getChannelName() const;
    bool empty() const;
    size_t usersCount() const;
    size_t operatorsCount() const;

    // Ordem de join pra broadcast e pra promover op
    std::vector<int> getUsersInChannelJoinOrder() const;

    // se ficar sem op, promove o mais antigo
    int ensureAtLeastOneOperator();

    //  Modes (ja prepara pro MODE depois
    bool inviteOnlyPolicy;          // +i
    bool topicLockPolicy;           // +t
    bool hasChannelPassword;        // +k
    std::string channelPassword;
    bool hasMaxUsersAmount;         // +l
    int maxUsersAmount;

private:
    std::string _channelName;
    std::string _channelTopic;

    std::set<int> _usersInChannel;        // members
    std::set<int> _channelOperators;      // ops
    std::set<int> _invitedUsers;          // invited

    std::vector<int> _usersJoinOrder;

    void _removeFromJoinOrder(int fd);
};

#endif