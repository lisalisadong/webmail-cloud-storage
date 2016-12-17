#ifndef CACHE_H_
#define CACHE_H_

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <memory>
#include <limits.h>
#include <vector>
#include <utility>
#include "file_system.h"
#include "logger.h"

#define CACHE_SIZE 2
#define WRT_OP 1

class Cache {
private:
  /************************************ instance variables ***********************************************/
  // meta data: <fileName, keys>
  std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> > fileToKeys;
  // std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>> > fileToKeys;

  // meta data: <row, <col, fileName> >
  std::unordered_map<std::string, std::unordered_map<std::string, std::string> > keysToFile;

  // data
  std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;

  FileSystem fs;

  // <fileName, count>
  std::unordered_map<std::string, int> fileCnt;

  int wrtCnt;

  Logger logger;

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

  Cache() {
    // fs.keys_to_fileToKeys(fileToKeys);

    wrtCnt = 0;

    fs.get_mappings(fileToKeys, keysToFile);

    fs.replay();

    logger.log_config("Cache");

    // std::unordered_set<std::pair<std::string, std::string>, Hash> set;
    // std::pair<std::string, std::string> p("lisa", "emails");
    // set.insert(p);
    // fileToKeys["lisaemails"] = set;
  }


  std::string get(std::string row, std::string col);

  bool put(std::string row, std::string col, std::string val);

  bool cput(std::string row, std::string col, std::string val1, std::string val2);

  bool remove(std::string row, std::string col);

  void migrate(std::string selfAddr, std::string otherAddr, std::string& data);

};



#endif