#include "onegin.h"

size_t GetFileSize (FILE* file) {

    fseek (file, 0, SEEK_END);
    size_t res = ftell (file);
    fseek (file, 0, SEEK_SET);

    return res;

}

char* ReadBuffer (FILE* file, size_t fileSize) {

    char* buffer = (char*) calloc (fileSize, sizeof(char));
    assert (buffer);

    fread (buffer, sizeof (char), fileSize, file);
    assert (buffer);

    return buffer;

}

void LinePrint (Line* string) {

    for (size_t symb = 0; symb < string->_length; symb++) {

        printf ("%c", string->_str [symb]);

    }

}

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

void DictionaryDestruct (Dictionary* dict) {

    assert (dict);

    free (dict->_data [0]);

    free (dict->_data);
    free (dict->_buffer);
    dict->_size = 0;
    free (dict);

}

size_t GetCountLines (char* buffer) {

    size_t countLines = 0;
    size_t countSymb  = 0;

    while (*(buffer + countSymb) != '\0') {

        if (*(buffer + countSymb) == '\n' || *(buffer + countSymb) == '\0')
            countLines++;

        countSymb++;

    }

    return countLines;

}

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
