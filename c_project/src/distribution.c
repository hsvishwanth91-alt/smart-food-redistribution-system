/**
 * @file distribution.c
 * @brief Distribution ledger management and allocation rollback / cancellation engine.
 *        Guarantees transactional consistency across donations and requests.
 */

#include "../include/distribution.h"
#include "../include/audit.h"

extern AuditLog g_audit_log;

void dist_init_ledger(DistributionLedger *ledger) {
    if (!ledger) return;
    ledger->count = 0;
    memset(ledger->records, 0, sizeof(ledger->records));
}

StatusCode dist_record_allocation(DistributionLedger *ledger, const DistributionRecord *rec) {
    if (!ledger || !rec) return ERR_INVALID_INPUT;
    if (ledger->count >= MAX_DISTRIBUTIONS) return ERR_FULL;

    ledger->records[ledger->count] = *rec;
    ledger->count++;
    return SUCCESS;
}

DistributionRecord* dist_find_by_id(DistributionLedger *ledger, const char *dist_id) {
    if (!ledger || !dist_id) return NULL;
    for (int i = 0; i < ledger->count; i++) {
        if (strcmp(ledger->records[i].distribution_id, dist_id) == 0) {
            return &ledger->records[i];
        }
    }
    return NULL;
}

StatusCode dist_cancel_and_rollback(
    DistributionLedger *ledger,
    DonationInventory *inv,
    RequestQueue *q,
    const char *dist_id,
    const char *reason
) {
    if (!ledger || !inv || !q || !dist_id) return ERR_INVALID_INPUT;

    DistributionRecord *rec = dist_find_by_id(ledger, dist_id);
    if (!rec) {
        printf("[ERROR] Distribution record '%s' not found.\n", dist_id);
        return ERR_NOT_FOUND;
    }

    if (rec->is_cancelled) {
        printf("[ERROR] Distribution '%s' has ALREADY been cancelled previously. Rollback aborted.\n", dist_id);
        return ERR_DUPLICATE;
    }

    if (rec->status == DIST_DELIVERED) {
        printf("[ERROR] Distribution '%s' is marked DELIVERED to recipient. Cannot perform standard automated rollback without physical recovery confirmation.\n", dist_id);
        return ERR_INVALID_INPUT;
    }

    /* Locate associated food donation */
    FoodDonation *don = donation_find_by_id(inv, rec->donation_id);
    if (!don) {
        printf("[ERROR] Associated donation '%s' not found in inventory.\n", rec->donation_id);
        return ERR_NOT_FOUND;
    }

    /* Locate associated food request */
    FoodRequest *req = req_queue_find_by_id(q, rec->request_id);

    /* 1. Restore quantity to donation */
    don->available_quantity += rec->allocated_quantity;

    /* Check if donation expired while allocated */
    double days = calculate_days_until_expiry(don->expiry_date);
    if (days < 0.0) {
        don->status = STATUS_EXPIRED;
        don->urgency = EXPIRY_EXPIRED;
        printf("[WARNING] Restored stock (%.1f %s) to donation '%s', but this batch has EXPIRED in the interim. Status set to EXPIRED.\n",
               rec->allocated_quantity, unit_to_string(rec->unit), don->donation_id);
    } else {
        if (don->available_quantity >= don->initial_quantity - 0.001) {
            don->status = STATUS_AVAILABLE;
        } else {
            don->status = STATUS_PARTIALLY_ALLOCATED;
        }
    }

    /* 2. Restore request balance */
    if (req) {
        req->fulfilled_quantity -= rec->allocated_quantity;
        if (req->fulfilled_quantity < 0.0) req->fulfilled_quantity = 0.0;
        req->remaining_quantity += rec->allocated_quantity;
        if (req->fulfilled_quantity == 0.0) {
            req->status = REQ_PENDING;
        } else {
            req->status = REQ_PARTIALLY_FULFILLED;
        }
    }

    /* 3. Mark distribution cancelled */
    rec->is_cancelled = true;
    rec->status = DIST_CANCELLED;
    safe_strcpy(rec->cancellation_reason, reason ? reason : "Operator Rollback", sizeof(rec->cancellation_reason));

    /* 4. Log to Audit Trail */
    char audit_desc[MAX_NOTES_LEN];
    snprintf(audit_desc, sizeof(audit_desc),
             "Rollback Distribution %s: Restored %.1f %s to %s and Request %s. Reason: %s",
             dist_id, rec->allocated_quantity, unit_to_string(rec->unit),
             rec->donation_id, rec->request_id, rec->cancellation_reason);
    audit_record(&g_audit_log, OP_ALLOCATION_CANCEL, dist_id, audit_desc, "Operator");

    printf("[SUCCESS] Distribution '%s' cancelled and stock rollback executed successfully!\n", dist_id);
    return SUCCESS;
}

