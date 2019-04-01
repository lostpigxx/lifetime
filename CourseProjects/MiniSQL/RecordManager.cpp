//
//  RecordManager.cpp
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#include "RecordManager.h"


void RecordManager::insert_record(char *rec, FileBlock *fb)
{
    fb->insert_record_in_block(rec);
}


void RecordManager::select_record(FileBlock *fb, string aname, int mode, void *value, vector<string> *vec)
{
    fb->select_record_from_block(aname, mode, value, vec);
}
