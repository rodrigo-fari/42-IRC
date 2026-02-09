#include "../../inc/commands/Commands.hpp"
#include "../../inc/commands/Channel.hpp"
// falta include core.hpp "server e client"
#include <string>
#include <vector>

//  Helpers locais copiados do Join
static void sendTo(User& c, const std::string& line) {
    c.enqueue(line + "\r\n");
}

static std::string prefix(const User& c) {
    return ":" + c.username;
}

static void broadcastChannel(ServerState& st, Channel& ch, const std::string& line) {
    std::vector<int> members = ch.getUsersInChannelJoinOrder();
    for (size_t i = 0; i < members.size(); ++i) {
        int fileDescriptor = members[i];
        User* target = st.clientsByFd[fileDescriptor];
        if (target)
            sendTo(*target, line);
    }
}

// PART
void handlePart(ServerState& st, User& c, const std::vector<std::string>& params, const std::string& trailing) {
    if (!c.isRegistered) {
        sendTo(c, ":" + st.serverName + " 451 " + c.username + " PART :You have not isRegistered");
        return;
    }
    if (params.size() < 1) {
        sendTo(c, ":" + st.serverName + " 461 " + c.username + " PART :Not enough parameters");
        return;
    }
    std::string chanName = params[0];

    if (st.channels.find(chanName) == st.channels.end()) {
        sendTo(c, ":" + st.serverName + " 403 " + c.username + " " + chanName + " :No such channel");
        return;
    }
    Channel& ch = st.channels[chanName];

    if (!ch.isUserInChannel(c.fileDescriptor)) {
        sendTo(c, ":" + st.serverName + " 442 " + c.username + " " + chanName + " :You're not on that channel");
        return;
    }
    std::string msg = trailing.empty() ? "" : " :" + trailing;

    // avisa todo mundo, inclusive o proprio antes de remover
    broadcastChannel(st, ch, prefix(c) + " PART " + chanName + msg);
    // remove o cliente do canal
    ch.removeUserFromChannel(c.fileDescriptor);
    // cannal vazio apaga o canal
    if (ch.empty()) {
        st.channels.erase(chanName);
        return;
    }
    // garante que haja pelo menos um operador, se o cliente que saiu era operador
    ch.ensureAtLeastOneOperator();
}