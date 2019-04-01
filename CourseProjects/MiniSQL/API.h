//
//  API.h
//  MiniSQL
//  API is the set of all interface of all other modules
//  Created by 梁潇 on 10/28/15.
//
//

#ifndef API_h
#define API_h

#include <iostream>
#include "RecordManager.h"
#include "BufferManager.h"
#include "CatalogManager.h"
#include "Query_condition.h"
#include "FileBlock.h"
#include "IndexManager.h"
#include "Table.h"


// The definition of (int sql)
#define INSERT 0
#define DELETE 1
#define SELECT 2
#define CREATE_TABLE 3
#define DROP_TABLE   4
#define CREATE_INDEX 5
#define DROP_INDEX   6

class API
{
public:
    // contain the pointers point to other modules
    RecordManager *RM;
    BufferManager *BM;
    CatalogManager *CM;
    IndexManager *IM;
public:

    void execute_sql(int sql, string pj, string tname, char *rec, Query_condition *qc, int num_condition, string iname, string aname);
    bool execute_insert(string tname, char *rec);
    bool execute_delete(string tname, Query_condition *qc, int num_condition);
    bool execute_select(string tname, string pj, Query_condition *qc, int num_condition);
    bool execute_create_table(string tname);
    bool execute_drop_table(string tname);
    bool execute_create_index(string tname, string iname, string aname);
    bool execute_drop_index(string tname, string iname, string aname);
    void output_record(char *rec, string tname);
private:
    //char decomposition(void **buf, char *r, string aname);
public:
    API();
    ~API();
};

#endif
