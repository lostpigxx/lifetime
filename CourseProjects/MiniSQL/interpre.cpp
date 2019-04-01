//
//  Interpre.cpp
//  Intepreter
//
//  Created by yy on 15/10/28.
//  Copyright (c) 2015年 Lynn. All rights reserved.
//

#include "interpre.h"
//API::
//execute_sql(int sql, string pj, string tname, char *rec, Query_condition *qc, int num_condition)
//

//获得下一个单词
string Interpreter::getWord(int *tmp){
    string word;
    int idx1,idx2;

    while ((s[*tmp] == ' ' || s[*tmp] == 10  || s[*tmp] == '\t') && s[*tmp] != 0)
    {
        (*tmp)++;
    }
    idx1 = *tmp;

    if (s[*tmp] == '(' || s[*tmp] == ',' || s[*tmp] == ')'|| s[*tmp] == ';'||s[*tmp] == '>'||s[*tmp] == '<'||s[*tmp] == '=')
    {
        (*tmp)++;
        idx2 = *tmp;
        word = s.substr(idx1,idx2-idx1);
        return word;
    }
    else if (s[*tmp] == 39)
    {
        (*tmp)++;
        while (s[*tmp] != 39 && s[*tmp] !=0)
            (*tmp)++;
        if (s[*tmp] == 39)
        {
            idx1++;
            idx2 = *tmp;
            (*tmp)++;
            word = s.substr(idx1,idx2-idx1);
            return word;
        }
        else
        {
            word = "";
            return word;
        }
    }
    else
    {
        while (s[*tmp] != ' ' &&s[*tmp] != '(' && s[*tmp] != 10 && s[*tmp] != 0 && s[*tmp] != ')' && s[*tmp] != ',' && s[*tmp] != ';'&& s[*tmp] != '>'&& s[*tmp] != '<'&& s[*tmp] != '=')
            (*tmp)++;
        idx2 = *tmp;
        if (idx1 != idx2)
            word = s.substr(idx1,idx2-idx1);
        else
            word = "";

        return word;
    }
}
void Interpreter::insertParse(string& word,int& tmp){
    sqlunit req;
    Table *tb;
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), "into")){
        cout << "Please input right SQl!" << endl;
        return;
    }
    word = getWord(&tmp);//获取表名
    if(!api.CM->check_table_exist(word)){
        cout << "Sorry, this table doesn't exist!" <<endl;
        return;
    }
    req.sql = INSERT;
    req.tname = word;
    //cout << "insertParse " << req.tname <<endl;
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), "values")){
        cout << "Please input right SQl!" << endl;
        return;
    }
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), "(")){
        cout << "Please input right SQl!" << endl;
        return;
    }
    tb = api.CM->tableInfo(req.tname);//获取表格信息
    req.rec = new char[api.CM->get_record_length(req.tname)];
    int i;
    word = getWord(&tmp);//第一个属性值
    for(i=0; i<tb->attrNum; i++){
        if(0 == strcmp(word.c_str(), ",") || 0 == strcmp(word.c_str(), ")") || word.empty()){
            cout << "Please input right SQL!" << endl;
            return;
        }
        switch(tb->attrlist[i].type){
            case 'D':
                if(isDouble(word)){
                    double ii;
                    sscanf(word.c_str(), "%lf", &ii);
                    memcpy(req.rec+tb->attrlist[i].offset, &ii,8);
                }
                else{
                    cout << "The type of attribute is wrong!" << endl;
                    return;
                }
                break;
            case 'I':
                if(isInt(word)){
                    int ii;
                    sscanf(word.c_str(), "%d", &ii);
                    memcpy(req.rec+tb->attrlist[i].offset, &ii,4);
                }
                else{
                    cout << "The type of attribute is wrong!" << endl;
                    return;
                }
                break;
            case 'C':
                if(isChar(word)){
                    memcpy(req.rec+tb->attrlist[i].offset, word.c_str(), tb->attrlist[i].lenth);//word.length());
                }
                else{
                    cout << "The type of attribute is wrong!" << endl;
                    return;
                }
                break;
            default:
                cout << "The typy of attribute is wrong!" <<endl;
                return;
        }
        word = getWord(&tmp);
        if((0 == strcmp(word.c_str(), ",")) && (i < tb->attrNum-1)){
            word = getWord(&tmp);
        }
        else if((0 == strcmp(word.c_str(), ")")) && (i == tb->attrNum-1)){
            word = getWord(&tmp);//获得封号
        }
        else{
            cout << "Please input right SQL!" << endl;
            return;
        }
    }
    if(i != tb->attrNum){
        cout << "Please input right SQL!" <<endl;
        return;
    }
    if(0 != strcmp(word.c_str(), ";")){
        cout << "Please input right SQl!" << endl;
        return;
    }
    api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
    cout << "insert success!" << endl;
    //api.CM->insert(req.tname,req.rec);
    return;
};
void Interpreter::parse(){
    int tmp = INSERT;
    string word = getWord(&tmp);
    if(0 == strcmp(word.c_str(),"create")){
        word = getWord(&tmp);
        if(0 == strcmp(word.c_str(),"table")){//CREATE_TABLE
            createTbParse(word,tmp);
        }
        else if(0 == strcmp(word.c_str(), "index")){//CREATE_INDEX
            createIndxPase(word, tmp);
        }
        else {
            cout << "Please input right SQL!" << endl;
            return;
        }
    }
    else if(0 == strcmp(word.c_str(),"insert")){//INSERT
        insertParse(word, tmp);
    }
    else if(0 == strcmp(word.c_str(),"select")){//SELECT
        selectParse(word, tmp);
    }
    else if(0 == strcmp(word.c_str(),"drop")){
        word = getWord(&tmp);
        if(0 == strcmp(word.c_str(), "table")){//DROP_TABLE
            dropTbParse(word, tmp);
        }
        else if(0 == strcmp(word.c_str(), "index")){//DROP_INDEX
            dropIndxParse(word, tmp);
        }
        else {
            cout << "Please input right SQL!" <<endl;
            return;
        }
    }
    else if(0 == strcmp(word.c_str(),"quit")){//QUIT
        quit(word, tmp);
    }
    else if(0 == strcmp(word.c_str(), "delete")){//DELETE
        deleteParse(word, tmp);
    }
    else if(0 == strcmp(word.c_str(),"execfile")){//EXECFILE
        execfile(word, tmp);
    }
    else if(word.empty()){
        cout << "Please input your SQL!" <<endl;
        return;
    }
    else{
        cout << "Please input right SQl!" << endl;
        return;
    }
    return;
};


