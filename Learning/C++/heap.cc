// 学习 heap(堆) 相关的内容

#include <iostream>
#include <queue>
#include <vector>

using namespace std;


// priority_queue，其实就是个堆
// https://cnblogs.com/yalphait/articles/8889221.html
// template
// <class T, class Container = vector<T>, class Compare = less<typename Container::value_type>>
// class priority_queue;

template <class T>
void Print(std::priority_queue<T> heap) {
    while (!heap.empty()) {
        cout << heap.top() << " ";
        heap.pop();
    }
    cout << endl;
}

void PriorityQueue() {

    std::priority_queue<int> heap;
    for (int i = 0; i < 10; ++i) {
        heap.push(i);
    }
    Print(heap);


}

int main() {
    PriorityQueue();
}