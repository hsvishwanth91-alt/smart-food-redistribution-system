/**
 * @file analytics.h
 * @brief Food Waste Prevention Analytics and Advanced Report Export (CSV/TXT).
 *        Computes non-overlapping metrics and exports readable reports.
 */

#ifndef ANALYTICS_H
#define ANALYTICS_H

#include "donation.h"
#include "donor.h"
#include "recipient.h"
#include "request_queue.h"
#include "distribution.h"

typedef struct {
    char waste_id[MAX_STR_LEN];
    char donation_id[MAX_STR_LEN];
    char food_name[MAX_STR_LEN];
    double wasted_quantity;
    FoodUnit unit;
    char logged_date[MAX_DATE_LEN];
    char reason[MAX_NOTES_LEN];
    char confirmed_by[MAX_STR_LEN];
} WasteRecord;

typedef struct {
    WasteRecord records[MAX_DONATIONS];
    int count;
} WasteLedger;

typedef struct {
    int total_donors;
    int total_recipients;
    int total_donations;
    int active_donations;
    int expired_donations;
    int near_expiry_donations;
    double total_donated_kg;
    double total_available_kg;
    double total_allocated_kg;
    double total_wasted_kg;
    int total_requests;
    int fulfilled_requests;
    int partial_requests;
    int unfulfilled_requests;
    int total_distributions;
    int cancelled_distributions;
    double redistribution_efficiency_rate;
} SystemAnalyticsSummary;

/* Operations */
void analytics_compute_summary(
    const DonorRegistry *donors,
    const RecipientRegistry *recipients,
    const DonationInventory *inv,
    const RequestQueue *requests,
    const DistributionLedger *dist,
    const WasteLedger *waste,
    SystemAnalyticsSummary *out_summary
);

void analytics_display_dashboard(const SystemAnalyticsSummary *summary);
void analytics_display_waste_ledger(const WasteLedger *waste);
StatusCode analytics_record_confirmed_waste(
    WasteLedger *waste,
    DonationInventory *inv,
    const char *donation_id,
    double qty,
    const char *reason,
    const char *operator_name
);

/* File Exports */
StatusCode export_donations_csv(const DonationInventory *inv, const char *filepath);
StatusCode export_recipients_csv(const RecipientRegistry *reg, const char *filepath);
StatusCode export_distributions_csv(const DistributionLedger *ledger, const char *filepath);
StatusCode export_expiry_report_txt(const DonationInventory *inv, const char *filepath);
StatusCode export_waste_report_csv(const WasteLedger *ledger, const char *filepath);
StatusCode export_full_summary_report_txt(const SystemAnalyticsSummary *summary, const char *filepath);

StatusCode waste_load_from_csv(WasteLedger *ledger, const char *filepath);
StatusCode waste_save_to_csv(const WasteLedger *ledger, const char *filepath);

#endif /* ANALYTICS_H */
