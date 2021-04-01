/*
    Simple key value generator
    Support multiple data distribution
*/

#ifndef INDEX_BENCHMARK_KVGENERATOR_H_
#define INDEX_BENCHMARK_KVGENERATOR_H_


#include <iostream>
#include <vector>
#include <random>

#include "util.h"

using namespace std;

const char alph[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";


class KeyGenerator {
  public:
    random_device rd_;
    default_random_engine rnd_;
    uniform_int_distribution<int> uniDis;

    KeyGenerator(int keyRange) : rnd_(rd_()), uniDis(0, keyRange) {}
};



class StringKeyGenerator : public KeyGenerator {
private:
    const static int stringKeyRange = 61;
    Random myRnd_;
public:
    StringKeyGenerator() : KeyGenerator(stringKeyRange) , myRnd_((uint32_t)time(nullptr)) {}
    vector<vector<string>> GenerateRandomStringKeys(int keyLength, int keyNums, int threadNums);
};

vector<vector<string>> StringKeyGenerator::GenerateRandomStringKeys(int keyLength, int keyNums, int threadNums) {
    vector<vector<string>> stringKeys;
    char* str = new char[keyLength];
    for (int i = 0; i < threadNums; i++) {
        stringKeys.push_back(vector<string>());
        for (int j = 0; j < keyNums; j++) {
            for (int k = 0; k < keyLength; k++) {
                // str[k] += alph[uniDis(rnd_)];
                // str[k] += alph[rnd_() % 62];
                str[k] += alph[myRnd_.Next() % 62];
            }
            stringKeys[i].push_back(string(str));
        }
    }
    return stringKeys;
}


#endif // INDEX_BENCHMARK_KVGENERATOR_H_