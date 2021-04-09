#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Onegin/onegin.h"

enum ListErr {

    NO_ERROR

};

enum AddSide {

    left,
    right

};

struct Node;

struct List {

    size_t      _size;

    Node*       _header;
    Node*       _tail;

    ListErr     _errno;
    const char* _dumpFile;

};

struct Node {

    Line*  _value;

    Node*  _next;
    Node*  _prev;

};

Node*   NodeInit            (Line* value, Node* next, Node* prev);
void    ListDestroy         (List* list);
List*   ListInit            (List* list);

Node*   PushHead            (List* list, Node* newHead);
Node*   PushTail            (List* list, Node* newTail);
Node*   ListInsert          (List* list, Node* newNode, Node* target, AddSide _side);
Node*   ListFindElem        (List* list, Line* toFind);
bool    DeleteElemFromList  (List* list, Line* toDel);

ListErr CheckList           (List* list);
void    ListDump            (List* list);
