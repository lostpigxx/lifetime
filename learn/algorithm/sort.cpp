#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <algorithm>
#include <time.h>

using namespace std;


void print(const vector<int> v) {
    for (auto val : v) {
        cout << val << " ";
    }
    cout << endl;
}

vector<int> Sort_stl(const vector<int>& array) {
    vector<int> v(array);
    sort(v.begin(), v.end());
    return v;
}


/*
    选择排序
    时间复杂度：平均 最差
*/
vector<int> Sort_select(const vector<int>& array) {
    vector<int> v(array);
    int n = v.size();
    
    for (int i = 0; i < n; ++i) {
        int min_idx = i;
        for (int j = i + 1; j < n; ++j) {
            if (v[j] < v[min_idx]) {
                min_idx = j;
            }
        }
        swap(v[i], v[min_idx]);
    }
    return v;
}

/*
    冒泡排序
    时间复杂度：平均 最差
*/
vector<int> Sort_bubble(const vector<int>& array) {
    vector<int> v(array);
    int n = v.size();

    for (int i = n - 1; i > 0; --i) {
        for (int j = 0; j < i; ++j) {
            if (v[j] > v[j+1])
                swap(v[j], v[j+1]);
        }
    }

    return v;
}


/*
    插入排序
    时间复杂度：平均 最差
*/
vector<int> Sort_insert(const vector<int>& array) {
    vector<int> v(array);
    int n = v.size();

    for (int i = 1; i < n; ++i) {
        int cur = v[i];
        int j;
        for (j = i - 1; j >= 0; --j) {
            if (v[j] > cur) {
                v[j+1] = v[j];
            }
            else {
                v[j+1] = cur;
                break;
            }
        }
        if (j == -1)
            v[0] = cur;
    }

    return v;
}


/*
    归并排序
    时间复杂度：平均 最差
*/
vector<int> Sort_merge(const vector<int>& array) {
    vector<int> v(array);
    int n = v.size();

    if (n < 2)
        return v;
    
    int mid = n / 2;
    vector<int> left(Sort_merge(vector<int>(v.begin(), v.begin() + mid)));
    vector<int> right(Sort_merge(vector<int>(v.begin() + mid, v.end())));

    v.clear();
    // merge
    int i = 0, j = 0;
    while (i < left.size() && j < right.size()) {
        if (left[i] < right[j])
            v.push_back(left[i++]);
        else 
            v.push_back(right[j++]);
    }
    while (i < left.size()) {
        v.push_back(left[i++]);
    }
    while (j < right.size()) {
        v.push_back(right[j++]);
    }

    return v;
}


int partition(vector<int>& v, int left, int right) {
    int pivot = left;
    int mid = left + 1;

    for (int i = mid; i < right; ++i) {
        if (v[i] < v[pivot]) {
            swap(v[i], v[mid++]);
        }
    }
    --mid;
    swap(v[pivot], v[mid]);
    return mid;
}

void quick_sort(vector<int>& v, int left, int right) {
    int n = right - left;
    if (n < 2)
        return;
    
    int mid = partition(v, left, right);
    quick_sort(v, left, mid + 1);
    quick_sort(v, mid + 1, right);

}

/*
    快速排序
    时间复杂度：平均 最差
*/
vector<int> Sort_quick(const vector<int>& array) {
    vector<int> v(array);
    int n = v.size();

    quick_sort(v, 0, n);

    return v;
}


/*
    堆排序
    时间复杂度：平均 最差
*/
vector<int> Sort_heap_stl(const vector<int>& array) {
    vector<int> v(array);
    int n = v.size();
    vector<int> ans;

    make_heap(v.begin(), v.end(), greater<int>());
    for (int i = 0; i < n; ++i) {
        ans.push_back(v[0]);
        pop_heap(v.begin(), v.begin() + n - i, greater<int>());
    }

    return ans;
}

vector<int> Sort_heap_priority_queue(const vector<int>& array) {
    vector<int> v(array);
    int n = v.size();

    priority_queue<int, vector<int>, greater<int>> pq(v.begin(), v.end());
    v.clear();
    while (!pq.empty()) {
        v.push_back(pq.top());
        pq.pop();
    }

    return v;
}

void Adjust_max_heap(vector<int>& v, int i, int n) {
    int left = i * 2 + 1;
    int right = i * 2 + 2;
    int max = i;

    if (left < n && v[left] > v[max]) {
        max = left;
    }

    if (right < n && v[right] > v[max]) {
        max = right;
    }

    if (max != i) {
        swap(v[i], v[max]);
        Adjust_max_heap(v, max, n);
    }
}


void Build_max_heap(vector<int>& v) {
    int n = v.size();

    for (int i = n-1; i >= 0; --i) {
        Adjust_max_heap(v, i, n);
    }
}


vector<int> Sort_heap(const vector<int>& array) {
    vector<int> v(array);
    int n = v.size();

    Build_max_heap(v);
    for (int i = 0; i < n; ++i) {
        swap(v[0], v[n-1-i]);
        Adjust_max_heap(v, 0, n - i - 1);
    }

    return v;
}


int main() {

    int n = 100;
    vector<int> array;

    // generate random number
    default_random_engine rnd(time(nullptr));
    uniform_int_distribution<int> uni(0, n*10);
    for (int i = 0; i < n; ++i) {
        array.push_back(uni(rnd));
    }

    print(Sort_stl(array));
    cout << endl;

    print(Sort_select(array));
    cout << endl;

    print(Sort_bubble(array));
    cout << endl;

    print(Sort_insert(array));
    cout << endl;

    print(Sort_merge(array));
    cout << endl;

    print(Sort_quick(array));
    cout << endl;

    print(Sort_heap_stl(array));
    cout << endl;

    print(Sort_heap_priority_queue(array));
    cout << endl;

    print(Sort_heap(array));
    cout << endl;
}
