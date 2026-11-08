/**
 * @file recipient.h
 * @brief Recipient beneficiary organization and family profile definitions.
 */

#ifndef RECIPIENT_H
#define RECIPIENT_H

#include "common.h"

typedef struct {
    char recipient_id[MAX_STR_LEN];
    char organization_name[MAX_STR_LEN];
    char contact_person[MAX_STR_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_STR_LEN];
    char address[MAX_STR_LEN];
    int beneficiaries_count;
    char dietary_requirements[MAX_STR_LEN];
    int total_requests_made;
    int fulfilled_requests_count;
    bool is_verified;
} Recipient;

typedef struct {
    Recipient recipients[MAX_RECIPIENTS];
    int count;
} RecipientRegistry;

/* Functions */
void recipient_init_registry(RecipientRegistry *reg);
StatusCode recipient_register(RecipientRegistry *reg, const Recipient *r);
Recipient* recipient_find_by_id(RecipientRegistry *reg, const char *recipient_id);
int recipient_find_by_contact(const RecipientRegistry *reg, const char *contact);
int recipient_check_duplicates(const RecipientRegistry *reg, const char *org_name, const char *contact);
void recipient_display_all(const RecipientRegistry *reg);
void recipient_display_single(const Recipient *r);
StatusCode recipient_load_from_csv(RecipientRegistry *reg, const char *filepath);
StatusCode recipient_save_to_csv(const RecipientRegistry *reg, const char *filepath);

#endif /* RECIPIENT_H */
