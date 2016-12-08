#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <memory>
#include <limits.h>
#include <vector>
#include <utility>
#include "file_system.h"
#include "hash.h"

#define CACHE_SIZE 1


class Cache {
private:
  struct Hash {
      size_t operator()(const std::pair<std::string, std::string>& p) const {
          std::string str = p.first + p.second;
          return simpleHash(str);
        // return 1;
      }
  };

  // <fileName, keys>
  std::unordered_map<std::string, std::vector<std::pair<std::string, std::string>> > fileToKeys;

  std::unordered_map<std::string, std::unordered_set<std::pair<std::string, std::string>, Hash> > test;

  std::unordered_map<std::string, std::unordered_map<std::string, std::string> > map;

  // <row, <col, fileName> >
  std::unordered_map<std::string, std::unordered_map<std::string, std::string> > keysToFile;

  FileSystem fs;

  // Log logger;

  // // <fileName, count>
  std::unordered_map<std::string, int> fileCnt;

  // bool writeSnapshot() {
  //   // writeMeta();
  //   // writeData();
  // }

  // void writeMeta() {

  // }

  /* evict the least used chunk */
  void evict() {
    if(fileCnt.size() < CACHE_SIZE) return;

    std::string lrFile = getLRFile();

    std::cout << "Write " << lrFile << " into disk." << std::endl;

    std::vector<std::pair<std::string, std::string> > keys = fileToKeys[lrFile];

    // std::vector<std::pair<std::string, std::string> > keys = fs.getKeys(lrFile);

    // write the file to disk and remove it from the cache(map).
    for(int i = 0; i < keys.size(); i++) {
      std::string row = keys[i].first;
      std::string col = keys[i].second;
      std::string val = map[row][col];

      fs.write(row, col, val);
      std::cout<< "write " << row << col << val << std::endl;
      map[row].erase(col); 

      if(map[row].size() == 0) {
        map.erase(row);
      }
    }

    fileToKeys.erase(lrFile);

    // remove the count
    fileCnt.erase(lrFile);
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

  /* put the chunk into cache */
  void updateCache(std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
         ::const_iterator rItr = chunk.begin();
    rItr++;

    std::string row = rItr->first;

    std::unordered_map<std::string, std::string>::const_iterator cItr = chunk[row].begin();
    cItr++;

    std::string col = cItr->first;

    std::string file = fs.keys_to_file(row, col);


    while(rItr != chunk.end()) {
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
      return false;
      
      // // a chunk is a file
      // std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk = fs.read(row, col);
      // if(chunk.size() == 0) {
      //   return false;
      // }

      // evict();

      // updateCache(chunk);
    }

    std::unordered_map<std::string,std::string>::const_iterator cfind = map.at(row).find(col);
    
    if (cfind == map.at(row).end()) return false;

    return true;
  }

public: 

  Cache() {
    // fs.keys_to_fileToKeys(fileToKeys);
    std::vector<std::pair<std::string, std::string> > vec;
    std::pair<std::string, std::string> p("lisa", "emails");
    vec.push_back(p);
    fileToKeys["lisaemails"] = vec;
  }

  /**
  *
  */
  std::string get(std::string row, std::string col) {
    if(!containsKey(row, col)) {
      throw std::exception();
    }

    std::string file = fs.keys_to_file(row, col);
    fileCnt[file] += 1;
    std::string val = map[row][col];

    std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    return val;
  }

  bool put(std::string row, std::string col, std::string val) {
    evict();
    // 1. add to map
    // 2. update the keys->file, file->keys mapping
    // 3. update file->cnt mapping

    std::string file = fs.keys_to_file(row, col);
    fileCnt[file] += 1;
    keysToFile[row][col] = file;

    std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    map[row][col] = val;
    return true;
  }

  bool cput(std::string row, std::string col, std::string val1, std::string val2) {
    if (!containsKey(row, col) || get(row, col) != val1) return false;

    evict();
    std::string file = fs.keys_to_file(row, col);
    fileCnt[file] += 1;
    keysToFile[row][col] = file;

    map[row][col] = val2;
    return true;
  }

  bool remove(std::string row, std::string col) {
    if(!containsKey(row, col)) return false;

    std::string file = fs.keys_to_file(row, col);
    fileCnt[file] += 1;
    // fileToKeys[file].erase(col);

    map[row].erase(col);
    if(map[row].size() == 0) {
      map.erase(row);
    }
    return true;
  }


};



