//
//  RecordManager.h
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#ifndef RecordMagener_h
#define RecordMagener_h

#include <iostream>
#include <vector>
#include "Attribute.h"
#include "FileBlock.h"
using namespace std;

#endif

class RecordManager
{
public:

public:
    void insert_record(char *rec, FileBlock *fb);
    void select_record(FileBlock *fb, string aname, int mode, void *value, vector<string> *vec);
    //void delete_record();
};
