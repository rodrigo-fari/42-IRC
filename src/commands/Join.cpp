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
        Client* member = st.clientByFd(members[i]);
        if (!m)
            continue;
        if (!first)
            names += " ";
        first = false;
        if (ch.isOp(m->fd))
            names += "@";
        names += m->nick;
    }
    sendTo(c, "353 " + c.nick + " = " + ch.getName() + " :" + names);
    sendTo(c, "366 " + c.nick + " " + ch.getName() + " :End of NAMES list");
}

// JOIN
void handleJoin(ServerState& st, Client& c, const std::vector<std::string>& params) {
    if (!client.registered) {
        sendTo(c, "451 " + c.nick + " JOIN :You have not registered");
        return;
    }
    if (params.size() < 1) {
        sendTo(client, ":" + st.serverName + " 461 " + client.nick + " JOIN :Not enough parameters");
        return;
    }
    const std::string chanName = params[0];
    const std::string providedKey = (params.size() >= 2) ? params[1] : "";

    // validacao basica nome do canal
    if (chanName.empty() || chanName[0] != '#') {
        sendTo(client, ":" + st.serverName + " 403 " + client.nick + " " + chanName + " :No such channel");
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
    if (ch.isMember(client.fd)) {
        sendTo(client, ":" + st.serverName + " 443 " + client.nick + " " + chanName + " :is already on channel");
        return;
    }

    // i+ invite only
    if (ch.inviteOnly && !ch.isInvited(client.fd)) {
        sendTo(client, ":" + st.serverName + " 473 " + client.nick + " " + chanName + " :Cannot join channel (invite only)");
        return;
    }

    // k+ key
    if (ch.hasKey && ch.key != providedKey) {
        sendTo(client, ":" + st.serverName + " 475 " + client.nick + " " + chanName + " :Cannot join channel (bad key)");
        return;
    }

    // l+ limit
    if (ch.hasLimit && ch.memberCount() >= ch.limit) {
        sendTo(client, ":" + st.serverName + " 471 " + client.nick + " " + chanName + " :Cannot join channel (channel is full)");
        return;
    }

    // adiciona o membro
    ch.addMember(client.fd);
    ch.uninvite(client.fd); // remove o convite se tiver

    // primeiro membro vira op
    if (create)
        ch.addOp(client.fd);
    
    // broadcast join
    broadcast(st, ch, prefix(client) + " JOIN " +  chanName);
    replyTopicAndNames(st, client, ch);
}
