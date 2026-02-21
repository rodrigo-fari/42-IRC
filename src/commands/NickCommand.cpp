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

NickCommand::NickCommand(UserRepository &ur, ChannelRepository &cr, ClientStateRepository &csr, const std::string &srv)
    : BaseCommand(ur, cr), clientStateRepository(csr), serverName(srv) {}

void NickCommand::execute(int fd, const MessagePayload &payload)
{
    ClientState &state = clientStateRepository.getClientStatus(fd);

    if (payload.params.empty())
        return;

    state.nickname = payload.params[0];
    state.hasNickname = true;
    if (!state.isRegistered && state.hasNickname && state.hasUsername)
    {
        bool created = userRepository.createUser(fd, state.nickname, "");
        if (created || userRepository.findUserByFileDescriptor(fd))
            state.isRegistered = true;
    }
}
