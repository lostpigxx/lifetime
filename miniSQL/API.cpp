//
//  API.cpp
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#include <iostream>
#include "API.h"
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

#define NO_NEXT_BLOCK 0xffffffffffffffff

typedef unsigned long FileAdr;



/*
 *Description: the constructor of class API
 */
API::API()
{
    RM = new RecordManager();
    BM = new BufferManager();
    CM = new CatalogManager();
    IM = new IndexManager();
}

/*
 *Description: the destructor of class
 */
API::~API()
{
    delete RM;
    delete BM;
    delete CM;
    delete IM;
}

/*
 *Description: 执行sql语句的总入口
 *Parameters: sql:sql语句类型  pj:投影的属性  tname:表名  rec:记录的字节流  qc:where条件数组  num_condition:条件的数量  iname:索引名  aname:属性名
 */
void API::execute_sql(int sql, string pj, string tname, char *rec, Query_condition *qc, int num_condition, string iname, string aname)
{
    switch(sql)
    {
        case INSERT:
            execute_insert(tname, rec); break;
        case DELETE:
            execute_delete(tname, qc, num_condition); break;
        case SELECT:
            execute_select(tname, pj, qc, num_condition); break;
        case CREATE_TABLE:
            execute_create_table(tname); break;
        case DROP_TABLE:
            execute_drop_table(tname); break;
        case CREATE_INDEX:
            execute_create_index(tname, iname, aname); break;
        case DROP_INDEX:
            execute_drop_index(tname, iname, aname); break;
    }
}

/*
 *Description: 执行insert语句
 *Parameters: 见execute_sql
 */
bool API::execute_insert(string tname, char *rec)
{
    bool flag = false;
    FileAdr fa, temp;
    FileBlock *fb;

    Attribute *att = CM->get_attribute(tname);
    int i;
    for(i=0; i<32; i++)
    {
        if(att[i].is_primary_key)
            break;
    }
    void *buf;
    switch(att[i].type)
    {
        case 'D':
            buf = new double(0);
            memcpy(buf, rec+att[i].offset, att[i].lenth);
            break;
        case 'I':
            buf = new int(0);
            memcpy(buf, rec+att[i].offset, att[i].lenth);
            break;
        case 'C':
            buf = new char[att[i].lenth];
            memcpy(buf, rec+att[i].offset, att[i].lenth);
            break;
        default: break;
    }
    if(IM->check_index_exist(tname, buf, att[i].type, ""))
    {
        cout << "Primary key conflict" << endl;
        return false;
    }

    fa = CM->get_first_fileadr(tname);
    temp = CM->get_new_fileadr(tname);
    fb = BM->prepare_for_fileblock_for_insert(&flag, fa, temp);
    memcpy(fb->att, CM->get_attribute(tname), sizeof(Attribute)*32);
    if(flag) //a new file block has been created, update CM
    {
        CM->store_fileadr(tname, temp, false);
    }
    RM->insert_record(rec, fb);

    IM->insert_into_index(tname, buf, fb->this_block, att[i].type, "");
    return true;
}

/*
 *Description: 执行delete语句
 *Parameters: 见execute_sql
 */
bool API::execute_delete(string tname, Query_condition *qc, int num_condition)
{
    FileAdr fa;
    FileBlock *fb;
    vector<string> vec;
    int relen = CM->get_record_length(tname);
    if(num_condition == 0)
    {
        fa = CM->get_first_fileadr(tname);
        do
        {
            fb = BM->prepare_for_fileblock(fa);
            memcpy(fb->att, CM->get_attribute(tname), sizeof(Attribute)*32);
            if(fb->offset_of_first_record == NO_FIRST_RECORD)
                return true;
            for(int i=0, j=fb->offset_of_first_record; i<fb->num_record; i++)
            {
                int t = fb->recd[j].next_record;
                fb->recd[j].next_record = RECORD_IS_EMPTY;
                string ttt(fb->recd[j].pr, relen);
                vec.push_back(ttt);
                j = t;
            }
            fb->offset_of_first_record = NO_FIRST_RECORD;
            fb->num_record = 0;
            fa = fb->next_block;
        }
        while(fa != NO_NEXT_BLOCK);
    }
    else if(num_condition == 1)
    {
        fa = CM->get_first_fileadr(tname);
        do
        {
            fb = BM->prepare_for_fileblock(fa);
            memcpy(fb->att, CM->get_attribute(tname), sizeof(Attribute)*32);
            if(fb->offset_of_first_record == NO_FIRST_RECORD)
                return true;

            int temp = fb->offset_of_first_record;
            for(int i=0, j=fb->offset_of_first_record; i<fb->max_num_record; i++)
            {
                if(fb->compare_attribute_value(qc[0].value, fb->recd[j].pr, qc[0].aname, qc[0].mode))
                {
                    if(j==fb->offset_of_first_record)
                    {
                        fb->offset_of_first_record = fb->recd[j].next_record;
                        fb->recd[j].next_record = RECORD_IS_EMPTY;
                        string ttt(fb->recd[j].pr, relen);
                        vec.push_back(ttt);
                        fb->num_record--;
                        j = fb->offset_of_first_record;
                        continue;
                    }
                    else
                        fb->recd[temp].next_record = fb->recd[j].next_record;
                    fb->recd[j].next_record = RECORD_IS_EMPTY;
                    string ttt(fb->recd[j].pr, relen);
                    vec.push_back(ttt);
                    fb->num_record--;
                }
                temp = j;
                if(fb->recd[j].next_record == NO_NEXT_RECORD || fb->recd[j].next_record == RECORD_IS_EMPTY)
                    break;
                j = fb->recd[j].next_record;
            }
            fa = fb->next_block;
        }
        while(fa != NO_NEXT_BLOCK);
    }
    else
    {

    }

    Attribute *att = CM->get_attribute(tname);
    int i;
    for(i=0; i<32; i++)
    {
        if(att[i].is_primary_key)
            break;
    }
    void *buf = NULL;
    for(int ii=0; ii<vec.size(); ii++)
    {
        switch(att[i].type)
        {
            case 'D':
                buf = new double(0);
                memcpy(buf, vec[ii].c_str()+att[i].offset, att[i].lenth);
                break;
            case 'I':
                buf = new int(0);
                memcpy(buf, vec[ii].c_str()+att[i].offset, att[i].lenth);
                break;
            case 'C':
                buf = new char[att[i].lenth];
                memcpy(buf, vec[ii].c_str()+att[i].offset, att[i].lenth);
                break;
            default: break;
        }
        IM->delete_index_value(tname, buf, att[i].type, "");
    }
    return true;
}

