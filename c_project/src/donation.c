/**
 * @file donation.c
 * @brief Food donation inventory operations, sorting routines, and CSV persistence.
 */

#include "../include/donation.h"

void donation_init_inventory(DonationInventory *inv) {
    if (!inv) return;
    inv->count = 0;
    memset(inv->donations, 0, sizeof(inv->donations));
}

StatusCode donation_add(DonationInventory *inv, const FoodDonation *item) {
    if (!inv || !item) return ERR_INVALID_INPUT;
    if (inv->count >= MAX_DONATIONS) return ERR_FULL;

    /* Validate unique ID */
    if (donation_find_by_id(inv, item->donation_id) != NULL) {
        return ERR_DUPLICATE;
    }

    /* Validate quantity */
    if (item->available_quantity <= 0.0) {
        return ERR_INVALID_INPUT;
    }

    inv->donations[inv->count] = *item;
    /* Compute initial urgency */
    inv->donations[inv->count].urgency = classify_expiry(item->expiry_date);
    if (inv->donations[inv->count].urgency == EXPIRY_EXPIRED) {
        inv->donations[inv->count].status = STATUS_EXPIRED;
    }
    inv->count++;
    return SUCCESS;
}

FoodDonation* donation_find_by_id(DonationInventory *inv, const char *donation_id) {
    if (!inv || !donation_id) return NULL;
    for (int i = 0; i < inv->count; i++) {
        if (strcmp(inv->donations[i].donation_id, donation_id) == 0) {
            return &inv->donations[i];
        }
    }
    return NULL;
}

void donation_refresh_urgencies(DonationInventory *inv) {
    if (!inv) return;
    for (int i = 0; i < inv->count; i++) {
        FoodDonation *d = &inv->donations[i];
        d->urgency = classify_expiry(d->expiry_date);
        if (d->urgency == EXPIRY_EXPIRED && d->status == STATUS_AVAILABLE) {
            d->status = STATUS_EXPIRED;
        }

        /* Priority score calculation: Lower days to expiry -> Higher score */
        double days = calculate_days_until_expiry(d->expiry_date);
        if (days < 0) {
            d->priority_score = 0; /* Expired */
        } else if (days <= 1.0) {
            d->priority_score = 100; /* Critical */
        } else if (days <= 3.0) {
            d->priority_score = 75;  /* Near Expiry */
        } else if (days <= 7.0) {
            d->priority_score = 50;
        } else {
            d->priority_score = 25;
        }
    }
}

void donation_display_single(const FoodDonation *d) {
    if (!d) return;
    double days = calculate_days_until_expiry(d->expiry_date);
    printf("+-----------------------------------------------------------------------------------+\n");
    printf("| Donation ID:  %-20s | Donor ID:       %-28s |\n", d->donation_id, d->donor_id);
    printf("| Food Item:    %-20s | Category:       %-28s |\n", d->food_name, category_to_string(d->category));
    printf("| Available:    %8.2f %-11s | Initial:        %8.2f %-18s |\n",
           d->available_quantity, unit_to_string(d->unit), d->initial_quantity, unit_to_string(d->unit));
    printf("| Prep Date:    %-20s | Expiry Date:    %-28s |\n", d->prep_date, d->expiry_date);
    printf("| Days Left:    %-20.1f | Classification: %-28s |\n", days, expiry_class_to_string(d->urgency));
    printf("| Status:       %-20s | Storage Type:   %-28s |\n", donation_status_to_string(d->status), d->storage_type);
    printf("| Notes:        %-67s |\n", d->notes);
    printf("+-----------------------------------------------------------------------------------+\n");
}

void donation_display_all(const DonationInventory *inv) {
    if (!inv) return;
    printf("\n==================================== FOOD DONATION INVENTORY (%d Items) ====================================\n", inv->count);
    printf("%-10s | %-16s | %-16s | %-12s | %-11s | %-17s | %-15s\n",
           "ID", "Food Name", "Category", "Available", "Expiry", "Urgency", "Status");
    printf("-------------------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        char qty_buf[32];
        snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", d->available_quantity, unit_to_string(d->unit));
        printf("%-10s | %-16.16s | %-16.16s | %-12s | %-11s | %-17s | %-15s\n",
               d->donation_id, d->food_name, category_to_string(d->category),
               qty_buf, d->expiry_date, expiry_class_to_string(d->urgency),
               donation_status_to_string(d->status));
    }
    printf("-------------------------------------------------------------------------------------------------------------\n");
}

