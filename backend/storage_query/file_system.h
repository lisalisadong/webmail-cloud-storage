#include <unordered_map>
#include <string>
#include <openssl/md5.h>
#include <fstream>

class FileSystem {
public:
	std::unordered_map<std::string, std::unordered_map<std::string, std::string> > read(std::string row, std::string col);
	void write(std::string row, std::string col, std::string val);
	std::string keys_to_file(std::string row, std::string col);

private:
	std::string serialize(std::string str);
	std::string get_next_tuple(std::ifstream& stream);
	std::string deserialize_next(std::string str, int& pos);
};

