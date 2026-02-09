#include "../../inc/commands/Commands.hpp"
#include "../../inc/commands/Channel.hpp"
// falta include core.hpp "server e client"
#include <string>
#include <vector>

// helpers
// envia uma linha para um cliente
static void sendTo(User& c, const std::string& line) {
    c.enqueue.line + "\r\n";
}

// cria o prefixo de uma mensagem, username do cliente
static std::string prefix(const User& c) {
    return ":" + c.username;
}

// manda uma mensagem para todos os membros do canal
static void broadcastChannel(ServerState& st, Channel& ch, const std::string& line) {
    std::vector<int> members = ch.getUsersInChannelJoinOrder();
    for (size_t i = 0; i < members.size(); ++i) {
        int fileDescriptor = members[i];
        User* target = st.clientByFd(fileDescriptor);
        if (target)
            sendTo(*target, line);
    }
}

// envia o tópico do canal e a lista de membros para um cliente
static void replyTopicAndNames(ServerState& st, User& c, Channel& ch) {
    // TOPIC 331/332
    if (ch.getChannelTopic().empty())
        sendTo(c, "331 " + c.username + " " + ch.getChannelName() + " :No topic is set");
    else
        sendTo(c, "332 " + c.username + " " + ch.getChannelName() + " :" + ch.getChannelTopic());

    // NAMES 353/366
    std::vector<int> members = ch.getUsersInChannelJoinOrder();
    std::string names;
    bool first = true;
    for (size_t i = 0; i < members.size(); ++i) {
        User* m = st.clientByFd(members[i]);
        if (!m)
            continue;
        if (!first)
            names += " ";
        first = false;
        if (ch.isChannelOperator(m->fileDescriptor))
            names += "@";
        names += m->username;
    }
    sendTo(c, ":" + st.serverName + " 353 " + c.username + " = " + ch.getChannelName() + " :" + names);
    sendTo(c, ":" + st.serverName + " 366 " + c.username + " " + ch.getChannelName() + " :End of NAMES list");
}

// JOIN
void handleJoin(ServerState& st, User& c, const std::vector<std::string>& params) {
    if (!c.isRegistered) {
        sendTo(c, ":" + st.serverName + " 451 " + c.username + " JOIN :You have not isRegistered");
        return;
    }
    if (params.size() < 1) {
        sendTo(c, ":" + st.serverName + " 461 " + c.username + " JOIN :Not enough parameters");
        return;
    }
    const std::string chanName = params[0];
    const std::string providedKey = (params.size() >= 2) ? params[1] : "";

    // validacao basica nome do canal
    if (chanName.empty() || chanName[0] != '#') {
        sendTo(c, ":" + st.serverName + " 403 " + c.username + " " + chanName + " :No such channel");
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
    if (ch.isUserInChannel(c.fileDescriptor)) {
        sendTo(c, ":" + st.serverName + " 443 " + c.username + " " + chanName + " :is already on channel");
        return;
    }

    // i+ inviteUser only
    if (ch.inviteOnlyPolicy && !ch.isUserInvited(c.fileDescriptor)) {
        sendTo(c, ":" + st.serverName + " 473 " + c.username + " " + chanName + " :Cannot join channel (inviteUser only)");
        return;
    }

    // k+ channelPassword
    if (ch.hasChannelPassword && ch.channelPassword != providedKey) {
        sendTo(c, ":" + st.serverName + " 475 " + c.username + " " + chanName + " :Cannot join channel (bad channelPassword)");
        return;
    }

    // l+ maxUsersAmount
    if (ch.hasMaxUsersAmount && ch.usersCount() >= ch.maxUsersAmount) {
        sendTo(c, ":" + st.serverName + " 471 " + c.username + " " + chanName + " :Cannot join channel (channel is full)");
        return;
    }

    // adiciona o membro
    ch.addUserToChannel(c.fileDescriptor);
    ch.uninviteUser(c.fileDescriptor); // remove o convite se tiver

    // primeiro membro vira op
    if (create)
        ch.addChannelOperator(c.fileDescriptor);
    
    // broadcast join
    broadcastChannel(st, ch, prefix(c) + " JOIN " +  chanName);
    replyTopicAndNames(st, c, ch);
}
