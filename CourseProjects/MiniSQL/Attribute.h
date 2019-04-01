//
//  Attribute.h
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//  the Attribute class is to record every attribute in a table

#ifndef Attribute_h
#define Attribute_h
#include <string>
using namespace std;

class Attribute
{
public:
    string name;
    unsigned short offset; //距离记录头的字节数
    unsigned short lenth;  //该属性所占字节数
    void *p;
    char type;  //'D' 'I' 'C'
    bool is_unique;
    bool is_primary_key;
    bool is_index;
    Attribute(){
           name = "";
           offset = 0;
           lenth = 0;
           p = NULL;
           type = 'I';
           is_unique = 0;
           is_primary_key = 0;
           is_index = 0;
    }
};



#endif
