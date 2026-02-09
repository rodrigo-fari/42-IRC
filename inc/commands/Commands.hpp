#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>

struct ServerState;
class User;

// JOIN params "#chan", "channelPassword"
void handleJoin(ServerState& st, User& client, const std::vector<std::string>& params);

// PART params "#chan"(sem ":")
void handlePart(ServerState& st, User& client, const std::vector<std::string>& params, const std::string& trailing);

#endif