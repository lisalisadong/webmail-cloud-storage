#include "file_system.h"
#include <iostream>
#include <fstream>
#include <cstddef>  
#include <sstream> 
#include <iomanip> 
#include <string>

#define STORE_DIR "./../store/"
#define MAPPING "mapping"
#define FULL_LOG "full_log.txt"
#define TEMP_LOG "temp_log.txt"
#define LOG_DIR "./../log/"

// std::string md5_string(const std::string& str) {
// 	unsigned char buf[MD5_DIGEST_LENGTH];
// 	MD5_CTX c;
// 	MD5_Init(&c);
// 	MD5_Update(&c, str.c_str(), str.length());
// 	MD5_Final(buf, &c);

// 	std::stringstream ss;
// 	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
// 		ss << std::setfill('0') << std::setw(2) << std::hex << (int)buf[i];
// 	}
	
// 	return ss.str();
// }

std::string FileSystem::serialize(std::string str) {
	int strLen = str.length();
	return std::to_string(strLen) + ":" + str;
}

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
	else logger.log_error("Cannot open file " + fileName + " to read"); //TODO: write loggers in utils
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

void FileSystem::write_entry(std::string row, std::string col, std::string val) {
	std::string raw = serialize(row) + serialize(col) + serialize(val);
	std::string tuple = serialize(raw);
	std::string fileName = keys_to_file(row, col);
	logger.log_trace("Openning file " + fileName + " to read");
	std::ofstream file (STORE_DIR + fileName, std::ios_base::app);
	if (file.is_open()) {
	    file << tuple;
	    file.close();
  	}
  	else logger.log_error("Cannot open file " + fileName + " to write entry"); //TODO: write loggers in utils
}

void FileSystem::delete_entry(std::string row, std::string col) {
	std::string fileName = keys_to_file(row, col);
	logger.log_trace("Openning file " + fileName + " to delete entry");
	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;
	read_file(STORE_DIR + fileName, map);
	map[row].erase(col);
	if (map[row].size() == 0) {
		map.erase(row);
	}
	write_file(fileName, map);
}

std::string FileSystem::keys_to_file(std::string row, std::string col) {
	return row + col;
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

// void FileSystem::file_to_keys(std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> >& map) {
// 	for (std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> >::iterator it = map.begin(); it != map.end(); it++) {
// 		std::string fileName = it->first;
// 		std::ifstream file (fileName);
// 		if (file.is_open()) {
// 			std::string tuple;
// 			while (true) {
// 				tuple = get_next_tuple(file);
// 				if (tuple.length() == 0)
// 					break;
// 				int pos = 0;
// 				std::string row = deserialize_next(tuple, pos);
// 				std::string col = deserialize_next(tuple, pos);
// 				std::pair<std::string, std::string> keys(row, col);
// 				it->second.insert(keys);
// 			}
// 			file.close();
// 		}
// 		else std::cout << "Cannot open file to read!" << std::endl; //TODO: write loggers in utils
// 	}
// }

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


void FileSystem::write_log(std::string row, std::string col, std::string val, std::string operation) {

	logger.log_trace("Logging " + operation + " " + row + " " + col + " " + val);

	if (row.empty() || col.empty()) return;

	std::string tuple = serialize(serialize(row) + serialize(col) + serialize(val) + serialize(operation + "\n"));

	std::ofstream full_log (std::string(LOG_DIR) + FULL_LOG, std::ios_base::app);
	std::ofstream temp_log (std::string(LOG_DIR) + TEMP_LOG, std::ios_base::app);

	if (!full_log.is_open() || !temp_log.is_open()) {
		logger.log_error("Cannot open logs to write");
		return;
	}

	full_log << tuple; //write to full log
	temp_log << tuple; //write to temp log

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
			std::string row = deserialize_next(tuple, pos);
			std::string col = deserialize_next(tuple, pos);
			std::string val = deserialize_next(tuple, pos);
			std::string operation = deserialize_next(tuple, pos);
			
			if (operation == "PUT") {
				write_entry(row, col, val);
			} else if (operation == "DELETE") {
				delete_entry(row, col);
			}
		}
		file.close();
	}
	else logger.log_error("Cannot open temp log to read"); //TODO: write loggers in utils
}

void FileSystem::clear_temp_log() {
	logger.log_trace("Clearing temp log");
	std::ofstream temp_log (std::string(LOG_DIR) + TEMP_LOG);
	temp_log.close();
}


// int main() {
// 	FileSystem fs;
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