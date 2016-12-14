#include <string>
#include "file_system.h"
using namespace std;

class Log {
public:
	void log(string row, string col, string val, char operation);
	void replay(FileSystem &fs);
	void emptyTempLog();

	// logging
	// void log_error(const string& message);

	// void log_assert(bool cond, string message = "");

	// void log_warn(const string& message);

	// void log_trace(const string& message);
};

