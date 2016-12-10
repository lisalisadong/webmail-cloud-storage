#include "file_system.h"
#include <iostream>
#include <fstream>
#include <cstddef>  
#include <sstream> 
#include <iomanip> 

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

std::unordered_map<std::string, std::unordered_map<std::string, std::string> > FileSystem::read_file(std::string fileName) {
	std::ifstream file (fileName);
	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;
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
			map[row][col] = val;
		}
		file.close();
	}
	else std::cout << "Cannot open file to read!" << std::endl; //TODO: write loggers in utils
	return map;
}

int FileSystem::write_file(std::string fileName, std::unordered_map<std::string, std::unordered_map<std::string, std::string> > entries) {
	std::ofstream file (fileName, std::ios_base::app);
	if (!file.is_open()) {
		std::cout << "Cannot open file to write!" << std::endl;
		return 1;
	}
	int ret = 0;
	std::string row;
	std::string col;
	std::string val;
	for (std::unordered_map<std::string, std::unordered_map<std::string, std::string> >::const_iterator it = entries.begin(); it != entries.end(); it++) {
		row = it->first;
		for (std::unordered_map<std::string, std::string>::const_iterator itr = it->second.begin(); itr != it->second.end(); itr++) {
			col = itr->first;
			val = itr->second;
			if (keys_to_file(row, col) != fileName) {
				ret = 1;
			} else {
				std::string raw = serialize(row) + serialize(col) + serialize(val);
				std::string tuple = serialize(raw);
				file << tuple;
			}
		}
	}
	file.close();
	return ret;
}

void FileSystem::write_entry(std::string row, std::string col, std::string val) {
	std::string raw = serialize(row) + serialize(col) + serialize(val);
	std::string tuple = serialize(raw);
	std::string fileName = keys_to_file(row, col);
	std::ofstream file (fileName, std::ios_base::app);
	if (file.is_open()) {
	    file << tuple;
	    file.close();
  	}
  	else std::cout << "Cannot open file to write!" << std::endl; //TODO: write loggers in utils
}

void FileSystem::delete_entry(std::string row, std::string col) {
	std::string fileName = keys_to_file(row, col);
	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map = read_file(fileName);
	map[row].erase(col);
	if (map[row].size() == 0) {
		map.erase(row);
	}
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
	std::ifstream file ("mapping.meta");
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
	}
	else std::cout << "Cannot open mapping.meta to read!" << std::endl;
	file.close();
}





// int main() {
// 	FileSystem fs;
// 	fs.write("lisa", "email", "123");
// 	fs.write("lis", "aemail", "456");
// 	fs.write("qingxiao", "drive", "xxx");
// 	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map = fs.read("lisa", "email");
// 	for(std::unordered_map<std::string, std::unordered_map<std::string, std::string> >::const_iterator it = map.begin(); it != map.end(); it++) {
// 		for (std::unordered_map<std::string, std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
// 			std::cout << it->first << "," << it2->first << "," << it2->second << std::endl;
// 		}
// 	}

// 	map = fs.read("qingxiao", "drive");
// 	for(std::unordered_map<std::string, std::unordered_map<std::string, std::string> >::const_iterator it = map.begin(); it != map.end(); it++) {
// 		for (std::unordered_map<std::string, std::string>::const_iterator it2 = it->second.begin(); it2 != it->second.end(); it2++) {
// 			std::cout << it->first << "," << it2->first << "," << it2->second << std::endl;
// 		}
// 	}