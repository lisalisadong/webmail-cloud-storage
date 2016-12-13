#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <memory>
#include <limits.h>
#include <vector>
#include <utility>
#include "utils.h"

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

  Log logger;

  // <fileName, count>
  std::unordered_map<std::string, int> fileCnt;

  int wrtCnt;

  /************************************ methods ***********************************************/
  bool writeSnapshot() {
    wrtCnt++;

    if(wrtCnt < WRT_OP) return false;

    wrtCnt = 0;

    std::cout << "start writing snapshot..." << std::endl;
    writeMeta();
    writeData();

    std::cout<< "Snapshot finished writing" << std::endl;
  } 

  /* write meta data into file system */
  void writeMeta() {
    fs.write_file("mapping", keysToFile);

    std::cout << "Meta data write succeeded" << std::endl;
  }

  /* write to map to file system */
  void writeData() {

    for(auto f = fileToKeys.begin(); f != fileToKeys.end(); ++f) {
      writeFileToFs(f->first, false);
      std::cout << f->first << " write succeeded" << std::endl;
    }

    logger.emptyTempLog();

  }

  /* evict the least used chunk */
  void evict() {
    if(fileCnt.size() < CACHE_SIZE) {
      std::cout<< "Nothing to evict!" << std::endl;
      return;
    }

    std::string lrFile = getLRFile();

    std::cout<< "Evict: " << lrFile << std::endl;

    writeFileToFs(lrFile, true);
  }

  void writeFileToFs(std::string file, bool isDelete) {

    std::unordered_set<std::pair<std::string, std::string>, Hash> keys = fileToKeys[file];

    std::unordered_map<std::string, std::unordered_map<std::string, std::string> > tmpMap;

    // write the file to disk and remove it from the cache(map).
    for (auto p = keys.begin(); p != keys.end(); ++p) {
      std::string row = p->first;
      std::string col = p->second;
      std::string val = map[row][col];

      tmpMap[row][col] = val;

      if(isDelete) {
        map[row].erase(col); 

        if(map[row].size() == 0) {
          map.erase(row);
        }
      }
    }

    if(isDelete) fileCnt.erase(file);
    
    std::cout << "Write " << file << " into disk." << std::endl;

    fs.write_file(file, tmpMap);
  }

  /* get the least used file */
  std::string getLRFile() {
    int cnt = INT_MAX;
    std::string lrFile;
    std::unordered_map<std::string, int>::const_iterator itr = fileCnt.begin();

    while(itr != fileCnt.end()) {
      if(itr->second < cnt) {
        cnt = itr->second;
        lrFile = itr->first;
      }
      itr++;
    }

    return lrFile;
  }

  /* put a chunk into cache */
  void updateCache(std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk) {
    if(chunk.size() <= 0) return;

    for(auto rItr = chunk.begin(); rItr != chunk.end(); ++rItr) {
        
        std::string row = rItr->first;

        for(auto cItr = chunk[row].begin(); cItr != chunk[row].end(); ++cItr) {
          std::string col = cItr->first;

          map[row][col] = chunk[row][col];
        }
    } 

  }

  bool containsKey(std::string row, std::string col) {

    // check meta data first
    auto rfind = keysToFile.find(row);

    if (rfind == keysToFile.end()) {
      return false;
    }

    auto cfind = keysToFile.at(row).find(col);
    if (cfind == keysToFile.at(row).end()) return false;

    // the disk has the keys
    /* check if in map */
    rfind = map.find(row);
    if (rfind == map.end() || rfind->second.find(col) == rfind->second.end()) {
      std::string file = keysToFile[row][col];

      std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk = fs.read_file(file);
      
      std::cout << "read " << file << " into cache " << std::endl;

      evict();

      updateCache(chunk);
    }

    return true;

  }

public: 

  Cache() {
    // fs.keys_to_fileToKeys(fileToKeys);

    wrtCnt = 0;

    fs.get_mappings(fileToKeys, keysToFile);

    logger.replay(fs);

    // std::unordered_set<std::pair<std::string, std::string>, Hash> set;
    // std::pair<std::string, std::string> p("lisa", "emails");
    // set.insert(p);
    // fileToKeys["lisaemails"] = set;
  }

  /**
  *
  */
  std::string get(std::string row, std::string col) {
    std::cout<< "=========================================" <<std::endl;
    if(!containsKey(row, col)) {
      throw std::exception();
    }

    std::string file = keysToFile[row][col];
    fileCnt[file] += 1;
    std::string val = map[row][col];

    std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    return val;
  }

  bool put(std::string row, std::string col, std::string val) {
    std::cout<< "=========================================" <<std::endl;
    // 1. add to map
    // 2. update the keys->file, file->keys mapping
    // 3. update file->cnt mapping

    std::string file = fs.keys_to_file(row, col);

    /* file counter plus 1 */
    fileCnt[file] += 1;
    std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    if(!containsKey(row, col)) {
      /* add keys -> file mapping */
      keysToFile[row][col] = file;
      std::cout<< "add mapping: <" << row << ", " << col << "> -> " << keysToFile[row][col] <<std::endl;

      /* add file -> keys mapping */
      std::pair<std::string, std::string> p(row, col);
      if(fileToKeys.find(file) == fileToKeys.end()) {
        std::unordered_set<std::pair<std::string, std::string>, Hash> set;
        fileToKeys[file] = set;
      }
      fileToKeys[file].insert(p);
      std::cout<< "add mapping: " << file << "-> <" << p.first << ", " << p.second << ">" <<std::endl;
    }

    map[row][col] = val;
    std::cout<< "add mapping: <" << row << ", " << col << "> -> " << map[row][col] <<std::endl;

    writeSnapshot();

    if(fileToKeys.size() > CACHE_SIZE) evict();

    logger.log(row, col, val, '+');

    return true;
  }

  bool cput(std::string row, std::string col, std::string val1, std::string val2) {
    if (!containsKey(row, col) || get(row, col) != val1) return false;

    std::string file = keysToFile[row][col];

    fileCnt[file] += 1;
    std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    map[row][col] = val2;
    std::cout<< "add mapping: <" << row << ", " << col << "> -> " << map[row][col] <<std::endl;

    writeSnapshot();

    logger.log(row, col, val2, '+');

    return true;
  }

  bool remove(std::string row, std::string col) {
    if(!containsKey(row, col)) return false;

    std::string file = keysToFile[row][col];
    fileCnt[file] += 1;
    std::pair<std::string, std::string> p(row, col);
    fileToKeys[file].erase(p);

    map[row].erase(col);
    if(map[row].size() == 0) {
      map.erase(row);
    }

    keysToFile[row].erase(col);
    if(keysToFile[row].size() == 0) {
      keysToFile.erase(row);
    }

    writeSnapshot();

    logger.log(row, col, "", '-');

    return true;
  }


};



