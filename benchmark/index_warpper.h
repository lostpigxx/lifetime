/*
    IndexWarpper
    为各类索引提供统一的访问接口
    
    添加新的索引的步骤
        1. 在option.h中添加索引的对应编码
        2. 在本文件中继承IndexWarpper基类，构建对应的索引类，实现设计的接口
        3. 在benchmark.cpp中main函数里添加索引对应的缩写
        4. 在benchmark.cpp中benchmark构造函数中添加索引对应的构造函数
*/


#ifndef INDEX_BENCHMARK_INDEX_WARPPER_H_
#define INDEX_BENCHMARK_INDEX_WARPPER_H_

#include <iostream>

#include "util.h"
#include "option.h"

#include "index/art.h"
#include "index/skiplist_leveldb.h"
#include "index/masstree.h"

using namespace std;



class IndexWarpper {
  public:
    StringKeyComparator cmp_;

    virtual void inline Insert(const char* key, void* val) = 0;

    virtual bool inline Contains(const char* key) const = 0;

};


class IndexSkiplistLevelDB : public IndexWarpper {
  private:
    SkipListLevelDB<const char*, StringKeyComparator> skiplist_;

  public:
    IndexSkiplistLevelDB() : skiplist_(cmp_) {}
    
    void inline Insert(const char* key, void* val) {
        skiplist_.Insert(key);
    }

    bool inline Contains(const char* key) const {
        return skiplist_.Contains(key);
    }


};

class IndexArt : public IndexWarpper {
  private:
    art_tree* art_;
    int keyLen_;
  public:
    IndexArt(int keyLen) : keyLen_(keyLen) {
        art_ = new art_tree;
        art_tree_init(art_);
    }

    void inline Insert(const char* key, void* val) {
        art_insert(art_, reinterpret_cast<const unsigned char*>(key), keyLen_, val);
    }

    bool inline Contains(const char* key) const {
        void* val = art_search(art_, reinterpret_cast<const unsigned char*>(key), keyLen_);
        if (val == NULL) 
            return false;
        else 
            return true;
    }

};


class IndexMassTree : public IndexWarpper {
  private:
    mass_tree* mt;
    int keyLen_;
  public:
    IndexMassTree(int keyLen) : keyLen_(keyLen) {
        mt = new_mass_tree();
    }

    void inline Insert(const char* key, void* val) {
        mass_tree_put(mt, reinterpret_cast<const void*>(key), keyLen_, val);
    }

    bool inline Contains(const char* key) const {
        void* val = mass_tree_get(mt, reinterpret_cast<const void*>(key), keyLen_);
        if (val == NULL) 
            return false;
        else 
            return true;
    }

};


#endif // INDEX_BENCHMARK_INDEX_WARPPER_H_