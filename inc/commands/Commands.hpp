#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <vector>

struct ServerState;
class Client;

// JOIN params "#chan", "key"
void handleJoin(ServerState& st, Client& client, const std::vector<std::string>& params);

// PART params "#chan"(sem ":")
void handlePart(ServerState& st, Client& client, const std::vector<std::string>& params, const std::string& trailing);

#endif