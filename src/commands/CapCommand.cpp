#include "commands/CapCommand.hpp"
#include <cctype>

void CapCommand::execute(int fd, const MessagePayload& payload, ReplyCollector& replies) {
	(void)fd;
	std::string subcmd = payload.params.size() >= 1 ? payload.params[0] : "";
	for (size_t i = 0; i < subcmd.length(); i++)
		subcmd[i] = std::toupper(subcmd[i]);

	if (subcmd == "LS")
		replies.raw(":" + serverName + " CAP * LS :");
	else if (subcmd == "REQ") {
		std::string requested = payload.params.size() >= 2 ? payload.params[1] : "";
		replies.raw(":" + serverName + " CAP * NAK :" + requested);
	}
}
