// This is options for benchmark

#ifndef INDEX_BENCHMARK_OPTION_H_
#define INDEX_BENCHMARK_OPTION_H_

#include <iostream>

using namespace std;


const static int test_type_random_reads = 1;
const static int test_type_random_write = 2;

const static int index_skiplist_leveldb = 1;
const static int index_skiplist_adaptive = 2;
const static int index_masstree = 3;
const static int index_adaptive_radix_tree = 4;

class Options {
  public:
    // KV数量
    int kvNums;
    
    // 测试模式
    int benchMode;
    
    // 哪种索引结构
    int indexType;
    
    // 线程数
    int threadNums;

    // key的长度, bytes
    size_t keyLen;

    // key是否是变长
    bool isKeyVariable;

    // value的长度, bytes
    size_t valLen;

  public:
    // 默认的配置
    Options() {
        kvNums = 100000;
        benchMode = test_type_random_write;
        indexType = index_skiplist_leveldb;
        threadNums = 1;
        keyLen = 16;
        isKeyVariable = false;
        valLen = 100;
    };
};


#endif // INDEX_BENCHMARK_OPTION_H_