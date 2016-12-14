#include "utils.h"
#include <stdio.h>
#include <iostream>
#include <fstream>

#define FULL_LOG "full_log.txt"
#define TEMP_LOG "temp_log.txt"
#define LOG_DIR "./../store/"
#define DELIMITER "\t"

bool log_verbose = true;

void Log::log(string row, string col, string val, char operation) {
	if (row.empty() || col.empty()) return;

	string s(1, operation);
	string new_info = s + DELIMITER + row + DELIMITER + col + DELIMITER + val;
	//cout << new_info << endl;

	ofstream full_log, temp_log;
	full_log.open(std::string(LOG_DIR) + FULL_LOG, ofstream::app);
	temp_log.open(std::string(LOG_DIR) + TEMP_LOG, ofstream::app);

	full_log << new_info << endl; //write to full log
	temp_log << new_info << endl; //write to temp log

	full_log.close();
	temp_log.close();
}


void Log::emptyTempLog() {
	ofstream temp_log;
	temp_log.open(std::string(LOG_DIR) + TEMP_LOG, ofstream::out);
	temp_log.close();
}


void Log::replay(FileSystem &fs) {
	string line;
	string operation, row, col, value;
	ifstream temp_log(std::string(LOG_DIR) + TEMP_LOG);

	if (!temp_log.is_open()) return; //TO DO: print error message

	while (getline(temp_log, line)) {
		//cout << "read in: " + line << endl;

		size_t pos = 0;
		int count = 0;
		while ((pos = line.find(DELIMITER)) != string::npos && count < 3) {
			if (count == 0) {
				operation = line.substr(0, pos);
			} else if (count == 1) {
				row = line.substr(0, pos);
			} else {
				col = line.substr(0, pos);
			}

			line.erase(0, pos + 1);
			count++;
		}
		value = line;
		//cout << operation << "\t" << row << "\t" << col << "\t" << value << endl;
			
		if (operation.compare("+") == 0) {
			fs.write_entry(row, col, value);
		} else if (operation.compare("-") == 0) {
			fs.delete_entry(row, col);
		} else {
			continue; // TO DO: should show error message here
		}
	}

	temp_log.close();
}

// void log_error(const string& message) {
// 	fprintf(stderr, "%s %s %d [ERROR] %s\n", timestamp_string().c_str(), log_location.c_str(), getpid(), message.c_str());
// 	exit(1);
// }

// void log_assert(bool cond, string message) {
// 	if (!cond) {
// 		log_error(message);
// 	}
// }

// void log_warn(const string& message) {
// 	if (log_verbose) {
// 		fprintf(stderr, "%s %s %d [WARN] %s\n", timestamp_string().c_str(), log_location.c_str(), getpid(), message.c_str());
// 	}
// }

// void log_trace(const string& message) {
// 	if (log_verbose) {
// 		fprintf(stderr, "%s %s %d [TRACE] %s\n", timestamp_string().c_str(), log_location.c_str(), getpid(), message.c_str());
// 	}
// }

/*
int main() {
	Log log_file;
	FileSystem fs;

	log_file.log("manh", "1", "manh@seas.upenn.edu", '+');
	log_file.log("manh", "1", "", '-');
	log_file.log("linphan", "1", "linphan@cis.upenn.edu", '+');
	log_file.emptyTempLog();
	log_file.log("linphan", "2", "linphan@cis.upenn.edu", '+');
	log_file.log("manh", "2", "", '-');
	
	log_file.replay(fs);
}
*/



