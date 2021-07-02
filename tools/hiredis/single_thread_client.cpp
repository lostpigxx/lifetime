// Author: LostPigxx
// E-mail: simplelx@zju.edu.cn
// 	  ٩(⊙o⊙*)و好!很有精神!
//
// This is a single thread hiredis client which can ...

#include <hiredis/hiredis.h>
#include <iostream>

int main() {
    redisContext* redis = redisConnect("127.0.0.1", 6379);
    redisReply* reply = (redisReply*)redisCommand(redis, "auth a");
    freeReplyObject(reply);
    reply = (redisReply*)redisCommand(redis, "set a b");
    freeReplyObject(reply);
    reply = (redisReply*)redisCommand(redis, "get a");
    std::cout << reply->str << std::endl;
    freeReplyObject(reply);
}