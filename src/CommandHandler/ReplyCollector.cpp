#include "commandHandler/ReplyCollector.hpp"

static std::string normalizeForWireCollector(const std::string& line) {
	if (line.size() >= 2 && line[line.size() - 2] == '\r' && line[line.size() - 1] == '\n')
		return line;
	if (!line.empty() && line[line.size() - 1] == '\n')
		return line.substr(0, line.size() - 1) + "\r\n";
	return line + "\r\n";
}

ReplyCollector::ReplyCollector(const std::string& serverName) : _serverName(serverName) {}

void ReplyCollector::raw(const std::string& line) {
	_buffer += normalizeForWireCollector(line);
}

void ReplyCollector::error(const ErrorReply& e) {
	_buffer += formatError(_serverName, e);
}

std::string ReplyCollector::flush() {
	std::string out = _buffer;
	_buffer.clear();
	return out;
}
