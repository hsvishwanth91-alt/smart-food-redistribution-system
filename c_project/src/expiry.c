/**
 * @file expiry.c
 * @brief Food expiry tracking, threshold classification, and dashboard analytics.
 */

#include "../include/expiry.h"

void expiry_init_default_config(ExpiryConfig *cfg) {
    if (!cfg) return;
    cfg->critical_hours_threshold = 24.0;
    cfg->near_expiry_days_threshold = 3.0;
    cfg->allow_manual_override = true;
}

void expiry_update_thresholds(ExpiryConfig *cfg, double critical_hours, double near_expiry_days) {
    if (!cfg) return;
    if (critical_hours > 0.0) cfg->critical_hours_threshold = critical_hours;
    if (near_expiry_days > 0.0) cfg->near_expiry_days_threshold = near_expiry_days;
    printf("[INFO] Expiry thresholds updated: Critical <= %.1f hrs, Near Expiry <= %.1f days.\n",
           cfg->critical_hours_threshold, cfg->near_expiry_days_threshold);
}

void expiry_print_safety_disclaimer(void) {
    printf("+-----------------------------------------------------------------------------------+\n");
    printf("| [IMPORTANT NOTICE & FOOD SAFETY DISCLAIMER]                                       |\n");
    printf("| This expiry tracking module operates as a record-management and inventory         |\n");
    printf("| decision-support utility. It does NOT guarantee food safety or replace physical   |\n");
    printf("| inspection, temperature logs, sensory checks, or local health authority rules.    |\n");
    printf("+-----------------------------------------------------------------------------------+\n");
}

void expiry_display_dashboard(const DonationInventory *inv, const ExpiryConfig *cfg) {
    if (!inv || !cfg) return;

    int expired_count = 0;
    int critical_count = 0;
    int near_expiry_count = 0;
    int normal_count = 0;
    int missing_info_count = 0;

    double critical_days = cfg->critical_hours_threshold / 24.0;

    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        if (!is_valid_date_format(d->expiry_date)) {
            missing_info_count++;
            continue;
        }

        double days = calculate_days_until_expiry(d->expiry_date);
        if (days < 0.0) {
            expired_count++;
        } else if (days <= critical_days) {
            critical_count++;
        } else if (days <= cfg->near_expiry_days_threshold) {
            near_expiry_count++;
        } else {
            normal_count++;
        }
    }

    expiry_print_safety_disclaimer();
    printf("\n=============================== FOOD EXPIRY DASHBOARD ===============================\n");
    printf("Configured Thresholds: Critical <= %.1f hrs | Near Expiry <= %.1f days\n",
           cfg->critical_hours_threshold, cfg->near_expiry_days_threshold);
    printf("-------------------------------------------------------------------------------------\n");
    printf(" [!] EXPIRED BATCHES (Action Required: Mark Waste / Quarantine): %d\n", expired_count);
    printf(" [!] CRITICAL URGENCY (< %.0f Hours Left - Priority Dispatch):    %d\n", cfg->critical_hours_threshold, critical_count);
    printf(" [*] NEAR EXPIRY (< %.1f Days Left - Recommended Matching):       %d\n", cfg->near_expiry_days_threshold, near_expiry_count);
    printf(" [-] NORMAL STATUS (Adequate Shelf-Life):                         %d\n", normal_count);
    printf(" [?] MISSING / MALFORMED EXPIRY DATES:                            %d\n", missing_info_count);
    printf("=====================================================================================\n");
}

void expiry_filter_and_display(const DonationInventory *inv, ExpiryClassification target_class) {
    if (!inv) return;
    printf("\n--- FILTERED DONATIONS: %s ---\n", expiry_class_to_string(target_class));
    printf("%-10s | %-16s | %-16s | %-12s | %-12s | %-10s\n",
           "ID", "Food Name", "Category", "Available", "Expiry Date", "Days Left");
    printf("-------------------------------------------------------------------------------------\n");

    int found = 0;
    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        if (d->urgency == target_class) {
            found++;
            char qty_buf[32];
            snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", d->available_quantity, unit_to_string(d->unit));
            double days = calculate_days_until_expiry(d->expiry_date);
            printf("%-10s | %-16.16s | %-16.16s | %-12s | %-12s | %-10.1f\n",
                   d->donation_id, d->food_name, category_to_string(d->category),
                   qty_buf, d->expiry_date, days);
        }
    }
    if (found == 0) {
        printf("No records matching classification '%s'.\n", expiry_class_to_string(target_class));
    }
    printf("-------------------------------------------------------------------------------------\n");
}

bool expiry_is_allocatable(const FoodDonation *d) {
    if (!d) return false;
    double days = calculate_days_until_expiry(d->expiry_date);
    return (days >= 0.0 && d->status != STATUS_EXPIRED && d->status != STATUS_WASTED);
}
