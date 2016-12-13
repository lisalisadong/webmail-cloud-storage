#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <stdio.h>

class Logger {
public:

	void log_config(std::string className);
	
	void log_error(const std::string& message);

	void log_assert(bool cond, std::string message = "");

	void log_warn(const std::string& message);

	void log_trace(const std::string& message);

private:
	std::string cName = "";

	std::string timestamp_string();

	bool log_verbose = true;
};

#endif