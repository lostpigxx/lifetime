//
//  FileBlock.cpp
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#include "FileBlock.h"
//#include "Attribute.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <stdlib.h>
using namespace std;

FileBlock::FileBlock()
{
    p = new char[1024 * 4];
    att = new Attribute[32];
    is_locked = false;
    is_change = false;
    offset_of_first_record = NO_FIRST_RECORD;
    recd = NULL;
}

FileBlock::FileBlock(string tname, unsigned short rlen, Attribute *sour, FileAdr a)
{
    p = new char[1024 * 4];
    att = new Attribute[32];
    is_locked = false;
    is_change = false;
    offset_of_first_record = NO_FIRST_RECORD;
    recd = NULL;
    this_block = a;
    next_block = NO_NEXT_BLOCK;
    set_block(tname, rlen, sour, a);
    for(int i=0; i<max_num_record; i++)
    {
        recd[i].pr = p + base_adr + i * (record_lenth + 2);
        recd[i].next_record = RECORD_IS_EMPTY;
    }
}

FileBlock::~FileBlock()
{
    write_block_to_disk();
    delete p;
    delete[] att;
    delete recd;
}


bool FileBlock::load_block_from_disk(FileAdr f)
{
    // load data into memory
    char fname[20];
    char fpath[100] = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/";
    FILE *fp;
    //ultoa(f, fname, 10);
    sprintf(fname, "%ld", f);
    strcat(fpath, fname);
    fp = fopen(fpath, "r+");
    if(fp == NULL)
    {
        return false;
    }
    //p = new char[1024 * 4];
    fread(p, 4096, 1, fp);
    fclose(fp);
    // get the head information
    // num_record = (p[0]<<8) | p[1];
    // offset_of_first_record = (p[2]<<8) | p[3];
    // memcpy(table_name, p+4, 20);
    // next_block = 0;
    this_block = f;
    // for(int i=0; i<8; i++)
    // {
    //     next_block |= (p[i+24] << (56-i*8));
    // }
    // record_lenth = (p[32]<<8) | p[33];
    // for(int i=0; i<num_record; i++)
    // {
    //     recd[i].pr = p + base_adr + i * (record_lenth + 2);
    //     recd[i].next_record = (recd[i].pr[record_lenth]<<8) | recd[i].pr[record_lenth+1];
    // }
    memcpy(&num_record, p, 2);
    memcpy(&offset_of_first_record, p+2, 2);
    memcpy(table_name, p+4, 20);
    memcpy(&next_block, p+24, 8);
    memcpy(&record_lenth, p+32, 2);
    max_num_record = (4096-base_adr) / (record_lenth+2);
    recd = new Record[max_num_record];
    for(int i=0; i<max_num_record; i++)
    {
        recd[i].pr = p + base_adr + i * (record_lenth + 2);
        memcpy(&recd[i].next_record, &recd[i].pr[record_lenth],2);
    }

    return true;
}

bool FileBlock::write_block_to_disk()
{
    char fname[20];
    //char fpath[32] = "/Data/";
    char fpath[100] = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/";
    FILE *fp;
    //ultoa(this_block, fname, 10);
    sprintf(fname, "%ld", this_block);
    strcat(fpath, fname);
    fp = fopen(fpath, "w+");
    if(fp == NULL)
    {
        return false;
    }
    memcpy(p, &num_record, 2);
    memcpy(p+2, &offset_of_first_record, 2);
    memcpy(p+4, table_name, 20);
    memcpy(p+24, &next_block, 8);
    memcpy(p+32, &record_lenth,2);
    for(int i=0; i<max_num_record; i++)
    {
        memcpy(&recd[i].pr[record_lenth], &recd[i].next_record, 2);
    }
    fwrite(p, 4096, 1, fp);
    fclose(fp);
    return true;
}

void FileBlock::set_block(string tname, unsigned short rlen, Attribute *sour,  FileAdr a)
{
    record_lenth = rlen;
    strcpy(table_name, tname.c_str());
    memcpy(att, sour, sizeof(Attribute) * 32);
    for(int i=0; i<32; i++)
    {
        if(att[i].is_primary_key)
            primarykey = i;
    }
    max_num_record = (4096-base_adr) / (rlen+2);
    recd = new Record[max_num_record];
}

void FileBlock::insert_record_in_block(char *rec)
{
    int posi = find_empty_postion();
    void *value = NULL;
    int i, j;
    if(posi == EMPTY_POSITION_NO_FOUND)
    {
        return;
    }
    else
    {
        if(offset_of_first_record == NO_FIRST_RECORD)
        {
            memcpy(recd[0].pr, rec, record_lenth);
            recd[0].next_record = NO_NEXT_RECORD;
            num_record++;
            offset_of_first_record = posi;
            return;
        }
        decomposition(&value, rec, att[primarykey].name);
        for(i=0, j=offset_of_first_record; i<max_num_record; i++)
        {
            if(recd[j].next_record == NO_NEXT_RECORD)
                break;
            if(compare_attribute_value(value, recd[j].pr, att[primarykey].name, MODE_QUEAL))
                return;
            if(compare_attribute_value(value, recd[j].pr, att[primarykey].name, MODE_GREAT))
                break;
            j=recd[j].next_record;
        }
        memcpy(recd[posi].pr, rec, record_lenth);
        recd[posi].next_record = recd[j].next_record;
        recd[j].next_record = posi;
    }
    num_record++;
//    if(value != NULL)
//        delete value;
}


