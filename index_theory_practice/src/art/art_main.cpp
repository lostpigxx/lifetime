#include <iostream>

#include "art.h"

using namespace std;

int main() {

    art_tree* art = new art_tree;

    art_tree_init(art);

    int* val = new int(10);

    string key("123456");
    string key1("123453");


    art_insert(art, reinterpret_cast<const unsigned char*>(key.c_str()), 6, val);


    cout << art_search(art, reinterpret_cast<const unsigned char*>(key1.c_str()), 6) << endl;
}