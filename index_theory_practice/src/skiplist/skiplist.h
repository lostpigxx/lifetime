/*
    C++ implement for Skip List data structure
    Author: Liang Xiao
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