/*
 *Description: 执行select语句
 *Parameters: 见execute_sql
 */
bool API::execute_select(string tname, string pj, Query_condition *qc, int num_condition)
{
    FileAdr fa;
    FileBlock *fb;
    vector<string> vec;
    if(num_condition == 0)
    {
        fa = CM->get_first_fileadr(tname);
        do
        {
            fb = BM->prepare_for_fileblock(fa);
            memcpy(fb->att, CM->get_attribute(tname), sizeof(Attribute)*32);
            RM->select_record(fb, "", ALL_WE_NEED, NULL, &vec);
            fa = fb->next_block;
        }
        while(fa != NO_NEXT_BLOCK);
        for(int i=0; i<vec.size(); i++)
        {
            unsigned short l = CM->get_record_length(tname);
            char *temp = new char[l];
            memcpy(temp, vec[i].c_str(), l);
            output_record(temp, tname);
        }
        return true;
    }
    if(num_condition == 1)
    {
        fa = CM->get_first_fileadr(tname);
        do
        {
            fb = BM->prepare_for_fileblock(fa);
            memcpy(fb->att, CM->get_attribute(tname), sizeof(Attribute)*32);
            RM->select_record(fb, qc[0].aname, qc[0].mode, qc[0].value, &vec);
            fa = fb->next_block;
        }
        while(fa != NO_NEXT_BLOCK);
        for(int i=0; i<vec.size(); i++)
        {
            unsigned short l = CM->get_record_length(tname);
            char *temp = new char[l];
            memcpy(temp, vec[i].c_str(), l);
            output_record(temp, tname);
        }
        return true;
    }
    vector<string> vec1;
    vector<string> result;
    set<string> s1;
    set<string> s2;
    for(int i=0; i<num_condition; i++)
    {

        fa = CM->get_first_fileadr(tname);
        do
        {
            if(i == 0)
            {
                fb = BM->prepare_for_fileblock(fa);
                memcpy(fb->att, CM->get_attribute(tname), sizeof(Attribute)*32);
                RM->select_record(fb, qc[i].aname, qc[i].mode, qc[i].value, &vec);
                fa = fb->next_block;
            }
            else
            {
                fb = BM->prepare_for_fileblock(fa);
                memcpy(fb->att, CM->get_attribute(tname), sizeof(Attribute)*32);
                RM->select_record(fb, qc[i].aname, qc[i].mode, qc[i].value, &vec1);
                fa = fb->next_block;
            }
        }
        while(fa != NO_NEXT_BLOCK);
        if(i == 0)
            continue;
        result.resize(vec.size());
        for(int k=0; k<vec.size(); k++)
        {
            s1.insert(vec[k]);
        }
        for(int j=0; j<vec1.size(); j++)
        {
            s2.insert(vec1[j]);
        }
        vector<string>::iterator retEndPos;
        retEndPos = set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), result.begin());
        result.resize(retEndPos - result.begin());
        vec.clear();
        vec.resize(result.size());
        vec.insert(vec.begin(), result.begin(), result.end());
    }

    for(int i=0; i<vec.size()/2; i++)
    {
        unsigned short l = CM->get_record_length(tname);
        char *temp = new char[l];
        memcpy(temp, vec[i].c_str(), l);
        output_record(temp, tname);
    }
    return true;
}

/*
 *Description: 执行create table语句
 *Parameters: 见execute_sql
 */
