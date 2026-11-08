/**
 * @file utils.c
 * @brief Core utility implementations for string manipulation, date calculations,
 *        enum conversions, and safe input processing.
 */

#include "../include/common.h"
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(dir) _mkdir(dir)
#else
#define MKDIR(dir) mkdir(dir, 0755)
#endif

void safe_strcpy(char *dest, const char *src, size_t dest_size) {
    if (!dest || dest_size == 0) return;
    if (!src) {
        dest[0] = '\0';
        return;
    }
    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
}

void trim_whitespace(char *str) {
    if (!str) return;
    char *end;

    /* Trim leading space */
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return;

    /* Trim trailing space */
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    /* Write new null terminator character */
    end[1] = '\0';
}

void clear_input_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int get_current_date_str(char *buffer, size_t size) {
    if (!buffer || size < 11) return -1;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
    return 0;
}

int get_current_time_str(char *buffer, size_t size) {
    if (!buffer || size < 9) return -1;
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "%H:%M:%S", tm_info);
    return 0;
}

bool is_valid_date_format(const char *date_str) {
    if (!date_str || strlen(date_str) != 10) return false;
    if (date_str[4] != '-' || date_str[7] != '-') return false;

    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit((unsigned char)date_str[i])) return false;
    }

    int year, month, day;
    if (sscanf(date_str, "%4d-%2d-%2d", &year, &month, &day) != 3) {
        return false;
    }

    if (year < 2000 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;

    /* Basic days per month validation */
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30) return false;
    if (month == 2) {
        bool is_leap = (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0));
        if (day > (is_leap ? 29 : 28)) return false;
    }

    return true;
}

double calculate_days_until_expiry(const char *expiry_date_str) {
    if (!is_valid_date_format(expiry_date_str)) {
        return 999.0; /* Unknown or future fallback */
    }

    int exp_y, exp_m, exp_d;
    if (sscanf(expiry_date_str, "%4d-%2d-%2d", &exp_y, &exp_m, &exp_d) != 3) {
        return 999.0;
    }

    struct tm exp_tm = {0};
    exp_tm.tm_year = exp_y - 1900;
    exp_tm.tm_mon = exp_m - 1;
    exp_tm.tm_mday = exp_d;
    exp_tm.tm_hour = 23;
    exp_tm.tm_min = 59;
    exp_tm.tm_sec = 59;
    exp_tm.tm_isdst = -1;

    time_t exp_time = mktime(&exp_tm);
    time_t now = time(NULL);

    double diff_seconds = difftime(exp_time, now);
    double days = diff_seconds / (24.0 * 3600.0);
    return days;
}

ExpiryClassification classify_expiry(const char *expiry_date_str) {
    double days = calculate_days_until_expiry(expiry_date_str);
    if (days < 0.0) {
        return EXPIRY_EXPIRED;
    } else if (days <= 1.0) {
        return EXPIRY_CRITICAL;
    } else if (days <= 3.0) {
        return EXPIRY_NEAR_EXPIRY;
    } else {
        return EXPIRY_NORMAL;
    }
}

const char* unit_to_string(FoodUnit unit) {
    switch (unit) {
        case UNIT_KG: return "kg";
        case UNIT_GRAM: return "g";
        case UNIT_PACKET: return "packets";
        case UNIT_PIECE: return "pieces";
        case UNIT_LITER: return "liters";
        default: return "units";
    }
}

FoodUnit string_to_unit(const char *str) {
    if (!str) return UNIT_KG;
    if (strcasecmp(str, "kg") == 0 || strcasecmp(str, "kilogram") == 0) return UNIT_KG;
    if (strcasecmp(str, "g") == 0 || strcasecmp(str, "gram") == 0) return UNIT_GRAM;
    if (strcasecmp(str, "packet") == 0 || strcasecmp(str, "packets") == 0 || strcasecmp(str, "pkt") == 0) return UNIT_PACKET;
    if (strcasecmp(str, "piece") == 0 || strcasecmp(str, "pieces") == 0 || strcasecmp(str, "pcs") == 0) return UNIT_PIECE;
    if (strcasecmp(str, "l") == 0 || strcasecmp(str, "liter") == 0 || strcasecmp(str, "liters") == 0) return UNIT_LITER;
    return UNIT_KG;
}

const char* category_to_string(FoodCategory cat) {
    switch (cat) {
        case CAT_GRAINS_STAPLES: return "Grains & Staples";
        case CAT_COOKED_MEALS: return "Cooked Meals";
        case CAT_DAIRY_EGGS: return "Dairy & Eggs";
        case CAT_PRODUCE_VEG_FRUIT: return "Fresh Produce";
        case CAT_BAKERY: return "Bakery & Bread";
        case CAT_CANNED_PACKAGED: return "Canned & Packaged";
        case CAT_BEVERAGES: return "Beverages";
        case CAT_OTHER: default: return "Other";
    }
}

FoodCategory string_to_category(const char *str) {
    if (!str) return CAT_OTHER;
    if (strcasestr(str, "grain") || strcasestr(str, "staple") || strcasestr(str, "rice") || strcasestr(str, "wheat")) return CAT_GRAINS_STAPLES;
    if (strcasestr(str, "cook") || strcasestr(str, "meal")) return CAT_COOKED_MEALS;
    if (strcasestr(str, "dairy") || strcasestr(str, "egg") || strcasestr(str, "milk")) return CAT_DAIRY_EGGS;
    if (strcasestr(str, "produce") || strcasestr(str, "veg") || strcasestr(str, "fruit")) return CAT_PRODUCE_VEG_FRUIT;
    if (strcasestr(str, "bake") || strcasestr(str, "bread")) return CAT_BAKERY;
    if (strcasestr(str, "can") || strcasestr(str, "pack")) return CAT_CANNED_PACKAGED;
    if (strcasestr(str, "bev") || strcasestr(str, "drink") || strcasestr(str, "juice")) return CAT_BEVERAGES;
    return CAT_OTHER;
}

const char* donation_status_to_string(DonationStatus st) {
    switch (st) {
        case STATUS_AVAILABLE: return "AVAILABLE";
        case STATUS_PARTIALLY_ALLOCATED: return "PARTIALLY_ALLOCATED";
        case STATUS_FULLY_ALLOCATED: return "FULLY_ALLOCATED";
        case STATUS_EXPIRED: return "EXPIRED";
        case STATUS_WASTED: return "CONFIRMED_WASTE";
        case STATUS_CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

const char* expiry_class_to_string(ExpiryClassification ec) {
    switch (ec) {
        case EXPIRY_EXPIRED: return "EXPIRED [!]";
        case EXPIRY_CRITICAL: return "CRITICAL (<24h)";
        case EXPIRY_NEAR_EXPIRY: return "NEAR EXPIRY (<72h)";
        case EXPIRY_NORMAL: default: return "NORMAL";
    }
}

const char* request_status_to_string(RequestStatus st) {
    switch (st) {
        case REQ_PENDING: return "PENDING";
        case REQ_PARTIALLY_FULFILLED: return "PARTIALLY_FULFILLED";
        case REQ_FULFILLED: return "FULFILLED";
        case REQ_CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

const char* distribution_status_to_string(DistributionStatus st) {
    switch (st) {
        case DIST_ALLOCATED: return "ALLOCATED";
        case DIST_IN_TRANSIT: return "IN_TRANSIT";
        case DIST_DELIVERED: return "DELIVERED";
        case DIST_CANCELLED: return "CANCELLED";
        default: return "UNKNOWN";
    }
}

void ensure_directory_exists(const char *path) {
    if (!path) return;
    MKDIR(path);
}
