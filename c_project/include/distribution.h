/**
 * @file distribution.h
 * @brief Distribution ledger, tracking deliveries, cancellations, and rollback logic.
 */

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include "common.h"
#include "donation.h"
#include "request_queue.h"

typedef struct {
    char distribution_id[MAX_STR_LEN];
    char request_id[MAX_STR_LEN];
    char donation_id[MAX_STR_LEN];
    char recipient_id[MAX_STR_LEN];
    double allocated_quantity;
    FoodUnit unit;
    char allocation_date[MAX_DATE_LEN];
    char allocation_time[MAX_DATE_LEN];
    DistributionStatus status;
    bool is_cancelled;
    char cancellation_reason[MAX_NOTES_LEN];
} DistributionRecord;

typedef struct {
    DistributionRecord records[MAX_DISTRIBUTIONS];
    int count;
} DistributionLedger;

/* Operations */
void dist_init_ledger(DistributionLedger *ledger);
StatusCode dist_record_allocation(DistributionLedger *ledger, const DistributionRecord *rec);
DistributionRecord* dist_find_by_id(DistributionLedger *ledger, const char *dist_id);
StatusCode dist_cancel_and_rollback(
    DistributionLedger *ledger,
    DonationInventory *inv,
    RequestQueue *q,
    const char *dist_id,
    const char *reason
);
void dist_display_all(const DistributionLedger *ledger);
void dist_display_single(const DistributionRecord *r);
StatusCode dist_load_from_csv(DistributionLedger *ledger, const char *filepath);
StatusCode dist_save_to_csv(const DistributionLedger *ledger, const char *filepath);

#endif /* DISTRIBUTION_H */
