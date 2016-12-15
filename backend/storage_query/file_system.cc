#include "file_system.h"
#include <iostream>
#include <fstream>
#include <cstddef>  
#include <sstream> 
#include <iomanip> 
#include <string>

#define STORE_DIR "./../store/"
#define LOG_DIR "./../log/"
#define MAPPING "mapping"
#define FULL_LOG "full_log"
#define TEMP_LOG "temp_log"
#define MAX_FILE_SIZE 500

/**
 * Public methods:
 */

void FileSystem::read_file(std::string fileName, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& entries) {
	logger.log_trace("Openning file " + fileName + " to read");
	std::ifstream file (STORE_DIR + fileName);
	if (file.is_open()) {
		std::string tuple;
		while (true) {
			tuple = get_next_tuple(file);
			if (tuple.length() == 0)
				break;
			int pos = 0;
			std::string row = deserialize_next(tuple, pos);
			std::string col = deserialize_next(tuple, pos);
			std::string val = deserialize_next(tuple, pos);
			entries[row][col] = val;
		}
		file.close();
	}
	else logger.log_error("Cannot open file " + fileName + " to read");
}

int FileSystem::write_file(std::string fileName, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& entries) {
	logger.log_trace("Openning file " + fileName + " to write");
	std::ofstream file (STORE_DIR + fileName);
	if (!file.is_open()) {
		logger.log_error("Cannot open file " + fileName + " to read");
		return 1;
	}
	std::string row;
	std::string col;
	std::string val;
	for (std::unordered_map<std::string, std::unordered_map<std::string, std::string> >::const_iterator it = entries.begin(); it != entries.end(); it++) {
		row = it->first;
		for (std::unordered_map<std::string, std::string>::const_iterator itr = it->second.begin(); itr != it->second.end(); itr++) {
			col = itr->first;
			val = itr->second;
			std::string raw = serialize(row) + serialize(col) + serialize(val);
			std::string tuple = serialize(raw);
			file << tuple;
		}
	}
	file.close();
	return 0;
}

std::string FileSystem::place_new_entry() {
	logger.log_trace("current file size " + std::to_string(file_size(curr_file)));
	if (file_size(curr_file) >= MAX_FILE_SIZE) {
		increment_file();
	}
	logger.log_trace("current file is " + curr_file);
	return curr_file;
}

void FileSystem::get_mappings(std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> >& fileToKey, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& keyToFile) {
	std::ifstream file (std::string(STORE_DIR) + MAPPING);
	logger.log_trace("Openning mapping to read");
	if (file.is_open()) {
		std::string tuple;
		while (true) {
			tuple = get_next_tuple(file);
			if (tuple.length() == 0)
				break;
			int pos = 0;
			std::string row = deserialize_next(tuple, pos);
			std::string col = deserialize_next(tuple, pos);
			std::string fileName = deserialize_next(tuple, pos);
			std::pair<std::string, std::string> keys(row, col);
			keyToFile[row][col] = fileName;
			fileToKey[fileName].insert(keys);
		}
		file.close();
	}
	else logger.log_error("Cannot open mapping to read");
}


void FileSystem::write_log(std::string fileName, std::string row, std::string col, std::string val, std::string operation) {

	logger.log_trace("Logging " + operation + " " + row + " " + col + " " + val);

	if (row.empty() || col.empty()) return;

	std::string tuple = serialize(serialize(fileName) + serialize(row) + serialize(col) + serialize(val) + serialize(operation + "\n"));

	std::ofstream full_log (std::string(LOG_DIR) + FULL_LOG, std::ios_base::app);
	std::ofstream temp_log (std::string(LOG_DIR) + TEMP_LOG, std::ios_base::app);

	if (!full_log.is_open() || !temp_log.is_open()) {
		logger.log_error("Cannot open logs to write");
		return;
	}

	full_log << tuple;
	temp_log << tuple;

	full_log.close();
	temp_log.close();
}

void FileSystem::replay() {

	logger.log_trace("Replaying temp log");

	std::ifstream file (std::string(LOG_DIR) + TEMP_LOG);
	if (file.is_open()) {
		std::string tuple;
		while (true) {
			tuple = get_next_tuple(file);
			if (tuple.length() == 0)
				break;
			int pos = 0;
			std::string fileName = deserialize_next(tuple, pos);
			std::string row = deserialize_next(tuple, pos);
			std::string col = deserialize_next(tuple, pos);
			std::string val = deserialize_next(tuple, pos);
			std::string operation = deserialize_next(tuple, pos);
			
			if (operation == "PUT") {
				write_entry(fileName, row, col, val);
			} else if (operation == "DELETE") {
				delete_entry(fileName, row, col);
			}
		}
		file.close();
	}
	else logger.log_error("Cannot open temp log to read");
}

