/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Dispatcher.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rde-fari <rde-fari@student.42porto.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/11 21:40:52 by rde-fari          #+#    #+#             */
/*   Updated: 2026/02/17 14:18:31 by rde-fari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>
#include <core/DEBUG.hpp>
#include <vector>
#include <string>
#include <cctype>
#include "parser/IrcParser.hpp"
#include <sstream>

std::string dispatch(int fd, const MessagePayload& payload);
