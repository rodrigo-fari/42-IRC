#pragma once

#include <string>
#include "ErrorReply.hpp"

struct DispatchResult {
	std::string wire;
	bool closeAfterFlush;
	bool fatalInternal;

	DispatchResult()
		: closeAfterFlush(false), fatalInternal(false) {}
};

