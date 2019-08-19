#include <iostream>
#include <vector>


using namespace std;

class UnionFindSet {
    vector<int> pre;

  public:
    int find(int i); 
    void join (int i, int j);
};

int UnionFindSet::find(int i) {
    while (pre[i] != i)
        i = pre[i];
    return i;
}

void UnionFindSet::join(int i, int j) {
    int fi = find(i);
    int fj = find(j);

    if (fi != fj)
        pre[fi] = fj;
}



int main() {


}