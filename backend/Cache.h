#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <pthread.h>
#include <sstream>
#include <signal.h>
#include <fcntl.h>

#include <map>

using namespace std;

class Cache {
public: 

  // map<string, map<string, int>> index;

  // map<int, string> map;

  map<string, string> cache;

  // int size;

  Cache() {

  }


  string get(string id) {
    if(cache.find(id) == cache.end()) {
      cout<<"Key not found!!!"<<endl;
      return "";
    }

    return cache.find(id)->second;
  }

  
  string set(string id, string val) {
    cache.insert(map<string, string>::value_type(id, val));
  }

};



























