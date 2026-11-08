/**
 * @file allocation.c
 * @brief Smart Food Allocation Engine and Multi-Donation Greedy Matching Engine.
 *        Implements deterministic, fully explainable allocation policies,
 *        strict unit validation, expiry prioritization, and operator review.
 */

#include "../include/allocation.h"
#include "../include/audit.h"

extern AuditLog g_audit_log; /* Global reference for auditing operations */

double calculate_allocation_score(const FoodDonation *d, const FoodRequest *r) {
    if (!d || !r) return -100.0;

    /* Rule 1: Category MUST match */
    if (d->category != r->preferred_category) return -1000.0;

    /* Rule 2: Food unit MUST match */
    if (d->unit != r->unit) return -1000.0;

    /* Rule 3: Must be available and unexpired */
    if (d->status != STATUS_AVAILABLE && d->status != STATUS_PARTIALLY_ALLOCATED) return -1000.0;
    if (d->available_quantity <= 0.0) return -1000.0;

    double days = calculate_days_until_expiry(d->expiry_date);
    if (days < 0.0) return -1000.0; /* Expired */

    /*
     * Deterministic Ranking Formula:
     *   Base Score = 50.0
     *   + Expiry Urgency Score: (Earlier expiry = Higher waste-prevention priority)
     *       <= 1 day (Critical): +60.0
     *       <= 3 days (Near Expiry): +40.0
     *       <= 7 days: +20.0
     *       > 7 days: +5.0
     *   + Recipient Priority Multiplier: (Priority 1..4) * 10.0
     *   + Stock Efficiency: ratio of (available / requested) capped at 10.0
     */
    double score = 50.0;

    if (days <= 1.0) {
        score += 60.0;
    } else if (days <= 3.0) {
        score += 40.0;
    } else if (days <= 7.0) {
        score += 20.0;
    } else {
        score += 5.0;
    }

    score += (double)r->priority * 10.0;

    if (d->available_quantity >= r->remaining_quantity) {
        score += 15.0; /* Exact single-source satisfaction bonus */
    } else {
        score += 5.0;
    }

    return score;
}

