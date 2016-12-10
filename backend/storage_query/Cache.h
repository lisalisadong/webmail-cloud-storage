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

  // Log logger;

  // <fileName, count>
  std::unordered_map<std::string, int> fileCnt;

  int wrtCnt;

  /************************************ methods ***********************************************/
  bool writeSnapshot() {
    wrtCnt++;

    if(wrtCnt < WRT_OP) return false;

    wrtCnt = 0;

    writeMeta();
    writeData();
  } 

  /* write meta data into file system */
  void writeMeta() {
    // std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
    //     ::const_iterator rItr;

    // for(rItr = keysToFile.begin(); rItr != keysToFile.end(); ++rItr) {
    //   std::string row = rItr->first;

    //   std::unordered_map<std::string, std::string>::const_iterator cItr;

    //   std::unordered_map<std::string, std::string> cols = rItr->second;
    //   for(cItr = cols.begin(); cItr != cols.end(); ++cItr) {
    //     std::string col = cItr->first;
    //     std::string val = cItr->second;

    //     fs.writeMeta(row, col, val);
    //   }
    // }

  }

  /* write to map to fiel system */
  void writeData() {

    std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
        ::const_iterator rItr;

    for(rItr = map.begin(); rItr != map.end(); ++rItr) {
      std::string row = rItr->first;

      std::unordered_map<std::string, std::string>::const_iterator cItr;

      std::unordered_map<std::string, std::string> cols = rItr->second;
      for(cItr = cols.begin(); cItr != cols.end(); ++cItr) {
        std::string col = cItr->first;
        std::string val = cItr->second;

        fs.write(row, col, val);
      }
    }
  }

  /* evict the least used chunk */
  void evict() {
    if(fileCnt.size() < CACHE_SIZE) return;

    std::string lrFile = getLRFile();

    std::cout << "Write " << lrFile << " into disk." << std::endl;

    std::unordered_set<std::pair<std::string, std::string>, Hash> keys = fileToKeys[lrFile];

    // write the file to disk and remove it from the cache(map).
    for (auto p = keys.begin(); p != keys.end(); ++p) {
      std::string row = p->first;
      std::string col = p->second;
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

  /* put a chunk into cache */
  void updateCache(std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk) {

    std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
         ::const_iterator rItr = chunk.begin();
    rItr++;

    std::string row = rItr->first;

    std::unordered_map<std::string, std::string>::const_iterator cItr = chunk[row].begin();
    cItr++;

    std::string col = cItr->first;

    std::string file = keysToFile[row][col];

    while(rItr != chunk.end()) {
      row = rItr->first;

      cItr = chunk[row].begin();

      cItr++;

      while (cItr != chunk[row].end()) {
        col = cItr->first;

        map[row][col] = chunk[row][col];    // put into map

        std::pair<std::string, std::string> p(row, col);

        cItr++;
      }

      rItr++;
    }

  }

  bool containsKey(std::string row, std::string col) {

    // check meta data first
    std::unordered_map<std::string, std::unordered_map<std::string, std::string> >
         ::const_iterator rfind = keysToFile.find(row);

    if (rfind == keysToFile.end()) {
      return false;
    }

    std::unordered_map<std::string,std::string>::const_iterator cfind = keysToFile.at(row).find(col);
    if (cfind == keysToFile.at(row).end()) return false;

    /* check if in map */
    rfind = map.find(row);
    if (rfind == map.end() || rfind->second.find(col) == rfind->second.end()) {
      return false;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk = fs.read(row, col);
    
    evict();

    updateCache(chunk);

    return true;
  }

public: 

  Cache() {
    // fs.keys_to_fileToKeys(fileToKeys);

    wrtCnt = 0;

    fs.initMeta(keysToFile, fileToKeys);

    // std::unordered_set<std::pair<std::string, std::string>, Hash> set;
    // std::pair<std::string, std::string> p("lisa", "emails");
    // set.insert(p);
    // fileToKeys["lisaemails"] = set;
  }

  /**
  *
  */
  std::string get(std::string row, std::string col) {
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
    evict();
    // 1. add to map
    // 2. update the keys->file, file->keys mapping
    // 3. update file->cnt mapping

    std::string file = fs.keys_to_file(row, col);
    fileCnt[file] += 1;
    keysToFile[row][col] = file;

    std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    map[row][col] = val;

    writeSnapshot();

    return true;
  }

  bool cput(std::string row, std::string col, std::string val1, std::string val2) {
    if (!containsKey(row, col) || get(row, col) != val1) return false;

    evict();
    std::string file = fs.keys_to_file(row, col);
    fileCnt[file] += 1;
    keysToFile[row][col] = file;

    map[row][col] = val2;

    writeSnapshot();

    return true;
  }

  bool remove(std::string row, std::string col) {
    if(!containsKey(row, col)) return false;

    std::string file = fs.keys_to_file(row, col);
    fileCnt[file] += 1;
    std::pair<std::string, std::string> p(row, col);
    fileToKeys[file].erase(p);

    map[row].erase(col);
    if(map[row].size() == 0) {
      map.erase(row);
    }

    writeSnapshot();

    return true;
  }


};



