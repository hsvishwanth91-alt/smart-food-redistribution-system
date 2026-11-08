/**
 * @file search.h
 * @brief Multi-field search and query filter operations.
 *        Supports case-insensitive substring search, category filters,
 *        and distribution status queries.
 */

#ifndef SEARCH_H
#define SEARCH_H

#include "donor.h"
#include "donation.h"
#include "recipient.h"
#include "distribution.h"

/* Search Operations */
void search_donations_by_keyword(const DonationInventory *inv, const char *keyword);
void search_donations_by_category(const DonationInventory *inv, FoodCategory cat);
void search_donors_by_name(const DonorRegistry *reg, const char *name_query);
void search_recipients_by_name(const RecipientRegistry *reg, const char *name_query);
void search_distributions_by_status(const DistributionLedger *ledger, DistributionStatus status);
void search_distributions_by_recipient(const DistributionLedger *ledger, const char *recipient_id);

#endif /* SEARCH_H */
