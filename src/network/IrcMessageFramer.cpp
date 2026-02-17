/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcMessageFramer.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/09 19:11:23 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/17 18:01:44 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "network/IrcMessageFramer.hpp"

/**
 * @brief Processa informacao crua do cliente
 * 
 * Recebe informacao "crua" do servidor ("recv()"), processa, e devolve um vetor com
 * cada um dos comandos por indice na totalidade.
 * 
 * @param fd File descriptor do cliente.
 * @param rawData Informacao crua recebida pelo servidor ("recv()").
 * @return extractedLines Vetor de strings com cada comando recebido, por indice, na totalidade (sem "\r\n").
 */
std::vector<std::string> IrcMessageFramer::processRawData(int fd, const std::string& rawData)
{
	size_t pos;
	std::vector<std::string> extractedLines;
	
	_buffers[fd] += rawData;

	while ((pos = _buffers[fd].find("\r\n")) != std::string::npos)
	{
		std::string line = _buffers[fd].substr(0, pos);
		extractedLines.push_back(line);
		_buffers[fd].erase(0, pos + 2);
	}
	return (extractedLines);
}

void IrcMessageFramer::clearClient(int fd)
{
	_buffers[fd].clear();
	std::cout << "DEBUG:\nIrcMessagrFramer.cpp -> 34\n_buffers cleared!\n";
}
