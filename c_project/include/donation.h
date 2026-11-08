/**
 * @file donation.h
 * @brief Food donation entity, inventory state, and operations.
 */

#ifndef DONATION_H
#define DONATION_H

#include "common.h"

typedef struct {
    char donation_id[MAX_STR_LEN];
    char donor_id[MAX_STR_LEN];
    char food_name[MAX_STR_LEN];
    FoodCategory category;
    double initial_quantity;
    double available_quantity;
    FoodUnit unit;
    char prep_date[MAX_DATE_LEN];
    char expiry_date[MAX_DATE_LEN];
    DonationStatus status;
    ExpiryClassification urgency;
    int priority_score;            /* Dynamic score derived from urgency & age */
    char storage_type[MAX_STR_LEN];/* e.g., Ambient, Refrigerated, Frozen */
    char notes[MAX_NOTES_LEN];
} FoodDonation;

typedef struct {
    FoodDonation donations[MAX_DONATIONS];
    int count;
} DonationInventory;

/* Functions */
void donation_init_inventory(DonationInventory *inv);
StatusCode donation_add(DonationInventory *inv, const FoodDonation *item);
FoodDonation* donation_find_by_id(DonationInventory *inv, const char *donation_id);
void donation_refresh_urgencies(DonationInventory *inv);
void donation_display_all(const DonationInventory *inv);
void donation_display_single(const FoodDonation *d);
StatusCode donation_load_from_csv(DonationInventory *inv, const char *filepath);
StatusCode donation_save_to_csv(const DonationInventory *inv, const char *filepath);

/* Sorting */
void donation_sort_by_expiry(DonationInventory *inv, bool ascending);
void donation_sort_by_quantity(DonationInventory *inv, bool ascending);
void donation_sort_by_urgency_score(DonationInventory *inv);

#endif /* DONATION_H */
