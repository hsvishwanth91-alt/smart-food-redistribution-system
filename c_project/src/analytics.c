/**
 * @file analytics.c
 * @brief Food Waste Prevention Analytics and Advanced Report Export (CSV & TXT).
 *        Computes non-overlapping operational metrics and handles file exports.
 */

#include "../include/analytics.h"
#include "../include/audit.h"

extern AuditLog g_audit_log;

void analytics_compute_summary(
    const DonorRegistry *donors,
    const RecipientRegistry *recipients,
    const DonationInventory *inv,
    const RequestQueue *requests,
    const DistributionLedger *dist,
    const WasteLedger *waste,
    SystemAnalyticsSummary *out_summary
) {
    if (!out_summary) return;
    memset(out_summary, 0, sizeof(SystemAnalyticsSummary));

    if (donors) out_summary->total_donors = donors->count;
    if (recipients) out_summary->total_recipients = recipients->count;

    if (inv) {
        out_summary->total_donations = inv->count;
        for (int i = 0; i < inv->count; i++) {
            const FoodDonation *d = &inv->donations[i];
            /* Convert to kg approximation for common reporting metric */
            double factor = (d->unit == UNIT_GRAM) ? 0.001 : 1.0;
            out_summary->total_donated_kg += d->initial_quantity * factor;
            out_summary->total_available_kg += d->available_quantity * factor;

            if (d->status == STATUS_AVAILABLE || d->status == STATUS_PARTIALLY_ALLOCATED) {
                out_summary->active_donations++;
            }
            if (d->urgency == EXPIRY_EXPIRED || d->status == STATUS_EXPIRED) {
                out_summary->expired_donations++;
            } else if (d->urgency == EXPIRY_CRITICAL || d->urgency == EXPIRY_NEAR_EXPIRY) {
                out_summary->near_expiry_donations++;
            }
        }
    }

    if (dist) {
        out_summary->total_distributions = dist->count;
        for (int i = 0; i < dist->count; i++) {
            const DistributionRecord *r = &dist->records[i];
            if (r->is_cancelled) {
                out_summary->cancelled_distributions++;
            } else {
                double factor = (r->unit == UNIT_GRAM) ? 0.001 : 1.0;
                out_summary->total_allocated_kg += r->allocated_quantity * factor;
            }
        }
    }

    if (waste) {
        for (int i = 0; i < waste->count; i++) {
            const WasteRecord *w = &waste->records[i];
            double factor = (w->unit == UNIT_GRAM) ? 0.001 : 1.0;
            out_summary->total_wasted_kg += w->wasted_quantity * factor;
        }
    }

    if (requests) {
        out_summary->total_requests = requests->count;
        int idx = requests->front;
        for (int i = 0; i < requests->count; i++) {
            const FoodRequest *r = &requests->items[idx];
            if (r->status == REQ_FULFILLED) {
                out_summary->fulfilled_requests++;
            } else if (r->status == REQ_PARTIALLY_FULFILLED) {
                out_summary->partial_requests++;
            } else {
                out_summary->unfulfilled_requests++;
            }
            idx = (idx + 1) % requests->capacity;
        }
    }

    /* Compute efficiency rate: allocated / (allocated + wasted) */
    double total_processed = out_summary->total_allocated_kg + out_summary->total_wasted_kg;
    if (total_processed > 0.0) {
        out_summary->redistribution_efficiency_rate = (out_summary->total_allocated_kg / total_processed) * 100.0;
    } else {
        out_summary->redistribution_efficiency_rate = 100.0;
    }
}

void analytics_display_dashboard(const SystemAnalyticsSummary *s) {
    if (!s) return;
    printf("\n============================ SYSTEM ANALYTICS & WASTE DASHBOARD ============================\n");
    printf(" [COMMUNITY NETWORK]       Total Donors: %-6d | Registered Recipients: %-6d\n", s->total_donors, s->total_recipients);
    printf(" [INVENTORY STATUS]        Total Batches: %-5d | Active Batches: %-6d | Expired/At-Risk: %d\n",
           s->total_donations, s->active_donations, s->expired_donations + s->near_expiry_donations);
    printf(" [MASS METRICS (kg)]       Donated: %10.2f kg | Available: %10.2f kg\n",
           s->total_donated_kg, s->total_available_kg);
    printf("                           Allocated: %8.2f kg | Confirmed Waste: %6.2f kg\n",
           s->total_allocated_kg, s->total_wasted_kg);
    printf(" [BENEFICIARY REQUESTS]    Total: %-8d | Fulfilled: %-6d | Partial: %-5d | Pending: %d\n",
           s->total_requests, s->fulfilled_requests, s->partial_requests, s->unfulfilled_requests);
    printf(" [DISTRIBUTION PIPELINE]   Dispatches: %-5d | Cancelled/Rolled-back: %d\n",
           s->total_distributions, s->cancelled_distributions);
    printf(" [REDISTRIBUTION SUCCESS]  Food Waste Diversion Efficiency: %.1f%%\n",
           s->redistribution_efficiency_rate);
    printf("============================================================================================\n");
}

