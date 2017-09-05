//
//  CatalogManager.h
//  Minisql
//
//  Created by Jackie on 15/10/27.
//  Copyright (c) 2015年 Jackie. All rights reserved.
//

#ifndef Minisql_CatalogManager_h
#define Minisql_CatalogManager_h
#include<string>
#include <fstream>
#include <iostream>
#include "Table.h"
#include "Attribute.h"


using namespace std;
typedef unsigned long FileAdr;
class CatalogManager
{
    public:
        CatalogManager();
        virtual ~CatalogManager();
    public:
        Table *pt;
    public:
        void create_table(Table &table);        //创建表
        FileAdr get_first_fileadr(string tableName);        //返回表记录的第一块的地址
        void store_fileadr(string tableName, FileAdr a, bool is_first);     //将表的记录地址存入文件
        void delete_fileadr(string tableName, FileAdr a, FileAdr n);
        unsigned short get_record_length(string tableName);     //获得记录长度
        Attribute *get_attribute(string tableName);     //返回第n个属性的类
        int get_num_attribute(string tableName);        //返回属性个数
        FileAdr get_new_fileadr(string tableName);      //返回一个还未被使用的文件块地址
        void check_table(string tableName);
        Table *tableInfo(string tname);

        char attrType(string tname, string aname);    //返回属性的类型

        void load_log_from_disk(string tableName);
        void save_log_to_disk();

        bool check_table_exist(string tableName);

        unsigned int calculate_degree(string tableName);

        void create_index(string  tablename, string indexname, string attrname);      //创建索引

        bool is_attrintable(string tableName, string attrName);       //判断该属性名是不是该表中的属性
        bool is_index(string tableName,string attrName,string indexname);     //判断索引是否存在
        void drop_table(string tableName);       //删除表
        void drop_index(string tableName, string attrname, string indexName);       //删除索引
};


#endif
