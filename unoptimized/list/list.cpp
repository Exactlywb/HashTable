#include "list.h"

#define PRINT_ERR(message) printf ("%s in function %s, catched error on line %d\n", message, __FUNCTION__, __LINE__)
#define LIST_OK(listP, returnValue) if ( listP->_errno = CheckList (listP) ) {                      \
                                        ListDump (listP);                                           \
                                        return returnValue;                                         \
                                    }

Node* NodeInit (Line* value, Node* next, Node* prev) {

    Node* newNode = (Node*)calloc (1, sizeof (*newNode));
    if (!newNode) {

        PRINT_ERR ("Memory error : can't make calloc");
        return nullptr;
        
    }

    newNode->_value = value;
    newNode->_next  = next;
    newNode->_prev  = prev;

    return newNode;

}

List* ListInit (List* list) {

    list->_size     = 0;

    list->_header   = nullptr;
    list->_tail     = nullptr;

    list->_errno    = NO_ERROR;

    return list;

}

Node* PushHead (List* list, Node* newHead) {

    if (!list) {

        PRINT_ERR("Null pointer on List* list");
        return nullptr;

    }
    if (!newHead) {

        PRINT_ERR("Null pointer on Node* newHead");
        return nullptr;

    }

    LIST_OK (list, nullptr)

    if (list->_header) {

        list->_header->_prev    = newHead;
        newHead->_next          = list->_header;
        list->_header           = newHead;

    } else {

        list->_header = newHead;
        list->_tail   = newHead;

    }

    list->_size++;

    LIST_OK (list, nullptr)

    return newHead;

}

Node* PushTail (List* list, Node* newTail) {

    if (!list) {

        PRINT_ERR("Null pointer on List* list");
        return nullptr;

    }
    if (!newTail) {

        PRINT_ERR("Null pointer on Node* newTail");
        return nullptr;

    }

    LIST_OK (list, nullptr)

    if (list->_tail) {
        
        list->_tail->_next      = newTail;
        newTail->_prev          = list->_tail;
        list->_tail             = newTail;

    } else {

        list->_header = newTail;
        list->_tail   = newTail;

    }

    list->_size++;

    LIST_OK (list, nullptr)

    return newTail;

}

ListErr CheckList (List* list) {

    return NO_ERROR;

}

void ListDump (List* list) {
    
    printf   ("\t\t");
    printf ("List [%p]\n"    , list);
    printf   ("\t\t");
    printf ("List size %zu\n", list->_size);
    printf   ("\t\t");
    printf ("* Head of list [%p]\n", list->_header);
    printf   ("\t\t");
    printf ("* Tail of list [%p]\n", list->_tail);

    size_t nodeNum = 0;

    for (Node* nextNode = list->_header; nextNode != nullptr; nextNode = nextNode->_next) {
        
        printf   ("\t\t");
        printf ("** Node #%zu with value ", nodeNum);
        LinePrint (nextNode->_value);
        printf (" and pointer [%p]\n", nextNode->_value);
        nodeNum++;

    }
    printf   ("\t\t");
    printf ("After calculation we got size of list is %zu!\n", nodeNum);

}

#define prevElem(node) node->_prev 
#define nextElem(node) node->_next

Node* ListInsert (List* list, Node* newNode, Node* target, AddSide _side) {

    if (!list) {

        PRINT_ERR("Null pointer on List* list");
        return nullptr;

    }
    if (!newNode) {

        PRINT_ERR("Null pointer on Node* newNode");
        return nullptr;

    }
    if (!target) {

        PRINT_ERR("Null pointer on Node* target");
        return nullptr;

    }

    LIST_OK (list, nullptr)

    if (_side == left) {

        if (prevElem(target))
            prevElem(target)->_next = newNode;
        else    
            list->_header = newNode;

        newNode->_prev   = prevElem(target);
        newNode->_next   = target;
        prevElem(target) = newNode;

    } else {

        if (nextElem(target))
            nextElem(target)->_prev = newNode;
        else
            list->_tail = newNode;

        newNode->_next   = nextElem(target);
        newNode->_prev   = target;
        nextElem(target) = newNode; 

    }

    LIST_OK (list, nullptr)

    list->_size++;

    return newNode;

}

void ListDestroy (List* list) {

    Node* nextNode = list->_header;
    while (nextNode != nullptr) {

        Node* next = nextNode->_next;
        free (nextNode);
        nextNode = next;

    }

}

Node* ListFindElem (List* list, Line* toFind) {

    assert (list);
    assert (toFind);

    Node* curNode = list->_header;
    while (curNode != nullptr) {

        if (!LineCmp (curNode->_value, toFind))
            break;

        curNode = curNode->_next;

    }

    return curNode;
    
}

bool DeleteElemFromList (List* list, Line* toDel) {

    assert (list);
    assert (toDel);

    Node* toFind = ListFindElem (list, toDel);
    if (!toFind) {

        return false;

    } else {

        if (prevElem (toFind))
            nextElem (prevElem (toFind)) = nextElem (toFind);

        if (nextElem (toFind))
            prevElem (nextElem (toFind)) = prevElem (toFind);    
            
        free (toFind);
        list->_size--;

    }

}
