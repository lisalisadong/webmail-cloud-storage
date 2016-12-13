#include <string>
#include <openssl/md5.h>


/* Define the state of the MD5 Algorithm. */
// typedef struct md5_state_s {
//     std::md5_word_t count[2];	/* message length in bits, lsw first */
//     std::md5_word_t abcd[4];		/* digest buffer */
//     std::md5_byte_t buf[64];		/* accumulate block */
// } md5_state_t;


class Hasher
{

public:
    long getHashVal(std::string addr)
	{
	    // int i;
	    // long hash = 0;
	    // unsigned char digest[16];
	 
	    // /*调用MD5相关函数，生成instr的MD5码，存入digest*/
	    // md5_state_t md5state;
	    // md5_init(&md5state);
	    // md5_append(&md5state, (const unsigned char *)addr.c_str(), addr.length());
	    // md5_finish(&md5state, digest);
	 
	    //  每四个字节构成一个32位整数，
	    // 将四个32位整数相加得到instr的hash值（可能溢出） 
	    // for(i = 0; i < 4; i++)
	    // {
	    //     hash += ((long)(digest[i*4 + 3]&0xFF) << 24)
	    //           | ((long)(digest[i*4 + 2]&0xFF) << 16)
	    //           | ((long)(digest[i*4 + 1]&0xFF) <<  8)
	    //           | ((long)(digest[i*4 + 0]&0xFF));
	    // }
	    // return hash;


	    return 1;
	}

};