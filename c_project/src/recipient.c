/**
 * @file recipient.c
 * @brief Recipient registry, duplicate detection, and CSV persistence.
 */

#include "../include/recipient.h"

void recipient_init_registry(RecipientRegistry *reg) {
    if (!reg) return;
    reg->count = 0;
    memset(reg->recipients, 0, sizeof(reg->recipients));
}

StatusCode recipient_register(RecipientRegistry *reg, const Recipient *r) {
    if (!reg || !r) return ERR_INVALID_INPUT;
    if (reg->count >= MAX_RECIPIENTS) return ERR_FULL;

    /* Validate unique ID */
    if (recipient_find_by_id(reg, r->recipient_id) != NULL) {
        return ERR_DUPLICATE;
    }

    if (recipient_find_by_contact(reg, r->phone) != -1) {
        printf("[WARNING] A recipient with phone '%s' already exists in the system!\n", r->phone);
    }

    reg->recipients[reg->count] = *r;
    reg->count++;
    return SUCCESS;
}

Recipient* recipient_find_by_id(RecipientRegistry *reg, const char *recipient_id) {
    if (!reg || !recipient_id) return NULL;
    for (int i = 0; i < reg->count; i++) {
        if (strcmp(reg->recipients[i].recipient_id, recipient_id) == 0) {
            return &reg->recipients[i];
        }
    }
    return NULL;
}

int recipient_find_by_contact(const RecipientRegistry *reg, const char *contact) {
    if (!reg || !contact) return -1;
    for (int i = 0; i < reg->count; i++) {
        if (strcmp(reg->recipients[i].phone, contact) == 0) {
            return i;
        }
    }
    return -1;
}

int recipient_check_duplicates(const RecipientRegistry *reg, const char *org_name, const char *contact) {
    if (!reg) return 0;
    int dup_count = 0;
    for (int i = 0; i < reg->count; i++) {
        bool match_name = (org_name && strcasecmp(reg->recipients[i].organization_name, org_name) == 0);
        bool match_contact = (contact && strcmp(reg->recipients[i].phone, contact) == 0);
        if (match_name || match_contact) {
            dup_count++;
            printf("  -> Potential Duplicate: ID: %s | Org: %s | Phone: %s\n",
                   reg->recipients[i].recipient_id, reg->recipients[i].organization_name, reg->recipients[i].phone);
        }
    }
    return dup_count;
}

void recipient_display_single(const Recipient *r) {
    if (!r) return;
    printf("+-------------------------------------------------------------------------+\n");
    printf("| Recipient ID:   %-55s |\n", r->recipient_id);
    printf("| Organization:   %-55s |\n", r->organization_name);
    printf("| Contact Person: %-55s |\n", r->contact_person);
    printf("| Phone:          %-55s |\n", r->phone);
    printf("| Email:          %-55s |\n", r->email);
    printf("| Address:        %-55s |\n", r->address);
    printf("| Beneficiaries:  %-5d | Dietary Needs: %-32s |\n", r->beneficiaries_count, r->dietary_requirements);
    printf("| Requests Made:  %-5d | Fulfilled:     %-5d | Verified: %-15s |\n",
           r->total_requests_made, r->fulfilled_requests_count, r->is_verified ? "YES" : "NO");
    printf("+-------------------------------------------------------------------------+\n");
}

void recipient_display_all(const RecipientRegistry *reg) {
    if (!reg) return;
    printf("\n============================= RECIPIENT REGISTRY (%d Records) =============================\n", reg->count);
    printf("%-10s | %-22s | %-16s | %-14s | %-8s | %-10s\n",
           "ID", "Organization", "Contact Person", "Phone", "Benefic.", "Status");
    printf("-------------------------------------------------------------------------------------------\n");
    for (int i = 0; i < reg->count; i++) {
        const Recipient *r = &reg->recipients[i];
        printf("%-10s | %-22.22s | %-16.16s | %-14s | %-8d | %-10s\n",
               r->recipient_id, r->organization_name, r->contact_person,
               r->phone, r->beneficiaries_count, r->is_verified ? "VERIFIED" : "PENDING");
    }
    printf("-------------------------------------------------------------------------------------------\n");
}

StatusCode recipient_save_to_csv(const RecipientRegistry *reg, const char *filepath) {
    if (!reg || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "w");
    if (!fp) return ERR_FILE_IO;

    fprintf(fp, "recipient_id,org_name,contact_person,phone,email,address,beneficiaries,dietary,requests_made,fulfilled,verified\n");
    for (int i = 0; i < reg->count; i++) {
        const Recipient *r = &reg->recipients[i];
        fprintf(fp, "%s,%s,%s,%s,%s,%s,%d,%s,%d,%d,%d\n",
                r->recipient_id, r->organization_name, r->contact_person,
                r->phone, r->email, r->address, r->beneficiaries_count,
                r->dietary_requirements, r->total_requests_made,
                r->fulfilled_requests_count, r->is_verified ? 1 : 0);
    }
    fclose(fp);
    return SUCCESS;
}

StatusCode recipient_load_from_csv(RecipientRegistry *reg, const char *filepath) {
    if (!reg || !filepath) return ERR_INVALID_INPUT;
    FILE *fp = fopen(filepath, "r");
    if (!fp) return ERR_NOT_FOUND;

    char line[1024];
    if (!fgets(line, sizeof(line), fp)) {
        fclose(fp);
        return SUCCESS;
    }

    while (fgets(line, sizeof(line), fp) && reg->count < MAX_RECIPIENTS) {
        trim_whitespace(line);
        if (strlen(line) == 0) continue;

        Recipient r = {0};
        char *token = strtok(line, ",");
        if (token) safe_strcpy(r.recipient_id, token, sizeof(r.recipient_id));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.organization_name, token, sizeof(r.organization_name));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.contact_person, token, sizeof(r.contact_person));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.phone, token, sizeof(r.phone));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.email, token, sizeof(r.email));

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.address, token, sizeof(r.address));

        token = strtok(NULL, ",");
        if (token) r.beneficiaries_count = atoi(token);

        token = strtok(NULL, ",");
        if (token) safe_strcpy(r.dietary_requirements, token, sizeof(r.dietary_requirements));

        token = strtok(NULL, ",");
        if (token) r.total_requests_made = atoi(token);

        token = strtok(NULL, ",");
        if (token) r.fulfilled_requests_count = atoi(token);

        token = strtok(NULL, ",");
        if (token) r.is_verified = (atoi(token) != 0);

        reg->recipients[reg->count++] = r;
    }

    fclose(fp);
    return SUCCESS;
}
