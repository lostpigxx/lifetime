//
//  CatalogManager.cpp
//  Minisql
//
//  Created by Jackie on 15/10/30.
//  Copyright (c) 2015年 Jackie. All rights reserved.
//

#include "CatalogManager.h"
#include <iostream>
#include <string>
#include <cstring>
#include <ctime>
using namespace std;
#define MAXDEGREE 100
#define MINDEGREE 50

CatalogManager::CatalogManager(){

}

 CatalogManager::~CatalogManager(){
    //save_log_to_disk();
}


void CatalogManager::create_table(Table &table){
    pt = new Table;
    pt->tableName = table.tableName;
    pt->attrNum = table.attrNum;
    pt->tupLength = table.tupLength;
    pt->primaryKey = table.primaryKey;
    memcpy(pt->attrlist, table.attrlist, sizeof(Attribute)*32);
    //firstblock
    pt->blockNum = 0;
    pt->blockList = NULL;
    save_log_to_disk();
}

FileAdr CatalogManager::get_first_fileadr(string tableName){
    check_table(tableName);
    return pt->firstblock;
}

void CatalogManager::store_fileadr(string tableName, FileAdr a, bool is_first){
    check_table(tableName);

    if(is_first)
    {
        if(pt->blockNum == 0)
        {
            pt->firstblock = a;
            save_log_to_disk();
            return;
        }
        FileAdr *temp = new FileAdr[pt->blockNum+1];
        memcpy(temp, pt->blockList, sizeof(FileAdr)*(pt->blockNum));
        temp[pt->blockNum] = pt->firstblock;
        pt->blockNum++;
        delete pt->blockList;
        pt->blockList = temp;
        pt->firstblock = a;
    }
    else
    {
        FileAdr *temp = new FileAdr[pt->blockNum+1];
        memcpy(temp, pt->blockList, sizeof(FileAdr)*(pt->blockNum));
        temp[pt->blockNum] = a;
        pt->blockNum++;
        delete pt->blockList;
        pt->blockList = temp;
    }
    save_log_to_disk();
}

void CatalogManager::delete_fileadr(string tableName, FileAdr a, FileAdr n){
    check_table(tableName);
    if(pt->firstblock == a)
    {
        pt->firstblock = n;
    }
    else
    {
        int i;
        for(i=0; i<pt->blockNum; i++)
        {
            if(pt->blockList[i] == a)
                break;
        }
        for(; i<pt->blockNum-1; i++)
        {
            pt->blockList[i] = pt->blockList[i+1];
        }
        pt->blockNum--;
    }
    save_log_to_disk();
}

unsigned short CatalogManager::get_record_length(string tableName){
    check_table(tableName);
    return pt->tupLength;
}

Attribute *CatalogManager::get_attribute(string tableName){
    check_table(tableName);
    Attribute* temp = new Attribute[32];
    memcpy(temp, pt->attrlist, 32*sizeof(Attribute));
    return temp;
}

int CatalogManager::get_num_attribute(string tableName){
    check_table(tableName);
    return pt->attrNum;
}

FileAdr CatalogManager::get_new_fileadr(string tableName){
    check_table(tableName);
    FileAdr i;
    if(pt->blockNum == 0)
    {
        for(i=1; ;i++)
        {
            char fname[20];
            char fpath[100] = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/";
            FILE *fp;
            sprintf(fname, "%ld", i);
            strcat(fpath, fname);
            fp = fopen(fpath, "r");
            if(fp == NULL)
                return i;
            fclose(fp);
        }
    }

    for(i=pt->blockList[pt->blockNum-1]; ;i++)
    {
        char fname[20];
        char fpath[100] = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/";
        FILE *fp;
        sprintf(fname, "%ld", i);
        strcat(fpath, fname);
        fp = fopen(fpath, "r");
        if(fp == NULL)
            return i;
        fclose(fp);
    }
}

void CatalogManager::check_table(string tableName){  //根据给的表名检查pt指向是不是对的表
    if(pt == NULL)
    {
        load_log_from_disk(tableName);
    }
    else if(pt->tableName == tableName)
        return;
    else
    {
        save_log_to_disk();
        load_log_from_disk(tableName);
    }
}

