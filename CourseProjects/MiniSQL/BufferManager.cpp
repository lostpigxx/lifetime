//
//  BufferManager.cpp
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#include <iostream>
#include "BufferManager.h"
//#include "FileBlock.h"



BufferManager::BufferManager()
{
    FB = new FileBlock[MAX_NUM_OF_BLOCK];
    counter = 0;
}

BufferManager::~BufferManager()
{
    delete[] FB;
}

FileBlock *BufferManager::prepare_for_fileblock_for_insert(bool *flag, FileAdr fa, FileAdr nfa)
{
    int i;
    *flag = false;
    for(i=0; i<MAX_NUM_OF_BLOCK; i++)
    {
        if(FB[i].this_block == fa) //already in buffer
        {
            if(!FB[i].check_block_is_full()) //not full
                return FB + i;
            else // full
            {
                if(FB[i].next_block == NO_NEXT_BLOCK) // create new file block
                {
                    FileBlock *tt = new FileBlock(FB[i].table_name, FB[i].record_lenth, FB[i].att, nfa);
                    FB[i].next_block = nfa;
                    tt->write_block_to_disk();
                    delete tt;
                    FileBlock *t = Replace_one_block(nfa);
                    *flag = true;
                    return prepare_for_fileblock_for_insert(flag, t->this_block, nfa);
                }
                else
                {
                    FileBlock *t = Replace_one_block(FB[i].next_block);
                    return prepare_for_fileblock_for_insert(flag, t->this_block, nfa);
                }
            }
        }
    }
    FileBlock *t1 = Replace_one_block(fa);
    return prepare_for_fileblock_for_insert(flag, t1->this_block, nfa);
}

FileBlock *BufferManager::prepare_for_fileblock(FileAdr fa)
{
    int i;
    for(i=0; i<MAX_NUM_OF_BLOCK; i++)
    {
        if(FB[i].this_block == fa)
            return FB + i;
    }
    FileBlock *t1 = Replace_one_block(fa);
    return prepare_for_fileblock(t1->this_block);
}

FileBlock *BufferManager::Replace_one_block(FileAdr rb)
{
    /*替换算法在此*/
    if(FB[counter%MAX_NUM_OF_BLOCK].is_locked)
    {
        counter++;
        return Replace_one_block(rb);

    }
    FB[counter%MAX_NUM_OF_BLOCK].write_block_to_disk();
    FB[counter%MAX_NUM_OF_BLOCK].load_block_from_disk(rb);
    return (FB + (counter++)%MAX_NUM_OF_BLOCK);
}
