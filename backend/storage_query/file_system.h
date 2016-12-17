#ifndef F_S
#define F_S

#include <unordered_map>
#include <string>
#include <openssl/md5.h>
#include <fstream>
#include <utility>
#include <unordered_set>
#include "logger.h"
#include "utils.h"

class FileSystem {
public:
	FileSystem() {logger.log_config("FileSystem");}

	void read_file(std::string fileName, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& entries);
	
	int write_file(std::string fileName, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& entries);
	
	std::string place_new_entry();
	
	void get_mappings(std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> >& fileToKey, std::unordered_map<std::string, std::unordered_map<std::string, std::string> >& keyToFile);

	void write_log(std::string fileName, std::string row, std::string col, std::string val, std::string operation);

	void replay();

	void clear_temp_log();

	void set_prefix(std::string pre);

private:

	Logger logger;

	std::string prefix;

	std::string curr_file = "000.000.000.000.000.001";

	std::ifstream::pos_type file_size(std::string filename);

	void increment_file();

	std::string get_next_tuple(std::ifstream& stream);

	void write_entry(std::string fileName, std::string row, std::string col, std::string val);
	
	void delete_entry(std::string fileName, std::string row, std::string col);
};

#endif