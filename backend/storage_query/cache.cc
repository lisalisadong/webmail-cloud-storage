#include "cache.h"
#include "utils.h"

/**************************public methods*******************************/
std::string Cache::get(std::string row, std::string col) {
    if(!containsKey(row, col)) {
        throw std::exception();
    }

    std::string file = keysToFile[row][col];
    fileCnt[file] += 1;
    std::string val = map[row][col];

    // std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    return val;
}

bool Cache::put(std::string row, std::string col, std::string val) {
    // 1. add to map
    // 2. update the keys->file, file->keys mapping
    // 3. update file->cnt mapping

    std::string file;
    if (containsKey(row, col)) {
        file = keysToFile[row][col];
    } else {
        file = fs.place_new_entry();
    }

    currFile = file;

    /* file counter plus 1 */
    fileCnt[file] += 1;
    // std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    if(!containsKey(row, col)) {
        /* add keys -> file mapping */
        keysToFile[row][col] = file;
        // std::cout<< "add mapping: <" << row << ", " << col << "> -> " << keysToFile[row][col] <<std::endl;

        /* add file -> keys mapping */
        std::pair<std::string, std::string> p(row, col);
        if(fileToKeys.find(file) == fileToKeys.end()) {
            std::unordered_set<std::pair<std::string, std::string>, Hash> set;
            fileToKeys[file] = set;
        }
        fileToKeys[file].insert(p);
        // std::cout<< "add mapping: " << file << "-> <" << p.first << ", " << p.second << ">" <<std::endl;
    }

    map[row][col] = val;
    // std::cout<< "add mapping: <" << row << ", " << col << "> -> " << map[row][col] <<std::endl;

    writeSnapshot();

    if(fileToKeys.size() > CACHE_SIZE) evict();

    fs.write_log(file, row, col, val, "PUT");

    return true;
}

bool Cache::cput(std::string row, std::string col, std::string val1, std::string val2) {
    if (!containsKey(row, col) || get(row, col) != val1) return false;

    std::string file = keysToFile[row][col];
    currFile = file;

    fileCnt[file] += 1;
    // std::cout<<file<<" is accessed "<<fileCnt[file]<<" times."<<std::endl;

    map[row][col] = val2;
    // std::cout<< "add mapping: <" << row << ", " << col << "> -> " << map[row][col] <<std::endl;

    writeSnapshot();

    fs.write_log(file, row, col, val2, "PUT");

    return true;
}

bool Cache::remove(std::string row, std::string col) {
    if(!containsKey(row, col)) return false;

    std::string file = keysToFile[row][col];
    currFile = file;
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

    fs.write_log(file, row, col, "", "DELETE");

    return true;
}

void Cache::migrate(std::string selfAddr, std::string otherAddr, std::string& data) {
    long selfHash = get_hash_val(selfAddr);
    long otherHash = get_hash_val(otherAddr);
    std::vector<std::pair<std::string, std::string> > toDelete;
    for (auto fp = keysToFile.begin(); fp != keysToFile.end(); fp++) {
        long entryHash = get_hash_val(fp->first);
        if (entryHash <= otherHash || entryHash > selfHash) {
            for (auto sp = fp->second.begin(); sp != fp->second.end(); sp++) {
                std::string val = get(fp->first, sp->first);
                data += serialize(serialize(fp->first) + serialize(sp->first) + serialize(val));
                logger.log_trace("migrating " + data);
                toDelete.push_back(std::make_pair(fp->first, sp->first));
            }
        }
    }
    for (auto p : toDelete) {
        remove(p.first, p.second);
    }
}

int Cache::get_raw_data(int start, int size, std::string& data) {
    int ret = 0;
    for (auto fp = keysToFile.begin(); fp != keysToFile.end(); fp++) {
        for (auto sp = fp->second.begin(); sp != fp->second.end(); sp++) {
            if (start > 0) {
                start--;
                continue;
            }
            if (size == 0) break;
            std::string val = get(fp->first, sp->first);
            data += serialize(serialize(fp->first) + serialize(sp->first) + serialize(val));
            ret++;
        }
    }
    return ret;
}


/**************************private methods*******************************/

bool Cache::writeSnapshot() {
    wrtCnt++;

    if(wrtCnt < WRT_OP) return false;

    wrtCnt = 0;

    logger.log_trace("start writing snapshot...");
    writeMeta();
    writeData();

    logger.log_trace("Snapshot finished writing");
} 

/* write meta data into file system */
void Cache::writeMeta() {
    fs.write_file(serverAddress + "mapping", keysToFile);
    // std::cout << "Meta data write succeeded" << std::endl;
}

/* write to map to file system */
void Cache::writeData() {

    for(auto f = fileCnt.begin(); f != fileCnt.end(); ++f) {
        writeFileToFs(f->first, false);
        // std::cout << f->first << " write succeeded" << std::endl;
    }

    fs.clear_temp_log();

}

/* evict the least used chunk */
void Cache::evict() {
    if(fileCnt.size() < CACHE_SIZE) {
        // std::cout<< "Nothing to evict!" << std::endl;
        return;
    }

    std::string lrFile = getLRFile();

    logger.log_trace("Evict: " + lrFile);

    writeFileToFs(lrFile, true);
}

void Cache::writeFileToFs(std::string file, bool isDelete) {

    std::unordered_set<std::pair<std::string, std::string>, Hash> keys = fileToKeys[file];

    std::unordered_map<std::string, std::unordered_map<std::string, std::string> > tmpMap;

    // write the file to disk and remove it from the cache(map).
    for (auto p = keys.begin(); p != keys.end(); ++p) {
        std::string row = p->first;
        std::string col = p->second;

        // if(!isDelete && (map.find(row) == map.end() || map[row].find(col) == map[row].end()) continue;

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

    logger.log_trace("Write " + file + " into disk.");
    fs.write_file(file, tmpMap);
}

/* get the least used file */
std::string Cache::getLRFile() {

    int cnt = INT_MAX;
    std::string lrFile;
    std::unordered_map<std::string, int>::const_iterator itr = fileCnt.begin();

    while(itr != fileCnt.end()) {
        if(itr->second < cnt && itr->first != currFile) {
            cnt = itr->second;
            lrFile = itr->first;
        }
        itr++;
    }

    return lrFile;
}

/* put a chunk into cache */
// void Cache::updateCache(std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk) {
//     if(chunk.size() <= 0) return;

//     for(auto rItr = chunk.begin(); rItr != chunk.end(); ++rItr) {
        
//         std::string row = rItr->first;

//         for(auto cItr = chunk[row].begin(); cItr != chunk[row].end(); ++cItr) {
//             std::string col = cItr->first;

//             map[row][col] = chunk[row][col];
//         }
//     } 

// }

bool Cache::containsKey(std::string row, std::string col) {

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

        // std::unordered_map<std::string, std::unordered_map<std::string, std::string> > chunk = fs.read_file(file);

        evict();

        fs.read_file(file, map);
      
        logger.log_trace("read " + file + " into cache ");

      

      // updateCache(chunk);
    }

    return true;

}