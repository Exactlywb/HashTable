#include "../list/list.h"
#include <assert.h>

enum HashTableErrors {

    NO_ERROR_TABLE

};

struct HashTable {

    unsigned int        (*_hash) (Line* string); 

    List**              _data;
    size_t              _size;

    Dictionary*         _dict;

    const char*         _csvFileName;

};

void  HashTableConstructor   (HashTable* hashTable, size_t size, unsigned int (*hash) (Line* string), const char* _csvFileName);
void  HashTableDestructor    (HashTable* hashTable);

void  FillTable              (HashTable* hashTable, const char* dictName);
void  PushTable              (HashTable* hashTable, Line* linePointer);
bool  DeleteElemFromTable    (HashTable* hashTable, Line* str);
Node* FindElemInTable        (HashTable* hashTable, Line* str);

void  HashTableDump          (HashTable* hashTable);
void  HashTableCSVOutput     (HashTable* hashTable);
