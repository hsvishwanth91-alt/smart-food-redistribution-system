/**
 * @file backup.h
 * @brief Backup and Restore operations for dataset persistence and disaster recovery.
 */

#ifndef BACKUP_H
#define BACKUP_H

#include "common.h"
#include "donor.h"
#include "donation.h"
#include "recipient.h"
#include "request_queue.h"
#include "distribution.h"
#include "analytics.h"
#include "audit.h"

typedef struct {
    char backup_id[MAX_STR_LEN];
    char timestamp[MAX_STR_LEN];
    char backup_dir_path[MAX_STR_LEN];
    int donor_count;
    int donation_count;
    int recipient_count;
    int distribution_count;
} BackupMetadata;

typedef struct {
    BackupMetadata backups[MAX_BACKUPS];
    int count;
} BackupRegistry;

/* Operations */
void backup_init(BackupRegistry *reg);
StatusCode backup_create_snapshot(
    BackupRegistry *reg,
    const DonorRegistry *donors,
    const DonationInventory *inv,
    const RecipientRegistry *recipients,
    const RequestQueue *requests,
    const DistributionLedger *ledger,
    const WasteLedger *waste,
    const AuditLog *audit,
    char *out_backup_id
);
void backup_display_all(const BackupRegistry *reg);
StatusCode backup_restore_snapshot(
    const BackupRegistry *reg,
    const char *backup_id,
    DonorRegistry *donors,
    DonationInventory *inv,
    RecipientRegistry *recipients,
    RequestQueue *requests,
    DistributionLedger *ledger,
    WasteLedger *waste,
    AuditLog *audit
);

#endif /* BACKUP_H */
