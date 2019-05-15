#include <iostream>
#include <vector>
#include <random>

using namespace std;


class TopK {
  public:
    vector<int> TopK_sort(const vector<int>& v, int k);
    vector<int> TopK_partial_sort(const vector<int>& v, int k);
    vector<int> TopK_heap(const vector<int>& v, int k);
    vector<int> TopK_random_select(const vector<int>& v, int k);
    vector<int> TopK_stl_nth_element(const vector<int>& v, int k);

};


void print(vector<int> ans) {
    for (auto val : ans) {
        cout << val << " ";
    }
    cout << endl;
}

bool cmp(const int& a, const int& b) {
    return a < b;
}

// int partition(int l, int r, vector<int>& array) {
//     int pivot = array[l];
//     int i = l + 1, j = r;
//     while (true) {
//         while (array[i] < pivot && i <= r) i++;
//         while (array[j] > pivot && j >= l) j--;
//         if (i < j)
//             swap(array[i++], array[j++]);
//         else 
//             break;
//     }
//     return j;
// }

// void topK(int l, int r, int K, vector<int>& array) {
//     print(array);

//     int mid = partition(l, r, array);
//     int left_length = mid - l + 1;
//     if (left_length < K)
//         topK(mid+1, r, K - left_length, array);
//     else if (left_length > K)
//         topK(l, mid, K, array);
// }


// vector<int> TopK_RandomSelect(vector<int>& array, int K) {
//     vector<int> ans;
    
//     topK(0, array.size(), K, array);
    
//     for (int i = 0; i < K; i++)
//         ans.push_back(array[i]);
//     return ans; 

// }

// time: O(n) expected
vector<int> TopK::TopK_random_select(const vector<int>& v, int k) {
    vector<int> tmp(v);

    

    return vector<int>(tmp.begin(), tmp.begin() + k);;
}

// time: O(nlogn)
vector<int> TopK::TopK_sort(const vector<int>& v, int k) {
    vector<int> tmp(v);

    sort(tmp.begin(), tmp.end());    

    return vector<int>(tmp.begin(), tmp.begin() + k);;
}

// time: O(klogn)
vector<int> TopK::TopK_heap(const vector<int>& v, int k) {
    vector<int> tmp(v);
    
    make_heap(tmp.begin(), tmp.begin() + k);
    for (int i = k; i < tmp.size(); i++) {
        if (tmp[i] < tmp[0]) {
            pop_heap(tmp.begin(), tmp.begin() + k);
            swap(tmp[k-1], tmp[i]);
            push_heap(tmp.begin(), tmp.begin() + k);
        }
    }

    return vector<int>(tmp.begin(), tmp.begin() + k);;
}

// time: O(n) expected
vector<int> TopK::TopK_stl_nth_element(const vector<int>& v, int k) {
    vector<int> tmp(v);

    nth_element(tmp.begin(), tmp.begin() + k, tmp.end());

    return vector<int>(tmp.begin(), tmp.begin() + k);
}

// time: O(k * n)
vector<int> TopK::TopK_partial_sort(const vector<int>& v, int k) {
    vector<int> tmp(v);

    for (int i = 0; i < k; i++) {
        int min = i;
        for (int j = i + 1; j < tmp.size(); j++) {
            if (tmp[j] < tmp[min])
                min = j;
        }
        swap(tmp[i], tmp[min]);
    }

    return vector<int>(tmp.begin(), tmp.begin() + k);
}

int main() {
    TopK solution;

    int n, k;
    n = 100;
    k = 10;

    vector<int> array;

    // generate random number
    default_random_engine rnd(time(nullptr));
    uniform_int_distribution<int> uni(0, n*10);
    for (int i = 0; i < n; ++i) {
        array.push_back(uni(rnd));
    }

    vector<int> ans0 = solution.TopK_stl_nth_element(array, k);
    vector<int> ans1 = solution.TopK_sort(array, k);
    vector<int> ans2 = solution.TopK_heap(array, k);
    vector<int> ans3 = solution.TopK_partial_sort(array, k);

    print(array);
    cout << endl;

    sort(ans0.begin(), ans0.end());
    print(ans0);
    cout << endl;
    
    sort(ans1.begin(), ans1.end());
    print(ans1);
    cout << endl;
    
    sort(ans2.begin(), ans2.end());
    print(ans2);
    cout << endl;

    sort(ans3.begin(), ans3.end());
    print(ans3);

}