StatusCode generate_smart_allocation_plan(
    const DonationInventory *inv,
    const FoodRequest *req,
    AllocationPlan *out_plan
) {
    if (!inv || !req || !out_plan) return ERR_INVALID_INPUT;

    memset(out_plan, 0, sizeof(AllocationPlan));
    safe_strcpy(out_plan->request_id, req->request_id, sizeof(out_plan->request_id));
    safe_strcpy(out_plan->recipient_id, req->recipient_id, sizeof(out_plan->recipient_id));
    out_plan->category = req->preferred_category;
    out_plan->unit = req->unit;
    out_plan->requested_quantity = req->remaining_quantity;
    out_plan->total_allocatable_quantity = 0.0;
    out_plan->candidate_count = 0;

    /* Temporary pool to collect and sort eligible donations */
    typedef struct {
        const FoodDonation *donation;
        double score;
    } ScoredDonation;

    ScoredDonation eligible[MAX_DONATIONS];
    int eligible_count = 0;

    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        double score = calculate_allocation_score(d, req);
        if (score > 0.0) {
            eligible[eligible_count].donation = d;
            eligible[eligible_count].score = score;
            eligible_count++;
        }
    }

    if (eligible_count == 0) {
        snprintf(out_plan->policy_summary, sizeof(out_plan->policy_summary),
                 "No compatible available stock found for Category '%s' with unit '%s'.",
                 category_to_string(req->preferred_category), unit_to_string(req->unit));
        out_plan->shortage_quantity = req->remaining_quantity;
        out_plan->is_fully_satisfied = false;
        return ERR_NOT_FOUND;
    }

    /* Sort eligible donations by score descending (Insertion Sort) */
    for (int i = 1; i < eligible_count; i++) {
        ScoredDonation key = eligible[i];
        int j = i - 1;
        while (j >= 0 && eligible[j].score < key.score) {
            eligible[j + 1] = eligible[j];
            j--;
        }
        eligible[j + 1] = key;
    }

    /* Greedy Multi-Donation Selection */
    double remaining_needed = req->remaining_quantity;

    for (int i = 0; i < eligible_count && remaining_needed > 0.001; i++) {
        if (out_plan->candidate_count >= MAX_MATCH_CANDIDATES) break;

        const FoodDonation *d = eligible[i].donation;
        double take_qty = (d->available_quantity >= remaining_needed) ? remaining_needed : d->available_quantity;

        MatchCandidate *cand = &out_plan->candidates[out_plan->candidate_count++];
        safe_strcpy(cand->donation_id, d->donation_id, sizeof(cand->donation_id));
        cand->offered_quantity = take_qty;
        cand->remaining_after = d->available_quantity - take_qty;
        cand->unit = d->unit;
        cand->days_to_expiry = calculate_days_until_expiry(d->expiry_date);
        cand->urgency = d->urgency;
        cand->match_score = eligible[i].score;

        snprintf(cand->reason_explanation, sizeof(cand->reason_explanation),
                 "Score: %.1f | Urgency: %s (%.1fd left) | %s stock",
                 cand->match_score, expiry_class_to_string(cand->urgency),
                 cand->days_to_expiry, (take_qty == d->available_quantity) ? "Consumes full" : "Partial allocation");

        out_plan->total_allocatable_quantity += take_qty;
        remaining_needed -= take_qty;
    }

    out_plan->shortage_quantity = remaining_needed;
    out_plan->is_fully_satisfied = (remaining_needed <= 0.001);

    if (out_plan->is_fully_satisfied) {
        if (out_plan->candidate_count == 1) {
            snprintf(out_plan->policy_summary, sizeof(out_plan->policy_summary),
                     "Request 100%% satisfied from 1 high-priority donation.");
        } else {
            snprintf(out_plan->policy_summary, sizeof(out_plan->policy_summary),
                     "Multi-Donation match: Request 100%% satisfied by combining %d donations.",
                     out_plan->candidate_count);
        }
    } else {
        snprintf(out_plan->policy_summary, sizeof(out_plan->policy_summary),
                 "Partial fulfillment: Stock satisfies %.1f %s, leaving a shortage of %.1f %s.",
                 out_plan->total_allocatable_quantity, unit_to_string(out_plan->unit),
                 out_plan->shortage_quantity, unit_to_string(out_plan->unit));
    }

    return SUCCESS;
}

void display_allocation_plan(const AllocationPlan *plan) {
    if (!plan) return;
    printf("\n================================== SMART ALLOCATION PROPOSAL ==================================\n");
    printf("Request ID:       %-20s | Recipient ID: %-20s\n", plan->request_id, plan->recipient_id);
    printf("Category:         %-20s | Food Unit:    %-20s\n", category_to_string(plan->category), unit_to_string(plan->unit));
    printf("Requested Qty:    %8.2f %-11s | Total Match:  %8.2f %-11s\n",
           plan->requested_quantity, unit_to_string(plan->unit),
           plan->total_allocatable_quantity, unit_to_string(plan->unit));
    printf("Shortage Qty:     %8.2f %-11s | Satisfied:    %-20s\n",
           plan->shortage_quantity, unit_to_string(plan->unit),
           plan->is_fully_satisfied ? "YES (Complete)" : "PARTIAL / SHORTAGE");
    printf("Engine Verdict:   %s\n", plan->policy_summary);
    printf("------------------------------------------------------------------------------------------------\n");
    printf("%-3s | %-12s | %-12s | %-12s | %-8s | %-38s\n",
           "#", "Donation ID", "Alloc Qty", "Rem. Stock", "Days Left", "Deterministic Recommendation Reason");
    printf("------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < plan->candidate_count; i++) {
        const MatchCandidate *c = &plan->candidates[i];
        printf("%-3d | %-12s | %8.2f %-3s | %8.2f %-3s | %-8.1f | %-38s\n",
               i + 1, c->donation_id, c->offered_quantity, unit_to_string(c->unit),
               c->remaining_after, unit_to_string(c->unit),
               c->days_to_expiry, c->reason_explanation);
    }
    printf("================================================================================================\n");
}

