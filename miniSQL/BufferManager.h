//
//  BufferManager.h
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#ifndef BufferManager_h
#define BufferManager_h

#include <iostream>
#include "FileBlock.h"


#define NO_NEXT_BLOCK 0xffffffffffffffff
#define MAX_NUM_OF_BLOCK 16

typedef unsigned long FileAdr;

class BufferManager
{
public:
    FileBlock *FB;
    int counter;
public:
    FileBlock *prepare_for_fileblock_for_insert(bool *flag, FileAdr fa, FileAdr nfa);
    FileBlock *prepare_for_fileblock(FileAdr fa);
private:
    FileBlock *Replace_one_block(FileAdr rb);
public:
    BufferManager();
    ~BufferManager();
};



#endif/* BufferManager_H */