#include "../../inc/commands/Channel.hpp"
#include <algorithm>

Channel::Channel()
    : inviteOnly(false), topicLock(false), hasKey(false), key(""),
        hasLimit(false), limit(0) {}

Channel::Channel(const std::string& name)
    : inviteOnly(false), topicLock(false), hasKey(false), key(""), 
        hasLimit(false), limit(0), _name(name), _topic("") {}

const std::string& Channel::getName() const {return _name;}
const std::string& Channel::getTopic() const {return _topic;}

void Channel::setTopic(const std::string& topic) {_topic = topic;}

// verifica se o membro foi encontrado no conteiner 
bool Channel::isMember(int fd) const {
    return _members.find(fd) != _members.end();
}

// verifica se o operador foi encontrado no conteiner
bool Channel::isOp(int fd) const {
    return _ops.find(fd) != _ops.end();
}

// tenta inserir no conteiner de membros, se foi novo registra
// se ja existia diz que nao adicionou
bool Channel::addMember(int fd) {
    std::pair<std::set<int>::iterator, bool> res = _members.insert(fd);
    if (res.second) {
        _joinOrder.push_back(fd);
        return true;
    }
    return false;
}

// se nao for membro retorna falso, 
// se for remove do conteiner e do join order
bool Channel::removeMember(int fd) {
    if (!isMember(fd)) return false;

    _members.erase(fd);
    _ops.erase(fd);
    _invited.erase(fd);
    _removeFromJoinOrder(fd);
    return true;
}

// verifica se e membro e tenta promover a opererador,
//se ja era operador ou nao era membro retorna falso
bool Channel::addOp(int fd) {
    if (!isMember(fd)) return false;
    std::pair<std::set<int>::iterator, bool> res = _ops.insert(fd);
    return res.second;
}

// verifica se e operador e tenta remover a opererador
bool Channel::removeOp(int fd) {
    if (!isOp(fd)) return false;
    _ops.erase(fd);
    return true;
}

// verifica se o membro esta na lista de convidados
bool Channel::isInvited(int fd) const {
    return _invited.find(fd) != _invited.end();
}

void Channel::invite(int fd) { _invited.insert(fd); } // adiciona o fd no set _invited, marca o user como convidado
void Channel::uninvite(int fd) {_invited.erase(fd); } // remove o fd do set _invited, marca o user como nao convidado
bool Channel::empty() const { return _members.empty(); } // verifica se o canal esta vazio, nao tem membros
size_t Channel::memberCount() const { return _members.size(); } // retorna a quantidade de membros no canal
size_t Channel::opCount() const { return _ops.size(); } // retorna a quantidade de operadores no canal

 // retorna os membros do canal na ordem em que entraram
std::vector<int> Channel::getMembersInJoinOrder() const {
    std::vector<int> out;
    for (size_t i = 0; i < _joinOrder.size(); ++i) {
        int fd = _joinOrder[i];
        if (isMember(fd)) {
            out.push_back(fd);
        }
    }
    return out;
}

// garante que haja pelo menos um operador no canal, promovendo o primeiro membro encontrado
int Channel::ensureLeastOneOp() {
    if (opCount() > 0) { return -1; }
    if (empty()) { return -1; }

    for (size_t i = 0; i < _joinOrder.size(); ++i) {
        int fd = _joinOrder[i];
        if (isMember(fd)) {
            addOp(fd);
            return fd;
        }
    }
    return -1;
}

// remove o fd do vetor de join order, usado quando um membro sai do canal
void Channel::_removeFromJoinOrder(int fd) {
    _joinOrder.erase(std::remove(_joinOrder.begin(), _joinOrder.end(), fd),
    _joinOrder.end());
}