#pragma once
#include <iostream>
#include <cerrno>

namespace Helpers {
	bool parsePort(const char* param, int& outPort);
}