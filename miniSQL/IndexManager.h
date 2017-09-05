//
//  IndexManager.h
//  MiniSQL
//
//  Created by 梁潇 on 15/10/17.
//
//

#ifndef IndexManager_h
#define IndexManager_h

#include <iostream>
#include "B_Plus_Tree.h"

#endif


class IndexManager
{
public:
    B_Plus_Tree<int> *bTreeInt;
    B_Plus_Tree<double> *bTreeDouble;
    B_Plus_Tree<string> *bTreeChar;
public:
    void insert_into_index(string tname, void *key, FileAdr f, char type, string iname);
    bool check_index_exist(string tname, void *key, char type, string iname);
    void create_new_index(string tname, string iname, string aname, char type, int degree);
    void delete_index_value(string tname, void *key, char type, string iname);
public:
    IndexManager();
    ~IndexManager();
};
