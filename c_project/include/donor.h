/**
 * @file donor.h
 * @brief Donor entity definitions and operations.
 */

#ifndef DONOR_H
#define DONOR_H

#include "common.h"

typedef struct {
    char donor_id[MAX_STR_LEN];
    char name[MAX_STR_LEN];
    char organization[MAX_STR_LEN];
    char contact[MAX_PHONE_LEN];
    char email[MAX_STR_LEN];
    char address[MAX_STR_LEN];
    char registered_date[MAX_DATE_LEN];
    int total_donations_count;
    double total_donated_weight; /* in primary unit / metric */
    bool is_active;
} Donor;

typedef struct {
    Donor donors[MAX_DONORS];
    int count;
} DonorRegistry;

/* Functions */
void donor_init_registry(DonorRegistry *reg);
StatusCode donor_register(DonorRegistry *reg, const Donor *d);
Donor* donor_find_by_id(DonorRegistry *reg, const char *donor_id);
int donor_find_by_contact(const DonorRegistry *reg, const char *contact);
int donor_check_duplicates(const DonorRegistry *reg, const char *name, const char *contact);
void donor_display_all(const DonorRegistry *reg);
void donor_display_single(const Donor *d);
StatusCode donor_load_from_csv(DonorRegistry *reg, const char *filepath);
StatusCode donor_save_to_csv(const DonorRegistry *reg, const char *filepath);

#endif /* DONOR_H */
