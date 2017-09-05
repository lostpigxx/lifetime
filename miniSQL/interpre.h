
//  Interpre.h
//  Intepreter
//
//  Created by yy on 15/10/28.
//  Copyright (c) 2015年 Lynn. All rights reserved.
//

#ifndef __Intepreter__Interpre__
#define __Intepreter__Interpre__

#include <stdio.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include "help.h"
#include "Query_condition.h"
#include "API.h"
using namespace std;

//一下是SQL语句的操作宏定义
#define INSERT 0
#define DELETE 1
#define SELECT 2
#define CREATE_TABLE 3
#define DROP_TABLE   4
#define CREATE_INDEX 5
#define DROP_INDEX   6
//class Query_condition{
//public:
//    string aname;
//    void *value;
//    int mode;
//};

//语句结构
class sqlunit{
public:
    int sql;
    string pj;
    string tname;
    string iname;
    string aname;
    char *rec;
    Query_condition *qc;
    int num_condition;

    sqlunit(){
        sql = 0;
        pj = "";
        tname = "";
        iname = "";
        aname  = "";
        rec = NULL;
        qc = new Query_condition[32];
        num_condition = 0;
    }
};
//属性结构
//class Attribute{
//public:
//    string name;
//    unsigned short offset; //距离记录头的字节数
//    unsigned short length;  //该属性所占字节数
//    void *p;
//    char type;  //'D' 'I' 'C'
//    bool is_unique;
//    bool is_primary_key;
//    bool is_index;
//    Attribute(){
//        name = "";
//        offset = 0;
//        length = 0;
//        p = NULL;
//        type = 'I';
//        is_unique = 0;
//        is_primary_key = 0;
//        is_index = 0;
//    }
//};
////表结构
//class Table{
//public:
//    string tableName;      //表名
//    int attrNum;             //属性个数
//    int tupLength;           //记录长度
//    string primaryKey;
//    int blockNum;           //表中数据所占块数
//    Attribute *attrlise = new Attribute[32];
//    Table(){
//        tableName = "";
//        attrNum = 0;
//        tupLength = 0;
//        primaryKey = "";
//        blockNum = 0;
//    }
//};
//翻译器
class Interpreter{
public:
    API api;
public:
    string s;
    string getWord(int *tmp);
    void  parse();
    void insertParse(string& word,int& tmp);
    void deleteParse(string& word,int& tmp);
    void selectParse(string& word,int& tmp);
    void createTbParse(string& word,int& tmp);
    void createIndxPase(string& word,int& tmp);
    void dropTbParse(string& word,int& tmp);
    void dropIndxParse(string& word,int& tmp);
    void quit(string& word,int& tmp);
    void execfile(string& word,int& tmp);
    void recordParse(char *rec, Table tb);
    ~Interpreter(){}
};

//语句分割
#endif /* defined(__Intepreter__Interpre__) */
