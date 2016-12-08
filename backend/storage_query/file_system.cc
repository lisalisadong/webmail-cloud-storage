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

std::unordered_map<std::string, std::unordered_map<std::string, std::string> > FileSystem::read(std::string row, std::string col) {
	std::string fileName = this->keys_to_file(row, col);
	std::ifstream file (fileName);
	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > ret;
	if (file.is_open()) {

	}
	else std::cout << "Cannot open file to read!" << std::endl; //TODO: write loggers in utils
	return ret;
}

void FileSystem::write(std::string row, std::string col, std::string val) {
	std::string raw = serialize(row) + serialize(col) + serialize(val);
	std::string tuple = serialize(raw);
	std::string fileName = this->keys_to_file(row, col);
	std::ofstream file (fileName);
	if (file.is_open()) {
	    file << tuple;
	    file.close();
  	}
  	else std::cout << "Cannot open file to write!" << std::endl; //TODO: write loggers in utils
}

std::string FileSystem::keys_to_file(std::string row, std::string col) {
	return "";
}

std::string FileSystem::get_next_tuple(std::ifstream& stream) {
	std::string lenStr;
	char c;
	while (stream.get(c)) {
		if (c == ':')
			break;
		lenStr += c;
	}
	int len = stoi(lenStr);
	char buf[len + 1];
	stream.read(buf, len);
	std::string ret(buf);
	return ret;
}

void FileSystem::deserialize(std::string str, std::string& row, std::string& col, std::string& val) {
	std::size_t found = str.find_first_of(":");
	return;
}

int main() {
	return 0;
}







