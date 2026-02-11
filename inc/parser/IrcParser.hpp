#pragma once

#include <iostream>
#include <vector>
#include <string>

struct messagePayload
{
	std::string command;
	std::vector<std::string> params;
};

messagePayload parseMessage(const std::string& line);