#include "../../inc/commands/ModeCommand.hpp"
#include "../../inc/commands/CommandHelpers.hpp"
#include <vector>
#include <climits>
#include <cerrno>
#include <cstdlib>

void ModeCommand::execute(int fd, const MessagePayload& payload) {
	User* sender = userRepository.findUserByFileDescriptor(fd);
	if (!sender)
		return;

	(void)clientStateRepository;
	(void)serverName;

	// Expected shape: MODE #channel <mode_tokens...> [mode_args...]
	if (payload.params.size() < 2)
		return;

	const std::string& channelName = payload.params[0];
	Channel* channel = channelRepository.findChannelByChannelName(channelName);
	if (!channel)
		return;

	std::vector<std::string> modeTokens;
	size_t tokenIndex = 1;
	while (tokenIndex < payload.params.size() && !payload.params[tokenIndex].empty() &&
		   (payload.params[tokenIndex][0] == '+' || payload.params[tokenIndex][0] == '-')) {
		modeTokens.push_back(payload.params[tokenIndex]);
		++tokenIndex;
	}

	if (modeTokens.empty())
		return;

	//validates syntax and collects parsed items.
	std::vector<char> parsedSigns;
	std::vector<char> parsedModes;
	std::vector<std::string> parsedArgs;

	for (size_t i = 0; i < modeTokens.size(); ++i) {
		const std::string& token = modeTokens[i];
		char sign = '\0';

		for (size_t j = 0; j < token.size(); ++j) {
			const char c = token[j];

			if (c == '+' || c == '-') {
				sign = c;
				continue;
			}

			if (sign == '\0') {
				sendTo(
					*sender,
					":" + serverName + " 472 " + sender->username + " " + channelName + " " +
						std::string(1, c) + " :is unknown mode char to me"
				);
				continue;
			}

			if (c == 'i' || c == 't' || c == 'k' || c == 'l' || c == 'o') {
				parsedSigns.push_back(sign);
				parsedModes.push_back(c);
				continue;
			}

			sendTo(
				*sender,
				":" + serverName + " 472 " + sender->username + " " + channelName + " " +
					std::string(1, c) + " :is unknown mode char to me"
			);
		}
	}

	// consume mode arguments in order.
	size_t argIndex = tokenIndex;
	for (size_t i = 0; i < parsedModes.size(); ++i) {
		const char sign = parsedSigns[i];
		const char mode = parsedModes[i];
		const bool needsArg = ((mode == 'k' && sign == '+') ||
							   (mode == 'l' && sign == '+') ||
							   (mode == 'o'));

		if (!needsArg) {
			parsedArgs.push_back("");
			continue;
		}

		if (argIndex >= payload.params.size()) {
			sendTo(
				*sender,
				":" + serverName + " 461 " + sender->username + " MODE " +
					std::string(1, sign) + std::string(1, mode) + " :Not enough parameters"
			);
			parsedArgs.push_back("");
			continue;
		}

		parsedArgs.push_back(payload.params[argIndex]);
		++argIndex;
	}

	// validate +l argument as positive integer.
	for (size_t i = 0; i < parsedModes.size(); ++i) {
		if (!(parsedSigns[i] == '+' && parsedModes[i] == 'l'))
			continue;
		if (parsedArgs[i].empty())
			continue;

		bool allDigits = true;
		for (size_t j = 0; j < parsedArgs[i].size(); ++j) {
			if (parsedArgs[i][j] < '0' || parsedArgs[i][j] > '9') {
				allDigits = false;
				break;
			}
		}

		bool valid = allDigits;
		if (valid) {
			errno = 0;
			char* end = 0;
			long value = std::strtol(parsedArgs[i].c_str(), &end, 10);
			if (errno != 0 || end == parsedArgs[i].c_str() || *end != '\0' || value <= 0 || value > INT_MAX)
				valid = false;
		}

		if (!valid) {
			sendTo(
				*sender,
				":" + serverName + " 696 " + sender->username + " " + channelName +
					" l :Invalid mode parameter"
			);
			parsedArgs[i].clear();
		}
	}

	// validate +/-o target nickname and membership.
	for (size_t i = 0; i < parsedModes.size(); ++i) {
		if (parsedModes[i] != 'o')
			continue;
		if (parsedArgs[i].empty())
			continue;

		User* target = userRepository.findUserByUsername(parsedArgs[i]);
		if (!target) {
			sendTo(
				*sender,
				":" + serverName + " 401 " + sender->username + " " + parsedArgs[i] + " :No such nick"
			);
			parsedArgs[i].clear();
			continue;
		}

		if (!channel->isUserInChannel(target->fileDescriptor)) {
			sendTo(
				*sender,
				":" + serverName + " 441 " + sender->username + " " + parsedArgs[i] + " " + channelName +
					" :They aren't on that channel"
			);
			parsedArgs[i].clear();
			continue;
		}
	}

	//first apply pass for no-arg flags (+/-i, +/-t).
	for (size_t i = 0; i < parsedModes.size(); ++i) {
		if (parsedModes[i] == 'i') {
			channel->inviteOnlyPolicy = (parsedSigns[i] == '+');
			continue;
		}
		if (parsedModes[i] == 't') {
			channel->topicLockPolicy = (parsedSigns[i] == '+');
			continue;
		}
	}

	// apply key/limit flags (+/-k, +/-l).
	for (size_t i = 0; i < parsedModes.size(); ++i) {
		if (parsedModes[i] == 'k') {
			if (parsedSigns[i] == '+') {
				if (parsedArgs[i].empty())
					continue;
				channel->hasChannelPassword = true;
				channel->channelPassword = parsedArgs[i];
			} else {
				channel->hasChannelPassword = false;
				channel->channelPassword.clear();
			}
			continue;
		}

		if (parsedModes[i] == 'l') {
			if (parsedSigns[i] == '+') {
				if (parsedArgs[i].empty())
					continue;
				errno = 0;
				char* end = 0;
				long value = std::strtol(parsedArgs[i].c_str(), &end, 10);
				if (errno != 0 || end == parsedArgs[i].c_str() || *end != '\0' ||
					value <= 0 || value > INT_MAX)
					continue;
				channel->hasMaxUsersAmount = true;
				channel->maxUsersAmount = static_cast<int>(value);
			} else {
				channel->hasMaxUsersAmount = false;
				channel->maxUsersAmount = 0;
			}
			continue;
		}
	}

	// apply operator flag (+/-o) for validated targets.
	for (size_t i = 0; i < parsedModes.size(); ++i) {
		if (parsedModes[i] != 'o')
			continue;
		if (parsedArgs[i].empty())
			continue;

		User* target = userRepository.findUserByUsername(parsedArgs[i]);
		if (!target)
			continue;

		if (parsedSigns[i] == '+')
			channel->addChannelOperator(target->fileDescriptor);
		else
			channel->removeChannelOperator(target->fileDescriptor);
	}

	// Debug print parsed signs, modes, and bound args
	std::cout << "[MODE PARSE] parsedSigns: ";
	for (size_t i = 0; i < parsedSigns.size(); ++i)
		std::cout << parsedSigns[i] << ' ';
	std::cout << "\n[MODE PARSE] parsedModes: ";
	for (size_t i = 0; i < parsedModes.size(); ++i)
		std::cout << parsedModes[i] << ' ';
	std::cout << "\n[MODE PARSE] parsedArgs: ";
	for (size_t i = 0; i < parsedArgs.size(); ++i)
		std::cout << "[" << parsedArgs[i] << "] ";
	std::cout << "\n[MODE APPLY] inviteOnly=" << (channel->inviteOnlyPolicy ? "on" : "off")
			  << " topicLock=" << (channel->topicLockPolicy ? "on" : "off")
			  << " hasKey=" << (channel->hasChannelPassword ? "on" : "off")
			  << " key=[" << channel->channelPassword << "]"
			  << " hasLimit=" << (channel->hasMaxUsersAmount ? "on" : "off")
			  << " limit=" << channel->maxUsersAmount
			  << " operators=" << channel->operatorsCount() << std::endl;

}
