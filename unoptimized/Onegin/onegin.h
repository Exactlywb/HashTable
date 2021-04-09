#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

struct Line {

    char*  _str;
    size_t _length;
    
};

struct Dictionary {

    Line** _data;
    size_t _size;

    char*  _buffer;

};

int         LineCmp             (Line* firstLine, Line* secondLine);
char*       ReadBuffer          (FILE* file, size_t fileSize);
size_t      GetFileSize         (FILE* file);
Dictionary* BuildDictionary     (const char* file);
void        DictionaryDestruct  (Dictionary* dict);
size_t      GetCountLines       (char* buffer);
void        LinePrint           (Line* string);
