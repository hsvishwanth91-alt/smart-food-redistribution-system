/**
 * @file audit.c
 * @brief System audit logging operations, query filtering, and CSV persistence.
 */

#include "../include/audit.h"

AuditLog g_audit_log; /* Global audit log instance */

void audit_init(AuditLog *log) {
    if (!log) return;
    log->count = 0;
    memset(log->entries, 0, sizeof(log->entries));
}

const char* audit_op_to_string(AuditOpType type) {
    switch (type) {
        case OP_DONOR_REGISTER: return "DONOR_REGISTER";
        case OP_DONATION_CREATE: return "DONATION_CREATE";
        case OP_DONATION_UPDATE: return "DONATION_UPDATE";
        case OP_RECIPIENT_REGISTER: return "RECIPIENT_REGISTER";
        case OP_REQUEST_CREATE: return "REQUEST_CREATE";
        case OP_ALLOCATION_CONFIRM: return "ALLOCATION_CONFIRM";
        case OP_ALLOCATION_CANCEL: return "ALLOCATION_ROLLBACK";
        case OP_STATUS_CHANGE: return "STATUS_CHANGE";
        case OP_EXPIRY_FLAGGED: return "EXPIRY_FLAGGED";
        case OP_WASTE_CONFIRMED: return "WASTE_CONFIRMED";
        case OP_BACKUP_CREATED: return "BACKUP_CREATED";
        case OP_BACKUP_RESTORED: return "BACKUP_RESTORED";
        case OP_DEMO_LOADED: return "DEMO_DATASET_LOAD";
        default: return "SYSTEM_OP";
    }
}

StatusCode audit_record(
    AuditLog *log,
    AuditOpType op_type,
    const char *record_id,
    const char *description,
    const char *operator_user
) {
    if (!log) return ERR_INVALID_INPUT;
    if (log->count >= MAX_AUDIT_LOGS) return ERR_FULL;

    AuditEntry *entry = &log->entries[log->count++];
    snprintf(entry->audit_id, sizeof(entry->audit_id), "AUD-%ld-%d", (long)time(NULL) % 100000, log->count);
    get_current_date_str(entry->date, sizeof(entry->date));
    get_current_time_str(entry->time, sizeof(entry->time));
    entry->op_type = op_type;
    safe_strcpy(entry->target_record_id, record_id ? record_id : "N/A", sizeof(entry->target_record_id));
    safe_strcpy(entry->description, description ? description : "System Operation", sizeof(entry->description));
    safe_strcpy(entry->operator_user, operator_user ? operator_user : "Admin", sizeof(entry->operator_user));

    /* Append directly to CSV file as well for persistence */
    FILE *fp = fopen(FILE_AUDIT, "a");
    if (fp) {
        fprintf(fp, "%s,%s,%s,%d,%s,\"%s\",%s\n",
                entry->audit_id, entry->date, entry->time, (int)entry->op_type,
                entry->target_record_id, entry->description, entry->operator_user);
        fclose(fp);
    }

    return SUCCESS;
}

void audit_display_recent(const AuditLog *log, int limit) {
    if (!log) return;
    printf("\n================================ SYSTEM AUDIT TRAIL ================================\n");
    if (log->count == 0) {
        printf("Audit log is currently empty.\n");
        printf("------------------------------------------------------------------------------------\n");
        return;
    }

    int start = log->count - limit;
    if (start < 0) start = 0;

    printf("Showing %d most recent operations (Total in log: %d):\n", log->count - start, log->count);
    printf("------------------------------------------------------------------------------------\n");
    printf("%-12s | %-19s | %-18s | %-12s | %-30s\n",
           "Audit ID", "Timestamp", "Operation", "Target ID", "Details");
    printf("------------------------------------------------------------------------------------\n");

    for (int i = log->count - 1; i >= start; i--) {
        const AuditEntry *e = &log->entries[i];
        char ts[32];
        snprintf(ts, sizeof(ts), "%s %s", e->date, e->time);
        printf("%-12s | %-19s | %-18.18s | %-12s | %-30.30s\n",
               e->audit_id, ts, audit_op_to_string(e->op_type),
               e->target_record_id, e->description);
    }
    printf("------------------------------------------------------------------------------------\n");
}

void audit_display_filtered_by_record(const AuditLog *log, const char *record_id) {
    if (!log || !record_id) return;
    printf("\n=== AUDIT HISTORY FOR RECORD: %s ===\n", record_id);
    int matches = 0;
    for (int i = 0; i < log->count; i++) {
        const AuditEntry *e = &log->entries[i];
        if (strcmp(e->target_record_id, record_id) == 0) {
            matches++;
            printf("[%s %s] %s | User: %s | Description: %s\n",
                   e->date, e->time, audit_op_to_string(e->op_type),
                   e->operator_user, e->description);
        }
    }
    if (matches == 0) {
        printf("No audit events found for record '%s'.\n", record_id);
    }
    printf("===================================================\n");
}

StatusCode audit_save_to_csv(const AuditLog *log, const char *filepath) {
    if (!log || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    fprintf(fp, "audit_id,date,time,op_type,record_id,description,user\n");
    for (int i = 0; i < log->count; i++) {
        const AuditEntry *e = &log->entries[i];
        fprintf(fp, "%s,%s,%s,%d,%s,\"%s\",%s\n",
                e->audit_id, e->date, e->time, (int)e->op_type,
                e->target_record_id, e->description, e->operator_user);
    }
    fclose(fp);
    return SUCCESS;
}

StatusCode audit_load_from_csv(AuditLog *log, const char *filepath) {
    if (!log || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "r");
    if (!fp) return ERR_NOT_FOUND;

    char line[1024];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return SUCCESS;
    }

    log->count = 0;
    while (fgets(line, sizeof(line), fp) && log->count < MAX_AUDIT_LOGS) {
        trim_whitespace(line);
        if (strlen(line) == 0) continue;

        AuditEntry e = {0};
        char *token = strtok(line, ",");
        if (token) safe_strcpy(e.audit_id, token, sizeof(e.audit_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(e.date, token, sizeof(e.date));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(e.time, token, sizeof(e.time));

        token = strtok(NULL, ",");
        if (token) e.op_type = (AuditOpType)atoi(token);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(e.target_record_id, token, sizeof(e.target_record_id));

        token = strtok(NULL, ",");
        if (token) {
            /* Remove surrounding quotes if present */
            char desc[MAX_NOTES_LEN] = {0};
            safe_strcpy(desc, token, sizeof(desc));
            if (desc[0] == '\"') {
                size_t len = strlen(desc);
                if (len > 1 && desc[len - 1] == '\"') desc[len - 1] = '\0';
                safe_strcpy(e.description, desc + 1, sizeof(e.description));
            } else {
                safe_strcpy(e.description, desc, sizeof(e.description));
            }
        }

        token = strtok(NULL, ",");
        if (token) safe_strcpy(e.operator_user, token, sizeof(e.operator_user));

        log->entries[log->count++] = e;
    }
    fclose(fp);
    return SUCCESS;
}
