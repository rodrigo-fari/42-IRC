#include "../../inc/commands/ModeCommand.hpp"
#include "../../inc/commands/CommandHelpers.hpp"
#include <vector>
#include <climits>
#include <cerrno>
#include <cstdlib>
#include <sstream>

void ModeCommand::execute(int fd, const MessagePayload& payload) {
    User* sender = userRepository.findUserByFileDescriptor(fd);
    if (!sender) return;

    // needs to be registered
    ClientState& st = clientStateRepository.getClientStatus(fd);
    if (!st.isRegistered) {
        sendTo(*sender, ":" + serverName + " 451 " + sender->username + " :You have not registered");
        return;
    }

    // You need at least the channel
    if (payload.params.size() < 1) {
        sendTo(*sender, ":" + serverName + " 461 " + sender->username + " MODE :Not enough parameters");
        return;
    }

    const std::string& channelName = payload.params[0];
    Channel* channel = channelRepository.findChannelByChannelName(channelName);
    if (!channel) {
        sendTo(*sender, ":" + serverName + " 403 " + sender->username + " " + channelName + " :No such channel");
        return;
    }

    // You have to be a member
    if (!channel->isUserInChannel(sender->fileDescriptor)) {
        sendTo(*sender, ":" + serverName + " 442 " + sender->username + " " + channelName + " :You're not on that channel");
        return;
    }

    // set MODE #chan
    if (payload.params.size() == 1) {
        std::string modes = "+";
        std::vector<std::string> args;

        if (channel->inviteOnlyPolicy) modes += "i";
        if (channel->topicLockPolicy)  modes += "t";

        if (channel->hasChannelPassword) {
            modes += "k";
            args.push_back(channel->channelPassword);
        }
        if (channel->hasMaxUsersAmount) {
            modes += "l";
            std::stringstream ss;
            ss << channel->maxUsersAmount;
            args.push_back(ss.str());
        }

        if (modes == "+") modes.clear();

        std::string line = ":" + serverName + " 324 " + sender->username + " " + channelName;
        if (!modes.empty()) {
            line += " " + modes;
            for (size_t i = 0; i < args.size(); ++i)
                line += " " + args[i];
        }
        sendTo(*sender, line);
        return;
    }

    // Get mode tokens +i -k +o etc
    std::vector<std::string> modeTokens;
    size_t tokenIndex = 1;
    while (tokenIndex < payload.params.size() && !payload.params[tokenIndex].empty() &&
           (payload.params[tokenIndex][0] == '+' || payload.params[tokenIndex][0] == '-')) {
        modeTokens.push_back(payload.params[tokenIndex]);
        ++tokenIndex;
    }

    if (modeTokens.empty())
        return;

    // Only OP can change it
    if (!channel->isChannelOperator(sender->fileDescriptor)) {
        sendTo(*sender, ":" + serverName + " 482 " + sender->username + " " + channelName + " :You're not channel operator");
        return;
    }

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
                sendTo(*sender, ":" + serverName + " 472 " + sender->username + " " + channelName + " " +
                               std::string(1, c) + " :is unknown mode char to me");
                continue;
            }

            if (c == 'i' || c == 't' || c == 'k' || c == 'l' || c == 'o') {
                parsedSigns.push_back(sign);
                parsedModes.push_back(c);
                continue;
            }

            sendTo(*sender, ":" + serverName + " 472 " + sender->username + " " + channelName + " " +
                           std::string(1, c) + " :is unknown mode char to me");
        }
    }

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
            sendTo(*sender, ":" + serverName + " 461 " + sender->username + " MODE " +
                           std::string(1, sign) + std::string(1, mode) + " :Not enough parameters");
            parsedArgs.push_back("");
            continue;
        }

        parsedArgs.push_back(payload.params[argIndex]);
        ++argIndex;
    }

    // validate +l
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
            sendTo(*sender, ":" + serverName + " 696 " + sender->username + " " + channelName +
                           " l :Invalid mode parameter");
            parsedArgs[i].clear();
        }
    }

    // validate +/-o
    for (size_t i = 0; i < parsedModes.size(); ++i) {
        if (parsedModes[i] != 'o')
            continue;
        if (parsedArgs[i].empty())
            continue;

        User* target = userRepository.findUserByUsername(parsedArgs[i]);
        if (!target) {
            sendTo(*sender, ":" + serverName + " 401 " + sender->username + " " + parsedArgs[i] + " :No such nick");
            parsedArgs[i].clear();
            continue;
        }

        if (!channel->isUserInChannel(target->fileDescriptor)) {
            sendTo(*sender, ":" + serverName + " 441 " + sender->username + " " + parsedArgs[i] + " " + channelName +
                           " :They aren't on that channel");
            parsedArgs[i].clear();
            continue;
        }
    }

    // i/t
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

    // k/l
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

    // o
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

    // at least 1 op
    channel->ensureAtLeastOneOperator();

    // broadcast MODE
    std::string outModes;
    std::vector<std::string> outArgs;
    char currentSign = '\0';

    for (size_t i = 0; i < parsedModes.size(); ++i) {
        char sign = parsedSigns[i];
        char mode = parsedModes[i];

        bool needsArg = ((mode == 'k' && sign == '+') ||
                         (mode == 'l' && sign == '+') ||
                         (mode == 'o'));

        if (needsArg && parsedArgs[i].empty())
            continue;

        if (currentSign != sign) {
            outModes += sign;
            currentSign = sign;
        }
        outModes += mode;

        if (needsArg)
            outArgs.push_back(parsedArgs[i]);
    }

    if (!outModes.empty()) {
        std::string line = prefix(*sender) + " MODE " + channelName + " " + outModes;
        for (size_t i = 0; i < outArgs.size(); ++i)
            line += " " + outArgs[i];

        broadcastToChannel(userRepository, *channel, line);
    }
}