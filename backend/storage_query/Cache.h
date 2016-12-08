#include <unordered_map>
#include <string>
#include <iostream>
#include <memory>
#include <limits.h>
#include <vector>
#include <utility>

#define CACHE_SIZE 100

class Cache {
private:
  struct SimpleHash {
      size_t operator()(const std::pair<std::string, std::string>& p) const {
          return p.first ^ p.second;
      }
  };
  // <fileName, keys>
  std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, SimpleHash > > fileToKeys;

  std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;

  // <row, <col, fileName> >
  // std::unordered_map<std::string, std::unordered_map<std::string, std::string> > keysToFile;

  FileSystem fs;

  // Util util;

  // <fileName, count>
  std::unordered_map<std::string, int> fileCnt;

  bool writeSnapshot() {
    // writeMeta();
    // writeData();
  }

  void writeMeta() {

  }

  /* evict the least used chunk */
  void evict() {
    if(fileCnt.size() < CACHE_SIZE) return;

    string lrFile = getLRFile();

    std::vector<std::pair<std::string, std::string> > keys = fileToKeys[lrFile];

    // std::vector<std::pair<std::string, std::string> > keys = fs.getKeys(lrFile);

    // write the file to disk and remove it from the cache(map).
    for(int i = 0; i < keys.size(); i++) {
      std::string row = keys[i].first;
      std::string col = keys[i].second;
      std::string val = map[row][col];

      fs.write(row, col, val);
      map[row].erase(col); 

      if(map[row].size() == 0) {
        map.erase(row);
      }
    }

    // remove the count
    fileCnt.erase(lrFile);
  }

  /* get the least used file */
  std::string getLRFile() {
    int cnt = INT_MAX;
    std::string lrFile;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
          ::const_iterator itr = fileCnt.begin();

    while(itr != fileCnt.end()) {
      if(itr->second < cnt) {
        cnt = itr->second;
        lrFile = itr->first;
      }
      itr++;
    }

    return lrFile;
  }

  /* put the chunk into cache */
  void updateCache(std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
         ::const_iterator rItr = chunk.begin();
    rItr++;

    std::string row = rItr->first;

    std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
         ::const_iterator cItr = chunk[row].begin();
    cItr++;

    std::string col = cItr->first;

    std::string file = fs.getFile(row, col);


    while(itr != chunk.end()) {
      row = rItr->first;

      cItr = chunk[row].begin();

      cItr++;

      while (cItr != chunk[row].end()) {
        col = cItr->first;

        map[row][col] = chunk[row][col];    // put into map

        std::pair <std::string, std::string> p (row, col);

        fileToKeys[file].push_back(p);  // put into meta

        cItr++;
      }

      rItr++;
    }

  }

  bool containsKey(std::string row, std::string col) {

    /* firstly check if in cache */
    std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
         ::const_iterator rfind = map.find(row);

    if (rfind == map.end()) {
      // a chunk is a file
      std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk = fs.read(row, col);
      if(chunk.size() == 0) {
        return false;
      }

      evict();

      updateCache(chunk);
    }

    std::unordered_map<std::string,std::string>::const_iterator cfind = map.at(row).find(col);
    
    if (cfind == map.at(row).end()) return false;

    return true;
  }

public: 

  Cache() {
    fs.getFileToKeys(fileToKeys);
  }

  /**
  *
  */
  std::string get(std::string row, std::string col) {
    if(!containsKey(row, col)) {
      throw exception();
    }

    std::string file = fs.getFile(row, col);
    fileCnt[file] += 1;
    std::string val = map.at(row).at(col);

    return val;
  }

  bool put(std::string row, std::string col, std::string val) {
    evict();
    // 1. add to map
    // 2. update the keys->file, file->keys mapping
    // 3. update file->cnt mapping

    std::string file = fs.getFile(row, col);
    fileCnt[file] += 1;
    keysToFile[row][col] = file;

    map[row][col] = val;
    return true;
  }

  bool cput(std::string row, std::string col, std::string val1, std::string val2) {
    if (!containsKey(row, col) || get(row, col) != val1) return false;

    evict();
    std::string file = fs.getFile(row, col);
    fileCnt[file] += 1;
    keysToFile[row][col] = file;

    map[row][col] = val2;
    return true;
  }

  bool remove(std::string row, std::string col) {
    if(!containsKey(row, col)) return false;

    std::string file = fs.getFile(row, col);
    fileCnt[file] += 1;
    fileToKeys[file].erase(col);
    if(fileToKey)

    // erase row?
    map[row].erase(col);
    if(map[row].size() == 0) {
      map.erase(row);
    }
    return true;
  }


};
