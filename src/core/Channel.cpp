#include "../../inc/commands/Channel.hpp"
#include <algorithm>

Channel::Channel()
    : inviteOnlyPolicy(false), topicLockPolicy(false), hasChannelPassword(false), channelPassword(""),
        hasMaxUsersAmount(false), maxUsersAmount(0) {}

Channel::Channel(const std::string& name)
    : inviteOnlyPolicy(false), topicLockPolicy(false), hasChannelPassword(false), channelPassword(""),
        hasMaxUsersAmount(false), maxUsersAmount(0), _channelName(name), _channelTopic("") {}

const std::string& Channel::getChannelName() const {return _channelName;}
const std::string& Channel::getChannelTopic() const {return _channelTopic;}

void Channel::setTopic(const std::string& topic) {_channelTopic = topic;}

// verifica se o membro foi encontrado no conteiner
bool Channel::isUserInChannel(int fileDescriptor) const {
    return _usersInChannel.find(fileDescriptor) != _usersInChannel.end();
}

// verifica se o operador foi encontrado no conteiner
bool Channel::isChannelOperator(int fileDescriptor) const {
    return _channelOperators.find(fileDescriptor) != _channelOperators.end();
}

// tenta inserir no conteiner de membros, se foi novo registra
// se ja existia diz que nao adicionou
bool Channel::addUserToChannel(int fileDescriptor) {
    std::pair<std::set<int>::iterator, bool> res = _usersInChannel.insert(fileDescriptor);
    if (res.second) {
        _usersJoinOrder.push_back(fileDescriptor);
        return true;
    }
    return false;
}

// se nao for membro retorna falso,
// se for remove do conteiner e do join order
bool Channel::removeUserFromChannel(int fileDescriptor) {
    if (!isUserInChannel(fileDescriptor)) return false;

    _usersInChannel.erase(fileDescriptor);
    _channelOperators.erase(fileDescriptor);
    _invitedUsers.erase(fileDescriptor);
    _removeFromUsersJoinOrder(fileDescriptor);
    return true;
}

// verifica se e membro e tenta promover a opererador,
//se ja era operador ou nao era membro retorna falso
bool Channel::addChannelOperator(int fileDescriptor) {
    if (!isUserInChannel(fileDescriptor)) return false;
    std::pair<std::set<int>::iterator, bool> res = _channelOperators.insert(fileDescriptor);
    return res.second;
}

// verifica se e operador e tenta remover a opererador
bool Channel::removeChannelOperator(int fileDescriptor) {
    if (!isChannelOperator(fileDescriptor)) return false;
    _channelOperators.erase(fileDescriptor);
    return true;
}

// verifica se o membro esta na lista de convidados
bool Channel::isUserInvited(int fileDescriptor) const {
    return _invitedUsers.find(fileDescriptor) != _invitedUsers.end();
}

void Channel::inviteUser(int fileDescriptor) { _invitedUsers.insert(fileDescriptor); }
void Channel::uninviteUser(int fileDescriptor) {_invitedUsers.erase(fileDescriptor); }
bool Channel::empty() const { return _usersInChannel.empty(); }
size_t Channel::usersCount() const { return _usersInChannel.size(); }
size_t Channel::operatorsCount() const { return _channelOperators.size(); }

std::vector<int> Channel::getUsersInChannelJoinOrder() const {
    std::vector<int> out;
    for (size_t i = 0; i < _usersJoinOrder.size(); ++i) {
        int fileDescriptor = _usersJoinOrder[i];
        if (isUserInChannel(fileDescriptor)) {
            out.push_back(fileDescriptor);
        }
    }
    return out;
}

// garante que haja pelo menos um operador no canal, promovendo o primeiro membro encontrado
int Channel::ensureAtLeastOneOperator() {
    if (operatorsCount() > 0) { return -1; }
    if (empty()) { return -1; }

    for (size_t i = 0; i < _usersJoinOrder.size(); ++i) {
        int fileDescriptor = _usersJoinOrder[i];
        if (isUserInChannel(fileDescriptor)) {
            addChannelOperator(fileDescriptor);
            return fileDescriptor;
        }
    }
    return -1;
}

void Channel::_removeFromUsersJoinOrder(int fileDescriptor) {
    _usersJoinOrder.erase(std::remove(_usersJoinOrder.begin(), _usersJoinOrder.end(), fileDescriptor),
    _usersJoinOrder.end());
}
