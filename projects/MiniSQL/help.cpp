//
//  help.cpp
//  test
//
//  Created by yy on 15/11/3.
//  Copyright (c) 2015年 yy. All rights reserved.
//

#include "help.h"
//判断属性是否为int数据类型
bool isInt(string s){
    bool flag = true;
    int num = 0;
    int i = 0;
    char cgp[32];
    for(int i=0;i<32;i++)
        cgp[i] = NULL;
    strcpy(cgp, s.c_str());
    if(cgp[0] == '-') i++;//去除符号位
    while(cgp[i] != '\0'){
        if(cgp[i]<'0' || cgp[i] > '9')
            return false;
        num = num * 10 + cgp[i] - '0';
        i++;
    }
    if(num > pow(2,32)-1){
        flag = false;
    }
    return flag;
};
//判断属性是否为double数据类型
bool isDouble(string s){
    bool pointflag = false;
    int i = 0;
    double intnum = 0;
    double pointnum = 0;
    char cgp[32];
    for(int i=0;i<32;i++)
        cgp[i] = NULL;
    if(cgp[0]=='-') i++;
    while(cgp[i] != '\0'){
        if(pointflag == true){
            if(cgp[i]>='0' && cgp[i]<='9')
                pointnum = pointnum * 10 + cgp[i] - '0';
            else
                return false;
        }
        if(cgp[i] == '.')
            pointflag = true;
        if(pointflag == true && cgp[i] == '.')
            return false;
        if(cgp[i] != '.' && pointflag == false){
            if(cgp[i]<'0' || cgp[i] > '9')
                return false;
            intnum = intnum * 10 + cgp[i] - '0';
        }
        i++;
    }
    if((intnum > pow(2,64)-1)||(pointnum > pow(2,64)-1)){
        return false;
    }
    return true;
}
//判断属性是否为char数据类型
bool isChar(string s){
    int i = 0;
    int count = 0;
    char cgp[32];
    for(int i=0;i<32;i++)
        cgp[i] = NULL;
    strcpy(cgp, s.c_str());
    while(cgp[i] != '\0'){
        if((cgp[i] >= 'a' && cgp[i] <= 'z' )||(cgp[i] >= 'A' && cgp[i] <= 'Z')||(cgp[i] >= '0' && cgp[i] <= '9')){
            count ++;
        }
        else
            return false;
        if(count > 32)//字符串不可以超过32个
            return false;
        i++;
    }
    return true;
}
bool isLegalName(string s){
    int i = 0;
    char cgp[32];
    strcpy(cgp, s.c_str());
    if(cgp[0] >= '0' && cgp[0] <= '9'){
        return false;//首字母不可以为数字
    }
    while(cgp[i] != '\0'){
        if((cgp[i] >= 'a' && cgp[i] <= 'z' )||(cgp[i] >= '0' && cgp[i] <= '9')||(cgp[i] == '_'))
            i++;
        else
            return  false;
    }
    return true;
}
/*
char *mystrcat(char *c1,char *c2){
    string s1(c1);
    unsigned long len = s1.length();
    char *c3 = new char[len+1];
    strcat(c3, c1);
    strcat(c3, c2);
    return c1;
}*/