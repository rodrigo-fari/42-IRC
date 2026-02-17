/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DEBUG.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/17 14:19:37 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/17 14:27:25 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "core/DEBUG.hpp"

void debugMessage(std::string debugMessage)
{
	std::cout << YELLOW << debugMessage << RESET << "\n";
}
