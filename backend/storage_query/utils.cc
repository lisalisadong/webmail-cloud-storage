#include <string>
#include <openssl/md5.h>
#include <limits.h>
#include "utils.h"


/* Define the state of the MD5 Algorithm. */
// typedef struct md5_state_s {
//     std::md5_word_t count[2];	/* message length in bits, lsw first */
//     std::md5_word_t abcd[4];		/* digest buffer */
//     std::md5_byte_t buf[64];		/* accumulate block */
// } md5_state_t;

unsigned int simpleHash(std::string& str) {
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

size_t Hash::operator()(const std::pair<std::string, std::string>& p) const {
    std::string str = p.first + p.second;
    return simpleHash(str);
  // return 1;
}

long get_hash_val(std::string addr) {
    // int i;
    // long hash = 0;
    // unsigned char digest[16];
 
    // /*调用MD5相关函数，生成instr的MD5码，存入digest*/
    // md5_state_t md5state;
    // md5_init(&md5state);
    // md5_append(&md5state, (const unsigned char *)addr.c_str(), addr.length());
    // md5_finish(&md5state, digest);
 
	unsigned char digestBuffer[MD5_DIGEST_LENGTH];
	MD5_CTX c;
	long hashcode = 0;

	MD5_Init(&c);
	MD5_Update(&c, (const unsigned char*) addr.c_str(), addr.length());
	MD5_Final(digestBuffer, &c);

    // 每四个字节构成一个32位整数，
    // 将四个32位整数相加得到instr的hash值（可能溢出） 
    for(int i = 0; i < 4; i++) {
        hashcode += ((long)(digestBuffer[i*4 + 3]&0xFF) << 24)
              | ((long)(digestBuffer[i*4 + 2]&0xFF) << 16)
              | ((long)(digestBuffer[i*4 + 1]&0xFF) <<  8)
              | ((long)(digestBuffer[i*4 + 0]&0xFF));
    }
    return hashcode;
}

long hash2(std::string addr) {
	long hash1 = get_hash_val(addr);

	long half = LONG_MAX / 2;
	long full = LONG_MAX;

	half = half - (full - hash1);

	if(half < 0) half += full;

	return half;

	// return hash1 / 2;
}

std::string deserialize_next(std::string str, int& pos) {
	std::size_t found = str.find(":", pos);
	std::string lenStr = str.substr(pos, found - pos);	
	int len = std::stoi(lenStr);
	std::string ret = str.substr(found + 1, len);
	pos = found + 1 + len;
	return ret;
}

std::string serialize(std::string str) {
	int strLen = str.length();
	return std::to_string(strLen) + ":" + str;
}

std::string get_real_addr(std::string virtual_addr) {
	std::size_t found = virtual_addr.find("_");
	return virtual_addr.substr(0, found);
}







