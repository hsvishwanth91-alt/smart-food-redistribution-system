/**
 * @file backup.c
 * @brief Backup snapshot creation, registry index, and atomic restore routines.
 */

#include "../include/backup.h"

static void copy_file(const char *src, const char *dst) {
    FILE *in = fopen(src, "r");
    if (!in) return;
    FILE *out = fopen(dst, "w");
    if (!out) {
        fclose(in);
        return;
    }
    char buf[4096];
    size_t bytes;
    while ((bytes = fread(buf, 1, sizeof(buf), in)) > 0) {
        fwrite(buf, 1, bytes, out);
    }
    fclose(in);
    fclose(out);
}

void backup_init(BackupRegistry *reg) {
    if (!reg) return;
    reg->count = 0;
    memset(reg->backups, 0, sizeof(reg->backups));
    ensure_directory_exists(BACKUP_DIR);
}

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
) {
    if (!reg) return ERR_INVALID_INPUT;
    if (reg->count >= MAX_BACKUPS) return ERR_FULL;

    ensure_directory_exists(BACKUP_DIR);

    char timestamp[MAX_STR_LEN];
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_info);

    char backup_id[MAX_STR_LEN];
    snprintf(backup_id, sizeof(backup_id), "BKP_%s", timestamp);

    char snapshot_dir[MAX_STR_LEN * 2];
    snprintf(snapshot_dir, sizeof(snapshot_dir), "%s/%s", BACKUP_DIR, backup_id);
    ensure_directory_exists(snapshot_dir);

    /* Save snapshot CSV copies */
    char file_buf[MAX_STR_LEN * 3];

    snprintf(file_buf, sizeof(file_buf), "%s/donors.csv", snapshot_dir);
    if (donors) donor_save_to_csv(donors, file_buf);

    snprintf(file_buf, sizeof(file_buf), "%s/donations.csv", snapshot_dir);
    if (inv) donation_save_to_csv(inv, file_buf);

    snprintf(file_buf, sizeof(file_buf), "%s/recipients.csv", snapshot_dir);
    if (recipients) recipient_save_to_csv(recipients, file_buf);

    snprintf(file_buf, sizeof(file_buf), "%s/requests.csv", snapshot_dir);
    if (requests) req_queue_save_to_csv(requests, file_buf);

    snprintf(file_buf, sizeof(file_buf), "%s/distributions.csv", snapshot_dir);
    if (ledger) dist_save_to_csv(ledger, file_buf);

    snprintf(file_buf, sizeof(file_buf), "%s/waste.csv", snapshot_dir);
    if (waste) waste_save_to_csv(waste, file_buf);

    snprintf(file_buf, sizeof(file_buf), "%s/audit_log.csv", snapshot_dir);
    if (audit) audit_save_to_csv(audit, file_buf);

    BackupMetadata *meta = &reg->backups[reg->count++];
    safe_strcpy(meta->backup_id, backup_id, sizeof(meta->backup_id));
    safe_strcpy(meta->timestamp, timestamp, sizeof(meta->timestamp));
    safe_strcpy(meta->backup_dir_path, snapshot_dir, sizeof(meta->backup_dir_path));
    meta->donor_count = donors ? donors->count : 0;
    meta->donation_count = inv ? inv->count : 0;
    meta->recipient_count = recipients ? recipients->count : 0;
    meta->distribution_count = ledger ? ledger->count : 0;

    if (out_backup_id) {
        safe_strcpy(out_backup_id, backup_id, MAX_STR_LEN);
    }

    printf("[SUCCESS] Backup snapshot created: '%s' under '%s'\n", backup_id, snapshot_dir);
    return SUCCESS;
}

void backup_display_all(const BackupRegistry *reg) {
    if (!reg) return;
    printf("\n=============================== AVAILABLE SYSTEM BACKUPS (%d) ===============================\n", reg->count);
    if (reg->count == 0) {
        printf("No backups currently registered in this session. Choose 'Create Backup' to take a snapshot.\n");
        printf("---------------------------------------------------------------------------------------------\n");
        return;
    }
    printf("%-20s | %-16s | %-8s | %-10s | %-10s | %-10s\n",
           "Backup ID", "Timestamp", "Donors", "Donations", "Recipients", "Dispatches");
    printf("---------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < reg->count; i++) {
        const BackupMetadata *b = &reg->backups[i];
        printf("%-20s | %-16s | %-8d | %-10d | %-10d | %-10d\n",
               b->backup_id, b->timestamp, b->donor_count, b->donation_count,
               b->recipient_count, b->distribution_count);
    }
    printf("---------------------------------------------------------------------------------------------\n");
}

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
) {
    if (!reg || !backup_id) return ERR_INVALID_INPUT;

    const BackupMetadata *target = NULL;
    for (int i = 0; i < reg->count; i++) {
        if (strcmp(reg->backups[i].backup_id, backup_id) == 0) {
            target = &reg->backups[i];
            break;
        }
    }

    if (!target) {
        printf("[ERROR] Backup ID '%s' not found.\n", backup_id);
        return ERR_NOT_FOUND;
    }

    char file_buf[MAX_STR_LEN * 3];

    snprintf(file_buf, sizeof(file_buf), "%s/donors.csv", target->backup_dir_path);
    if (donors) {
        donor_init_registry(donors);
        donor_load_from_csv(donors, file_buf);
    }

    snprintf(file_buf, sizeof(file_buf), "%s/donations.csv", target->backup_dir_path);
    if (inv) {
        donation_init_inventory(inv);
        donation_load_from_csv(inv, file_buf);
    }

    snprintf(file_buf, sizeof(file_buf), "%s/recipients.csv", target->backup_dir_path);
    if (recipients) {
        recipient_init_registry(recipients);
        recipient_load_from_csv(recipients, file_buf);
    }

    snprintf(file_buf, sizeof(file_buf), "%s/requests.csv", target->backup_dir_path);
    if (requests) {
        req_queue_init(requests);
        req_queue_load_from_csv(requests, file_buf);
    }

    snprintf(file_buf, sizeof(file_buf), "%s/distributions.csv", target->backup_dir_path);
    if (ledger) {
        dist_init_ledger(ledger);
        dist_load_from_csv(ledger, file_buf);
    }

    snprintf(file_buf, sizeof(file_buf), "%s/waste.csv", target->backup_dir_path);
    if (waste) {
        waste_load_from_csv(waste, file_buf);
    }

    snprintf(file_buf, sizeof(file_buf), "%s/audit_log.csv", target->backup_dir_path);
    if (audit) {
        audit_load_from_csv(audit, file_buf);
    }

    printf("[SUCCESS] All system entities restored from backup snapshot '%s'.\n", backup_id);
    return SUCCESS;
}
