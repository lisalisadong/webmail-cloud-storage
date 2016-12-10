#ifndef HASH
#define HASH

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <memory>
#include <limits.h>
#include <vector>
#include <utility>

unsigned int simpleHash(std::string& str);

struct Hash {
  size_t operator()(const std::pair<std::string, std::string>& p) const;
};

#endif