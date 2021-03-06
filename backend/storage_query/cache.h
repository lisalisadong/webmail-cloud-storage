#ifndef CACHE_H_
#define CACHE_H_

#include <unordered_map>
#include <map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <memory>
#include <limits.h>
#include <vector>
#include <utility>
#include "file_system.h"
#include "logger.h"

#define CACHE_SIZE 200
#define WRT_OP 1

class Cache {
private:
  /************************************ instance variables ***********************************************/
  // meta data: <fileName, keys>
  std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> > fileToKeys;
  // std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>> > fileToKeys;

  // meta data: <row, <col, fileName> >
  std::map<std::string, std::map<std::string, std::string> > keysToFile;

  // data
  std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;

  FileSystem fs;

  // <fileName, count>
  std::unordered_map<std::string, int> fileCnt;

  int wrtCnt;

  Logger logger;

  std::string serverAddress;

  /************************************ methods ***********************************************/
  bool writeSnapshot();

  /* write meta data into file system */
  void writeMeta();

  /* write to map to file system */
  void writeData();

  /* evict the least used chunk */
  void evict();

  void writeFileToFs(std::string file, bool isDelete);

  /* get the least used file */
  std::string getLRFile();

  /* put a chunk into cache */
  void updateCache(std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk);

  bool containsKey(std::string row, std::string col);

public:

  std::string currFile;

  Cache(std::string serverAddr) {
    logger.log_config("Cache");

    fs.set_prefix(serverAddr);

    serverAddress = serverAddr;

    wrtCnt = 0;

    fs.replay();

    fs.get_mappings(fileToKeys, keysToFile);

    logger.log_trace("In cache fileTokeys: " + std::to_string(fileToKeys.size())); 

    logger.log_trace("In cache keysToFile: " + std::to_string(keysToFile.size())); 

  }

  static Cache create_cache(std::string serverAddr) {
    return Cache(serverAddr);
  }

  std::string get(std::string row, std::string col);

  bool put(std::string row, std::string col, std::string val);

  bool cput(std::string row, std::string col, std::string val1, std::string val2);

  bool remove(std::string row, std::string col);

  void migrate(std::string selfAddr, std::string otherAddr, std::string& data);

  int get_raw_data(int start, int size, std::string& data);

};



#endif