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

    if (firstLine->_length == secondLine->_length) {

        for (size_t symb = 0; symb < firstLine->_length; symb++) {

            if (firstLine->_str [symb] != secondLine->_str[symb])
                return (firstLine->_str [symb] - secondLine->_str[symb]);

        }

        return 0;

    } else
        return (firstLine->_length - secondLine->_length);

}
