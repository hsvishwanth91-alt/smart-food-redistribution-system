/**
 * @file priority_queue.c
 * @brief Binary Min-Heap Priority Queue DSA implementation.
 *        Organizes donations by expiry urgency (least days to expiry at root).
 *        Time Complexities:
 *          - Insert: O(log N) via sift_up
 *          - Extract Min: O(log N) via sift_down
 *          - Peek: O(1)
 *          - Rebuild (Heapify): O(N)
 */

#include "../include/priority_queue.h"

static void swap_nodes(HeapNode *a, HeapNode *b) {
    HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

static bool is_higher_priority(const HeapNode *a, const HeapNode *b) {
    /* Lower days to expiry has higher priority (Min-Heap principle) */
    /* If days are equal, tie-break by urgency rank */
    if (a->days_to_expiry < b->days_to_expiry) return true;
    if (a->days_to_expiry > b->days_to_expiry) return false;
    return (a->urgency_rank < b->urgency_rank);
}

static void sift_up(DonationPriorityQueue *pq, int index) {
    while (index > 0) {
        int parent = (index - 1) / 2;
        if (is_higher_priority(&pq->nodes[index], &pq->nodes[parent])) {
            swap_nodes(&pq->nodes[index], &pq->nodes[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

static void sift_down(DonationPriorityQueue *pq, int index) {
    int smallest = index;
    int left = 2 * index + 1;
    int right = 2 * index + 2;

    if (left < pq->size && is_higher_priority(&pq->nodes[left], &pq->nodes[smallest])) {
        smallest = left;
    }
    if (right < pq->size && is_higher_priority(&pq->nodes[right], &pq->nodes[smallest])) {
        smallest = right;
    }

    if (smallest != index) {
        swap_nodes(&pq->nodes[index], &pq->nodes[smallest]);
        sift_down(pq, smallest);
    }
}

void pq_init(DonationPriorityQueue *pq) {
    if (!pq) return;
    pq->size = 0;
    pq->capacity = MAX_DONATIONS;
    memset(pq->nodes, 0, sizeof(pq->nodes));
}

bool pq_is_empty(const DonationPriorityQueue *pq) {
    return (pq == NULL || pq->size == 0);
}

bool pq_is_full(const DonationPriorityQueue *pq) {
    return (pq != NULL && pq->size >= pq->capacity);
}

StatusCode pq_insert(DonationPriorityQueue *pq, const HeapNode *node) {
    if (!pq || !node) return ERR_INVALID_INPUT;
    if (pq_is_full(pq)) return ERR_FULL;

    /* Expired donations cannot be inserted into dispatch priority queue */
    if (node->days_to_expiry < 0.0) {
        return ERR_EXPIRED;
    }

    int idx = pq->size;
    pq->nodes[idx] = *node;
    pq->size++;
    sift_up(pq, idx);
    return SUCCESS;
}

StatusCode pq_extract_min(DonationPriorityQueue *pq, HeapNode *out_node) {
    if (!pq || pq_is_empty(pq)) return ERR_EMPTY;

    if (out_node) {
        *out_node = pq->nodes[0];
    }

    pq->nodes[0] = pq->nodes[pq->size - 1];
    pq->size--;

    if (pq->size > 0) {
        sift_down(pq, 0);
    }
    return SUCCESS;
}

const HeapNode* pq_peek(const DonationPriorityQueue *pq) {
    if (!pq || pq_is_empty(pq)) return NULL;
    return &pq->nodes[0];
}

void pq_rebuild_from_inventory(DonationPriorityQueue *pq, const DonationInventory *inv) {
    if (!pq || !inv) return;
    pq_init(pq);

    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        /* Only consider active, unexpired, available stock */
        if (d->status == STATUS_AVAILABLE || d->status == STATUS_PARTIALLY_ALLOCATED) {
            double days = calculate_days_until_expiry(d->expiry_date);
            if (days >= 0.0 && d->available_quantity > 0.0) {
                HeapNode node = {0};
                safe_strcpy(node.donation_id, d->donation_id, sizeof(node.donation_id));
                node.days_to_expiry = days;
                node.urgency_rank = (int)d->urgency;
                node.available_quantity = d->available_quantity;
                node.unit = d->unit;
                node.category = d->category;
                pq_insert(pq, &node);
            }
        }
    }
}

void pq_display(const DonationPriorityQueue *pq) {
    if (!pq) return;
    printf("\n======================== BINARY MIN-HEAP PRIORITY QUEUE (%d In Heap) ========================\n", pq->size);
    if (pq_is_empty(pq)) {
        printf("Priority Queue is EMPTY. No eligible unallocated donations found.\n");
        printf("---------------------------------------------------------------------------------------------\n");
        return;
    }

    printf("Heap Root (Highest Urgency): ID: %s | Days to Expiry: %.1f | Qty: %.1f %s\n",
           pq->nodes[0].donation_id, pq->nodes[0].days_to_expiry,
           pq->nodes[0].available_quantity, unit_to_string(pq->nodes[0].unit));
    printf("---------------------------------------------------------------------------------------------\n");
    printf("%-5s | %-12s | %-18s | %-14s | %-14s | %-12s\n",
           "Index", "Donation ID", "Category", "Quantity", "Days Left", "Urgency Rank");
    printf("---------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < pq->size; i++) {
        const HeapNode *n = &pq->nodes[i];
        char qty_buf[32];
        snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", n->available_quantity, unit_to_string(n->unit));
        printf("#%-4d | %-12s | %-18.18s | %-14s | %-14.1f | %-12s\n",
               i, n->donation_id, category_to_string(n->category),
               qty_buf, n->days_to_expiry,
               n->urgency_rank == 1 ? "CRITICAL" : (n->urgency_rank == 2 ? "NEAR EXPIRY" : "NORMAL"));
    }
    printf("---------------------------------------------------------------------------------------------\n");
}

void pq_explain_complexity(void) {
    printf("\n=== BINARY HEAP DATA STRUCTURE COMPLEXITY ANALYSIS ===\n");
    printf("1. Structure: Complete Binary Tree represented sequentially in flat contiguous array.\n");
    printf("2. Parent/Child Index Mapping: Parent = (i - 1)/2 | Left = 2i + 1 | Right = 2i + 2\n");
    printf("3. Insert Operation: Adds element at index N, then calls sift_up -> Time Complexity O(log N)\n");
    printf("4. Extract-Min: Moves root to output, replaces with last leaf, calls sift_down -> Time Complexity O(log N)\n");
    printf("5. Peek Root: Direct array access at index 0 -> Time Complexity O(1)\n");
    printf("6. Space Complexity: O(N) auxiliary space for heap nodes array.\n");
    printf("======================================================\n");
}
