//
//  IndexManager.cpp
//  MiniSQL
//
//  Created by 梁潇 on 15/10/17.
//
//
#include "IndexManager.h"

IndexManager::IndexManager()
{
    bTreeInt = new B_Plus_Tree<int>;
    bTreeDouble = new B_Plus_Tree<double>;
    bTreeChar = new B_Plus_Tree<string>;
}

IndexManager::~IndexManager()
{
    if(bTreeInt->tname != " ")
        bTreeInt->write_to_disk();
    if(bTreeDouble->tname != " ")
        bTreeDouble->write_to_disk();
    if(bTreeChar->tname != " ")
        bTreeChar->write_to_disk();
}

void IndexManager::insert_into_index(string tname, void *key, FileAdr f, char type, string iname)
{
    switch(type)
    {
        case 'D' :
            if(bTreeDouble->tname != tname || bTreeDouble->iname != iname)
            {
                bTreeDouble->write_to_disk();
                bTreeDouble->tname = tname;
                bTreeDouble->iname = iname;
                bTreeDouble->read_from_disk();
            }
            bTreeDouble->insert_key(*(double*)key, f);
            break;
        case 'I' :
            if(bTreeInt->tname != tname || bTreeInt->iname != iname)
            {
                bTreeInt->write_to_disk();
                bTreeInt->tname = tname;
                bTreeInt->iname = iname;
                bTreeInt->read_from_disk();
            }
            bTreeInt->insert_key(*(int*)key, f);
            break;
        case 'C' :
            if(bTreeChar->tname != tname || bTreeChar->iname != iname)
            {
                bTreeChar->write_to_disk();
                bTreeChar->tname = tname;
                bTreeChar->iname = iname;
                bTreeChar->read_from_disk();
            }
            string a((char *)key);
            bTreeChar->insert_key(a, f);
            break;
    }
}

bool IndexManager::check_index_exist(string tname, void *key, char type, string iname)
{
    index_offset t = {0, 0};
    switch(type)
    {
        case 'D' :
            if(bTreeDouble->tname != tname || bTreeDouble->iname != iname)
            {
                bTreeDouble->write_to_disk();
                bTreeDouble->tname = tname;
                bTreeDouble->iname = iname;
                bTreeDouble->read_from_disk();
            }
            t = bTreeDouble->search_key(*(double *)key);
            break;
        case 'I' :
            if(bTreeInt->tname != tname || bTreeInt->iname != iname)
            {
                bTreeInt->write_to_disk();
                bTreeInt->tname = tname;
                bTreeInt->iname = iname;
                bTreeInt->read_from_disk();
            }
            t = bTreeInt->search_key(*(int*)key);
            break;
        case 'C' :
            if(bTreeChar->tname != tname || bTreeChar->iname != iname)
            {
                bTreeChar->write_to_disk();
                bTreeChar->tname = tname;
                bTreeChar->iname = iname;
                bTreeChar->read_from_disk();
            }
            string a((char *)key);
            t = bTreeChar->search_key(a);
            break;
 //       default : break;
    }
    if(t.outer_offset != 0)
        return true;
    return false;
}


void IndexManager::create_new_index(string tname, string iname, string aname, char type, int degree)
{
    switch(type)
    {
        case 'D' :
            if(bTreeDouble->tname != tname || bTreeDouble->iname != iname)
            {
                bTreeDouble->write_to_disk();
                bTreeDouble->tname = tname;
                bTreeDouble->iname = iname;
            }
            bTreeDouble = new B_Plus_Tree<double>(degree, tname, iname, aname);
            bTreeDouble->write_to_disk();
            break;
        case 'I' :
            if(bTreeInt->tname != tname || bTreeInt->iname != iname)
            {
                bTreeInt->write_to_disk();
                bTreeInt->tname = tname;
                bTreeInt->iname = iname;
            }
            bTreeInt = new B_Plus_Tree<int>(degree, tname, iname, aname);
            bTreeInt->write_to_disk();
            break;
        case 'C' :
            if(bTreeChar->tname != tname || bTreeChar->iname != iname)
            {
                bTreeChar->write_to_disk();
                bTreeChar->tname = tname;
                bTreeChar->iname = iname;
            }
            bTreeChar = new B_Plus_Tree<string>(degree, tname, iname, aname);
            bTreeChar->write_to_disk();
            break;
        default : break;
    }
}

void IndexManager::delete_index_value(string tname, void *key, char type, string iname)
{
    switch(type)
    {
        case 'D' :
            if(bTreeDouble->tname != tname || bTreeDouble->iname != iname)
            {
                bTreeDouble->write_to_disk();
                bTreeDouble->tname = tname;
                bTreeDouble->iname = iname;
                bTreeDouble->read_from_disk();
            }
            bTreeDouble->delete_key(*(double*)key);
            break;
        case 'I' :
            if(bTreeInt->tname != tname || bTreeInt->iname != iname)
            {
                bTreeInt->write_to_disk();
                bTreeInt->tname = tname;
                bTreeInt->iname = iname;
                bTreeInt->read_from_disk();
            }
            bTreeInt->delete_key(*(int*)key);
            break;
        case 'C' :
            if(bTreeChar->tname != tname || bTreeChar->iname != iname)
            {
                bTreeChar->write_to_disk();
                bTreeChar->tname = tname;
                bTreeChar->iname = iname;
                bTreeChar->read_from_disk();
            }
            string a((char *)key);
            bTreeChar->delete_key(a);
            break;
    }
}

















//
