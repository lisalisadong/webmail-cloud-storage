#include "logger.h"
#include <ctime>
#include <sys/time.h>

/******************public methods******************/

void Logger::log_config(std::string className) {
	cName = className;
}

void Logger::log_error(const std::string& message) {
	fprintf(stderr, "%s [ERROR] [%s] %s\n", timestamp_string().c_str(), cName.c_str(), message.c_str());
	exit(1);
}

void Logger::log_assert(bool cond, std::string message) {
	if (!cond) {
		log_error(message);
	}
}

void Logger::log_warn(const std::string& message) {
	if (log_verbose) {
		fprintf(stderr, "%s [WARN ] [%s] %s\n", timestamp_string().c_str(), cName.c_str(), message.c_str());
	}
}

void Logger::log_trace(const std::string& message) {
	if (log_verbose) {
		fprintf(stderr, "%s [TRACE] [%s] %s\n", timestamp_string().c_str(), cName.c_str(), message.c_str());
	}
}


/********************private methods*******************/

std::string Logger::timestamp_string() {
	time_t rt;
	struct tm * ti;
	char timestamp[200];

	struct timeval time;
   	gettimeofday(&time, NULL);
  	rt = time.tv_sec;
	ti = localtime(&rt);

	// Tue Nov 15 20:14:44 2016
	//strftime(timestamp, 160, "%a %b %d %H:%M:%S %Y",ti);

	// 20:30:38.261768
	char buffer[160];
	strftime(buffer, 160, "%H:%M:%S",ti);
	sprintf(timestamp, "%s.%Lu", buffer, (long long unsigned int)time.tv_usec); 

	return std::string(timestamp);
}