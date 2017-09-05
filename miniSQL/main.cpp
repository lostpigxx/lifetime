//
//  main.cpp
//  MiniSQL
//
//  Created by 梁潇 on 15/10/17.
//
//

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include "interpre.h"
using namespace std;


#define INSERT 0
#define DELETE 1
#define SELECT 2
#define CREATE_TABLE 3
#define DROP_TABLE   4
#define CREATE_INDEX 5
#define DROP_INDEX   6

int main()
{
    Interpreter God;
//    for(int i=1; i<1010; i++)
//    {
//        char s[100];
//        
//        
//        double a = (double)i / 100;
//
//        sprintf(s, "insert into student values ( %d , '%08d' , %lf ) ;", i, i*2,a);
//        string t(s);
//        God.s = t;
//        God.parse();
//    }
    while(1)
    {
        getline(cin, God.s);
        God.parse();
    }
    return 0;
}

//int main()
//{
//    Interpreter God;
//    string sql;
////    for(int i=1000; i<1010; i++)
////    {
////        char s[100];
////        sprintf(s, "insert into test values ( '%08d', %d ) ;", i, i%18);
////        string t(s);
////        God.s = t;
////        God.parse();
////    }
//    
//    //sql = "select * from test where id = '00000098' and age = 8;";
////    sql = "delete from test where id = '00001000' ;";
//    //sql = "select * from test;";
////    sql = "create table test (id char(10) , age int , primary key(id) );";
////    God.s = sql;
////    God.parse();
////    sql = "insert into test values ('12345678' , 19 );";
//
//    //sql = "delete from test ;";
//    
//    //sql = "select * from test;";
//    sql = "drop table test ;";
//    God.s = sql;
//    God.parse();
//    return 0;
//}


//int main()
//{
//    Attribute a[32];
//    a[0].name = "name";
//    a[0].offset = 0;
//    a[0].lenth = 20;
//    a[0].type = 'C';
//    a[0].is_unique = true;
//    a[0].is_primary_key = true;
//    a[1].name = "age";
//    a[1].offset = 20;
//    a[1].lenth = 4;
//    a[1].type = 'I';
//    a[1].is_unique = false;
//    a[1].is_primary_key = false;
//
//
//    Table t;
//    t.tableName = "student";   //char name[20] int age
//    t.attrNum = 2;
//    t.tupLength = 24;
//    t.primaryKey = "name";
//    memcpy(t.attrlist, a, sizeof(Attribute)*32);
//
//
//    API api;
    //  api.CM->create_table(t);
    //  api.execute_sql(CREATE_TABLE, "*", "student", NULL, NULL, 0, "", " ");
    //
//     for(int i=1000; i<1002; i++)
//     {
//         char na[20];
//         sprintf(na, "%08d", i);
//         int ag = 15;
//         char *r = new char[24];
//         memcpy(r, na, 20);
//         memcpy(r+20, &ag, 4);
//         api.execute_sql(INSERT, "*", "student", r, NULL, 0," ", " ");
//     }
    // Query_condition q[2];
    // int cc = 18;
    // char qq[4] = "lx";
    // q[0].aname = "age";
    // q[0].value = &cc;
    // q[0].mode = MODE_QUEAL;
    // q[1].aname = "name";
    // q[1].value = qq;
    // q[1].mode = MODE_QUEAL;
    //api.execute_sql(SELECT, "*", "student", NULL, q, 2, " ", " ");
//    Query_condition q;
//    int cc = 18;
//    q.aname = "age";
//    q.value = &cc;
//    q.mode = MODE_QUEAL;
//    api.execute_sql(SELECT, "*", "student", NULL, NULL, 0, " ", " ");
    // return 0;

    //api.execute_sql(DELETE, "*", "student", NULL, NULL, 0, " ", " ");
    //api.execute_sql(DELETE, "*", "student", NULL, &q, 1, " ", " ");

    //api.execute_sql(SELECT, "*", "student", NULL, NULL, 0, " ", " ");
//}



//int main()
//{
//    B_Plus_Tree<int> test(3);
//    int a = 7;
//    for(int i=0; i<10; i++)
//    {
//        test.insert_key(i, 0);
//         test.print_tree(test.root);
//        cout << endl;
//    }
////    for(int i=0; i>-10; i--)
////    {
////        test.insert_key(i, 0);
////        test.print_tree(test.root);
////        cout << endl;
////    }
////
//
//    //test.insert_key(a, 0);
//    a = 6;
//    test.delete_key(a);
//    test.print_tree(test.root);
//
////    a=9;
////    test.insert_key(a, 0);
////    test.print_tree(test.root);
////	getchar();
//    return 0;
//}
