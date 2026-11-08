/**
 * @file request_queue.h
 * @brief Circular Recipient Request Queue (FIFO DSA Implementation).
 *        Maintains separate processing queue from persistent database.
 */

#ifndef REQUEST_QUEUE_H
#define REQUEST_QUEUE_H

#include "common.h"

typedef struct {
    char request_id[MAX_STR_LEN];
    char recipient_id[MAX_STR_LEN];
    FoodCategory preferred_category;
    double requested_quantity;
    double fulfilled_quantity;
    double remaining_quantity;
    FoodUnit unit;
    RequestPriority priority;
    char request_date[MAX_DATE_LEN];
    char required_by_date[MAX_DATE_LEN];
    RequestStatus status;
    char notes[MAX_NOTES_LEN];
} FoodRequest;

typedef struct {
    FoodRequest items[MAX_REQUESTS];
    int front;
    int rear;
    int count;
    int capacity;
} RequestQueue;

/* Queue Operations */
void req_queue_init(RequestQueue *q);
bool req_queue_is_empty(const RequestQueue *q);
bool req_queue_is_full(const RequestQueue *q);
StatusCode req_queue_enqueue(RequestQueue *q, const FoodRequest *req);
StatusCode req_queue_dequeue(RequestQueue *q, FoodRequest *out_req);
FoodRequest* req_queue_peek(RequestQueue *q);
FoodRequest* req_queue_find_by_id(RequestQueue *q, const char *request_id);
void req_queue_display(const RequestQueue *q);
StatusCode req_queue_load_from_csv(RequestQueue *q, const char *filepath);
StatusCode req_queue_save_to_csv(const RequestQueue *q, const char *filepath);

#endif /* REQUEST_QUEUE_H */
