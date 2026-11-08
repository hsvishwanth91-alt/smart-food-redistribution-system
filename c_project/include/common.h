/**
 * @file common.h
 * @brief Common constants, enumerations, error codes, and utility prototypes
 *        for Smart Food Redistribution System (Professional DSA Edition).
 * @version 2.0.0
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define MAX_STR_LEN         128
#define MAX_DATE_LEN        16
#define MAX_PHONE_LEN       24
#define MAX_NOTES_LEN       256
#define MAX_DONORS          500
#define MAX_DONATIONS       1000
#define MAX_RECIPIENTS      500
#define MAX_REQUESTS        1000
#define MAX_DISTRIBUTIONS   2000
#define MAX_AUDIT_LOGS      5000
#define MAX_BACKUPS         50

/* File storage paths */
#define FILE_DONORS         "data/donors.csv"
#define FILE_DONATIONS      "data/donations.csv"
#define FILE_RECIPIENTS     "data/recipients.csv"
#define FILE_REQUESTS       "data/requests.csv"
#define FILE_DISTRIBUTIONS  "data/distributions.csv"
#define FILE_AUDIT          "data/audit_log.csv"
#define FILE_WASTE          "data/waste.csv"
#define BACKUP_DIR          "data/backups"

/* Food Units */
typedef enum {
    UNIT_KG = 0,
    UNIT_GRAM,
    UNIT_PACKET,
    UNIT_PIECE,
    UNIT_LITER
} FoodUnit;

/* Food Categories */
typedef enum {
    CAT_GRAINS_STAPLES = 0,
    CAT_COOKED_MEALS,
    CAT_DAIRY_EGGS,
    CAT_PRODUCE_VEG_FRUIT,
    CAT_BAKERY,
    CAT_CANNED_PACKAGED,
    CAT_BEVERAGES,
    CAT_OTHER
} FoodCategory;

/* Donation Availability Status */
typedef enum {
    STATUS_AVAILABLE = 0,
    STATUS_PARTIALLY_ALLOCATED,
    STATUS_FULLY_ALLOCATED,
    STATUS_EXPIRED,
    STATUS_WASTED,
    STATUS_CANCELLED
} DonationStatus;

/* Expiry Urgency Classification */
typedef enum {
    EXPIRY_EXPIRED = 0,      /* Past use-by date */
    EXPIRY_CRITICAL,         /* Within 24 hours */
    EXPIRY_NEAR_EXPIRY,      /* Within 72 hours (3 days) */
    EXPIRY_NORMAL            /* Safe (> 3 days) */
} ExpiryClassification;

/* Recipient Request Status */
typedef enum {
    REQ_PENDING = 0,
    REQ_PARTIALLY_FULFILLED,
    REQ_FULFILLED,
    REQ_CANCELLED
} RequestStatus;

/* Recipient Priority */
typedef enum {
    PRIORITY_STANDARD = 1,
    PRIORITY_MEDIUM = 2,
    PRIORITY_HIGH = 3,
    PRIORITY_EMERGENCY = 4
} RequestPriority;

/* Distribution Status */
typedef enum {
    DIST_ALLOCATED = 0,
    DIST_IN_TRANSIT,
    DIST_DELIVERED,
    DIST_CANCELLED
} DistributionStatus;

/* Common Return Codes */
typedef enum {
    SUCCESS = 0,
    ERR_NOT_FOUND = -1,
    ERR_DUPLICATE = -2,
    ERR_FULL = -3,
    ERR_EMPTY = -4,
    ERR_INVALID_INPUT = -5,
    ERR_INSUFFICIENT_STOCK = -6,
    ERR_EXPIRED = -7,
    ERR_UNIT_MISMATCH = -8,
    ERR_FILE_IO = -9
} StatusCode;

/* Utility function prototypes */
void safe_strcpy(char *dest, const char *src, size_t dest_size);
void trim_whitespace(char *str);
int get_current_date_str(char *buffer, size_t size);
int get_current_time_str(char *buffer, size_t size);
double calculate_days_until_expiry(const char *expiry_date_str);
ExpiryClassification classify_expiry(const char *expiry_date_str);
const char* unit_to_string(FoodUnit unit);
FoodUnit string_to_unit(const char *str);
const char* category_to_string(FoodCategory cat);
FoodCategory string_to_category(const char *str);
const char* donation_status_to_string(DonationStatus st);
const char* expiry_class_to_string(ExpiryClassification ec);
const char* request_status_to_string(RequestStatus st);
const char* distribution_status_to_string(DistributionStatus st);
bool is_valid_date_format(const char *date_str);
void clear_input_buffer(void);
void ensure_directory_exists(const char *path);

#endif /* COMMON_H */
