/**
 * @file allocation.h
 * @brief Smart Food Allocation Engine and Multi-Donation Greedy Matching Engine.
 *        Deterministic scoring formula, unit validation, expiry prioritization,
 *        and explainable recommendation breakdown.
 */

#ifndef ALLOCATION_H
#define ALLOCATION_H

#include "donation.h"
#include "request_queue.h"
#include "distribution.h"

#define MAX_MATCH_CANDIDATES 20

typedef struct {
    char donation_id[MAX_STR_LEN];
    double offered_quantity;
    double remaining_after;
    FoodUnit unit;
    double days_to_expiry;
    ExpiryClassification urgency;
    double match_score;
    char reason_explanation[MAX_NOTES_LEN];
} MatchCandidate;

typedef struct {
    char request_id[MAX_STR_LEN];
    char recipient_id[MAX_STR_LEN];
    FoodCategory category;
    FoodUnit unit;
    double requested_quantity;
    double total_allocatable_quantity;
    double shortage_quantity;
    bool is_fully_satisfied;
    int candidate_count;
    MatchCandidate candidates[MAX_MATCH_CANDIDATES];
    char policy_summary[MAX_NOTES_LEN];
} AllocationPlan;

/* Operations */
double calculate_allocation_score(const FoodDonation *d, const FoodRequest *r);
StatusCode generate_smart_allocation_plan(
    const DonationInventory *inv,
    const FoodRequest *req,
    AllocationPlan *out_plan
);
void display_allocation_plan(const AllocationPlan *plan);
StatusCode confirm_and_execute_allocation(
    DonationInventory *inv,
    RequestQueue *req_queue,
    DistributionLedger *ledger,
    AllocationPlan *plan,
    const char *operator_notes
);

#endif /* ALLOCATION_H */
