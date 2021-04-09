#include "hashtable/hashtable.h"
#include <nmmintrin.h>

unsigned int UselessHash (Line* string) { //trash

    return 1;

}

unsigned int LengthHash (Line* string) { //still useless

    return (unsigned int)string->_length;

}

unsigned int AsciiSum (Line* string) { //not so useless but so bad so still useless

    unsigned int resultHash = 0;

    for (size_t symb = 0; symb < string->_length; symb++) 
        resultHash += string->_str [symb];

    return resultHash;

}

unsigned int firstSymbHash (Line* string) { //That was not my idea, for real

    return string->_str [0];

}

#define ROR(num) ((num << 31) | (num >> 1))

unsigned int RORHash (Line* string) { //not bad, not bad, but still not good or i am an idiot :(

    unsigned int resultHash   = 0;
    unsigned int prevSymbHash = 0;

    for (size_t symbNum = 1; symbNum < string->_length; symbNum++) {

        prevSymbHash = ROR (prevSymbHash) xor string->_str [symbNum];
        resultHash   += prevSymbHash;

    }

    return resultHash;

}

unsigned int MurmurHash2A (Line* string) {

    const unsigned int magicConst   = 0x5bd1e995;
    const unsigned int seed         = 0;
    const          int marg         = 24;

          unsigned int hash         = seed ^ string->_length;
    
    const unsigned char*  data      = (const unsigned char*)string->_str;
                   size_t len       = string->_length;

          unsigned int    symb      = 0; 

    while (len >= 4) {

        symb  = data [0];
        symb |= data [1] << 8;
        symb |= data [2] << 16;
        symb |= data [3] << 24;

        symb *= magicConst;
        symb ^= symb >> marg;
        symb *= magicConst;

        hash *= magicConst;
        hash ^= symb;

        data += 4;
        len  -= 4;

    }      

    switch (len) {

        case 3:
            hash ^= data [2] << 16;
        case 2:
            hash ^= data [1] << 8;
        case 1:
            hash ^= data [0];
            hash *= magicConst;

    };

    hash ^= hash >> 13;
    hash *= magicConst;
    hash ^= hash >> 15;

    return hash;

}
 
unsigned int CRC32 (Line* string) {

    const size_t tableSize = 256;
    
    unsigned int table [tableSize]  = {0};
    unsigned int crc                = 0;

    for (size_t i = 0; i < tableSize; i++) {

        crc = i;
        for (size_t j = 0; j < 8; j++) 
            crc = crc & 1 ? (crc >> 1) ^ 0xEDB88320UL : crc >> 1;

        table [i] = crc;

    }

    crc = 0xFFFFFFFFUL;

    for (size_t symb = 0; symb < string->_length; symb++)
        crc = table [(crc ^ *(string->_str + symb)) & 0xFF] ^ (crc >> 8);

    return crc;

}

int main () {
    
    HashTable mainTable = {};
    HashTableConstructor (&mainTable, 0xFFFF, &CRC32, "csv/CRC2Hash.csv");
    
    FillTable (&mainTable, "dictionary");

    HashTableCSVOutput (&mainTable);

    Node* test = nullptr;
    for (size_t i = 0; i < 300000; i++) {

        test = FindElemInTable (&mainTable, mainTable._dict->_data[i]);

    }

    HashTableDestructor (&mainTable);

    return 0;

}