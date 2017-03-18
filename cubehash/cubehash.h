/*
20081110
D. J. Bernstein
Public domain.
*/

#ifndef cubehash_h
#define cubehash_h

#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include "crypto_uint32.h"

using std::string;
using std::cout;
using std::cin;
using std::endl;

typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHBITLEN = 2 } HashReturn;


typedef struct {
  int hashbitlen;
  int pos; /* number of bits read into x from current block */
  crypto_uint32 x[32];
} hashState;

HashReturn Init(hashState *state, int hashbitlen);
HashReturn Update(hashState *state, const BitSequence *data, DataLength databitlen);
HashReturn Final(hashState *state, BitSequence *hashval);
HashReturn Hash(int hashbitlen, const BitSequence *data, DataLength databitlen, BitSequence *hashval);

/*
Simple CubeHash class by Vlasov Aleksander 
2017
*/

class CubeHash
{
public:

    CubeHash(string message) :
        hash_(new crypto_uint8 [bits_ / 8])
    {
        setMessage(message);
    }

    CubeHash() :
        hash_(new crypto_uint8 [bits_ / 8])
    {

    }

    ~CubeHash()
    {
        delete [] hash_;
    }

    string getMessage() const
    {
        return message_;
    }

    string getHash()
    {
		hashStringGen();
        return hash_str_;
    }

    crypto_uint8 getHashChar() const
    {
        return *hash_;
    }

    void setMessage(string message)
    {
        message_ = string(message);
        if (Init(&state_, bits_) != SUCCESS)
            throw 1;
        if (Update(&state_, (unsigned char*)(message.c_str()), message.length() * 8) != SUCCESS)
            throw 1;
        if (Final(&state_, hash_) != SUCCESS)
            throw 1;
    }

    void setHash(string hash)
    {
        if (hash.length() != bits_ / 4)
            throw 1;
        message_ = string(" ");
        for (size_t i = 0; i < hash.length() / 2; i++)
        {
               hash_[i] = (crypto_uint8)strtoul(hash.substr(i * 2, 2).c_str(), 0, 16);
        }
    }

    void show()
    {
		hashStringGen();
        cout << "Mesg: " << message_ << endl;
        cout << "Hash: " << hash_str_ << endl << endl;
    }

	int hashStrLength() const
	{
		return bits_ / 4;
	}

	int hashBitLength() const
	{
		return bits_;
	}

private:

    hashState state_;
    int bits_ = 128;

    string message_;
    string hash_str_;
    crypto_uint8 *hash_;
    friend inline bool operator ==(const CubeHash& h1, const CubeHash& h2);

    void hashStringGen()
    {
        char *str = new char [bits_ / 4 + 1];
        for (int i = 0; i < bits_ / 8; i++)
        {
            sprintf(str + 2 * i,"%02x", hash_[i]);
        }
        hash_str_ = string(str);
        delete [] str;
    }

};

inline bool operator ==(const CubeHash& h1, const CubeHash& h2)
{
    crypto_uint8 *hash1 = h1.hash_, *hash2 = h2.hash_;
    if (h1.bits_ != h2.bits_)
    {
        return false;
    }
    for (int i = 0; i < h1.bits_ / 8; i++)
    {
            if (hash1[i] != hash2[i])
            {
                return false;
            }
    }
    return true;
}


#endif