void FileSystem::clear_temp_log() {
	logger.log_trace("Clearing temp log");
	std::ofstream temp_log (std::string(LOG_DIR) + TEMP_LOG);
	temp_log.close();
}


/**
 * Private methods:
 */

void FileSystem::write_entry(std::string fileName, std::string row, std::string col, std::string val) {
	std::string raw = serialize(row) + serialize(col) + serialize(val);
	std::string tuple = serialize(raw);
	logger.log_trace("Openning file " + fileName + " to read");
	std::ofstream file (STORE_DIR + fileName, std::ios_base::app);
	if (file.is_open()) {
	    file << tuple;
	    file.close();
  	}
  	else logger.log_error("Cannot open file " + fileName + " to write entry");
}

void FileSystem::delete_entry(std::string fileName, std::string row, std::string col) {
	logger.log_trace("Openning file " + fileName + " to delete entry");
	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;
	read_file(STORE_DIR + fileName, map);
	map[row].erase(col);
	if (map[row].size() == 0) {
		map.erase(row);
	}
	write_file(fileName, map);
}

void FileSystem::increment_file() {
	int pos = curr_file.length() - 1;
	while (pos >= 0 && (curr_file[pos] == '9' || curr_file[pos] == '.')) {
		if (curr_file[pos] == '9') {
			curr_file[pos] = '0';
		}
		pos--;
	}
	curr_file[pos] += 1;
}


std::string FileSystem::serialize(std::string str) {
	int strLen = str.length();
	return std::to_string(strLen) + ":" + str;
}

std::string FileSystem::get_next_tuple(std::ifstream& stream) {
	std::string lenStr;
	char c;
	while (stream.get(c)) {
		if (c == ':')
			break;
		lenStr += c;
	}
	if (lenStr.length() == 0) {
		return "";
	}
	int len = std::stoi(lenStr);
	char buf[len + 1];
	stream.read(buf, len);
	buf[len] = '\0';
	std::string ret(buf);
	return ret;
}



std::string FileSystem::deserialize_next(std::string str, int& pos) {
	std::size_t found = str.find(":", pos);
	std::string lenStr = str.substr(pos, found - pos);	
	int len = std::stoi(lenStr);
	std::string ret = str.substr(found + 1, len);
	pos = found + 1 + len;
	return ret;
}

std::ifstream::pos_type FileSystem::file_size(std::string filename) {
    std::ifstream in(std::string(STORE_DIR) + filename, std::ifstream::ate | std::ifstream::binary);
    return in.tellg(); 
}


// int main() {
// 	FileSystem fs;
// 	std::cout << fs.file_size("./../store/r1c1") << std::endl;
// }
// 	fs.write_entry("lisa", "email", "123");
// 	fs.write_entry("lis", "aemail", "456");
// 	fs.write_entry("qingxiao", "drive", "xxx");
// 	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map = fs.read_file("lisaemail");
// 	for(std::unordered_map<std::string, std::unordered_map<std::string, std::string> >::const_iterator it = map.begin(); it != map.end(); it++) {
// 		for (std::unordered_map<std::string, std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
// 			std::cout << it->first << "," << it2->first << "," << it2->second << std::endl;
// 		}
// 	}
// 	std::cout << std::endl;

// 	map = fs.read_file("qingxiaodrive");
// 	for(std::unordered_map<std::string, std::unordered_map<std::string, std::string> >::const_iterator it = map.begin(); it != map.end(); it++) {
// 		for (std::unordered_map<std::string, std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
// 			std::cout << it->first << "," << it2->first << "," << it2->second << std::endl;
// 		}
// 	}
// 	std::cout << std::endl;

// 	map = fs.read_file("lisaemail");
// 	fs.delete_entry("lisa", "email");
// 	fs.delete_entry("lis", "aemail");
// 	fs.write_file("lisaemail", map);
// 	map = fs.read_file("lisaemail");
// 	for(std::unordered_map<std::string, std::unordered_map<std::string, std::string> >::const_iterator it = map.begin(); it != map.end(); it++) {
// 		for (std::unordered_map<std::string, std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
// 			std::cout << it->first << "," << it2->first << "," << it2->second << std::endl;
// 		}
// 	}
// 	std::cout << std::endl;
// }