StatusCode analytics_record_confirmed_waste(
    WasteLedger *waste,
    DonationInventory *inv,
    const char *donation_id,
    double qty,
    const char *reason,
    const char *operator_name
) {
    if (!waste || !inv || !donation_id || qty <= 0.0) return ERR_INVALID_INPUT;
    if (waste->count >= MAX_DONATIONS) return ERR_FULL;

    FoodDonation *don = donation_find_by_id(inv, donation_id);
    if (!don) {
        printf("[ERROR] Donation '%s' not found.\n", donation_id);
        return ERR_NOT_FOUND;
    }

    if (qty > don->available_quantity) {
        printf("[ERROR] Cannot mark %.2f %s as waste. Only %.2f %s available in batch.\n",
               qty, unit_to_string(don->unit), don->available_quantity, unit_to_string(don->unit));
        return ERR_INSUFFICIENT_STOCK;
    }

    /* Deduct from available quantity */
    don->available_quantity -= qty;
    if (don->available_quantity <= 0.001) {
        don->available_quantity = 0.0;
        don->status = STATUS_WASTED;
    }

    WasteRecord *w = &waste->records[waste->count++];
    snprintf(w->waste_id, sizeof(w->waste_id), "WST-%ld", (long)time(NULL) % 100000);
    safe_strcpy(w->donation_id, don->donation_id, sizeof(w->donation_id));
    safe_strcpy(w->food_name, don->food_name, sizeof(w->food_name));
    w->wasted_quantity = qty;
    w->unit = don->unit;
    get_current_date_str(w->logged_date, sizeof(w->logged_date));
    safe_strcpy(w->reason, reason ? reason : "Physical inspection failed / spoiled", sizeof(w->reason));
    safe_strcpy(w->confirmed_by, operator_name ? operator_name : "Operator", sizeof(w->confirmed_by));

    char audit_desc[MAX_NOTES_LEN];
    snprintf(audit_desc, sizeof(audit_desc),
             "Waste Confirmed for %s: %.1f %s discarded. Reason: %s",
             don->donation_id, qty, unit_to_string(don->unit), w->reason);
    audit_record(&g_audit_log, OP_WASTE_CONFIRMED, w->waste_id, audit_desc, w->confirmed_by);

    printf("[SUCCESS] Confirmed waste record '%s' logged. Inventory updated.\n", w->waste_id);
    return SUCCESS;
}

void analytics_display_waste_ledger(const WasteLedger *waste) {
    if (!waste) return;
    printf("\n================================ CONFIRMED WASTE LEDGER (%d Records) ================================\n", waste->count);
    if (waste->count == 0) {
        printf("No food waste records logged. 100%% active food preservation.\n");
        printf("----------------------------------------------------------------------------------------------------\n");
        return;
    }
    printf("%-10s | %-12s | %-16s | %-12s | %-12s | %-22s\n",
           "Waste ID", "Donation ID", "Food Item", "Quantity", "Logged Date", "Reason");
    printf("----------------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < waste->count; i++) {
        const WasteRecord *w = &waste->records[i];
        char qty_buf[32];
        snprintf(qty_buf, sizeof(qty_buf), "%.1f %s", w->wasted_quantity, unit_to_string(w->unit));
        printf("%-10s | %-12s | %-16.16s | %-12s | %-12s | %-22.22s\n",
               w->waste_id, w->donation_id, w->food_name, qty_buf, w->logged_date, w->reason);
    }
    printf("----------------------------------------------------------------------------------------------------\n");
}

/* File Exports */
StatusCode export_donations_csv(const DonationInventory *inv, const char *filepath) {
    return donation_save_to_csv(inv, filepath);
}

StatusCode export_recipients_csv(const RecipientRegistry *reg, const char *filepath) {
    return recipient_save_to_csv(reg, filepath);
}

StatusCode export_distributions_csv(const DistributionLedger *ledger, const char *filepath) {
    return dist_save_to_csv(ledger, filepath);
}

StatusCode export_waste_report_csv(const WasteLedger *ledger, const char *filepath) {
    return waste_save_to_csv(ledger, filepath);
}

