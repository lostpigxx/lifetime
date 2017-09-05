//
//  FileBlock.h
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#ifndef FileBlock_h
#define FileBlock_h

#include <iostream>
#include <string>
#include <vector>
#include "Attribute.h"

typedef unsigned long FileAdr;
typedef struct record
{
    char *pr;
    short next_record;  //-1 means no next record  -2 means this record is empty
}Record;


#define NO_NEXT_RECORD -1
#define RECORD_IS_EMPTY -2

#define EMPTY_POSITION_NO_FOUND -3
#define MAX_NAME_LENTH 20

#define NO_FIRST_RECORD 0xffff

#define NO_NEXT_BLOCK 0xffffffffffffffff

//== !+ > >= < <=
#define MODE_QUEAL 0
#define MODE_NOT_QUEAL 1
#define MODE_GREAT 2
#define MODE_GREAT_OR_EQUAL 3
#define MODE_LESS 4
#define MODE_LESS_OR_QUEAL 5
#define ALL_WE_NEED   6

using namespace std;



class FileBlock
{
public:
    char *p;
    unsigned short num_record;
    unsigned short max_num_record;
    unsigned short offset_of_first_record;
    unsigned short record_lenth;
    unsigned short primarykey;  //记录primary key在attribute数组中的位置
    char table_name[20];
    FileAdr this_block;
    FileAdr next_block;
    bool is_change;
    bool is_locked;
    Attribute *att;
    Record *recd;
    const unsigned short base_adr = 34; //表头所占的字节为32
public:
    bool load_block_from_disk(FileAdr f);
    bool write_block_to_disk();

    void insert_record_in_block(char *rec);
    void delete_record_from_block(char *rec);
    void select_record_from_block(string aname, int mode, void *value, vector<string> *vec);

    void set_block(string tname, unsigned short rlen, Attribute *sour, FileAdr a);

    int find_empty_postion();
    bool compare_attribute_value(void *s, char *r, string aname , int mode);
    char decomposition(void **buf, char *r, string aname);
    bool check_block_is_full();
private:
    bool compare_2_char_is_equal(char *a, char *b);

public:
    FileBlock();
    FileBlock(string tname, unsigned short rlen, Attribute *sour, FileAdr a);
    ~FileBlock();
};

#endif /* FileBlock_h */
// 记录的实际地址 = p + 基址 + 偏移 * （记录长度+2） 2存放下一条记录的偏移


//总条目数2 首条记录偏移2 表名20 下一块地址8 记录长度2
