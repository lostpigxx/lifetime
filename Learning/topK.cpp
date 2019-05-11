#include <iostream>

#include <vector>

using namespace std;

void print(vector<int> ans) {
    for (auto val : ans) {
        cout << val << " ";
    }
    cout << endl;
}


int partition(int l, int r, vector<int>& array) {
    int pivot = array[l];
    int i = l + 1, j = r;
    while (true) {
        while (array[i] < pivot && i <= r) i++;
        while (array[j] > pivot && j >= l) j--;
        if (i < j)
            swap(array[i++], array[j++]);
        else 
            break;
    }
    return j;
}

void topK(int l, int r, int K, vector<int>& array) {
    print(array);

    int mid = partition(l, r, array);
    int left_length = mid - l + 1;
    if (left_length < K)
        topK(mid+1, r, K - left_length, array);
    else if (left_length > K)
        topK(l, mid, K, array);
}


vector<int> TopK_RandomSelect(vector<int>& array, int K) {
    vector<int> ans;
    
    topK(0, array.size(), K, array);
    
    for (int i = 0; i < K; i++)
        ans.push_back(array[i]);
    return ans; 

}

int main() {
    vector<int> array = {4, 21, 56, 14, 90, 24, 18, 1};

    print(array);
    vector<int> ans = TopK_RandomSelect(array, 3);
    

}