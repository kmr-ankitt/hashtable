#include "hash_table.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/**
 * ht_new_item - creates a new hash table item
 * @key: the key of the item
 * @value: the value of the item
 **/

static ht_item *ht_new_item(const char *key, const char *value) {
  ht_item *item = malloc(sizeof(ht_item));
  item->key = strdup(key);
  item->value = strdup(value);

  return item;
}

/**
 * ht_new - creates a new hash table
 * initialize size 53, count 0, items to NULL
 **/

ht_hash_table *ht_new() {
  ht_hash_table *ht = malloc(sizeof(ht_hash_table));

  // TODO: size is currently fixed, but we should implement resizing
  ht->size = 53;
  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item *));

  return ht;
}
