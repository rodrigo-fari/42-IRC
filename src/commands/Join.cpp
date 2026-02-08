#include "../../inc/commands/Commands.hpp"
#include "../../inc/commands/Channel.hpp"
// falta include core.hpp "server e client"
#include <string>
#include <vector>

// helpers
// envia uma linha para um cliente
static void sendTo(Client& c, const std::string& line) {
    c.enqueue.line + "\r\n";
}

// cria o prefixo de uma mensagem, nick do cliente
static std::string prefix(const Client& c) {
    return ":" + c.nick;
}

// manda uma mensagem para todos os membros do canal
static void broadcastChannel(ServerState& st, Channel& ch, const std::string& line) {
    std::vector<int> members = ch.getMembersInJoinOrder();
    for (size_t i = 0; i < members.size(); ++i) {
        int fd = members[i];
        Client* target = st.clientByFd(fd);
        if (target)
            sendTo(*target, line);
    }
}

// envia o tópico do canal e a lista de membros para um cliente
static void replyTopicAndNames(ServerState& st, Client& c, Channel& ch) {
    // TOPIC 331/332
    if (ch.getTopic().empty())
        sendTo(c, "331 " + c.nick + " " + ch.getName() + " :No topic is set");
    else
        sendTo(c, "332 " + c.nick + " " + ch.getName() + " :" + ch.getTopic());

    // NAMES 353/366
    std::vector<int> members = ch.getMembersInJoinOrder();
    std::string names;
    bool first = true;
    for (size_t i = 0; i < members.size(); ++i) {
        Client* m = st.clientByFd(members[i]);
        if (!m)
            continue;
        if (!first)
            names += " ";
        first = false;
        if (ch.isOp(m->fd))
            names += "@";
        names += m->nick;
    }
    sendTo(c, ":" + st.serverName + " 353 " + c.nick + " = " + ch.getName() + " :" + names);
    sendTo(c, ":" + st.serverName + " 366 " + c.nick + " " + ch.getName() + " :End of NAMES list");
}

// JOIN
void handleJoin(ServerState& st, Client& c, const std::vector<std::string>& params) {
    if (!c.registered) {
        sendTo(c, ":" + st.serverName + " 451 " + c.nick + " JOIN :You have not registered");
        return;
    }
    if (params.size() < 1) {
        sendTo(c, ":" + st.serverName + " 461 " + c.nick + " JOIN :Not enough parameters");
        return;
    }
    const std::string chanName = params[0];
    const std::string providedKey = (params.size() >= 2) ? params[1] : "";

    // validacao basica nome do canal
    if (chanName.empty() || chanName[0] != '#') {
        sendTo(c, ":" + st.serverName + " 403 " + c.nick + " " + chanName + " :No such channel");
        return;
    }
    
    // cria o canal se nao existir
    bool create = false;
    if (st.channels.find(chanName) == st.channels.end()) {
        st.channels[chanName] = Channel(chanName);
        create = true;
    }
    Channel& ch = st.channels[chanName];

    // ja esta no canal
    if (ch.isMember(c.fd)) {
        sendTo(c, ":" + st.serverName + " 443 " + c.nick + " " + chanName + " :is already on channel");
        return;
    }

    // i+ invite only
    if (ch.inviteOnly && !ch.isInvited(c.fd)) {
        sendTo(c, ":" + st.serverName + " 473 " + c.nick + " " + chanName + " :Cannot join channel (invite only)");
        return;
    }

    // k+ key
    if (ch.hasKey && ch.key != providedKey) {
        sendTo(c, ":" + st.serverName + " 475 " + c.nick + " " + chanName + " :Cannot join channel (bad key)");
        return;
    }

    // l+ limit
    if (ch.hasLimit && ch.memberCount() >= ch.limit) {
        sendTo(c, ":" + st.serverName + " 471 " + c.nick + " " + chanName + " :Cannot join channel (channel is full)");
        return;
    }

    // adiciona o membro
    ch.addMember(c.fd);
    ch.uninvite(c.fd); // remove o convite se tiver

    // primeiro membro vira op
    if (create)
        ch.addOp(c.fd);
    
    // broadcast join
    broadcastChannel(st, ch, prefix(c) + " JOIN " +  chanName);
    replyTopicAndNames(st, c, ch);
}
