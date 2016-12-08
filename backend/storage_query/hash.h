#include <unordered_map>
#include <unordered_set>
#include <string>
#include <iostream>
#include <memory>
#include <limits.h>
#include <vector>
#include <utility>

struct Hash {
  size_t operator()(const std::pair<std::string, std::string>& p) const {
      std::string str = p.first + p.second;
      return simpleHash(str);
    // return 1;
  }
};

unsigned int simpleHash(std::string& str)
{
    unsigned int b    = 378551;
    unsigned int a    = 63689;
    unsigned int hash = 0;

    for(std::size_t i = 0; i < str.length(); i++)
    {
        hash = hash * a + str[i];
        a    = a * b;
    }

    return (hash & 0x7FFFFFFF);
  // return 1;
 }