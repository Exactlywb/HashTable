#include "hashtable.h"

#define PRINT_ERR(message) printf ("%s - catched in function %s on line %d\n", message, __FUNCTION__, __LINE__)

#define TABLE_DATA  hashTable->_data
#define HASH(str)   hashTable->_hash (str)

void HashTableConstructor (HashTable* hashTable, size_t size, unsigned int (*hash) (Line* string), const char* csvFileName) {

    if (!hashTable) {

        PRINT_ERR ("Null pointer on HashTable* hashTable");
        return;
            
    }
    if (!*hash) {

        PRINT_ERR ("No function unsigned int (*hash)");
        return;

    }

    hashTable->_size = size;

    TABLE_DATA = (List**)calloc (size, sizeof (List*));
    if (!TABLE_DATA) {

        PRINT_ERR ("Bad memory: can not do calloc for __data");
        return;

    }

    for (size_t listNum = 0; listNum < size; listNum++) {

        TABLE_DATA [listNum] = (List*)calloc (1, sizeof (List));
        ListInit (TABLE_DATA [listNum]);

    }

    hashTable->_hash = hash;

    hashTable->_csvFileName = csvFileName;

}

void FillTable (HashTable* hashTable, const char* dictName) {

    if (!hashTable) {

        PRINT_ERR ("Null pointer on HashTable* hashTable");
        return;
    
    }

    Dictionary* dictionary = BuildDictionary (dictName);

    hashTable->_dict = dictionary;

    for (size_t wordNum = 0; wordNum < dictionary->_size; wordNum++)
        PushTable (hashTable, dictionary->_data [wordNum]);

}

void PushTable (HashTable* hashTable, Line* linePointer) {

    if (!hashTable) {

        PRINT_ERR ("Null pointer on HashTable* hashTable");
        return;

    }
    if (!linePointer) {

        PRINT_ERR ("Null pointer on Line* linePointer");
        return;

    }

    PushTail ( TABLE_DATA [HASH (linePointer) % hashTable->_size], NodeInit (linePointer, nullptr, nullptr));

}

void HashTableDestructor (HashTable* hashTable) {
    
    if (!hashTable) {

        PRINT_ERR ("Null pointer on HashTable* hashTable");
        return;

    }

    DictionaryDestruct (hashTable->_dict);
    for (size_t listNum = 0; listNum < hashTable->_size; listNum++) {

        ListDestroy (TABLE_DATA [listNum]);

    }

    free (TABLE_DATA [0]);

    free (TABLE_DATA);

}

void HashTableDump (HashTable* hashTable) {

    if (!hashTable) {

        PRINT_ERR("Null pointer on HashTable* hashTable");
        return;

    }

    FILE* writeInto = fopen ("hashtable.log", "w");
    if (!writeInto) {

        PRINT_ERR ("Can't open FILE* writeInto");
        return;

    }

    fprintf (writeInto, "Hash table [%p]\n"  , hashTable);
    fprintf (writeInto, "Hash table size %zu\n", hashTable->_size);

    for (size_t listNum = 0; listNum < hashTable->_size; listNum++) {

        fprintf   (writeInto, "*** List number %zu with pointer [%p]:\n", listNum, &TABLE_DATA[listNum]);
        
        fprintf   (writeInto, "\t\t");
        fprintf   (writeInto, "List [%p]\n", TABLE_DATA [listNum]);
        fprintf   (writeInto, "\t\t");
        fprintf   (writeInto, "List size %zu\n", TABLE_DATA [listNum]->_size);
        fprintf   (writeInto, "\t\t");
        fprintf   (writeInto, "* Head of list [%p]\n", TABLE_DATA [listNum]->_header);
        fprintf   (writeInto, "\t\t");
        fprintf   (writeInto, "* Tail of list [%p]\n", TABLE_DATA [listNum]->_tail);

        size_t nodeNum = 0;

        for (Node* nextNode = TABLE_DATA [listNum]->_header; nextNode != nullptr; nextNode = nextNode->_next) {
            
            fprintf   (writeInto, "\t\t");
            fprintf   (writeInto, "** Node #%zu with value %s and pointer [%p]\n", nodeNum, nextNode->_value->_str, nextNode);

            nodeNum++;

        }

    }

    fclose (writeInto);

}

void HashTableCSVOutput (HashTable* hashTable) {

    if (!hashTable) {

        PRINT_ERR ("Null pointer on Hashtable* hashTable");
        return;

    }

    FILE* csvFile = fopen (hashTable->_csvFileName, "w");
    if (!csvFile) {

        PRINT_ERR ("Can't open csv file");
        return;
        
    }

    for (size_t listNum = 0; listNum < hashTable->_size; listNum++) {

        fprintf (csvFile, "%zu\n", hashTable->_data [listNum]->_size);

    }

    fclose (csvFile);

}

bool DeleteElemFromTable (HashTable* hashTable, Line* str) {

    if (!hashTable) {

        PRINT_ERR("Null pointer on HashTable* hashTable");
        return false;

    }
    if (!str) {

        PRINT_ERR("Null pointer on char* str");
        return false;

    }

    return DeleteElemFromList (TABLE_DATA [HASH (str)], str);

}

Node* FindElemInTable (HashTable* hashTable, Line* str) {

    if (!hashTable) {

        PRINT_ERR ("Null pointer on HashTable* hashTable");
        return nullptr;

    }
    if (!str) {

        PRINT_ERR ("Null pointer on HashTable* hashTable");
        return nullptr;

    }

    return ListFindElem (TABLE_DATA [HASH (str) % hashTable->_size], str);

}
