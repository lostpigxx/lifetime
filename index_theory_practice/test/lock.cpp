#include <iostream>
#include <mutex>

using namespace std;

int main() {
    mutex mu;
    mu.lock();
    cout << 0 << endl;
    mu.lock();
    cout << 1 << endl;
    mu.lock();
    cout << 2 << endl;
    mu.unlock();
    cout << 3 << endl;
    mu.unlock();
    cout << 4 << endl;
}