void Interpreter::deleteParse(string& word,int& tmp){
    sqlunit req;
    Table tb;
    word =getWord(&tmp);
    if(0 != strcmp(word.c_str(), "from")){
        cout << "Please input right SQl!" << endl;
        return;
    }
    word = getWord(&tmp);
    if(!api.CM->check_table_exist(word)){
        cout << "The table doesn't exist!" << endl;
        return;
    }
    req.sql = DELETE;
    req.tname = word;
    word = getWord(&tmp);
    if(0 == strcmp(word.c_str(), ";")){
        api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
        cout << "execute success!" <<endl;
    }
    else if(0 == strcmp(word.c_str(), "where")){
        word = getWord(&tmp);
        int i=0;
        while(!word.empty() && (0 != strcmp(word.c_str(), ";"))){
            if(!api.CM->is_attrintable(req.tname, word)){
                cout << "The attribute doesn't exist in the table !" <<endl;
                return;
            }
            req.qc[i].aname = word;
            req.num_condition++;
            word = getWord(&tmp);
            if(0 == strcmp(word.c_str(), "=")){
                req.qc[i].mode = 0;
            }
            else if(0 == strcmp(word.c_str(), "!=")){
                req.qc[i].mode = 1;
            }
            else if(0 == strcmp(word.c_str(), ">")){
                req.qc[i].mode = 2;
            }
            else if(0 == strcmp(word.c_str(), ">=")){
                req.qc[i].mode = 3;
            }
            else if(0 == strcmp(word.c_str(), "<")){
                req.qc[i].mode = 4;
            }
            else if(0 == strcmp(word.c_str(), "<=")){
                req.qc[i].mode = 5;
            }
            else {
                cout << "Please input right SQL!" <<endl;
                return;
            }
            word = getWord(&tmp);
            char type = api.CM->attrType(req.tname, req.qc[i].aname);
            switch(type){
                case 'D':
                    if(isDouble(word)){
                        double *ii = new double;
                        sscanf(word.c_str(), "%lf", ii);
                        req.qc[i].value = ii;
                        //req.qc[i].value = new double(atof(word.c_str()));
                        //req.qc[i].value = atof(word.c_str());
                    }
                    else{
                        cout << "The type of attribute is wrong!" << endl;
                        return;
                    }
                    break;
                case 'I':
                    if(isInt(word)){
                        int *ii = new int;
                        sscanf(word.c_str(), "%d", ii);
                        req.qc[i].value = ii;
                        //req.qc[i].value = new int(atoi(word.c_str()));
                        //*(int *)req.qc[i].value = atoi(word.c_str());
                    }
                    else{
                        cout << "The type of attribute is wrong!" << endl;
                        return;
                    }
                    break;
                case 'C':
                    if(isChar(word)){
                        req.qc[i].value = new char[word.length()+1];
                        memcpy(req.qc[i].value, word.c_str(),word.length()+1);
                        //req.qc[i].value = new string(word);
                    }
                    break;
                default:
                    cout << "The typy of attribute is wrong!" <<endl;
                    return;
            }
            i++;
            word = getWord(&tmp);
        }
        req.num_condition = i;
        if(0 == strcmp(word.c_str(), ";")){
            api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
            cout << "execute success!" <<endl;
        }
        else {
            cout << "Please input right SQL!" <<endl;
            return;
        }
    }
    else {
        cout << "Please input right SQl!" << endl;
        return;
    }
    return;
};


