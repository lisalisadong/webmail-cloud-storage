#ifndef F_S
#define F_S

#include <unordered_map>
#include <string>
#include <openssl/md5.h>
#include <fstream>
#include <utility>
#include <unordered_set>
#include "hash.h"


class FileSystem {
public:
	void read_file(std::string fileName, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& entries);
	
	int write_file(std::string fileName, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& entries);
	
	void write_entry(std::string row, std::string col, std::string val);
	
	void delete_entry(std::string row, std::string col);
	
	std::string keys_to_file(std::string row, std::string col);
	
	// void file_to_keys(std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> >& map);

	void get_mappings(std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> >& fileToKey, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& keyToFile);

	void write_log(std::string row, std::string col, std::string val, std::string operation);

	void replay();

	void clear_temp_log();

private:
	std::string serialize(std::string str);
	std::string get_next_tuple(std::ifstream& stream);
	std::string deserialize_next(std::string str, int& pos);
};

#endif