StatusCode export_expiry_report_txt(const DonationInventory *inv, const char *filepath) {
    if (!inv || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    char cur_date[MAX_DATE_LEN];
    get_current_date_str(cur_date, sizeof(cur_date));

    fprintf(fp, "=========================================================================\n");
    fprintf(fp, "           SMART FOOD REDISTRIBUTION SYSTEM - EXPIRY AUDIT REPORT         \n");
    fprintf(fp, "           Generated Date: %s                                            \n", cur_date);
    fprintf(fp, "=========================================================================\n\n");

    for (int i = 0; i < inv->count; i++) {
        const FoodDonation *d = &inv->donations[i];
        double days = calculate_days_until_expiry(d->expiry_date);
        fprintf(fp, "ID: %-10s | Item: %-16s | Expiry: %-10s | Days Left: %5.1f | Status: %-10s | Urgency: %s\n",
                d->donation_id, d->food_name, d->expiry_date, days,
                donation_status_to_string(d->status), expiry_class_to_string(d->urgency));
    }

    fprintf(fp, "\n============================ END OF REPORT =============================\n");
    fclose(fp);
    return SUCCESS;
}

StatusCode export_full_summary_report_txt(const SystemAnalyticsSummary *s, const char *filepath) {
    if (!s || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    char cur_date[MAX_DATE_LEN];
    get_current_date_str(cur_date, sizeof(cur_date));

    fprintf(fp, "=========================================================================\n");
    fprintf(fp, "    SMART FOOD REDISTRIBUTION SYSTEM - EXECUTIVE SUMMARY REPORT           \n");
    fprintf(fp, "    Generated Date: %s                                                   \n", cur_date);
    fprintf(fp, "=========================================================================\n\n");
    fprintf(fp, "1. Community Engagement:\n");
    fprintf(fp, "   - Total Registered Donors:       %d\n", s->total_donors);
    fprintf(fp, "   - Total Beneficiary Recipient Organizations: %d\n\n", s->total_recipients);
    fprintf(fp, "2. Inventory & Stock Status:\n");
    fprintf(fp, "   - Total Donation Batches:        %d\n", s->total_donations);
    fprintf(fp, "   - Active Unallocated Batches:    %d\n", s->active_donations);
    fprintf(fp, "   - Total Mass Donated:            %.2f kg\n", s->total_donated_kg);
    fprintf(fp, "   - Current Stock Available:       %.2f kg\n", s->total_available_kg);
    fprintf(fp, "   - Successfully Allocated Stock:  %.2f kg\n", s->total_allocated_kg);
    fprintf(fp, "   - Confirmed Spoiled / Waste:     %.2f kg\n\n", s->total_wasted_kg);
    fprintf(fp, "3. Beneficiary Demand Fulfillment:\n");
    fprintf(fp, "   - Total Food Requests:           %d\n", s->total_requests);
    fprintf(fp, "   - 100%% Fulfilled Requests:       %d\n", s->fulfilled_requests);
    fprintf(fp, "   - Partially Fulfilled:           %d\n", s->partial_requests);
    fprintf(fp, "   - Pending / Unfulfilled:         %d\n\n", s->unfulfilled_requests);
    fprintf(fp, "4. Key Performance Indicators (KPIs):\n");
    fprintf(fp, "   - Food Waste Diversion Rate:     %.2f%%\n", s->redistribution_efficiency_rate);
    fprintf(fp, "=========================================================================\n");

    fclose(fp);
    return SUCCESS;
}

StatusCode waste_save_to_csv(const WasteLedger *ledger, const char *filepath) {
    if (!ledger || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    fprintf(fp, "waste_id,donation_id,food_name,wasted_qty,unit,logged_date,reason,confirmed_by\n");
    for (int i = 0; i < ledger->count; i++) {
        const WasteRecord *w = &ledger->records[i];
        fprintf(fp, "%s,%s,%s,%.2f,%d,%s,%s,%s\n",
                w->waste_id, w->donation_id, w->food_name, w->wasted_quantity,
                (int)w->unit, w->logged_date, w->reason, w->confirmed_by);
    }
    fclose(fp);
    return SUCCESS;
}

StatusCode waste_load_from_csv(WasteLedger *ledger, const char *filepath) {
    if (!ledger || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "r");
    if (!fp) return ERR_NOT_FOUND;

    char line[1024];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return SUCCESS;
    }

    ledger->count = 0;
    while (fgets(line, sizeof(line), fp) && ledger->count < MAX_DONATIONS) {
        trim_whitespace(line);
        if (strlen(line) == 0) continue;

        WasteRecord w = {0};
        char *token = strtok(line, ",");
        if (token) safe_strcpy(w.waste_id, token, sizeof(w.waste_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(w.donation_id, token, sizeof(w.donation_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(w.food_name, token, sizeof(w.food_name));

        token = strtok(NULL, ",");
        if (token) w.wasted_quantity = atof(token);

        token = strtok(NULL, ",");
        if (token) w.unit = (FoodUnit)atoi(token);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(w.logged_date, token, sizeof(w.logged_date));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(w.reason, token, sizeof(w.reason));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(w.confirmed_by, token, sizeof(w.confirmed_by));

        ledger->records[ledger->count++] = w;
    }
    fclose(fp);
    return SUCCESS;
}
