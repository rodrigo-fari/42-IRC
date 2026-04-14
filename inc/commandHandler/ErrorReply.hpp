#pragma once

#include <sstream>
#include <string>
#include "Dispatcher.hpp"

enum IrcErrorCode {
	ERR_NOSUCHNICK = 401,
	ERR_NOSUCHCHANNEL = 403,
	ERR_UNKNOWNCOMMAND = 421,
	ERR_NONICKNAMEGIVEN = 431,
	ERR_USERNOTINCHANNEL = 441,
	ERR_NOTONCHANNEL = 442,
	ERR_USERONCHANNEL = 443,
	ERR_NOTREGISTERED = 451,
	ERR_NEEDMOREPARAMS = 461,
	ERR_ALREADYREGISTERED = 462,
	ERR_PASSWDMISMATCH = 464,
	ERR_CHANNELISFULL = 471,
	ERR_UNKNOWNMODE = 472,
	ERR_INVITEONLYCHAN = 473,
	ERR_BADCHANNELKEY = 475,
	ERR_CHANOPRIVSNEEDED = 482,
	ERR_INVALIDMODEPARAM = 696
};


struct ErrorReply {
	IrcErrorCode code;
	std::string target;
	std::string cmd;
	std::string arg;
	std::string text;

	ErrorReply()
		: code(ERR_UNKNOWNCOMMAND) {}

	ErrorReply(IrcErrorCode c, const std::string &tgt, const std::string &command,
		const std::string &argument, const std::string &message)
		: code(c), target(tgt), cmd(command), arg(argument), text(message) {}
};

inline std::string formatError(const std::string &serverName, const ErrorReply &e)
{
	std::ostringstream oss;
	const std::string target = e.target.empty() ? "*" : e.target;

	oss << ":" << serverName << " " << static_cast<int>(e.code) << " " << target;

	switch (e.code)
	{
		case ERR_NOSUCHNICK:
		case ERR_NOSUCHCHANNEL:
		case ERR_NOTONCHANNEL:
		case ERR_INVITEONLYCHAN:
		case ERR_BADCHANNELKEY:
		case ERR_CHANOPRIVSNEEDED:
		case ERR_CHANNELISFULL:
			if (!e.arg.empty())
				oss << " " << e.arg;
			break;
		case ERR_UNKNOWNCOMMAND:
			if (!e.arg.empty())
				oss << " " << e.arg;
			break;
		case ERR_NEEDMOREPARAMS:
			if (!e.cmd.empty())
				oss << " " << e.cmd;
			break;
		case ERR_UNKNOWNMODE:
			if (!e.cmd.empty())
				oss << " " << e.cmd;
			if (!e.arg.empty())
				oss << " " << e.arg;
			break;
		case ERR_USERNOTINCHANNEL:
		case ERR_USERONCHANNEL:
			if (!e.arg.empty())
				oss << " " << e.arg;
			if (!e.cmd.empty())
				oss << " " << e.cmd;
			break;
		case ERR_INVALIDMODEPARAM:
			if (!e.cmd.empty())
				oss << " " << e.cmd;
			if (!e.arg.empty())
				oss << " " << e.arg;
			break;
		default:
			break;
	}

	if (!e.text.empty())
		oss << " :" << e.text;

	oss << "\r\n";
	return oss.str();
}