/* Sorting using Insertion Sort - stable, efficient for small-to-medium inventory */
void donation_sort_by_expiry(DonationInventory *inv, bool ascending) {
    if (!inv || inv->count <= 1) return;
    for (int i = 1; i < inv->count; i++) {
        FoodDonation key = inv->donations[i];
        double key_days = calculate_days_until_expiry(key.expiry_date);
        int j = i - 1;

        while (j >= 0) {
            double cur_days = calculate_days_until_expiry(inv->donations[j].expiry_date);
            bool condition = ascending ? (cur_days > key_days) : (cur_days < key_days);
            if (!condition) break;
            inv->donations[j + 1] = inv->donations[j];
            j--;
        }
        inv->donations[j + 1] = key;
    }
}

void donation_sort_by_quantity(DonationInventory *inv, bool ascending) {
    if (!inv || inv->count <= 1) return;
    for (int i = 1; i < inv->count; i++) {
        FoodDonation key = inv->donations[i];
        int j = i - 1;

        while (j >= 0) {
            bool condition = ascending ?
                (inv->donations[j].available_quantity > key.available_quantity) :
                (inv->donations[j].available_quantity < key.available_quantity);
            if (!condition) break;
            inv->donations[j + 1] = inv->donations[j];
            j--;
        }
        inv->donations[j + 1] = key;
    }
}

void donation_sort_by_urgency_score(DonationInventory *inv) {
    if (!inv || inv->count <= 1) return;
    for (int i = 1; i < inv->count; i++) {
        FoodDonation key = inv->donations[i];
        int j = i - 1;

        /* Sort descending by priority score (100 is most urgent) */
        while (j >= 0 && inv->donations[j].priority_score < key.priority_score) {
            inv->donations[j + 1] = inv->donations[j];
            j--;
        }
        inv->donations[j + 1] = key;
    }
}

StatusCode donation_save_to_csv(const DonationInventory *inv, const char *filepath) {
    if (!inv || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    fprintf(fp, "donation_id,donor_id,food_name,category,initial_qty,available_qty,unit,prep_date,expiry_date,status,storage,notes\n");
    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        fprintf(fp, "%s,%s,%s,%d,%.2f,%.2f,%d,%s,%s,%d,%s,%s\n",
                d->donation_id, d->donor_id, d->food_name, (int)d->category,
                d->initial_quantity, d->available_quantity, (int)d->unit,
                d->prep_date, d->expiry_date, (int)d->status,
                d->storage_type, d->notes);
    }
    fclose(fp);
    return SUCCESS;
}

StatusCode donation_load_from_csv(DonationInventory *inv, const char *filepath) {
    if (!inv || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "r");
    if (!fp) return ERR_NOT_FOUND;

    char line[1024];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return SUCCESS;
    }

    while (fgets(line, sizeof(line), fp) && inv->count < MAX_DONATIONS) {
        trim_whitespace(line);
        if (strlen(line) == 0) continue;

        FoodDonation d = {0};
        char *token = strtok(line, ",");
        if (token) safe_strcpy(d.donation_id, token, sizeof(d.donation_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.donor_id, token, sizeof(d.donor_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.food_name, token, sizeof(d.food_name));

        token = strtok(NULL, ",");
        if (token) d.category = (FoodCategory)atoi(token);

        token = strtok(NULL, ",");
        if (token) d.initial_quantity = atof(token);

        token = strtok(NULL, ",");
        if (token) d.available_quantity = atof(token);

        token = strtok(NULL, ",");
        if (token) d.unit = (FoodUnit)atoi(token);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.prep_date, token, sizeof(d.prep_date));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.expiry_date, token, sizeof(d.expiry_date));

        token = strtok(NULL, ",");
        if (token) d.status = (DonationStatus)atoi(token);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.storage_type, token, sizeof(d.storage_type));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.notes, token, sizeof(d.notes));

        d.urgency = classify_expiry(d.expiry_date);
        inv->donations[inv->count++] = d;
    }

    fclose(fp);
    donation_refresh_urgencies(inv);
    return SUCCESS;
}