void Interpreter::selectParse(string& word,int& tmp){
    sqlunit req;
    Table *tb;
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), "*")){
        cout << "Please input right SQL!" <<endl;
        return;
    }
    req.sql = SELECT;
    req.pj = "*";
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), "from")){
        cout << "Please input right SQL!" <<endl;
        return;
    }
    word = getWord(&tmp);//表名
    if(!api.CM->check_table_exist(word)){
        cout << "The table doesn't exist!" <<endl;
        return;
    }
    req.tname = word;
    tb = api.CM->tableInfo(word);
    word = getWord(&tmp);
    if(0 == strcmp(word.c_str(), "where")){
        int i=0;
        while(!word.empty() && (0 != strcmp(word.c_str(), ";"))){
            if(0 != i && 0 != strcmp(word.c_str(), "and")){
                cout << "Please input right SQL!" <<endl;
                return;
            }
            word = getWord(&tmp);
            if(!api.CM->is_attrintable(req.tname, word)){
                cout << "The attribute doesn't exist in the table !" <<endl;
                return;
            }
            req.qc[i].aname = word;
            word = getWord(&tmp);
            if(0 == strcmp(word.c_str(), "=")){
                req.qc[i].mode = 0;
            }
            else if(0 == strcmp(word.c_str(), "!=")){
                req.qc[i].mode = 1;
            }
            else if(0 == strcmp(word.c_str(), ">")){
                req.qc[i].mode = 2;
            }
            else if(0 == strcmp(word.c_str(), ">=")){
                req.qc[i].mode = 3;
            }
            else if(0 == strcmp(word.c_str(), "<")){
                req.qc[i].mode = 4;
            }
            else if(0 == strcmp(word.c_str(), "<=")){
                req.qc[i].mode = 5;
            }
            else {
                cout << "Please input right SQL!" <<endl;
                return;
            }
            word = getWord(&tmp);
            char type = api.CM->attrType(req.tname, req.qc[i].aname);
            switch(type){
                case 'D':
                    if(isDouble(word)){
                        double *ii = new double;
                        sscanf(word.c_str(), "%lf", ii);
                        req.qc[i].value = ii;
                        //req.qc[i].value = new double(atof(word.c_str()));
                    }
                    else {
                        cout << "The type of attribute is wrong!" << endl;
                        return;
                    }
                    break;
                case 'I':
                    if(isInt(word)){
                        int *ii = new int;
                        sscanf(word.c_str(), "%d", ii);
                        req.qc[i].value = ii;
                        //req.qc[i].value = new int(atoi(word.c_str()));
                    }
                    else{
                        cout << "The type of attribute is wrong!" << endl;
                        return;
                    }
                    break;
                case 'C':
                    if(isChar(word)){
                        req.qc[i].value = new char[word.length()+1];
                        memcpy(req.qc[i].value, word.c_str(), word.length()+1);
                    }
                    else{
                        cout << "The type of attribute is wrong!" << endl;
                        return;
                    }
                    break;
                default:
                    cout << "The typy of attribute is wrong!" <<endl;
                    return;
            }
            i++;
            word = getWord(&tmp);
        }
        req.num_condition = i;
        if(0 == strcmp(word.c_str(), ";")){
            api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
            cout << "execute success!" << endl;
        }
        else {
            cout << "Please input right SQL!" <<endl;
            return;
        }
    }
    else if(0 == strcmp(word.c_str(), ";")){
        api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
        cout << "execute success!" <<endl;
    }
    else {
        cout << "Please input right SQL!" <<endl;
        return;
    }

    return;
};
void Interpreter::createTbParse(string& word,int& tmp){
    sqlunit req;
    Table tb;
    word = getWord(&tmp);
    if(!word.empty()){//CREATE_TABLE
        req.sql = CREATE_TABLE;
        if(api.CM->check_table_exist(word)){
            cout << "Sorry, this table has existed!" <<endl;
            return;
        }
        if(isLegalName(word)){
            req.tname = word;//表格名字
            tb.tableName = word;//表格名字
            word = getWord(&tmp);
            if(0 == strcmp(word.c_str(),"(")){
                word = getWord(&tmp);
                while((0 != strcmp(word.c_str(),"primary")) && (!word.empty())){
                    if(!isLegalName(word)){
                        cout << "The attribute's name is illegal!" << endl;
                        return ;
                    }
                    tb.attrlist[tb.attrNum].name = word;//属性名字
                    if(tb.attrNum){//属性偏移：第一个属性偏移为0，后面属性为原来属性offset加length；
                        tb.attrlist[tb.attrNum].offset = tb.attrlist[tb.attrNum-1].lenth + tb.attrlist[tb.attrNum-1].offset;
                    }
                    word = getWord(&tmp);//属性数据类型
                    if(word.empty()|| (0 == strcmp(word.c_str(),",")) || (0 == strcmp(word.c_str(),")"))){
                        cout<< "Please input right SQL!" <<endl;
                        return;
                    }
                    else{
                        if(0==strcmp(word.c_str(), "int")){
                            tb.attrlist[tb.attrNum].type = 'I';//属性数据类型
                            tb.attrlist[tb.attrNum].lenth = 4;//属性字节长度
                            tb.tupLength += 4;
                            //cout << tb.tupLength << endl;
                        }
                        else if(0==strcmp(word.c_str(), "double")){
                            tb.attrlist[tb.attrNum].type = 'D';//属性数据类型
                            tb.attrlist[tb.attrNum].lenth = 8;//属性字节长度
                            tb.tupLength += 8;
                            //cout << tb.tupLength << endl;
                        }
                        else if(0==strcmp(word.c_str(), "char")){
                            word = getWord(&tmp);
                            if(0 == strcmp(word.c_str(), "(")){
                                word = getWord(&tmp);//char的长度,在[1，255]之间
                                int len = stoi(word);
                                if(len<1 || len>255){
                                    cout<<"Sorry,your char are too large(small)!" <<endl;
                                    cout<< "Please input right SQL!" <<endl;
                                    return;
                                }
                                else {
                                    tb.attrlist[tb.attrNum].type = 'C';//属性数据类型
                                    tb.attrlist[tb.attrNum].lenth = len;//属性字节长度
                                    tb.tupLength += len;
                                    //cout << tb.tupLength << endl;
                                }
                            }
                            else{
                                cout<< "Please input right SQL!" <<endl;
                                return;
                            }
                            word = getWord(&tmp);
                            if(0 != strcmp(word.c_str(), ")")){
                                cout<< "Please input right SQL!" <<endl;
                                return;
                            }
                        }
                        else{
                            cout<< "Please input right SQL!" <<endl;
                            cout << "There only three types for ATTRIBUTE:char,int,double"<<endl;
                            return;
                        }
                    }
                    word = getWord(&tmp);//属性是否unique
                    if(0 == strcmp(word.c_str(), "unique")){
                        tb.attrlist[tb.attrNum].is_unique = true;
                        word = getWord(&tmp);
                        if(0 != strcmp(word.c_str(), ",")){
                            cout<< "Please input right SQL!" <<endl;
                            return;
                        }
                    }
                    else if(0 != strcmp(word.c_str(), ",")){
                        cout<< "Please input right SQL!" <<endl;
                        return;
                    }
                    tb.attrNum ++;//属性数目
                    if(tb.attrNum>32){
                        cout << "Sorry, the number of attributes is less than 32!" << endl;
                        return;
                    }
                    word = getWord(&tmp);
                }
                if(0 == strcmp(word.c_str(),"primary")){//主键
                    word = getWord(&tmp);
                    if(0 != strcmp(word.c_str(),"key")){
                        cout<< "Please input right SQL!" <<endl;
                        return;
                    }
                    word = getWord(&tmp);
                    if(0 != strcmp(word.c_str(),"(")){
                        cout<< "Please input right SQL!" <<endl;
                        return;
                    }
                    word = getWord(&tmp);
                    int flag = 0;
                    int i;
                    for(i=0; i< tb.attrNum;i++){
                        if(tb.attrlist[i].name == word){
                            flag = 1;
                            break;
                        }
                    }
                    if(flag){
                        tb.attrlist[i].is_primary_key = true;
                        tb.primaryKey = tb.attrlist[i].name;
                    }
                    else {
                        cout << "There is something wrong with primary key!" <<endl;
                        return;
                    }
                    word = getWord(&tmp);
                    if(0 != strcmp(word.c_str(),")")){
                        cout<< "Please input right SQL!" <<endl;
                        return;
                    }
                    word = getWord(&tmp);
                    if(0 != strcmp(word.c_str(),")")){
                        cout<< "Need a '）' in the end of sentence!!" <<endl;
                        return;
                    }
                    word = getWord(&tmp);
                    if(0 != strcmp(word.c_str(),";")){
                        cout<< "Need a ';' in the end of sentence!" <<endl;
                        return;
                    }
                }
            }
            else {
                cout<< "Please input right SQL!" <<endl;
                return;
            }

        }
        else {
            cout << "Please input right SQL!" <<endl;
            return;
        }
        api.CM->create_table(tb);
        api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
        cout << "create success!" <<endl;
    }
    return;
};
void Interpreter::createIndxPase(string& word,int& tmp){
    sqlunit req;
    Table tb;
    word = getWord(&tmp);//索引名称
    req.sql = CREATE_INDEX;
    req.iname = word;
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(),"on")){
        cout << "Please input right SQL!" << endl;
        return;
    }
    word = getWord(&tmp);//索引基于的表名
    if(!api.CM->check_table_exist(word)){//检验表是否存在
        cout << "Sorry, this table doesn't exist!" <<endl;
        return;
    }
    req.tname = word;
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), "(")){
        cout << "Please input right SQL!" << endl;
        return;
    }
    word = getWord(&tmp);//索引基于的属性
    if(!api.CM->is_attrintable(req.tname,word)){//检验表中是否存在该属性
        cout << "Sorry, the attribute dosen't exist in the table!" << endl;
        return;
    }
    req.aname = word;
    if(api.CM->is_index(req.tname, req.aname, word)){
        cout << "Sorry, this index has existed!" << endl;
        return;
    }
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), ")")){
        cout << "Need a '）' in the end of sentence!" << endl;
        return;
    }
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), ";")){
        cout << "Need a ';' in the end of sentence!" << endl;
        return;
    }
    api.CM->create_index(req.tname, req.iname, req.aname);
    api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
    //cout << "create success!" <<endl;
    return;
};
void Interpreter::dropTbParse(string& word,int& tmp){
    sqlunit req;
    Table tb;
    word = getWord(&tmp);
    if(!api.CM->check_table_exist(word)){
        cout << "The table doesn't exist!" <<endl;
        return;
    }
    req.sql = DROP_TABLE;
    req.tname = word;
    word = getWord(&tmp);
    if(0 == strcmp(word.c_str(), ";")){
        api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
        //api.CM->drop_table(req.tname);
        cout << "drop success!" << endl;
    }
    else{
        cout << "Need a ';' in the end of request!" <<endl;
        return;
    }
    return;
};
void Interpreter::dropIndxParse(string& word,int& tmp){
    sqlunit req;
    Table tb;
    word = getWord(&tmp);

    req.sql= DROP_INDEX;
    req.iname = word; //索引名字
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), "on")){
        cout << "Please input right SQL!" <<endl;
        return;
    }
    word = getWord(&tmp);
    if(!api.CM->check_table_exist(word)){
        cout << "The table doesn't exist!" <<endl;
        return;
    }
    req.tname=word;//索引基于的表格
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), "(")){
        cout << "Please input right SQL!" <<endl;
        return;
    }
    word = getWord(&tmp);
    if(!api.CM->is_attrintable(req.tname,word) && !api.CM->is_index(req.tname,word,req.iname)){
        cout << "Please input right SQL!" <<endl;
        return;
    }
    req.aname = word;//索引基于的属性
    if(!api.CM->is_index(req.tname,word,req.iname)){
        cout << "The index doesn't exist!" <<endl;
        return;
    }
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), ")")){
        cout << "Please input right SQL!" <<endl;
        return;
    }
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), ";")){
        cout << "Please input right SQL!" <<endl;
        return;
    }
    api.execute_sql(req.sql, req.pj, req.tname, req.rec, req.qc, req.num_condition, req.iname, req.aname);
    api.CM->drop_index(req.tname,req.iname, req.iname);
    cout << "drop success!" <<endl;
    return;
};
void Interpreter::execfile(string& word,int& tmp){
    word = getWord(&tmp);//文件名
    string filename=word;
    word = getWord(&tmp);//封号
    if(0 != strcmp(word.c_str(), ";")){
        cout << "Need a ';' in the end of request!" <<endl;
        return;
    }
    string PATH = "/Users/liangxiao/Documents/Xcodeworkspace/MiniSQL/MiniSQL/";
    filename = PATH + filename;
    ifstream sqlfile(filename);
    if(sqlfile.is_open()){
        while(getline(sqlfile,s,'\n')){
            tmp = 0;
            word = getWord(&tmp);
            if(0 == strcmp(word.c_str(),"create")){
                word = getWord(&tmp);
                if(0 == strcmp(word.c_str(),"table"))//CREATE_TABLE
                    createTbParse(word,tmp);
                else if(0 == strcmp(word.c_str(), "index"))//CREATE_INDEX
                    createIndxPase(word, tmp);
                else {
                    cout << "Please input right SQL!" << endl;
                    return;
                }
            }
            else if(0 == strcmp(word.c_str(),"insert"))//INSERT
                insertParse(word, tmp);
            else if(0 == strcmp(word.c_str(),"select"))//SELECT
                selectParse(word, tmp);
            else if(0 == strcmp(word.c_str(),"drop")){
                word = getWord(&tmp);
                if(0 == strcmp(word.c_str(), "table"))//DROP_TABLE
                    dropTbParse(word, tmp);
                else if(0 == strcmp(word.c_str(), "index"))//DROP_INDEX
                    dropIndxParse(word, tmp);
                else {
                    cout << "Please input right SQL!" <<endl;
                    return;
                }
            }
            else if(0 == strcmp(word.c_str(),"quit"))//QUIT
                quit(word, tmp);
            else if(0 == strcmp(word.c_str(), "delete"))//DELETE
                deleteParse(word, tmp);
            else{
                cout << "There is something that can't be interpretd." <<endl;
                return;
            }
        }
        sqlfile.close();
    }
    else{
        cout << "This file can not open, or it doesn't exits!" << endl;
        return;
    }
    return;
};
void Interpreter::quit(string& word,int& tmp){
    word = getWord(&tmp);
    if(0 != strcmp(word.c_str(), ";")){
        cout << "Need a ';' in the end of request!" << endl;
        return;
    }
    cout << "GOOD----------------BY" << endl;
    exit(1);
    return;
};
void Interpreter::recordParse(char *rec, Table tb){
    if(rec == NULL){
        cout << "There is no record under the condition." <<endl;
    }
    int i = 0, count = 0;
    int key =0;
    unsigned short rec_len = strlen(rec)-1;
    for(i = 0; i<rec_len ; i = i + tb.tupLength){
        for(count =0; count <tb.attrNum;count++){
            cout << tb.attrlist[count].name << ' ';
            unsigned short off = tb.attrlist[count].lenth;
            for (int j = i+key; j<i+key+off; j++){
                cout << rec[j];
            }
            if(count == tb.attrNum-1)
                cout << ';';
            else
                cout << ',' << ' ';
            key += off;
        }
        cout << '\n';
    }
    return;
};
