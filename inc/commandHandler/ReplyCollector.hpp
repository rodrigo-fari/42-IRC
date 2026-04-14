#pragma once

#include <string>
#include "ErrorReply.hpp"


class ReplyCollector {
	private:
		std::string _serverName;
		std::string _buffer;

	public:
		ReplyCollector(const std::string &serverName);

		void raw(const std::string &line);
		void error(const ErrorReply &e);
		std::string flush();
};