void dist_display_single(const DistributionRecord *r) {
    if (!r) return;
    printf("+-------------------------------------------------------------------------+\n");
    printf("| Distribution ID: %-53s |\n", r->distribution_id);
    printf("| Request ID:      %-53s |\n", r->request_id);
    printf("| Donation ID:     %-53s |\n", r->donation_id);
    printf("| Recipient ID:    %-53s |\n", r->recipient_id);
    printf("| Quantity:        %8.2f %-42s |\n", r->allocated_quantity, unit_to_string(r->unit));
    printf("| Timestamp:       %s %-42s |\n", r->allocation_date, r->allocation_time);
    printf("| Status:          %-20s | Cancelled: %-19s |\n",
           distribution_status_to_string(r->status), r->is_cancelled ? "YES" : "NO");
    if (r->is_cancelled) {
        printf("| Cancellation Reason: %-50s |\n", r->cancellation_reason);
    }
    printf("+-------------------------------------------------------------------------+\n");
}

void dist_display_all(const DistributionLedger *ledger) {
    if (!ledger) return;
    printf("\n================================== DISTRIBUTION LEDGER (%d Records) ==================================\n", ledger->count);
    printf("%-12s | %-10s | %-10s | %-12s | %-12s | %-11s | %-12s\n",
           "Dist ID", "Req ID", "Donation", "Recipient", "Quantity", "Date", "Status");
    printf("------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < ledger->count; i++) {
        const DistributionRecord *r = &ledger->records[i];
        char qty_buf[32];
        snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", r->allocated_quantity, unit_to_string(r->unit));
        printf("%-12s | %-10s | %-10s | %-12s | %-12s | %-11s | %-12s\n",
               r->distribution_id, r->request_id, r->donation_id, r->recipient_id,
               qty_buf, r->allocation_date,
               r->is_cancelled ? "CANCELLED" : distribution_status_to_string(r->status));
    }
    printf("------------------------------------------------------------------------------------------------------\n");
}

StatusCode dist_save_to_csv(const DistributionLedger *ledger, const char *filepath) {
    if (!ledger || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    fprintf(fp, "dist_id,req_id,donation_id,recipient_id,qty,unit,date,time,status,is_cancelled,reason\n");
    for (int i = 0; i < ledger->count; i++) {
        const DistributionRecord *r = &ledger->records[i];
        fprintf(fp, "%s,%s,%s,%s,%.2f,%d,%s,%s,%d,%d,%s\n",
                r->distribution_id, r->request_id, r->donation_id, r->recipient_id,
                r->allocated_quantity, (int)r->unit, r->allocation_date, r->allocation_time,
                (int)r->status, r->is_cancelled ? 1 : 0, r->cancellation_reason);
    }
    fclose(fp);
    return SUCCESS;
}

StatusCode dist_load_from_csv(DistributionLedger *ledger, const char *filepath) {
    if (!ledger || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "r");
    if (!fp) return ERR_NOT_FOUND;

    char line[1024];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return SUCCESS;
    }

    while (fgets(line, sizeof(line), fp) && ledger->count < MAX_DISTRIBUTIONS) {
        trim_whitespace(line);
        if (strlen(line) == 0) continue;

        DistributionRecord r = {0};
        char *token = strtok(line, ",");
        if (token) safe_strcpy(r.distribution_id, token, sizeof(r.distribution_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.request_id, token, sizeof(r.request_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.donation_id, token, sizeof(r.donation_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.recipient_id, token, sizeof(r.recipient_id));

        token = strtok(NULL, ",");
        if (token) r.allocated_quantity = atof(token);

        token = strtok(NULL, ",");
        if (token) r.unit = (FoodUnit)atoi(token);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.allocation_date, token, sizeof(r.allocation_date));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.allocation_time, token, sizeof(r.allocation_time));

        token = strtok(NULL, ",");
        if (token) r.status = (DistributionStatus)atoi(token);

        token = strtok(NULL, ",");
        if (token) r.is_cancelled = (atoi(token) != 0);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.cancellation_reason, token, sizeof(r.cancellation_reason));

        ledger->records[ledger->count++] = r;
    }

    fclose(fp);
    return SUCCESS;
}
