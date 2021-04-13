# The Hash Table

## Table of contents

- [The Hash Table](#the-hash-table)
  - [Table of contents](#table-of-contents)
  - [Background](#background)
  - [The task](#the-task)
  - [Hash functions analysis](#hash-functions-analysis)
    - [Inference](#inference)
  - [The optimization of the hash table](#the-optimization-of-the-hash-table)
    - [Inference](#inference-1)
  - [Conclusion](#conclusion)

## Background

This work was handed out by [@ded32](google.com) as part of the "Computer Architecture" course held in MIPT. 

## The task

* The implementation of the hash table using the chaining method.
* Hash functions analysis.
* The optimization of the hash table.

## Hash functions analysis

> If you want to see the full report, you may look at [LaTex document](google.com).

In this part of work I have implemented seven hash functions and carried out the research.

* _The hash which just returns 1_

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/uselessHash.png"/>
</p>

As we can see from the bar chart it isn't useful.

* _The length hash_ which returns the length of strings

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/stillUselessHash.png"/>
</p>

It also is the useless function which has too many collisions.

* _The ASCII sum hash_ which returns a sum of ASCII numbers of chars in string

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/asciiSum.png"/>
</p>

It may seem as an acceptable function but it's not true: it also has too many collisions (approximately 1000)

* _The first symbol hash_ which returns first symbol's ASCII code

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/firstSymb.png"/>
</p>

This function is not beneficial as well.

* _The ROR hash_ uses ROR to count hash. It's the first real hash, so it's interesting to have a look at the result

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/RORHash.png"/>
</p>

This hash function is better than the previous functions. It has far less collisions.

* _The Murmur2A hash_ is the implementation of the famous hash function that is slower than standard _Murmur_ for 30-40%, but, however, it has less collisions.

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/MurmurHash2A.png"/>
</p>

From all of the analysed functions this one is the most efficient.

* _The CRC32 hash_ or _cyclic redundancy check_ is based on the theory of cyclic error-correcting codes

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>

This one is as beneficial as _the Murmur2A hash_.

### Inference

_The CRC32_ and _the Murmur2A_ show the best results, thus we will optimize our code with _the CRC32 hash_, because optimization of _the Murmur2A_ doesn't look so easy.

## The optimization of the hash table

> All compilations will be done with -O2 optimization.

First, we have to look into the _callgrind_ and find the most time-consuming functions and we should try to fix them.

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>
<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>

As we can see, the slowest functions are _the CRC32_, _the FillTable_, _the PushTable_ and _the FindElemInTable_. Let's optimize _the CRC32 hash_ using intrinsics. The old code we had:

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

Then we use _the mm_crc32_u8_. So the new code:

```cpp
unsigned int CRC32 (Line* string) {

    unsigned int crc = 0;

    for (size_t symbNum = 0; symbNum < string->_length; symbNum++) 
        crc = _mm_crc32_u8 (crc, *(string->_str + symbNum) );

    return crc;

}
```

Let's look at our acceleration:

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>
<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>

_God bless Intel: we got 80% optimization!_

Now we can try to optimize _the FillTable_ function, but I have to notice that _the FillTable_ function is slow due to many calls for _the PushTable_ function. Let's look at _the PushTable_ code:

```cpp
void PushTable (HashTable* hashTable, Line* linePointer) {

    PushTail (  TABLE_DATA [HASH (linePointer) % hashTable->_size], 
                NodeInit (linePointer, nullptr, nullptr));

}
```

It is a simple function which just calls for two other's. One of them, _the CRC32 hash_ function, has already been optimized and the other one, _the NodeInit_ function, is slow mainly due to memory allocation. After a long fight between angel and demon on my shoulders I decided that I'm not a superman. That's why I won't try to speed up _the calloc_ function. 

But I can try to implement the trick. It's known that working with memory always needs much time due to many reasons, but we'll win time by allocating large pieces of memory! Let's look at our function _the BuildDictionary_:

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

So it doesn't look fast. Indeed, we allocate memory for every _Line*_ in the loop. Let's fix it and implement _the calloc_ for the large memory. The new code:

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

Let's keep our fingers crossed and take a look into _the callgrind_:

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>
<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>

It's hard to call it a success. The optimization is not satisfactory (~9%). I think that it's time to optimize the search for elements. One of the most important functions in the search process is _the LineCmp_. Let's take a look into the code:

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

This function doesn't look smart. It's not efficient to compare character by character. Let's implement refactoring of our function using intrinsics. The new code we get:

```cpp
int LineCmp (Line* firstLine, Line* secondLine) {

    int cmpResult         = 0;

    __m128i firstStr  = {0};
    __m128i secondStr = {0};

    if (firstLine->_length == secondLine->_length) {

        for (size_t startSymb = 0; startSymb < firstLine->_length; startSymb += 16) {
            
            firstStr  = _mm_loadu_si128 ((__m128i*)(firstLine->_str + startSymb));
            secondStr = _mm_loadu_si128 ((__m128i*)(secondLine->_str + startSymb));

            cmpResult = _mm_cmpestri    (firstStr, firstLine->_length - startSymb > 16 ? 16 : (firstLine->_length - startSymb), 
                                        secondStr, secondLine->_length - startSymb > 16 ? 16 : (secondLine->_length - startSymb), 
                                        _SIDD_CMP_EQUAL_EACH);

            if (cmpResult != 0)
                break;
            
        }

    } else
        return (firstLine->_length - secondLine->_length);

    return cmpResult;

}
```

The new value of the run-time is:

<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>
<p align = "center">
  <img src="https://github.com/Exactlywb/HashTable/blob/master/unoptimized/csv/CRC.png"/>
</p>

This is 29% more optimized compared to the previous test! _God bless Intel twice_.

### Inference

After all the steps taken, we reduced the program execution time from ~1.678s to ~0.200s (88% or in 8 times)! Ded's coefficient $Dede = \frac{8}{4} \cdot 1000\$ = 2000\$$. An interesting fact: this is the salary per second that awaits a person who writes a good readme.

## Conclusion

Congratulations, colleagues. We have implemented the hash table, optimized it and got 2000$. 

<p align = "center">
  <img src = "https://lh3.googleusercontent.com/proxy/sZm9obAtopRDU5w18M-EY1mGwEljPtm4b9AoOszAxiRGUk-RemRTNw9KvzgEGSuJ7cALcsKN-RJIVcMYAI-oDJn7gxyYCykdyfhQkv1CrNx7LTJunKIWl-x-Kj4suEGaMk6JSLXEt_G-qnTxJrpa9mIJI3Tqrrojelc" width = "400" />
</p>
