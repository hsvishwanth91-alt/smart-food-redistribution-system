/**
 * @file demo.h
 * @brief Demonstration Mode and predefined evaluation test dataset loader.
 *        Populates deterministic test cases illustrating all DSA edge cases.
 */

#ifndef DEMO_H
#define DEMO_H

#include "donor.h"
#include "donation.h"
#include "recipient.h"
#include "request_queue.h"
#include "distribution.h"
#include "audit.h"
#include "priority_queue.h"

/* Operations */
StatusCode demo_load_dataset(
    DonorRegistry *donors,
    DonationInventory *inv,
    RecipientRegistry *recipients,
    RequestQueue *requests,
    DistributionLedger *ledger,
    AuditLog *audit,
    DonationPriorityQueue *pq
);

void demo_explain_scenario(void);

#endif /* DEMO_H */