StatusCode confirm_and_execute_allocation(
    DonationInventory *inv,
    RequestQueue *req_queue,
    DistributionLedger *ledger,
    AllocationPlan *plan,
    const char *operator_notes
) {
    if (!inv || !req_queue || !ledger || !plan) return ERR_INVALID_INPUT;
    if (plan->candidate_count == 0 || plan->total_allocatable_quantity <= 0.0) {
        printf("[ERROR] Cannot execute empty allocation plan.\n");
        return ERR_INVALID_INPUT;
    }

    char cur_date[MAX_DATE_LEN];
    char cur_time[MAX_DATE_LEN];
    get_current_date_str(cur_date, sizeof(cur_date));
    get_current_time_str(cur_time, sizeof(cur_time));

    /* Find target request in queue */
    FoodRequest *req = req_queue_find_by_id(req_queue, plan->request_id);
    if (!req) {
        printf("[ERROR] Request ID '%s' no longer in active queue.\n", plan->request_id);
        return ERR_NOT_FOUND;
    }

    /* Execute allocation updates for each matched donation */
    for (int i = 0; i < plan->candidate_count; i++) {
        MatchCandidate *cand = &plan->candidates[i];
        FoodDonation *don = donation_find_by_id(inv, cand->donation_id);
        if (!don) {
            printf("[CRITICAL ERROR] Donation '%s' not found during commit!\n", cand->donation_id);
            continue;
        }

        /* Check stock safety */
        if (don->available_quantity < cand->offered_quantity) {
            printf("[ERROR] Stock concurrency collision on donation '%s'. Aborting partial step.\n", don->donation_id);
            continue;
        }

        /* Deduct stock */
        don->available_quantity -= cand->offered_quantity;
        if (don->available_quantity <= 0.001) {
            don->available_quantity = 0.0;
            don->status = STATUS_FULLY_ALLOCATED;
        } else {
            don->status = STATUS_PARTIALLY_ALLOCATED;
        }

        /* Generate unique distribution ID */
        DistributionRecord dist_rec = {0};
        snprintf(dist_rec.distribution_id, sizeof(dist_rec.distribution_id),
                 "DIS-%ld-%d", (long)time(NULL) % 100000, i + 1);
        safe_strcpy(dist_rec.request_id, plan->request_id, sizeof(dist_rec.request_id));
        safe_strcpy(dist_rec.donation_id, don->donation_id, sizeof(dist_rec.donation_id));
        safe_strcpy(dist_rec.recipient_id, plan->recipient_id, sizeof(dist_rec.recipient_id));
        dist_rec.allocated_quantity = cand->offered_quantity;
        dist_rec.unit = cand->unit;
        safe_strcpy(dist_rec.allocation_date, cur_date, sizeof(dist_rec.allocation_date));
        safe_strcpy(dist_rec.allocation_time, cur_time, sizeof(dist_rec.allocation_time));
        dist_rec.status = DIST_ALLOCATED;
        dist_rec.is_cancelled = false;

        dist_record_allocation(ledger, &dist_rec);

        /* Audit entry */
        char audit_desc[MAX_NOTES_LEN];
        snprintf(audit_desc, sizeof(audit_desc),
                 "Allocated %.1f %s from %s to Request %s (Dist: %s). Operator Note: %s",
                 cand->offered_quantity, unit_to_string(cand->unit), don->donation_id,
                 plan->request_id, dist_rec.distribution_id, operator_notes ? operator_notes : "Standard");
        audit_record(&g_audit_log, OP_ALLOCATION_CONFIRM, dist_rec.distribution_id, audit_desc, "Operator");
    }

    /* Update request remaining quantity */
    req->fulfilled_quantity += plan->total_allocatable_quantity;
    req->remaining_quantity -= plan->total_allocatable_quantity;
    if (req->remaining_quantity <= 0.001) {
        req->remaining_quantity = 0.0;
        req->status = REQ_FULFILLED;
    } else {
        req->status = REQ_PARTIALLY_FULFILLED;
    }

    printf("\n[SUCCESS] Allocation executed successfully! Distribution records generated.\n");
    return SUCCESS;
}
