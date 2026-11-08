/**
 * @file audit.h
 * @brief System Audit Log module.
 *        Records all critical state modifications for project demonstration,
 *        traceability, and regulatory compliance.
 */

#ifndef AUDIT_H
#define AUDIT_H

#include "common.h"

typedef enum {
    OP_DONOR_REGISTER = 0,
    OP_DONATION_CREATE,
    OP_DONATION_UPDATE,
    OP_RECIPIENT_REGISTER,
    OP_REQUEST_CREATE,
    OP_ALLOCATION_CONFIRM,
    OP_ALLOCATION_CANCEL,
    OP_STATUS_CHANGE,
    OP_EXPIRY_FLAGGED,
    OP_WASTE_CONFIRMED,
    OP_BACKUP_CREATED,
    OP_BACKUP_RESTORED,
    OP_DEMO_LOADED
} AuditOpType;

typedef struct {
    char audit_id[MAX_STR_LEN];
    char date[MAX_DATE_LEN];
    char time[MAX_DATE_LEN];
    AuditOpType op_type;
    char target_record_id[MAX_STR_LEN];
    char description[MAX_NOTES_LEN];
    char operator_user[MAX_STR_LEN];
} AuditEntry;

typedef struct {
    AuditEntry entries[MAX_AUDIT_LOGS];
    int count;
} AuditLog;

/* Operations */
void audit_init(AuditLog *log);
StatusCode audit_record(
    AuditLog *log,
    AuditOpType op_type,
    const char *record_id,
    const char *description,
    const char *operator_user
);
void audit_display_recent(const AuditLog *log, int limit);
void audit_display_filtered_by_record(const AuditLog *log, const char *record_id);
const char* audit_op_to_string(AuditOpType type);
StatusCode audit_load_from_csv(AuditLog *log, const char *filepath);
StatusCode audit_save_to_csv(const AuditLog *log, const char *filepath);

#endif /* AUDIT_H */
