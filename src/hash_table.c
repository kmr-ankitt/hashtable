#include "hash_table.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "prime.h"

static ht_item HT_DELETED_ITEM = {NULL, NULL};

/**
 * ht_new_item - creates a new hash table item
 **/

static ht_item *ht_new_item(const char *key, const char *value) {
  ht_item *item = malloc(sizeof(ht_item));
  item->key = strdup(key);
  item->value = strdup(value);

  return item;
}

static ht_hash_table *ht_new_sized(const int base_size) {
  ht_hash_table *ht = malloc(sizeof(ht_hash_table));
  ht->base_size = base_size;

  ht->size = next_prime(ht->base_size);

  ht->count = 0;
  ht->items = calloc((size_t)ht->size, sizeof(ht_item *));
  return ht;
}

static void ht_resize(ht_hash_table *ht, const int base_size) {
  if (base_size < HT_INITIAL_BASE_SIZE) {
    return;
  }
  ht_hash_table *new_ht = ht_new_sized(base_size);
  for (int i = 0; i < ht->size; i++) {
    ht_item *item = ht->items[i];
    if (item != NULL && item != &HT_DELETED_ITEM) {
      ht_insert(new_ht, item->key, item->value);
    }
  }

  ht->base_size = new_ht->base_size;
  ht->count = new_ht->count;

  // To delete new_ht, we give it ht's size and items
  const int tmp_size = ht->size;
  ht->size = new_ht->size;
  new_ht->size = tmp_size;

  ht_item **tmp_items = ht->items;
  ht->items = new_ht->items;
  new_ht->items = tmp_items;

  ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table *ht) {
  const int new_size = ht->base_size * 2;
  ht_resize(ht, new_size);
}

static void ht_resize_down(ht_hash_table *ht) {
  const int new_size = ht->base_size / 2;
  ht_resize(ht, new_size);
}

/**
 * ht_new - creates a new hash table
 **/

ht_hash_table *ht_new() { return ht_new_sized(HT_INITIAL_BASE_SIZE); }

static void ht_del_item(ht_item *item) {
  free(item->key);
  free(item->value);
  free(item);
}

void ht_del_hash_table(ht_hash_table *ht) {
  for (int i = 0; i < ht->size; i++) {
    ht_item *item = ht->items[i];
    if (item != NULL && item != &HT_DELETED_ITEM) ht_del_item(item);
  }

  free(ht->items);
  free(ht);
}

/**
 * ht_hash - calculates a hash for a given string
 *
 * It uses the polynomial rolling hash algorithm, which is a common and
 *efficient way to hash strings. The choice of a prime number for 'a' helps to
 *reduce collisions, and the modulus operator ensures that the hash value fits
 *within the number of buckets in the hash table.
 **/
int ht_hash(const char *s, const int a, const int buckets) {
  long hash = 0;
  const int len_s = strlen(s);

  for (int i = 0; i < len_s; i++) {
    hash += (long)pow(a, len_s - (i + 1)) * s[i];
    hash = hash % buckets;
  }

  return (int)hash;
}

/**
 * ht_get_hash - calculates the hash for given string
 *
 * Uses double hashing to calculate the hash value for a given string to handle
 *collisions in the hash table. It combines two different hash functions
 *(ht_hash with different prime numbers) to generate a new hash value based on
 *the number of attempts (collisions) that have occurred. This approach helps to
 *reduce the likelihood of collisions and ensures a more even distribution of
 *hash values across the buckets in the hash table.
 **/
int ht_get_hash(const char *s, const int num_buckets, const int attempt) {
  const int hash_a = ht_hash(s, HT_PRIME_1, num_buckets);
  const int hash_b = ht_hash(s, HT_PRIME_2, num_buckets);

  return (hash_a + (attempt * (hash_b + 1))) % num_buckets;
}

/**
 * ht_insert - inserts a key-value pair into the hash table
 *
 * - creates a new item with the given key and value
 * - calculates the index for that item
 * - if there is a collision, it will use double hashing
 * - else insert the item into the hash table
 *
 * (if the key already exists, it will delete the old item and insert the new)
 **/

void ht_insert(ht_hash_table *ht, const char *key, const char *value) {
  const int load = ht->count * 100 / ht->size;
  if (load > 70) {
    ht_resize_up(ht);
  }

  ht_item *item = ht_new_item(key, value);
  int index = ht_get_hash(item->key, ht->size, 0);
  ht_item *cur_item = ht->items[index];
  int i = 1;

  while (cur_item != NULL) {
    if (cur_item != &HT_DELETED_ITEM) {
      if (strcmp(cur_item->key, key) == 0) {
        ht_del_item(cur_item);
        ht->items[index] = item;
        return;
      }
    }
    index = ht_get_hash(item->key, ht->size, i);
    cur_item = ht->items[index];
    i++;
  }

  ht->items[index] = item;
  ht->count++;
}

/**
 * ht_search - searches for a key in the hash table and returns its value
 *
 * - calculates the index for the given key
 * - check if it is the item we are looking for, if so return value
 * - else use double hashing to find the next index
 **/

char *ht_search(ht_hash_table *ht, const char *key) {
  int index = ht_get_hash(key, ht->size, 0);
  ht_item *item = ht->items[index];
  int i = 1;

  while (item != NULL) {
    if (item != &HT_DELETED_ITEM) {
      if (strcmp(item->key, key) == 0) return item->value;
    }

    index = ht_get_hash(key, ht->size, i);
    item = ht->items[index];
    i++;
  }

  return NULL;
}

/**
 * ht_delete - deletes a key from the hash table
 *
 * (we don't actually delete the item, we just mark it as deleted item)
 * - calculates the index for the given key
 * - check if it is the item we are looking for, if so mark it as deleted
 * - else use double hashing to find the next index
 **/
void ht_delete(ht_hash_table *ht, const char *key) {
  const int load = ht->count * 100 / ht->size;
  if (load < 10) {
    ht_resize_down(ht);
  }

  int index = ht_get_hash(key, ht->size, 0);
  ht_item *item = ht->items[index];
  int i = 1;

  while (item != NULL) {
    if (item != &HT_DELETED_ITEM) {
      if (strcmp(item->key, key) == 0) {
        ht_del_item(item);
        ht->items[index] = &HT_DELETED_ITEM;
        ht->count--;
        return;
      }
    }

    index = ht_get_hash(key, ht->size, i);
    item = ht->items[index];
    i++;
  }
}
