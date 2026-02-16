#include "../../inc/commands/Channel.hpp"
#include <algorithm>

Channel::Channel()
: inviteOnlyPolicy(false),
  topicLockPolicy(false),
  hasChannelPassword(false),
  channelPassword(""),
  hasMaxUsersAmount(false),
  maxUsersAmount(0),
  _channelName(""),
  _channelTopic("") {}

Channel::Channel(const std::string& channelName)
: inviteOnlyPolicy(false),
  topicLockPolicy(false),
  hasChannelPassword(false),
  channelPassword(""),
  hasMaxUsersAmount(false),
  maxUsersAmount(0),
  _channelName(channelName),
  _channelTopic("") {}

const std::string& Channel::getChannelName() const { return _channelName; }

const std::string& Channel::getChannelTopic() const { return _channelTopic; }
void Channel::setTopic(const std::string& topic) { _channelTopic = topic; }

// verifica se o membro foi encontrado no conteiner
bool Channel::isUserInChannel(int fd) const {
    return _usersInChannel.find(fd) != _usersInChannel.end();
}

// verifica se o operador foi encontrado no conteiner
bool Channel::isChannelOperator(int fd) const {
    return _channelOperators.find(fd) != _channelOperators.end();
}

// tenta inserir no conteiner de membros, se foi novo registra
// se ja existia diz que nao adicionou
bool Channel::addUserToChannel(int fd) {
    std::pair<std::set<int>::iterator, bool> res = _usersInChannel.insert(fd);
    if (res.second) {
        _usersJoinOrder.push_back(fd);
        return true;
    }
    return false;
}

// se nao for membro retorna falso,
// se for remove do conteiner e do join order
bool Channel::removeUserFromChannel(int fd) {
    if (!isUserInChannel(fd)) return false;

    _usersInChannel.erase(fd);
    _channelOperators.erase(fd);
    _invitedUsers.erase(fd);
    _removeFromJoinOrder(fd);
    return true;
}

// verifica se e membro e tenta promover a opererador,
//se ja era operador ou nao era membro retorna falso
bool Channel::addChannelOperator(int fd) {
    if (!isUserInChannel(fd)) return false;
    std::pair<std::set<int>::iterator, bool> res = _channelOperators.insert(fd);
    return res.second;
}

// verifica se e operador e tenta remover a opererador
bool Channel::removeChannelOperator(int fd) {
    if (!isChannelOperator(fd)) return false;
    _channelOperators.erase(fd);
    return true;
}

// verifica se o membro esta na lista de convidados
bool Channel::isUserInvited(int fd) const {
    return _invitedUsers.find(fd) != _invitedUsers.end();
}

void Channel::inviteUser(int fd) { _invitedUsers.insert(fd); }
void Channel::uninviteUser(int fd) { _invitedUsers.erase(fd); }

bool Channel::empty() const { return _usersInChannel.empty(); }
size_t Channel::usersCount() const { return _usersInChannel.size(); }
size_t Channel::operatorsCount() const { return _channelOperators.size(); }

std::vector<int> Channel::getUsersInChannelJoinOrder() const {
    std::vector<int> out;
    for (size_t i = 0; i < _usersJoinOrder.size(); ++i) {
        int fd = _usersJoinOrder[i];
        if (isUserInChannel(fd)) out.push_back(fd);
    }
    return out;
}

// garante que haja pelo menos um operador no canal, promovendo o primeiro membro encontrado
int Channel::ensureAtLeastOneOperator() {
    if (operatorsCount() > 0) return -1;
    if (empty()) return -1;

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
        std::remove(_usersJoinOrder.begin(), _usersJoinOrder.end(), fd),
        _usersJoinOrder.end()
    );
}
