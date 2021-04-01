/*  
    benchmark arguments
        --num
        --benchmarks
        --index
        --keylen
        --thread
*/

#include <iostream>
#include <thread>
#include <string>
#include <vector>
#include <atomic>

#include <time.h>
#include <string.h>

#include "option.h"
#include "kv_generator.h"
#include "util.h"
#include "index_warpper.h"

using namespace std;


class Benchmark {
  private:
    Options option_;

    StringKeyGenerator strKeyGen_;
    vector<vector<string>> randomStrKeys_;

    vector<thread*> threads_;
    vector<double> durations_;
    vector<timespec> startTime_;
    vector<timespec> endTime_;

    IndexWarpper* index_;
    void* val_;

  public:
    Benchmark(Options option);

    void Test();


  private:
    void DoTestWork(int keyNums, int ID, int mode);
    void PrintResult(const double& duration, const int& keyNums);

};


Benchmark::Benchmark(Options option) : option_(option) {
    if (option.indexType == index_skiplist_leveldb) {
        cout << "---- Skiplist in LevelDB ----" << endl;
        index_ = new IndexSkiplistLevelDB();
        val_ = nullptr;
    }
    else if (option.indexType == index_adaptive_radix_tree) {
        cout << "---- Adaptive Radix Tree ----" << endl;
        index_ = new IndexArt(option.keyLen);
        val_ = new char[option.valLen];
    }
    else if (option.indexType == index_masstree) {
        cout << "---- MassTree ----" << endl;
        index_ = new IndexMassTree(option.keyLen);
        val_ = new char[option.valLen];
    }
}

void Benchmark::Test() {
    
    cout << "Client thread: "  << option_.threadNums << endl;

    
    durations_.clear();
    durations_.resize(option_.threadNums);
    startTime_.clear();
    startTime_.resize(option_.threadNums);
    endTime_.clear();
    endTime_.resize(option_.threadNums);

    int kvNumsPerThread = option_.kvNums/option_.threadNums;

    // generate key
    cout << "Generate key" << endl;
    randomStrKeys_ = strKeyGen_.GenerateRandomStringKeys(option_.keyLen, kvNumsPerThread, option_.threadNums);

    // prepare data for read
    if (option_.benchMode == test_type_random_reads) {
        cout << "Random read" << endl;
        for (int i = 0; i < kvNumsPerThread; i++) {
            index_->Insert(randomStrKeys_[0][i].c_str(), val_);
        }
    }
    else {
        cout << "Random write" << endl;
    }

    // do work
    threads_.clear();
    timespec start;
    clock_gettime(CLOCK_REALTIME, &start);
    for (int i = 0; i < option_.threadNums; i++) {
        thread* t = new thread(&Benchmark::DoTestWork, this, 
                                kvNumsPerThread, i, option_.benchMode);
        threads_.push_back(t);
    }

    // wait for all threads to finish
    for (int i = 0; i < option_.threadNums; i++) {
        threads_[i]->join();
    }

    // find last finish thread
    timespec* end = &endTime_[0];
    for (int i = 1; i < option_.threadNums; i++) {
        if ((endTime_[i].tv_sec > end->tv_sec) || 
            (endTime_[i].tv_sec == end->tv_sec && endTime_[i].tv_nsec > end->tv_nsec)) {
            end = &endTime_[i];
        }    
    }

    double duration = (end->tv_sec - start.tv_sec) * 1000 + (end->tv_nsec - start.tv_nsec) / 1000000;
    PrintResult(duration, option_.kvNums);
}


void Benchmark::DoTestWork(int keyNums, int ID, int mode) {
    
    timespec t0, t1;
    if (mode == test_type_random_reads) {
        clock_gettime(CLOCK_REALTIME, &t0);
        startTime_[ID] = t0;
        for (int i = 0; i < keyNums; i++) {
            index_->Contains(randomStrKeys_[ID][i].c_str());
        }
        clock_gettime(CLOCK_REALTIME, &t1);
        endTime_[ID] = t1;
    }
    else if (mode == test_type_random_write) {
        clock_gettime(CLOCK_REALTIME, &t0);
        startTime_[ID] = t0;
        for (int i = 0; i < keyNums; i++) {
            index_->Insert(randomStrKeys_[ID][i].c_str(), val_);
        }
        clock_gettime(CLOCK_REALTIME, &t1);
        endTime_[ID] = t1;
    }

    // ms
    double duration = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_nsec - t0.tv_nsec) / 1000000;
    durations_[ID] = duration;
}





void Benchmark::PrintResult(const double& duration, const int& keyNums) {
    cout << "Key number: " << keyNums << endl;
    cout << "Total duration: " << duration << " ms" << endl;
    cout << "Average latency: " << duration * 1000 / keyNums << " us" << endl;
    cout << "Throughput: " << 1000 / (duration * 1000 / keyNums) << " kops/s" << endl;
    cout << "-----------------------" << endl;
}




int main(int argc, char* argv[]) {
    Options option;

    for (int i = 1; i < argc; i+=2) {
        string arg(argv[i]);
        string val(argv[i+1]);

        if (arg == "--num") {
            option.kvNums = stoi(val);
        } 
        else if (arg == "--benchmarks") {
            // readseq, readrand, writeseq, writerand
            if (val == "writerand")
                option.benchMode = test_type_random_write;
            else if (val == "readrand")
                option.benchMode = test_type_random_reads;
        }
        else if (arg == "--index") {
            // skiplistleveldb, art, masstree
            if (val == "skiplistleveldb")
                option.indexType = index_skiplist_leveldb;
            else if (val == "art")
                option.indexType = index_adaptive_radix_tree;
            else if (val == "masstree")
                option.indexType = index_masstree;
        }
        else if (arg == "--keylen") {
            option.keyLen = stoi(val);
        }
        else if (arg == "--thread") {
            option.threadNums = stoi(val);
        }
        
    }


    Benchmark benchmark(option);

    benchmark.Test();
}