void FileBlock::delete_record_from_block(char *rec)
{
    if(offset_of_first_record == NO_FIRST_RECORD)
        return;
    int t = offset_of_first_record;
    for(int i=0, j=offset_of_first_record; i<max_num_record; i++)
    {
        // if(recd[j].next_record == NO_NEXT_RECORD)
        //     break;
        if(compare_2_char_is_equal(rec, recd[j].pr))
        {
            if(j == offset_of_first_record)
            {
                if(recd[j].next_record == NO_NEXT_RECORD)
                {
                    offset_of_first_record = NO_FIRST_RECORD;
                }
                else
                {
                    offset_of_first_record = recd[j].next_record;
                }
                recd[j].next_record = RECORD_IS_EMPTY;
            }
            else
            {
                recd[t].next_record = recd[j].next_record;
                recd[j].next_record = RECORD_IS_EMPTY;
            }
            num_record--;
            return;
        }
        t = j;
        j = recd[j].next_record;
    }
}

void FileBlock::select_record_from_block(string aname, int mode, void *value, vector<string> *vec)
{
    if(offset_of_first_record == NO_FIRST_RECORD)
        return;
    for(int i=0, j=offset_of_first_record; i<max_num_record; i++)
    {
        if(compare_attribute_value(value, recd[j].pr, aname, mode))
        {
            string t(recd[j].pr, record_lenth);
            vec->push_back(t);
        }
        if(recd[j].next_record == NO_NEXT_RECORD)
            break;
        j = recd[j].next_record;
    }
}

int FileBlock::find_empty_postion()
{
    for(int i=0; i<max_num_record; i++)
    {
        if(recd[i].next_record == RECORD_IS_EMPTY)
        {
            return i;
        }
    }
    return EMPTY_POSITION_NO_FOUND;
}

bool FileBlock::compare_attribute_value(void *s, char *r, string aname, int mode)
{
    void *buf = NULL;
    char type = decomposition(&buf, r, aname);
    if(mode == ALL_WE_NEED)
        return true;
    if(type == 'D')
    {
        switch (mode) // buf < s
        {
            case MODE_QUEAL:
                return (*(double *)s == *(double *)buf);
            case MODE_NOT_QUEAL:
                return (*(double *)s != *(double *)buf);
            case MODE_GREAT:
                return (*(double *)s < *(double *)buf);
            case MODE_GREAT_OR_EQUAL:
                return (*(double *)s <= *(double *)buf);
            case MODE_LESS:
                return (*(double *)s > *(double *)buf);
            case MODE_LESS_OR_QUEAL:
                return (*(double *)s >= *(double *)buf);
        }
    }
    else if(type == 'I')
    {
        switch (mode)
        {
            case MODE_QUEAL:
                return (*(int *)s == *(int *)buf);
            case MODE_NOT_QUEAL:
                return (*(int *)s != *(int *)buf);
            case MODE_GREAT:
                return (*(int *)s < *(int *)buf);
            case MODE_GREAT_OR_EQUAL:
                return (*(int *)s <= *(int *)buf);
            case MODE_LESS:
                return (*(int *)s > *(int *)buf);
            case MODE_LESS_OR_QUEAL:
                return (*(int *)s >= *(int *)buf);
        }
    }
    else
    {
        string a((char *)s);
        string b((char *)buf);
        switch (mode)
        {
            case MODE_QUEAL:
                return (a == b);
            case MODE_NOT_QUEAL:
                return (a != b);
            case MODE_GREAT:
                return (a < b);
            case MODE_GREAT_OR_EQUAL:
                return (a <= b);
            case MODE_LESS:
                return (a > b);
            case MODE_LESS_OR_QUEAL:
                return (a >= b);
        }
    }
    return false;
}

bool FileBlock::check_block_is_full()
{
    for(int i=0; i<max_num_record; i++)
    {
        if(recd[i].next_record == RECORD_IS_EMPTY)
            return false;
    }
    return true;
}

char FileBlock::decomposition(void **buf, char *r, string aname)
{
    for(int i=0; i<32; i++)
    {
        string temp(att[i].name);
        if(temp == aname)
        {
            switch(att[i].type)
            {
                case 'D':
                    *buf = new double(0);
                    memcpy(*buf, r+att[i].offset, att[i].lenth);
                    break;
                case 'I':
                    *buf = new int(0);
                    memcpy(*buf, r+att[i].offset, att[i].lenth);
                    break;
                case 'C':
                    *buf = new char[att[i].lenth];
                    memcpy(*buf, r+att[i].offset, att[i].lenth);
                    break;
                default: break;
            }
            return att[i].type;
        }
    }
    return NULL;
}

bool FileBlock::compare_2_char_is_equal(char *a, char *b)
{
    for(int i=0; i<record_lenth; i++)
    {
        if(a[i] != b[i])
            return false;
    }
    return true;
}











//
