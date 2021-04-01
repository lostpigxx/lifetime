//
//  Table.h
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#ifndef Table_h
#define Table_h
#include "Attribute.h"
#include <string>
using namespace std;

typedef unsigned long FileAdr;

class Table
{
public:
    string tableName;
    int attrNum; // the number of attributes
    unsigned short tupLength; // the length of a single record
    string primaryKey;
    Attribute attrlist[32];
    FileAdr firstblock; // the address of the first block in this table
    int blockNum; // the total number of the block
    FileAdr *blockList; // store all the address of block in this table
    Table(){
           tableName = "";
           attrNum = 0;
           tupLength = 0;
           primaryKey = "";
           blockNum = 0;
       }
};


#endif