void CatalogManager::load_log_from_disk(string tableName){    //从磁盘中取出tableName对应的catalog
    FILE *fp;
    string path = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/" + tableName + ".log";
    fp = fopen(path.c_str(), "r+");
    if(fp == NULL)
    {
        return;
    }
    if(pt == NULL)
    {
        pt = new Table;
    }
    fread(pt, sizeof(Table), 1, fp);
    pt->blockList = new FileAdr[pt->blockNum];
    fread(pt->blockList, sizeof(FileAdr)*pt->blockNum, 1, fp);
    fclose(fp);
}

void CatalogManager::save_log_to_disk(){       //将存有表信息的catalog存入磁盘
    FILE *fp;
    string path = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/" + pt->tableName + ".log";
    fp = fopen(path.c_str(), "w+");
    if(fp == NULL)
    {
        return;
    }
    fwrite(pt, sizeof(Table), 1, fp);
    fwrite(pt->blockList, sizeof(FileAdr)*pt->blockNum, 1, fp);
    if(pt->blockNum != 0)
        delete pt->blockList;
    fclose(fp);
}

bool CatalogManager::check_table_exist(string tableName){       //检查文件中有没有catalog存在
    FILE *fp;
    string path = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/" + tableName + ".log";
    fp = fopen(path.c_str(), "r+");
    if(fp == NULL)
    {
        return false;
    }
    return true;
}

unsigned int CatalogManager::calculate_degree(string tableName){
    check_table(tableName);
    unsigned int degree;
    int Size_Table = pt->tupLength;
    if(Size_Table != 0)
        degree = 4 * 1024 / Size_Table;
    else
        degree = 0;

    if(degree > MAXDEGREE)
        degree = MAXDEGREE;
    else if(degree < MINDEGREE)
        degree = MINDEGREE;
    return degree;
}

void CatalogManager::create_index(string tableName, string indexname, string attrname){
    check_table(tableName);
    //pt->index = indexname;
    for (int i = 0; i < pt->attrNum; i++)
    {
        if (pt->attrlist[i].name == attrname && (pt->attrlist[i].is_unique || pt->attrlist[i].is_primary_key))
        {
            pt->attrlist[i].is_index = true;
        }
        // else
        //     printf("The attribute does not exist.\n");
    }
    save_log_to_disk();
}

bool CatalogManager::is_attrintable(string tableName, string attrName){
    check_table(tableName);
    for (int i = 0; i < pt->attrNum; i++) {
        if (pt->attrlist[i].name == attrName) {
            //cout << "The attribute" << attrName << "is in this table" << endl;
            return true;
        }
    }
    //cout << "The attribute" << attrName << "is not in this table" << endl;
    return false;
}

bool CatalogManager::is_index(string tableName, string attrName, string indexname){
    check_table(tableName);
    for (int i = 0; i < pt->attrNum; i++) {
        if (pt->attrlist[i].name == attrName && pt->attrlist[i].is_index == true) {
            //cout << "The attribute" << attrName << "is the index of this table" << endl;
            return true;
        }
    }
    //cout << "There is no index in this table" << endl;
    return false;

}

void CatalogManager::drop_table(string tableName){
    check_table(tableName);
    char fname[20];
    char fpath[100] = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/";
    sprintf(fname, "%ld", pt->firstblock);
    strcat(fpath, fname);
    remove(fpath);
    for(int i=0; i<pt->blockNum; i++)
    {
        char fname[20];
        char fpath[100] = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/";
        sprintf(fname, "%ld", pt->blockList[i]);
        strcat(fpath, fname);
        remove(fpath);
    }
    string path = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/" + pt->tableName + ".log";
    remove(path.c_str());
    string path1 = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/" + pt->tableName + ".idx";
    remove(path1.c_str());
}

void CatalogManager::drop_index(string tableName, string attrname, string indexName){
    check_table(tableName);
    for (int i = 0; i < pt->attrNum; i++)
    {
        if (pt->attrlist[i].is_index == true && pt->attrlist[i].name == attrname)
        {
            pt->attrlist[i].is_index = false;
            string path = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/Data/" + pt->tableName + indexName+ ".idx";
            remove(path.c_str());
        }
    }
    save_log_to_disk();
}


Table *CatalogManager::tableInfo(string tname)
{
    check_table(tname);
    return pt;
}

char CatalogManager::attrType(string tname, string aname)
{
    check_table(tname);
    for(int i=0; i<32; i++)
    {
        if(pt->attrlist[i].name == aname)
            return pt->attrlist[i].type;
    }
    return 'N';
}




















//
