#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>

class IrcMessageFramer {
	private:
		std::map<int, std::string> _buffers;

	public:
		std::vector<std::string> processRawData(int fd, const std::string& rawData);
		void clearClient(int fd);
};
