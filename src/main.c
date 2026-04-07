#include <stdio.h>

#include "hash_table.h"

int main() {
  ht_hash_table* ht = ht_new();

  ht_insert(ht, "name", "ankit");
  ht_insert(ht, "lang", "C");

  ht_insert(ht, "aa", "1");
  ht_insert(ht, "bb", "2");
  ht_insert(ht, "cc", "3");
  ht_insert(ht, "dd", "4");
  ht_insert(ht, "ee", "5");

  char* val = ht_search(ht, "name");
  if (val) {
    printf("%s\n", val);
  }

  ht_delete(ht, "name");

  ht_del_hash_table(ht);
  return 0;
}
