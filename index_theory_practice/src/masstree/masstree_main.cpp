/**
 *    author:     UncP
 *    date:    2018-12-14
 *    license:    BSD-3
**/
#include <iostream>
#include <assert.h>

#include "masstree.h"

using namespace std;

/* a simple example as how to use mass tree */

int main()
{
  static const char *hello = "hello";

  mass_tree *mt = new_mass_tree();

  // since mass tree is just a toy, value is not stored in mass tree, use `null` as value,
  // extra infomartion about key is stored for debug purpose
  mass_tree_put(mt, (const void *)hello, 5, (void *)0);
  cout << (char*)mass_tree_get(mt, (const void *)hello, 5) << endl;

  // free_mass_tree(mt);
  return 0;
}