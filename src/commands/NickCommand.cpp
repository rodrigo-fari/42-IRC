/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NickCommand.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rerodrig <rerodrig@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 17:51:56 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/20 22:59:15 by rerodrig         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "commands/NickCommand.hpp"
#include "commands/CommandGuards.hpp"

NickCommand::NickCommand(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr, const std::string &srv)
    : BaseCommand(ur, cr), clientStateRepository(csr), serverName(srv) {}

void NickCommand::execute(int fd, const MessagePayload &payload, ReplyCollector &replies)
{
    ClientState &state = clientStateRepository.getClientStatus(fd);
    User *user = userRepository.findUserByFileDescriptor(fd);
    const std::string target = resolveReplyTarget(state, user);
    const bool wasRegistered = state.isRegistered;

    if (!requireParams(payload.params.size(), 1, target, "NICK", replies))
        return;

    state.nickname = payload.params[0];
    state.hasNickname = true;
    if (!state.isRegistered && state.hasPassword && state.hasNickname && state.hasUsername)
    {
        bool created = userRepository.createUser(fd, state.nickname, "");
        if (created || userRepository.findUserByFileDescriptor(fd))
            state.isRegistered = true;
    }

    if (!wasRegistered && state.isRegistered)
        replies.raw(":" + serverName + " 001 " + state.nickname +
            " :Welcome to the Internet Relay Network " + state.nickname);
}
