/**
 * @file donor.c
 * @brief Donor management implementation with duplicate detection and CSV persistence.
 */

#include "../include/donor.h"

void donor_init_registry(DonorRegistry *reg) {
    if (!reg) return;
    reg->count = 0;
    memset(reg->donors, 0, sizeof(reg->donors));
}

StatusCode donor_register(DonorRegistry *reg, const Donor *d) {
    if (!reg || !d) return ERR_INVALID_INPUT;
    if (reg->count >= MAX_DONORS) return ERR_FULL;

    /* Check ID uniqueness */
    if (donor_find_by_id(reg, d->donor_id) != NULL) {
        return ERR_DUPLICATE;
    }

    /* Check duplicate contact */
    if (donor_find_by_contact(reg, d->contact) != -1) {
        printf("[WARNING] A donor with contact '%s' already exists in the system!\n", d->contact);
    }

    reg->donors[reg->count] = *d;
    reg->count++;
    return SUCCESS;
}

Donor* donor_find_by_id(DonorRegistry *reg, const char *donor_id) {
    if (!reg || !donor_id) return NULL;
    for (int i = 0; i < reg->count; i++) {
        if (strcmp(reg->donors[i].donor_id, donor_id) == 0) {
            return &reg->donors[i];
        }
    }
    return NULL;
}

int donor_find_by_contact(const DonorRegistry *reg, const char *contact) {
    if (!reg || !contact) return -1;
    for (int i = 0; i < reg->count; i++) {
        if (strcmp(reg->donors[i].contact, contact) == 0) {
            return i;
        }
    }
    return -1;
}

int donor_check_duplicates(const DonorRegistry *reg, const char *name, const char *contact) {
    if (!reg) return 0;
    int dup_count = 0;
    for (int i = 0; i < reg->count; i++) {
        bool match_name = (name && strcasecmp(reg->donors[i].name, name) == 0);
        bool match_contact = (contact && strcmp(reg->donors[i].contact, contact) == 0);
        if (match_name || match_contact) {
            dup_count++;
            printf("  -> Potential Duplicate: ID: %s | Name: %s | Contact: %s\n",
                   reg->donors[i].donor_id, reg->donors[i].name, reg->donors[i].contact);
        }
    }
    return dup_count;
}

void donor_display_single(const Donor *d) {
    if (!d) return;
    printf("+-------------------------------------------------------------------------+\n");
    printf("| Donor ID:      %-56s |\n", d->donor_id);
    printf("| Name:          %-56s |\n", d->name);
    printf("| Organization:  %-56s |\n", d->organization);
    printf("| Contact:       %-56s |\n", d->contact);
    printf("| Email:         %-56s |\n", d->email);
    printf("| Address:       %-56s |\n", d->address);
    printf("| Registered:    %-56s |\n", d->registered_date);
    printf("| Total Donations: %-4d | Total Weight: %-10.2f kg | Active: %-7s |\n",
           d->total_donations_count, d->total_donated_weight, d->is_active ? "YES" : "NO");
    printf("+-------------------------------------------------------------------------+\n");
}

void donor_display_all(const DonorRegistry *reg) {
    if (!reg) return;
    printf("\n================================ DONOR REGISTRY (%d Records) ================================\n", reg->count);
    printf("%-10s | %-20s | %-18s | %-14s | %-8s | %-10s\n",
           "ID", "Name", "Organization", "Phone", "Donations", "Status");
    printf("----------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < reg->count; i++) {
        const Donor *d = &reg->donors[i];
        printf("%-10s | %-20.20s | %-18.18s | %-14s | %-8d | %-10s\n",
               d->donor_id, d->name, d->organization, d->contact,
               d->total_donations_count, d->is_active ? "ACTIVE" : "INACTIVE");
    }
    printf("----------------------------------------------------------------------------------------------\n");
}

StatusCode donor_save_to_csv(const DonorRegistry *reg, const char *filepath) {
    if (!reg || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    fprintf(fp, "donor_id,name,organization,contact,email,address,registered_date,total_donations,total_weight,is_active\n");
    for (int i = 0; i < reg->count; i++) {
        const Donor *d = &reg->donors[i];
        fprintf(fp, "%s,%s,%s,%s,%s,%s,%s,%d,%.2f,%d\n",
                d->donor_id, d->name, d->organization, d->contact,
                d->email, d->address, d->registered_date,
                d->total_donations_count, d->total_donated_weight, d->is_active ? 1 : 0);
    }
    fclose(fp);
    return SUCCESS;
}

StatusCode donor_load_from_csv(DonorRegistry *reg, const char *filepath) {
    if (!reg || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "r");
    if (!fp) return ERR_NOT_FOUND;

    char line[1024];
    /* Skip header line */
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return SUCCESS;
    }

    while (fgets(line, sizeof(line), fp) && reg->count < MAX_DONORS) {
        trim_whitespace(line);
        if (strlen(line) == 0) continue;

        Donor d = {0};
        int is_act = 1;
        char *token = strtok(line, ",");
        if (token) safe_strcpy(d.donor_id, token, sizeof(d.donor_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.name, token, sizeof(d.name));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.organization, token, sizeof(d.organization));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.contact, token, sizeof(d.contact));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.email, token, sizeof(d.email));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.address, token, sizeof(d.address));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(d.registered_date, token, sizeof(d.registered_date));

        token = strtok(NULL, ",");
        if (token) d.total_donations_count = atoi(token);

        token = strtok(NULL, ",");
        if (token) d.total_donated_weight = atof(token);

        token = strtok(NULL, ",");
        if (token) is_act = atoi(token);
        d.is_active = (is_act != 0);

        reg->donors[reg->count++] = d;
    }

    fclose(fp);
    return SUCCESS;
}
