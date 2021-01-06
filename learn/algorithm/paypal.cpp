#include <iostream>
#include <random>
#include <vector>
#include <stack>

using namespace std;

// 生成n个随机数，范围 0 ~ n*10
vector<int> Generate_random_num(int n) {
    vector<int> array;

    // generate random number
    default_random_engine rnd(time(nullptr));
    uniform_int_distribution<int> uni(0, n*10);
    for (int i = 0; i < n; ++i) {
        array.push_back(uni(rnd));
    }

    return array;
}

// 利用2个栈，对一列数排序 时间复杂度O(n2)
void Two_stack_sort() {
    vector<int> v = Generate_random_num(50);
    stack<int> s0, s1;
    
    s0.push(v[0]);
    for (int i = 1; i < v.size(); i++) {
        if (v[i] >= s0.top()) {
            s0.push(v[i]);
            continue;
        }

        while (!s0.empty() && s0.top() > v[i]) {
            s1.push(s0.top());
            s0.pop();
        }
        s0.push(v[i]);
        while (!s1.empty()) {
            s0.push(s1.top());
            s1.pop();
        }
    }

    while (!s0.empty()) {
        cout << s0.top() << " ";
        s0.pop();
    }
    cout << endl;
}

int main() {
    Two_stack_sort();
}