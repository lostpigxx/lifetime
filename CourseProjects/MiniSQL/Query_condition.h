//
//  Query_condition.h
//  MiniSQL
//
//  Created by 梁潇 on 10/28/15.
//
//

#ifndef Query_condition_h
#define Query_condition_h

//== !+ > >= < <=
#define MODE_QUEAL 0
#define MODE_NOT_QUEAL 1
#define MODE_GREAT 2
#define MODE_GREAT_OR_EQUAL 3
#define MODE_LESS 4
#define MODE_LESS_OR_QUEAL 5




class Query_condition
{
public:
    string aname;
    void *value;
    int mode;
};


#endif