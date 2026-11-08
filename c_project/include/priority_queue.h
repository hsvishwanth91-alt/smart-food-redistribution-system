/**
 * @file priority_queue.h
 * @brief Binary Min-Heap Priority Queue for Food Donations (DSA Implementation).
 *        Root contains the donation with the highest urgency (earliest expiry / highest priority score).
 *        Supports O(log N) insert and extract-min, O(1) peek.
 */

#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "donation.h"

typedef struct {
    char donation_id[MAX_STR_LEN];
    double days_to_expiry;
    int urgency_rank; /* 0: expired, 1: critical, 2: near expiry, 3: normal */
    double available_quantity;
    FoodUnit unit;
    FoodCategory category;
} HeapNode;

typedef struct {
    HeapNode nodes[MAX_DONATIONS];
    int size;
    int capacity;
} DonationPriorityQueue;

/* Heap Operations */
void pq_init(DonationPriorityQueue *pq);
bool pq_is_empty(const DonationPriorityQueue *pq);
bool pq_is_full(const DonationPriorityQueue *pq);
StatusCode pq_insert(DonationPriorityQueue *pq, const HeapNode *node);
StatusCode pq_extract_min(DonationPriorityQueue *pq, HeapNode *out_node);
const HeapNode* pq_peek(const DonationPriorityQueue *pq);
void pq_rebuild_from_inventory(DonationPriorityQueue *pq, const DonationInventory *inv);
void pq_display(const DonationPriorityQueue *pq);
void pq_explain_complexity(void);

#endif /* PRIORITY_QUEUE_H */
