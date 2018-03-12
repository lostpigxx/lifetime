/*
    C++ implement for Skip List data structure
    Author: Liang Xiao
    Characteristic: 
        - Use doubly linked list as level_0, support reverse scan.
        - Level_0 data are persisted in non-volatile storage, can reconstruct other level.
 */

#ifndef SKIPLIST_H
#define SKIPLIST_H

#include <iostream>

using namespace std;



struct skiplistHead {

};

struct skiplistNode {

};

class skiplist {
private:
    skiplistHead head;
    skiplistNode *nodes;

public:
    skiplist();
    ~skiplist();
    bool insertNode();
    bool deleteNode();
};



#endif /* Skiplist */