#include "Helpers.h"


namespace Helpers {
	//function to parse int value of a port that's being transmitted as one of the command line parameters
	 bool parsePort(const char* param, int& outPort) {
		if (param == nullptr) {
			return false;
		}
		char* endptr = nullptr;
		errno = 0;
		long val = strtol(param, &endptr, 10);

		if (endptr == param || *endptr != '\0') {
			return false;
		}
		if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || val < 1 || val > 65535) {
			return false;
		}
		outPort = static_cast<int>(val);
		return true;
	}
}
