#pragma once

#include <iostream>
#include <vector>
#include <string>

struct MessagePayload
{
	std::string command;
	std::vector<std::string> params;
};

MessagePayload parseMessage(const std::string& line);
