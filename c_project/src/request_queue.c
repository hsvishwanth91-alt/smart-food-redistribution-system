/**
 * @file request_queue.c
 * @brief Circular FIFO Queue DSA implementation for recipient food requests.
 *        Separates transient in-flight processing order from recipient entity registry.
 */

#include "../include/request_queue.h"

void req_queue_init(RequestQueue *q) {
    if (!q) return;
    q->front = 0;
    q->rear = -1;
    q->count = 0;
    q->capacity = MAX_REQUESTS;
    memset(q->items, 0, sizeof(q->items));
}

bool req_queue_is_empty(const RequestQueue *q) {
    return (q == NULL || q->count == 0);
}

bool req_queue_is_full(const RequestQueue *q) {
    return (q != NULL && q->count >= q->capacity);
}

StatusCode req_queue_enqueue(RequestQueue *q, const FoodRequest *req) {
    if (!q || !req) return ERR_INVALID_INPUT;
    if (req_queue_is_full(q)) {
        printf("[ERROR] Request Queue is FULL (Capacity: %d reached).\n", q->capacity);
        return ERR_FULL;
    }

    /* Check unique request ID across queue */
    if (req_queue_find_by_id(q, req->request_id) != NULL) {
        printf("[ERROR] Request with ID '%s' already pending in queue.\n", req->request_id);
        return ERR_DUPLICATE;
    }

    q->rear = (q->rear + 1) % q->capacity;
    q->items[q->rear] = *req;
    q->count++;
    return SUCCESS;
}

StatusCode req_queue_dequeue(RequestQueue *q, FoodRequest *out_req) {
    if (!q) return ERR_INVALID_INPUT;
    if (req_queue_is_empty(q)) {
        printf("[INFO] Request Queue is EMPTY. No pending requests.\n");
        return ERR_EMPTY;
    }

    if (out_req) {
        *out_req = q->items[q->front];
    }
    q->front = (q->front + 1) % q->capacity;
    q->count--;
    return SUCCESS;
}

FoodRequest* req_queue_peek(RequestQueue *q) {
    if (!q || req_queue_is_empty(q)) return NULL;
    return &q->items[q->front];
}

FoodRequest* req_queue_find_by_id(RequestQueue *q, const char *request_id) {
    if (!q || !request_id || req_queue_is_empty(q)) return NULL;
    int idx = q->front;
    for (int i = 0; i < q->count; i++) {
        if (strcmp(q->items[idx].request_id, request_id) == 0) {
            return &q->items[idx];
        }
        idx = (idx + 1) % q->capacity;
    }
    return NULL;
}

void req_queue_display(const RequestQueue *q) {
    if (!q) return;
    printf("\n=============================== RECIPIENT REQUEST FIFO QUEUE (%d In Queue) ===============================\n", q->count);
    if (req_queue_is_empty(q)) {
        printf("Queue is currently EMPTY. Use Menu Option to enqueue new recipient requests.\n");
        printf("----------------------------------------------------------------------------------------------------------\n");
        return;
    }

    printf("Queue Architecture: Circular Array | Front: Index %d | Rear: Index %d | Count: %d / %d\n",
           q->front, q->rear, q->count, q->capacity);
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("%-4s | %-10s | %-12s | %-16s | %-14s | %-10s | %-15s\n",
           "Pos", "Req ID", "Recip ID", "Category", "Quantity", "Priority", "Status");
    printf("----------------------------------------------------------------------------------------------------------\n");

    int idx = q->front;
    for (int pos = 1; pos <= q->count; pos++) {
        const FoodRequest *r = &q->items[idx];
        char qty_buf[32];
        snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", r->remaining_quantity, unit_to_string(r->unit));

        const char *prio_str = "STANDARD";
        if (r->priority == PRIORITY_EMERGENCY) prio_str = "EMERGENCY!";
        else if (r->priority == PRIORITY_HIGH) prio_str = "HIGH";
        else if (r->priority == PRIORITY_MEDIUM) prio_str = "MEDIUM";

        printf("#%-3d | %-10s | %-12s | %-16.16s | %-14s | %-10s | %-15s\n",
               pos, r->request_id, r->recipient_id, category_to_string(r->preferred_category),
               qty_buf, prio_str, request_status_to_string(r->status));
        idx = (idx + 1) % q->capacity;
    }
    printf("----------------------------------------------------------------------------------------------------------\n");
}

StatusCode req_queue_save_to_csv(const RequestQueue *q, const char *filepath) {
    if (!q || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    fprintf(fp, "request_id,recipient_id,category,requested_qty,fulfilled_qty,remaining_qty,unit,priority,req_date,required_by,status,notes\n");
    int idx = q->front;
    for (int i = 0; i < q->count; i++) {
        const FoodRequest *r = &q->items[idx];
        fprintf(fp, "%s,%s,%d,%.2f,%.2f,%.2f,%d,%d,%s,%s,%d,%s\n",
                r->request_id, r->recipient_id, (int)r->preferred_category,
                r->requested_quantity, r->fulfilled_quantity, r->remaining_quantity,
                (int)r->unit, (int)r->priority, r->request_date, r->required_by_date,
                (int)r->status, r->notes);
        idx = (idx + 1) % q->capacity;
    }
    fclose(fp);
    return SUCCESS;
}

StatusCode req_queue_load_from_csv(RequestQueue *q, const char *filepath) {
    if (!q || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "r");
    if (!fp) return ERR_NOT_FOUND;

    char line[1024];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return SUCCESS;
    }

    req_queue_init(q);

    while (fgets(line, sizeof(line), fp) && q->count < q->capacity) {
        trim_whitespace(line);
        if (strlen(line) == 0) continue;

        FoodRequest r = {0};
        char *token = strtok(line, ",");
        if (token) safe_strcpy(r.request_id, token, sizeof(r.request_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.recipient_id, token, sizeof(r.recipient_id));

        token = strtok(NULL, ",");
        if (token) r.preferred_category = (FoodCategory)atoi(token);

        token = strtok(NULL, ",");
        if (token) r.requested_quantity = atof(token);

        token = strtok(NULL, ",");
        if (token) r.fulfilled_quantity = atof(token);

        token = strtok(NULL, ",");
        if (token) r.remaining_quantity = atof(token);

        token = strtok(NULL, ",");
        if (token) r.unit = (FoodUnit)atoi(token);

        token = strtok(NULL, ",");
        if (token) r.priority = (RequestPriority)atoi(token);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.request_date, token, sizeof(r.request_date));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.required_by_date, token, sizeof(r.required_by_date));

        token = strtok(NULL, ",");
        if (token) r.status = (RequestStatus)atoi(token);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.notes, token, sizeof(r.notes));

        req_queue_enqueue(q, &r);
    }

    fclose(fp);
    return SUCCESS;
}