bool API::execute_create_table(string tname)
{
    FileAdr a = CM->get_new_fileadr(tname);
    FileBlock *fb = new FileBlock(tname, CM->get_record_length(tname), CM->get_attribute(tname), a);
    fb->write_block_to_disk();
    delete fb;
    CM->store_fileadr(tname, a, true);

    Attribute *att = CM->get_attribute(tname);
    int i;
    for(i=0; i<32; i++)
    {
        if(att[i].is_primary_key)
            break;
    }
    IM->create_new_index(tname, "", att[i].name, att[i].type, CM->calculate_degree(tname));
    return true;
}

/*
 *Description: 执行drop table语句
 *Parameters: 见execute_sql
 */
bool API::execute_drop_table(string tname)
{
    Attribute *att = CM->get_attribute(tname);
    int i;
    for(i=0; i<32; i++)
    {
        if(att[i].is_primary_key)
            break;
    }
    CM->drop_index(tname, att[i].name, "");
    CM->drop_table(tname);
    return true;
}

/*
 *Description: 输出记录到命令行
 *Parameters: 见execute_sql
 */
void API::output_record(char *rec, string tname)
{
    Attribute *att = CM->get_attribute(tname);
    int m = CM->get_num_attribute(tname);
    for(int i=0; i<m; i++)
    {
        void *buf;
        string temp(att[i].name);
        switch(att[i].type)
        {
            case 'D':
                buf = new double(0);
                memcpy(buf, rec+att[i].offset, att[i].lenth);
                cout << *(double *)buf << ' ';
                break;
            case 'I':
                buf = new int(0);
                memcpy(buf, rec+att[i].offset, att[i].lenth);
                cout << *(int *)buf << ' ';
                break;
            case 'C':
                buf = new char[att[i].lenth];
                memcpy(buf, rec+att[i].offset, att[i].lenth);
                string temp1((char *)buf);
                cout << temp1 << ' ';
                break;
        }
    }
    cout << endl;
}

/*
 *Description: 执行create index语句
 *Parameters: 见execute_sql
 */
bool API::execute_create_index(string tname, string iname, string aname)
{
    Attribute *att = CM->get_attribute(tname);
    int i;
    for(i=0; i<32; i++)
    {
        if(att[i].name == aname)
            break;
    }
    if(!att[i].is_unique && !att[i].is_primary_key)
    {
        cout << "Cannot create index on non-unique attribute" << endl;
        return false;
    }
    IM->create_new_index(tname, iname, aname, att[i].type, CM->calculate_degree(tname));

    FileAdr fa;
    FileBlock *fb;
    vector<string> vec1;
    vector<FileAdr> vec2;
    unsigned long ind = 0;

    fa = CM->get_first_fileadr(tname);
    do
    {
        fb = BM->prepare_for_fileblock(fa);
        memcpy(fb->att, CM->get_attribute(tname), sizeof(Attribute)*32);
        RM->select_record(fb, "", ALL_WE_NEED, NULL, &vec1);
        for(int i=0; i<vec1.size() - ind; i++)
        {
            vec2.push_back(fb->this_block);
        }
        ind = vec1.size();
        fa = fb->next_block;
    }
    while(fa != NO_NEXT_BLOCK);

    void *buf = NULL;
    for(int ii=0; ii<vec1.size(); ii++)
    {
        switch(att[i].type)
        {
            case 'D':
                buf = new double(0);
                memcpy(buf, vec1[ii].c_str()+att[i].offset, att[i].lenth);
                break;
            case 'I':
                buf = new int(0);
                memcpy(buf, vec1[ii].c_str()+att[i].offset, att[i].lenth);
                break;
            case 'C':
                buf = new char[att[i].lenth];
                memcpy(buf, vec1[ii].c_str()+att[i].offset, att[i].lenth);
                break;
            default: break;
        }
        IM->insert_into_index(tname, buf, vec2[ii], att[i].type, iname);
    }

    CM->create_index(tname, iname, aname);
    cout << "index create success" << endl;
    return true;
}

/*
 *Description: 执行drop index语句
 *Parameters: 见execute_sql
 */
bool API::execute_drop_index(string tname, string iname, string aname)
{
    CM->drop_index(tname, aname, iname);
    return true;
}
//
// char API::decomposition(void **buf, char *r, string aname)
// {
//     for(int i=0; i<32; i++)
//     {
//         string temp(att[i].name);
//         if(temp == aname)
//         {
//             switch(att[i].type)
//             {
//                 case 'D':
//                     *buf = new double(0);
//                     memcpy(*buf, r+att[i].offset, att[i].lenth);
//                     break;
//                 case 'I':
//                     *buf = new int(0);
//                     memcpy(*buf, r+att[i].offset, att[i].lenth);
//                     break;
//                 case 'C':
//                     *buf = new char[att[i].lenth];
//                     memcpy(*buf, r+att[i].offset, att[i].lenth);
//                     break;
//                 default: break;
//             }
//             return att[i].type;
//         }
//     }
//     return NULL;
// }













//
