#include <string>
#include "file_system.h"
using namespace std;

class Log {
public:
	void log(string row, string col, string val, char operation);
	void replay(FileSystem &fs);
	void emptyTempLog();
};

