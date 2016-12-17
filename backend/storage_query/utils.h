#ifndef UTS_
#define UTS_

#include <string>
#include <openssl/md5.h>
#include <limits.h>


/* Define the state of the MD5 Algorithm. */
// typedef struct md5_state_s {
//     std::md5_word_t count[2];	/* message length in bits, lsw first */
//     std::md5_word_t abcd[4];		/* digest buffer */
//     std::md5_byte_t buf[64];		/* accumulate block */
// } md5_state_t;
unsigned int simpleHash(std::string& str);

struct Hash {
  size_t operator()(const std::pair<std::string, std::string>& p) const;
};

long get_hash_val(std::string addr);

long hash2(std::string addr);

std::string deserialize_next(std::string str, int& pos);

std::string serialize(std::string str);

std::string get_real_addr(std::string virtual_addr);

#endif







