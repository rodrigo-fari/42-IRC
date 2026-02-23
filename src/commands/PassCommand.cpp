#include "commands/PassCommand.hpp"
#include "commands/CommandGuards.hpp"

void PassCommand::execute(int fd, const MessagePayload& payload, ReplyCollector &replies)
{
	ClientState &state = clientStateRepository.getClientStatus(fd);
	User *user = userRepository.findUserByFileDescriptor(fd);
	const std::string target = resolveReplyTarget(state, user);

	if (state.isRegistered)
	{
		replies.error(ErrorReply(ERR_ALREADYREGISTERED, target, "", "", "You may not reregister"));
		return;
	}

	if (!requireParams(payload.params.size(), 1, target, "PASS", replies))
		return;

	if (payload.params[0] != serverPassword)
	{
		replies.error(ErrorReply(ERR_PASSWDMISMATCH, target, "", "", "Password incorrect"));
		return;
	}

	state.hasPassword = true;
	if (!state.isRegistered && state.hasPassword && state.hasNickname && state.hasUsername)
	{
		bool created = userRepository.createUser(fd, state.nickname, "");
		if (created || userRepository.findUserByFileDescriptor(fd))
		{
			state.isRegistered = true;
			replies.raw(":" + serverName + " 001 " + state.nickname +
				" :Welcome to the Internet Relay Network " + state.nickname);
		}
	}
}
