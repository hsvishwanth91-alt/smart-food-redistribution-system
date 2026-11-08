/**
 * @file search.c
 * @brief Multi-field search routines and query filter engines.
 *        Supports case-insensitive substring searching and attribute filtering.
 */

#include "../include/search.h"

void search_donations_by_keyword(const DonationInventory *inv, const char *keyword) {
    if (!inv || !keyword || strlen(keyword) == 0) return;

    printf("\n=== SEARCH RESULTS: Donations matching '%s' ===\n", keyword);
    printf("%-10s | %-16s | %-16s | %-12s | %-12s | %-12s\n",
           "ID", "Food Name", "Category", "Available", "Expiry", "Status");
    printf("---------------------------------------------------------------------------------\n");

    int matches = 0;
    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        if (strcasestr(d->food_name, keyword) ||
            strcasestr(d->donation_id, keyword) ||
            strcasestr(d->notes, keyword)) {
            matches++;
            char qty_buf[32];
            snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", d->available_quantity, unit_to_string(d->unit));
            printf("%-10s | %-16.16s | %-16.16s | %-12s | %-12s | %-12s\n",
                   d->donation_id, d->food_name, category_to_string(d->category),
                   qty_buf, d->expiry_date, donation_status_to_string(d->status));
        }
    }
    if (matches == 0) {
        printf("No donation records matched the keyword '%s'.\n", keyword);
    } else {
        printf("Found %d matching record(s). Search Complexity: O(N) linear scan.\n", matches);
    }
    printf("---------------------------------------------------------------------------------\n");
}

void search_donations_by_category(const DonationInventory *inv, FoodCategory cat) {
    if (!inv) return;
    printf("\n=== SEARCH RESULTS: Category '%s' ===\n", category_to_string(cat));
    printf("%-10s | %-16s | %-12s | %-12s | %-14s | %-12s\n",
           "ID", "Food Name", "Available", "Expiry", "Urgency", "Status");
    printf("---------------------------------------------------------------------------------\n");

    int matches = 0;
    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        if (d->category == cat) {
            matches++;
            char qty_buf[32];
            snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", d->available_quantity, unit_to_string(d->unit));
            printf("%-10s | %-16.16s | %-12s | %-12s | %-14s | %-12s\n",
                   d->donation_id, d->food_name, qty_buf, d->expiry_date,
                   expiry_class_to_string(d->urgency), donation_status_to_string(d->status));
        }
    }
    if (matches == 0) {
        printf("No records found for category '%s'.\n", category_to_string(cat));
    }
    printf("---------------------------------------------------------------------------------\n");
}

void search_donors_by_name(const DonorRegistry *reg, const char *name_query) {
    if (!reg || !name_query) return;
    printf("\n=== SEARCH RESULTS: Donors matching '%s' ===\n", name_query);
    printf("%-10s | %-20s | %-18s | %-14s | %-10s\n",
           "ID", "Name", "Organization", "Phone", "Status");
    printf("---------------------------------------------------------------------------------\n");

    int matches = 0;
    for (int i = 0; i < reg->count; i++) {
        const Donor *d = &reg->donors[i];
        if (strcasestr(d->name, name_query) || strcasestr(d->organization, name_query)) {
            matches++;
            printf("%-10s | %-20.20s | %-18.18s | %-14s | %-10s\n",
                   d->donor_id, d->name, d->organization, d->contact,
                   d->is_active ? "ACTIVE" : "INACTIVE");
        }
    }
    if (matches == 0) {
        printf("No donors matching '%s'.\n", name_query);
    }
    printf("---------------------------------------------------------------------------------\n");
}

void search_recipients_by_name(const RecipientRegistry *reg, const char *name_query) {
    if (!reg || !name_query) return;
    printf("\n=== SEARCH RESULTS: Recipients matching '%s' ===\n", name_query);
    printf("%-10s | %-22s | %-16s | %-14s | %-10s\n",
           "ID", "Organization", "Contact Person", "Phone", "Verified");
    printf("---------------------------------------------------------------------------------\n");

    int matches = 0;
    for (int i = 0; i < reg->count; i++) {
        const Recipient *r = &reg->recipients[i];
        if (strcasestr(r->organization_name, name_query) || strcasestr(r->contact_person, name_query)) {
            matches++;
            printf("%-10s | %-22.22s | %-16.16s | %-14s | %-10s\n",
                   r->recipient_id, r->organization_name, r->contact_person,
                   r->phone, r->is_verified ? "YES" : "NO");
        }
    }
    if (matches == 0) {
        printf("No recipients matching '%s'.\n", name_query);
    }
    printf("---------------------------------------------------------------------------------\n");
}

void search_distributions_by_status(const DistributionLedger *ledger, DistributionStatus status) {
    if (!ledger) return;
    printf("\n=== SEARCH RESULTS: Distributions with Status '%s' ===\n", distribution_status_to_string(status));
    printf("%-12s | %-10s | %-10s | %-12s | %-12s | %-12s\n",
           "Dist ID", "Req ID", "Donation", "Recipient", "Quantity", "Date");
    printf("---------------------------------------------------------------------------------\n");

    int matches = 0;
    for (int i = 0; i < ledger->count; i++) {
        const DistributionRecord *r = &ledger->records[i];
        if (r->status == status) {
            matches++;
            char qty_buf[32];
            snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", r->allocated_quantity, unit_to_string(r->unit));
            printf("%-12s | %-10s | %-10s | %-12s | %-12s | %-12s\n",
                   r->distribution_id, r->request_id, r->donation_id, r->recipient_id,
                   qty_buf, r->allocation_date);
        }
    }
    if (matches == 0) {
        printf("No distribution records with this status.\n");
    }
    printf("---------------------------------------------------------------------------------\n");
}

void search_distributions_by_recipient(const DistributionLedger *ledger, const char *recipient_id) {
    if (!ledger || !recipient_id) return;
    printf("\n=== SEARCH RESULTS: Distributions for Recipient '%s' ===\n", recipient_id);
    printf("%-12s | %-10s | %-10s | %-12s | %-12s | %-12s\n",
           "Dist ID", "Req ID", "Donation", "Quantity", "Date", "Status");
    printf("---------------------------------------------------------------------------------\n");

    int matches = 0;
    for (int i = 0; i < ledger->count; i++) {
        const DistributionRecord *r = &ledger->records[i];
        if (strcmp(r->recipient_id, recipient_id) == 0) {
            matches++;
            char qty_buf[32];
            snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", r->allocated_quantity, unit_to_string(r->unit));
            printf("%-12s | %-10s | %-10s | %-12s | %-12s | %-12s\n",
                   r->distribution_id, r->request_id, r->donation_id,
                   qty_buf, r->allocation_date,
                   r->is_cancelled ? "CANCELLED" : distribution_status_to_string(r->status));
        }
    }
    if (matches == 0) {
        printf("No distribution records found for recipient '%s'.\n", recipient_id);
    }
    printf("---------------------------------------------------------------------------------\n");
}
