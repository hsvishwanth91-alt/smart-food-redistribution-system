/**
 * @file expiry.h
 * @brief Dedicated food expiry management module.
 *        Configurable thresholds, expiry classification, expiry dashboard,
 *        sorting, and validation safeguards.
 */

#ifndef EXPIRY_H
#define EXPIRY_H

#include "donation.h"

typedef struct {
    double critical_hours_threshold;   /* Default: 24.0 hours */
    double near_expiry_days_threshold; /* Default: 3.0 days */
    bool allow_manual_override;
} ExpiryConfig;

/* Operations */
void expiry_init_default_config(ExpiryConfig *cfg);
void expiry_update_thresholds(ExpiryConfig *cfg, double critical_hours, double near_expiry_days);
void expiry_display_dashboard(const DonationInventory *inv, const ExpiryConfig *cfg);
void expiry_filter_and_display(const DonationInventory *inv, ExpiryClassification target_class);
bool expiry_is_allocatable(const FoodDonation *d);
void expiry_print_safety_disclaimer(void);

#endif /* EXPIRY_H */
