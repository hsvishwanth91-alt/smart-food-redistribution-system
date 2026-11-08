/**
 * @file hash_table.c
 * @brief Hash Table implementation with djb2 hash function and separate chaining.
 *        Enables O(1) average time complexity lookup for donors, donations, and recipients.
 */

#include "../include/hash_table.h"

void ht_init(HashTable *ht) {
    if (!ht) return;
    ht->total_entries = 0;
    for (int i = 0; i < HASH_TABLE_BUCKETS; i++) {
        ht->buckets[i] = NULL;
    }
}

/* Classic djb2 hash algorithm by Dan Bernstein */
unsigned long ht_hash(const char *key) {
    if (!key) return 0;
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*key++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash % HASH_TABLE_BUCKETS;
}

StatusCode ht_insert(HashTable *ht, const char *key, RecordType type, int index_in_array) {
    if (!ht || !key) return ERR_INVALID_INPUT;

    unsigned long bucket_idx = ht_hash(key);
    HashNode *current = ht->buckets[bucket_idx];

    /* Check if key already exists in bucket chain */
    while (current != NULL) {
        if (strcmp(current->key_id, key) == 0 && current->type == type) {
            current->index_in_array = index_in_array; /* update */
            return SUCCESS;
        }
        current = current->next;
    }

    /* Allocate new node */
    HashNode *new_node = (HashNode*)malloc(sizeof(HashNode));
    if (!new_node) return ERR_FULL;

    safe_strcpy(new_node->key_id, key, sizeof(new_node->key_id));
    new_node->type = type;
    new_node->index_in_array = index_in_array;
    new_node->next = ht->buckets[bucket_idx];
    ht->buckets[bucket_idx] = new_node;
    ht->total_entries++;
    return SUCCESS;
}

int ht_search(const HashTable *ht, const char *key, RecordType expected_type) {
    if (!ht || !key) return -1;
    unsigned long bucket_idx = ht_hash(key);
    HashNode *current = ht->buckets[bucket_idx];

    while (current != NULL) {
        if (strcmp(current->key_id, key) == 0 && current->type == expected_type) {
            return current->index_in_array;
        }
        current = current->next;
    }
    return -1;
}

StatusCode ht_remove(HashTable *ht, const char *key) {
    if (!ht || !key) return ERR_INVALID_INPUT;
    unsigned long bucket_idx = ht_hash(key);
    HashNode *current = ht->buckets[bucket_idx];
    HashNode *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->key_id, key) == 0) {
            if (prev == NULL) {
                ht->buckets[bucket_idx] = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            ht->total_entries--;
            return SUCCESS;
        }
        prev = current;
        current = current->next;
    }
    return ERR_NOT_FOUND;
}

void ht_clear(HashTable *ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_TABLE_BUCKETS; i++) {
        HashNode *current = ht->buckets[i];
        while (current != NULL) {
            HashNode *next = current->next;
            free(current);
            current = next;
        }
        ht->buckets[i] = NULL;
    }
    ht->total_entries = 0;
}

void ht_display_stats(const HashTable *ht) {
    if (!ht) return;
    int used_buckets = 0;
    int max_chain_length = 0;

    for (int i = 0; i < HASH_TABLE_BUCKETS; i++) {
        if (ht->buckets[i] != NULL) {
            used_buckets++;
            int chain_len = 0;
            HashNode *cur = ht->buckets[i];
            while (cur) {
                chain_len++;
                cur = cur->next;
            }
            if (chain_len > max_chain_length) {
                max_chain_length = chain_len;
            }
        }
    }

    double load_factor = (double)ht->total_entries / HASH_TABLE_BUCKETS;
    printf("\n=== HASH TABLE DIAGNOSTICS ===\n");
    printf("Total Buckets:     %d\n", HASH_TABLE_BUCKETS);
    printf("Total Entries:     %d\n", ht->total_entries);
    printf("Occupied Buckets:  %d (%.1f%%)\n", used_buckets, (double)used_buckets * 100.0 / HASH_TABLE_BUCKETS);
    printf("Max Chain Length:  %d\n", max_chain_length);
    printf("Load Factor (α):   %.2f\n", load_factor);
    printf("Search Complexity: O(1 + α) average time\n");
    printf("===============================\n");
}
