#include <string>
#include <iostream>
#include <fstream>

#define FULL_LOG "full_log.txt"
#define TEMP_LOG "temp_log.txt"

using namespace std;


class Log {


	void log(string row, string col, string val, char operation) {
		if (row == NULL || col == NULL) return;
		string new_info = operation + "\t" + row + "\t" + col + "\t" + val;

		ofstream full_log, temp_log;
		full_log.open(FULL_LOG, ofstream::app);
		full_log << new_info << endl; //write to full log

		if (operation == '#') {//snapshot
			temp_log.open(TEMP_LOG, ofstream::out);
		} else {
			temp_log.open(TEMP_LOG, ofstream::append);
			temp_log << new_info << endl;
		}

	}


	void replay(FileSystem fs) {
		
	}



}

