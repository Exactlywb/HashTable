# HashTable optimization

## Task

* Realize a hash table using the chaining method
* Hash functions research
* Analyze the slowest functions and optimize them.

## Hash table realization

My hash table uses the chain method in dealing with collisions. More about chain method you may see on [wikipedia](https://en.wikipedia.org/wiki/Hash_table) or in short paragraph about it in my [_LaTex_](https://github.com/Exactlywb/HashTable/blob/master/HashFunctions.pdf) report. 

You may see my unoptimized version of hash table [here](https://github.com/Exactlywb/HashTable/tree/master/unoptimized).

## Hash functions research

[_LaTex report version_](https://github.com/Exactlywb/HashTable/blob/master/HashFunctions.pdf)

In this part of work I realized 7 hash functions:

* ### _trash hash_ or _useless hash_ or _hash which just returns 1_

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/uselessHash.png" width = "400"></center>

* ### _length hash_ which returns length of our string.

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/stillUselessHash.png" width = "400"></center>

* ### _ascii summary hash_ which returns sum of ascii numbers of chars in string

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/asciiSum.png" width = "400"></center>

* ### _first symbol hash_ which returns first symbol ascii code

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/firstSymb.png" width = "400"></center>

* ### _ROR hash_  uses ROR to count hash. It's the first real hash, so it's better to show the result.

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/RORHash.png" width = "400"></center>

* ### _Murmur2A hash_ is realization of famous hash function that is slower than standart _Murmur_ for 30-40% but it leaves less collisions.

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/MurmurHash2A.png" width = "400"></center>

* ### _CRC32 hash_ or _cyclic redundancy check_ is based on theory of cyclic error-correcting codes.

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/CRC.png" width = "400"></center>

So _CRC32_ and _Murmur_ show the best results, thus we will optimize our code with _CRC32_, because optimization of _Murmur_ doesn't look so easy.

## Optimization

Firstly we have to look into _callgrind_ and find the most time consuming functions and then we should try to fix them.

//картинка сюда

As we can see the slowest is _malloc_, _FillTable_, _PushTable_ and our _CRC32_ hash. After a long fight between angel and demon on my shoulders I decided that I'm not a superman. That's why I won't rewrite _malloc_ function, but I'll apply the trick. It's known that working with memory always needs much time due to many reasons, but we'll win time by allocating large pieces of memory! Let's look at our function

```cpp
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
```

So it doesn't look smart: each time in cicle we allocate memory by little pieces and _malloc_ constantly has to start a search for us. It would be better if we allocate large memory one time and then just separate it. 

New code:

```cpp
List* CallocForLists (List* memory, size_t index) {

    assert (memory);

    return (memory + index);

}

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

    List* bigMemory = (List*)calloc (size, sizeof (List));

    for (size_t listNum = 0; listNum < size; listNum++) {

        TABLE_DATA [listNum] = CallocForLists (bigMemory, listNum);
        ListInit (TABLE_DATA [listNum]);

    }

    hashTable->_hash = hash;

    hashTable->_csvFileName = csvFileName;

}
```

Let's check our turbo acceleration:

//картинка

A bit faster. Let's continue and realize the same method with allocation for _Dictionary*_. This structure appears when we're reading our file. A function we'll optimize:

```cpp

Dictionary* BuildDictionary (const char* file) {

    FILE* readFile = fopen (file, "r");
    assert (readFile);

    size_t filesize = GetFileSize (readFile);
    char* buffer    = ReadBuffer  (readFile, filesize);

    size_t countLines = GetCountLines (buffer);
    Line** dictionary = (Line**)calloc (countLines, sizeof (Line*));

    size_t countSymb  = 0;

    for (size_t counterForLines = 0; counterForLines < countLines; counterForLines++) {

        dictionary [counterForLines]            = (Line*)calloc (1, sizeof (Line));

        dictionary [counterForLines]->_str      = buffer + countSymb;
        dictionary [counterForLines]->_length   = 0;

        while ( *(buffer + countSymb) != '\n' && *(buffer + countSymb) != '\0' ) {

            dictionary [counterForLines]->_length++;
            countSymb++;

        }

        countSymb++;

    }

    fclose (readFile);

    Dictionary* result = (Dictionary*)calloc (1, sizeof(Dictionary));
    result->_data   = dictionary;
    result->_size   = countLines;
    result->_buffer = buffer;

    return result;

}

```

And then we get:

```cpp

Line* CallocForLine (Line* bigMemory, size_t index) {

    assert (bigMemory);

    return (bigMemory + index);

}

Dictionary* BuildDictionary (const char* file) {

    FILE* readFile = fopen (file, "r");
    assert (readFile);

    size_t filesize = GetFileSize (readFile);
    char* buffer    = ReadBuffer  (readFile, filesize);

    size_t countLines = GetCountLines (buffer);
    Line** dictionary = (Line**)calloc (countLines, sizeof (Line*));

    size_t countSymb  = 0;
    Line*  bigMemory  = (Line*)calloc (countLines, sizeof (Line));
    for (size_t counterForLines = 0; counterForLines < countLines; counterForLines++) {

        dictionary [counterForLines]            = CallocForLine (bigMemory, counterForLines);

        dictionary [counterForLines]->_str      = buffer + countSymb;
        dictionary [counterForLines]->_length   = 0;

        while ( *(buffer + countSymb) != '\n' && *(buffer + countSymb) != '\0' ) {

            dictionary [counterForLines]->_length++;
            countSymb++;

        }

        countSymb++;

    }

    fclose (readFile);

    Dictionary* result = (Dictionary*)calloc (1, sizeof(Dictionary));
    result->_data   = dictionary;
    result->_size   = countLines;
    result->_buffer = buffer;

    return result;

}

```

So the optimization we got:

//картинка сюда

Optimization with allocation of large memory is a bit more benefitial, but it doesn't provide necessary result. It may be just a time filler's mistake. Let's take a more complicated problem and optimize our _CRC32_. The old code with _CRC32_:

```cpp
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
```

For optimization our hash function we'll use __mm_crc32_u8_ from SSE optimization. So the new code:

```cpp
unsigned int CRC32 (Line* string) {

    unsigned int crc = 0;

    for (size_t symbNum = 0; symbNum < string->_length; symbNum++) 
        crc = _mm_crc32_u8 (crc, *(string->_str + symbNum) );

    return crc;

}
```

Let's count our acceleration:

//картинка сюда

_God bless Intel_

Some of our functions are still not used. Of course I can try to optimize all of our functions but it looks useless for our project. Guided by this conclusion I decided to optimize one of the most useful functions _FindElemInTable_. I have to say that this function is not independence. _FindElemInTable_ calls _ListFindElem_ which is already optimized by _O2_ without my participation. But we can optimize _LineCmp_. We'll do it with _SSE_ tricks. Old code we had:

```cpp
int LineCmp (Line* firstLine, Line* secondLine) {

    assert (firstLine);
    assert (secondLine);

    if (firstLine->_length == secondLine->_length) {

        for (size_t symb = 0; symb < firstLine->_length; symb++) {

            if (firstLine->_str [symb] != secondLine->_str[symb])
                return (firstLine->_str [symb] - secondLine->_str[symb]);

        }

        return 0;

    } else
        return (firstLine->_length - secondLine->_length);

}
```

Let's use _SSE_:

```cpp
int LineCmp (Line* firstLine, Line* secondLine) {

    assert (firstLine);
    assert (secondLine);

    int cmpResult         = 0;

    __m128i firstStr  = {0};
    __m128i secondStr = {0};

    if (firstLine->_length == secondLine->_length) {

        for (size_t startSymb = 0; startSymb < firstLine->_length; startSymb += 16) {
            
            firstStr  = _mm_loadu_si128 ((__m128i*)(firstLine->_str + startSymb));
            secondStr = _mm_loadu_si128 ((__m128i*)(secondLine->_str + startSymb));

            cmpResult = _mm_cmpestri    (firstStr, 16, secondStr, 16, _SIDD_CMP_EQUAL_EACH);

            if (cmpResult != 0)
                break;
            
        }

    } else
        return (firstLine->_length - secondLine->_length);

    return cmpResult;

}
```

So it gives little effect:

//картинка

<center><img src = "https://www.google.com/url?sa=i&url=http%3A%2F%2Fwww.gta.ru%2Fforums%2Fgta_san_andreas%2Ftalk%2Fboltalka_gta_ru-787942%2F&psig=AOvVaw0eRMHndZmrSJOQ-Yy4zCsi&ust=1618060106528000&source=images&cd=vfe&ved=0CAIQjRxqFwoTCMD2uOWd8e8CFQAAAAAdAAAAABAO" width = "200"></center>