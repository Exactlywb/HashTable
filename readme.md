# HashTable optimization

## Task

* Realize a hash table using the chaining method
* Hash functions research
* Analyze the slowest functions and optimize them.

## Hash table realization

My hash table uses the chain method to deal with collisions. More unformation about chain method you may see on [wikipedia](https://en.wikipedia.org/wiki/Hash_table) or in short paragraph about it in my [_LaTex_](https://github.com/Exactlywb/HashTable/blob/master/HashFunctions.pdf) report. 

You may see the unoptimized version of hash table [here](https://github.com/Exactlywb/HashTable/tree/master/unoptimized).

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

* ### _ROR hash_  uses ROR to count hash. It's the first real hash, so it's interesting to have a loot at the result.

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/RORHash.png" width = "400"></center>

* ### _Murmur2A hash_ is realization of famous hash function that is slower than standart _Murmur_ for 30-40%, but, however, it leaves less collisions.

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/MurmurHash2A.png" width = "400"></center>

* ### _CRC32 hash_ or _cyclic redundancy check_ is based on theory of cyclic error-correcting codes.

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/optimized/csv/CRC.png" width = "400"></center>

So _CRC32_ and _Murmur_ show the best results, thus we will optimize our code with _CRC32_, because optimization of _Murmur_ doesn't look so easy.

## Optimization

Firstly we have to look into _callgrind_ and find the most time-consuming functions and consequently we should try to fix them.

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/firstPics/withoutOpt.PNG"        width = "600"></center>
<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/firstPics/withoutOptKcache.PNG"  width = "600"></center>

As we can see the slowest is _malloc_, _FillTable_, _PushTable_ and our _CRC32_ hash. After a long fight between angel and demon on my shoulders I decided that I'm not a superman. That's why I won't rewrite _malloc_ function, but I'll apply the trick. It's known that working with memory always needs much time due to many reasons, but we'll win time by allocating large pieces of memory! Let's look at our function

```cpp
void HashTableConstructor (HashTable* hashTable, size_t size, unsigned int (*hash) (Line* string), const char* csvFileName) {

    hashTable->_size = size;

    TABLE_DATA = (List**)calloc (size, sizeof (List*));

    for (size_t listNum = 0; listNum < size; listNum++) {

        TABLE_DATA [listNum] = (List*)calloc (1, sizeof (List));
        ListInit (TABLE_DATA [listNum]);

    }

    hashTable->_hash = hash;

    hashTable->_csvFileName = csvFileName;

}
```

The are some disadvantages in it: each time in cicle we allocate memory by little pieces and _malloc_ constantly has to start a search for us. It would be better if we allocate large memory one time and then just separate it. 

New code:

```cpp
List* CallocForLists (List* memory, size_t index) {

    return (memory + index);

}

void HashTableConstructor (HashTable* hashTable, size_t size, unsigned int (*hash) (Line* string), const char* csvFileName) {

    hashTable->_size = size;

    TABLE_DATA = (List**)calloc (size, sizeof (List*));

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

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/secondPocs/hashCalloc.PNG" width = "600"></center>

A bit faster. Let's continue and realize the same method with allocation for _Dictionary*_. This structure appears when we're reading our file. Here you can see the function we'll optimize:

```cpp

Dictionary* BuildDictionary (const char* file) {

    FILE* readFile = fopen (file, "r");

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

    return (bigMemory + index);

}

Dictionary* BuildDictionary (const char* file) {

    FILE* readFile = fopen (file, "r");

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

Eventually, the optimization we got:

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/secondPocs/dictionaryCalloc.PNG" width = "600"></center>

Optimization with allocation of large memory is a bit more benefitial, but it doesn't provide required result. It may be just a time filler's mistake. Let's take a more complicated task and optimize our _CRC32_. The old code with _CRC32_:

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

To optimize our hash function we'll use __mm_crc32_u8__ from SSE optimization. So the new code:

```cpp
unsigned int CRC32 (Line* string) {

    unsigned int crc = 0;

    for (size_t symbNum = 0; symbNum < string->_length; symbNum++) 
        crc = _mm_crc32_u8 (crc, *(string->_str + symbNum) );

    return crc;

}
```

Let's count our acceleration:

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/thirdPics/CRCtime.PNG"   width = "600"></center>
<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/thirdPics/CRCKcache.PNG" width = "600"></center>

_God bless Intel_

Some of our functions are still not used.  I,surely, can try to optimize all of our functions but it looks useless for our project. Guided by this conclusion I decided to optimize one of the most useful functions _FindElemInTable_. I have to say that this function is not independent. _FindElemInTable_ calls _ListFindElem_ which has already been optimized by _O2_ without my participation. But we can optimize _LineCmp_. We'll do it with _SSE_ tricks. Old code we had:

```cpp
int LineCmp (Line* firstLine, Line* secondLine) {

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
I have to say that it isn't so fast:
<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/searchOpt/NoOptTime.PNG"   width = "600"></center>
<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/searchOpt/NoOptKcache.PNG" width = "600"></center>

Let's use _SSE_:

```cpp
int LineCmp (Line* firstLine, Line* secondLine) {

    int cmpResult         = 0;

    __m128i firstStr  = {0};
    __m128i secondStr = {0};

    if (firstLine->_length == secondLine->_length) {

        for (size_t startSymb = 0; startSymb < firstLine->_length; startSymb += 16) {
            
            firstStr  = _mm_loadu_si128 ((__m128i*)(firstLine->_str + startSymb));
            secondStr = _mm_loadu_si128 ((__m128i*)(secondLine->_str + startSymb));

            cmpResult = _mm_cmpestri    (firstStr, firstLine->_length - startSymb > 16 ? 16 : (firstLine->_length - startSymb), secondStr, secondLine->_length - startSymb > 16 ? 16 : (secondLine->_length - startSymb), _SIDD_CMP_EQUAL_EACH);

            if (cmpResult != 0)
                break;
            
        }

    } else
        return (firstLine->_length - secondLine->_length);

    return cmpResult;

}
```

Thus it provides some benefit:

<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/searchOpt/YesOptTime.PNG" width = "600"></center>
<center><img src = "https://github.com/Exactlywb/HashTable/blob/master/screenshots/searchOpt/YesOptKcache.PNG" width = "600"></center>

Finally we got 75% optimization or in 4 times!

<center><img src = "https://gtaguide.net/wp-content/uploads/jobs-in-gta-online-5-1.jpg" width = "400"></center>
