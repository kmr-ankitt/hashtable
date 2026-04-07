#define HT_PRIME_1 151
#define HT_PRIME_2 163
#define HT_INITIAL_BASE_SIZE 50

/**
 * ht_item
 * A hash table item which contains a key and a value.
 **/

typedef struct {
  char *key;
  char *value;
} ht_item;

/**
 * ht_hash_table
 * A hash table which stores the size of the hash table, the count of items in
 * the hash table and array of pointers to items.
 **/

typedef struct {
  int size;
  int count;
  int base_size;
  ht_item **items;
} ht_hash_table;

ht_hash_table *ht_new();
void ht_del_hash_table(ht_hash_table *ht);
void ht_insert(ht_hash_table *ht, const char *key, const char *value);
char *ht_search(ht_hash_table *ht, const char *key);
void ht_delete(ht_hash_table *ht, const char *key);
