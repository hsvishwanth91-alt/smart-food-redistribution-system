/**
 * @file hash_table.h
 * @brief Hash Table for O(1) average time record lookup by ID (DSA Implementation).
 *        Uses djb2 hash algorithm with separate chaining for collision resolution.
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "common.h"

#define HASH_TABLE_BUCKETS 127

typedef enum {
    RECORD_DONOR,
    RECORD_DONATION,
    RECORD_RECIPIENT
} RecordType;

typedef struct HashNode {
    char key_id[MAX_STR_LEN];
    RecordType type;
    int index_in_array; /* Pointer to index in respective registry array */
    struct HashNode *next;
} HashNode;

typedef struct {
    HashNode *buckets[HASH_TABLE_BUCKETS];
    int total_entries;
} HashTable;

/* Hash Table Operations */
void ht_init(HashTable *ht);
unsigned long ht_hash(const char *key);
StatusCode ht_insert(HashTable *ht, const char *key, RecordType type, int index_in_array);
int ht_search(const HashTable *ht, const char *key, RecordType expected_type);
StatusCode ht_remove(HashTable *ht, const char *key);
void ht_clear(HashTable *ht);
void ht_display_stats(const HashTable *ht);

#endif /* HASH_TABLE_H */
