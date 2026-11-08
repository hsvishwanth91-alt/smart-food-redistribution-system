/**
 * @file main.c
 * @brief Main Menu-Driven Terminal Application for Smart Food Redistribution System.
 *        Professional DSA Edition with Binary Heap, Hash Table, Circular Queue,
 *        Multi-Donation Allocation Engine, and Audit Persistence.
 */

#include "../include/common.h"
#include "../include/donor.h"
#include "../include/donation.h"
#include "../include/recipient.h"
#include "../include/request_queue.h"
#include "../include/priority_queue.h"
#include "../include/hash_table.h"
#include "../include/allocation.h"
#include "../include/distribution.h"
#include "../include/expiry.h"
#include "../include/analytics.h"
#include "../include/audit.h"
#include "../include/backup.h"
#include "../include/search.h"
#include "../include/demo.h"

/* Global System State */
static DonorRegistry          g_donors;
static DonationInventory       g_inventory;
static RecipientRegistry       g_recipients;
static RequestQueue            g_requests;
static DistributionLedger      g_distributions;
static WasteLedger             g_waste;
static DonationPriorityQueue   g_priority_queue;
static HashTable               g_hash_table;
static BackupRegistry          g_backups;
static ExpiryConfig            g_expiry_cfg;

static void rebuild_all_indexes(void) {
    ht_clear(&g_hash_table);
    for (int i = 0; i < g_donors.count; i++) {
        ht_insert(&g_hash_table, g_donors.donors[i].donor_id, RECORD_DONOR, i);
    }
    for (int i = 0; i < g_inventory.count; i++) {
        ht_insert(&g_hash_table, g_inventory.donations[i].donation_id, RECORD_DONATION, i);
    }
    for (int i = 0; i < g_recipients.count; i++) {
        ht_insert(&g_hash_table, g_recipients.recipients[i].recipient_id, RECORD_RECIPIENT, i);
    }
    pq_rebuild_from_inventory(&g_priority_queue, &g_inventory);
    donation_refresh_urgencies(&g_inventory);
}

static void save_all_data(void) {
    ensure_directory_exists("data");
    donor_save_to_csv(&g_donors, FILE_DONORS);
    donation_save_to_csv(&g_inventory, FILE_DONATIONS);
    recipient_save_to_csv(&g_recipients, FILE_RECIPIENTS);
    req_queue_save_to_csv(&g_requests, FILE_REQUESTS);
    dist_save_to_csv(&g_distributions, FILE_DISTRIBUTIONS);
    waste_save_to_csv(&g_waste, FILE_WASTE);
    audit_save_to_csv(&g_audit_log, FILE_AUDIT);
    printf("[INFO] All database records committed to persistent CSV files under 'data/'.\n");
}

static void load_all_data(void) {
    ensure_directory_exists("data");
    donor_init_registry(&g_donors);
    donation_init_inventory(&g_inventory);
    recipient_init_registry(&g_recipients);
    req_queue_init(&g_requests);
    dist_init_ledger(&g_distributions);
    waste_load_from_csv(&g_waste, FILE_WASTE);
    audit_init(&g_audit_log);
    audit_load_from_csv(&g_audit_log, FILE_AUDIT);

    donor_load_from_csv(&g_donors, FILE_DONORS);
    donation_load_from_csv(&g_inventory, FILE_DONATIONS);
    recipient_load_from_csv(&g_recipients, FILE_RECIPIENTS);
    req_queue_load_from_csv(&g_requests, FILE_REQUESTS);
    dist_load_from_csv(&g_distributions, FILE_DISTRIBUTIONS);

    /* If datasets are empty, prompt evaluator or auto-seed demo */
    if (g_donors.count == 0 && g_inventory.count == 0) {
        printf("[NOTICE] No prior data files detected. Initializing with Evaluator Demo Dataset...\n");
        demo_load_dataset(&g_donors, &g_inventory, &g_recipients, &g_requests,
                          &g_distributions, &g_audit_log, &g_priority_queue);
        save_all_data();
    } else {
        rebuild_all_indexes();
    }
}

/* Submenu Handlers */

static void handle_smart_allocation(void) {
    printf("\n==================== SMART FOOD ALLOCATION ENGINE ====================\n");
    if (req_queue_is_empty(&g_requests)) {
        printf("No requests pending in the queue. Enqueue a recipient request first.\n");
        return;
    }

    req_queue_display(&g_requests);

    char req_id[MAX_STR_LEN];
    printf("\nEnter Request ID to match (or press ENTER to process FIFO Queue Front '%s'): ",
           g_requests.items[g_requests.front].request_id);
    if (fgets(req_id, sizeof(req_id), stdin)) {
        trim_whitespace(req_id);
    }

    FoodRequest *target_req = NULL;
    if (strlen(req_id) == 0) {
        target_req = req_queue_peek(&g_requests);
    } else {
        target_req = req_queue_find_by_id(&g_requests, req_id);
    }

    if (!target_req) {
        printf("[ERROR] Request not found in queue.\n");
        return;
    }

    if (target_req->status == REQ_FULFILLED) {
        printf("[INFO] Request '%s' is already 100%% fulfilled.\n", target_req->request_id);
        return;
    }

    AllocationPlan plan;
    StatusCode st = generate_smart_allocation_plan(&g_inventory, target_req, &plan);

    if (st == ERR_NOT_FOUND) {
        printf("\n[ALLOCATION RESULT] %s\n", plan.policy_summary);
        return;
    }

    display_allocation_plan(&plan);

    char confirm[16];
    printf("\nOperator Action: Confirm and commit this allocation? (y/N): ");
    if (fgets(confirm, sizeof(confirm), stdin)) {
        trim_whitespace(confirm);
        if (confirm[0] == 'y' || confirm[0] == 'Y') {
            char notes[MAX_NOTES_LEN];
            printf("Enter operator verification note (optional): ");
            if (fgets(notes, sizeof(notes), stdin)) {
                trim_whitespace(notes);
            }
            confirm_and_execute_allocation(&g_inventory, &g_requests, &g_distributions, &plan, notes);
            rebuild_all_indexes();
            save_all_data();
        } else {
            printf("[CANCELLED] Allocation aborted by operator. No stock modified.\n");
        }
    }
}

static void handle_request_queue_ops(void) {
    int choice = -1;
    do {
        printf("\n------------- RECIPIENT REQUEST QUEUE (FIFO) -------------\n");
        printf("1. View Current Queue State (Front, Rear, Capacity)\n");
        printf("2. Enqueue New Recipient Request\n");
        printf("3. Dequeue Next Completed/Processed Request\n");
        printf("4. Peek Queue Front\n");
        printf("0. Return to Main Menu\n");
        printf("Select choice [0-4]: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                req_queue_display(&g_requests);
                break;
            case 2: {
                FoodRequest r = {0};
                snprintf(r.request_id, sizeof(r.request_id), "REQ-%ld", (long)time(NULL) % 10000);
                printf("\n--- Enqueue Recipient Request (Generated ID: %s) ---\n", r.request_id);

                recipient_display_all(&g_recipients);
                printf("Enter Recipient ID: ");
                if (fgets(r.recipient_id, sizeof(r.recipient_id), stdin)) trim_whitespace(r.recipient_id);

                printf("Select Food Category:\n");
                printf("  0. Grains & Staples\n  1. Cooked Meals\n  2. Dairy & Eggs\n  3. Fresh Produce\n  4. Bakery\n  5. Canned & Packaged\n  6. Beverages\n");
                printf("Choice [0-6]: ");
                int cat_choice = 0;
                if (scanf("%d", &cat_choice) == 1) r.preferred_category = (FoodCategory)cat_choice;
                clear_input_buffer();

                printf("Enter Requested Quantity: ");
                if (scanf("%lf", &r.requested_quantity) != 1 || r.requested_quantity <= 0.0) {
                    clear_input_buffer();
                    printf("[ERROR] Quantity must be positive.\n");
                    break;
                }
                clear_input_buffer();
                r.remaining_quantity = r.requested_quantity;
                r.fulfilled_quantity = 0.0;

                printf("Select Food Unit:\n  0. kg\n  1. g\n  2. packets\n  3. pieces\n  4. liters\nChoice [0-4]: ");
                int unit_choice = 0;
                if (scanf("%d", &unit_choice) == 1) r.unit = (FoodUnit)unit_choice;
                clear_input_buffer();

                printf("Select Priority Level:\n  1. Standard\n  2. Medium\n  3. High\n  4. Emergency\nChoice [1-4]: ");
                int prio = 1;
                if (scanf("%d", &prio) == 1 && prio >= 1 && prio <= 4) r.priority = (RequestPriority)prio;
                clear_input_buffer();

                get_current_date_str(r.request_date, sizeof(r.request_date));
                safe_strcpy(r.required_by_date, r.request_date, sizeof(r.required_by_date));
                r.status = REQ_PENDING;

                printf("Enter Specific Notes: ");
                if (fgets(r.notes, sizeof(r.notes), stdin)) trim_whitespace(r.notes);

                if (req_queue_enqueue(&g_requests, &r) == SUCCESS) {
                    printf("[SUCCESS] Request '%s' enqueued successfully.\n", r.request_id);
                    audit_record(&g_audit_log, OP_REQUEST_CREATE, r.request_id, "New Request Enqueued", "Operator");
                    save_all_data();
                }
                break;
            }
            case 3: {
                FoodRequest popped;
                if (req_queue_dequeue(&g_requests, &popped) == SUCCESS) {
                    printf("[SUCCESS] Dequeued Request '%s' for Recipient '%s' (Status: %s).\n",
                           popped.request_id, popped.recipient_id, request_status_to_string(popped.status));
                    save_all_data();
                }
                break;
            }
            case 4: {
                FoodRequest *front = req_queue_peek(&g_requests);
                if (front) {
                    printf("\n[PEEK FRONT ELEMENT]: ID: %s | Recipient: %s | Qty Needed: %.1f %s | Priority: %d\n",
                           front->request_id, front->recipient_id, front->remaining_quantity,
                           unit_to_string(front->unit), (int)front->priority);
                } else {
                    printf("Queue is currently empty.\n");
                }
                break;
            }
        }
    } while (choice != 0);
}

static void handle_priority_queue_ops(void) {
    int choice = -1;
    do {
        printf("\n------------- DONATION PRIORITY QUEUE (BINARY MIN-HEAP) -------------\n");
        printf("1. View Current Binary Heap State\n");
        printf("2. Peek Root (Highest Urgency / Earliest Expiry Batch)\n");
        printf("3. Extract Root (Extract-Min O(log N))\n");
        printf("4. Rebuild Heap from Live Inventory (O(N) Heapify)\n");
        printf("5. Display Heap Algorithm Complexity & Proof\n");
        printf("0. Return to Main Menu\n");
        printf("Select choice [0-5]: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                pq_display(&g_priority_queue);
                break;
            case 2: {
                const HeapNode *root = pq_peek(&g_priority_queue);
                if (root) {
                    printf("\n[HEAP ROOT (O(1) Peek)]:\n");
                    printf("  Donation ID:      %s\n", root->donation_id);
                    printf("  Category:         %s\n", category_to_string(root->category));
                    printf("  Available Qty:    %.1f %s\n", root->available_quantity, unit_to_string(root->unit));
                    printf("  Days to Expiry:   %.1f days\n", root->days_to_expiry);
                } else {
                    printf("Priority Queue is empty.\n");
                }
                break;
            }
            case 3: {
                HeapNode extracted;
                if (pq_extract_min(&g_priority_queue, &extracted) == SUCCESS) {
                    printf("\n[EXTRACT-MIN SUCCESS (O(log N))]:\n");
                    printf("  Extracted Donation: %s | Expiry Days Left: %.1f | Qty: %.1f %s\n",
                           extracted.donation_id, extracted.days_to_expiry,
                           extracted.available_quantity, unit_to_string(extracted.unit));
                    printf("  Remaining Heap Size: %d\n", g_priority_queue.size);
                } else {
                    printf("Priority Queue is empty.\n");
                }
                break;
            }
            case 4:
                pq_rebuild_from_inventory(&g_priority_queue, &g_inventory);
                printf("[SUCCESS] Priority Queue rebuilt with %d active batches.\n", g_priority_queue.size);
                break;
            case 5:
                pq_explain_complexity();
                break;
        }
    } while (choice != 0);
}

static void handle_expiry_ops(void) {
    int choice = -1;
    do {
        printf("\n---------------- FOOD EXPIRY MANAGEMENT MODULE ----------------\n");
        printf("1. Display Expiry Dashboard & Breakdown\n");
        printf("2. List Expired Batches (Quarantine Required)\n");
        printf("3. List Critical Urgency Batches (<24 Hours Left)\n");
        printf("4. List Near-Expiry Batches (<3 Days Left)\n");
        printf("5. Configure Thresholds (Current: Critical=%.0fh, Near=%.0fd)\n",
               g_expiry_cfg.critical_hours_threshold, g_expiry_cfg.near_expiry_days_threshold);
        printf("6. Sort Inventory by Earliest Expiry Date\n");
        printf("7. Display Food Safety Governance Disclaimer\n");
        printf("0. Return to Main Menu\n");
        printf("Select choice [0-7]: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                expiry_display_dashboard(&g_inventory, &g_expiry_cfg);
                break;
            case 2:
                expiry_filter_and_display(&g_inventory, EXPIRY_EXPIRED);
                break;
            case 3:
                expiry_filter_and_display(&g_inventory, EXPIRY_CRITICAL);
                break;
            case 4:
                expiry_filter_and_display(&g_inventory, EXPIRY_NEAR_EXPIRY);
                break;
            case 5: {
                double hours, days;
                printf("Enter new Critical Threshold in HOURS (e.g. 24.0): ");
                if (scanf("%lf", &hours) != 1) hours = 24.0;
                printf("Enter new Near Expiry Threshold in DAYS (e.g. 3.0): ");
                if (scanf("%lf", &days) != 1) days = 3.0;
                clear_input_buffer();
                expiry_update_thresholds(&g_expiry_cfg, hours, days);
                break;
            }
            case 6:
                donation_sort_by_expiry(&g_inventory, true);
                printf("[SUCCESS] Inventory sorted by Earliest Expiry (Ascending).\n");
                donation_display_all(&g_inventory);
                break;
            case 7:
                expiry_print_safety_disclaimer();
                break;
        }
    } while (choice != 0);
}

static void handle_rollback_ops(void) {
    printf("\n================ ALLOCATION CANCELLATION & ROLLBACK ================\n");
    if (g_distributions.count == 0) {
        printf("No distribution records currently on file.\n");
        return;
    }

    dist_display_all(&g_distributions);

    char dist_id[MAX_STR_LEN];
    printf("\nEnter Distribution ID to cancel and rollback: ");
    if (fgets(dist_id, sizeof(dist_id), stdin)) trim_whitespace(dist_id);

    DistributionRecord *rec = dist_find_by_id(&g_distributions, dist_id);
    if (!rec) {
        printf("[ERROR] Record '%s' not found.\n", dist_id);
        return;
    }

    dist_display_single(rec);

    char confirm[16];
    printf("\nAre you SURE you want to restore stock and reverse this transaction? (y/N): ");
    if (fgets(confirm, sizeof(confirm), stdin)) {
        trim_whitespace(confirm);
        if (confirm[0] == 'y' || confirm[0] == 'Y') {
            char reason[MAX_NOTES_LEN];
            printf("Enter reason for cancellation: ");
            if (fgets(reason, sizeof(reason), stdin)) trim_whitespace(reason);

            dist_cancel_and_rollback(&g_distributions, &g_inventory, &g_requests, dist_id, reason);
            rebuild_all_indexes();
            save_all_data();
        } else {
            printf("[ABORTED] Rollback cancelled by user.\n");
        }
    }
}

static void handle_waste_ops(void) {
    int choice = -1;
    do {
        printf("\n------------- FOOD WASTE PREVENTION & CONFIRMED WASTE -------------\n");
        printf("1. View Confirmed Waste Ledger\n");
        printf("2. Log Confirmed Waste (Operator Verification Required)\n");
        printf("3. Display System Loss Prevention KPIs\n");
        printf("0. Return to Main Menu\n");
        printf("Select choice [0-3]: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                analytics_display_waste_ledger(&g_waste);
                break;
            case 2: {
                donation_display_all(&g_inventory);
                char don_id[MAX_STR_LEN];
                printf("Enter Donation ID to mark spoiled/waste: ");
                if (fgets(don_id, sizeof(don_id), stdin)) trim_whitespace(don_id);

                double qty = 0.0;
                printf("Enter Wasted Quantity: ");
                if (scanf("%lf", &qty) != 1 || qty <= 0.0) {
                    clear_input_buffer();
                    printf("[ERROR] Invalid quantity.\n");
                    break;
                }
                clear_input_buffer();

                char reason[MAX_NOTES_LEN];
                printf("Enter physical inspection failure reason: ");
                if (fgets(reason, sizeof(reason), stdin)) trim_whitespace(reason);

                char op[MAX_STR_LEN];
                printf("Enter Operator Name confirming waste: ");
                if (fgets(op, sizeof(op), stdin)) trim_whitespace(op);

                analytics_record_confirmed_waste(&g_waste, &g_inventory, don_id, qty, reason, op);
                rebuild_all_indexes();
                save_all_data();
                break;
            }
            case 3: {
                SystemAnalyticsSummary summary;
                analytics_compute_summary(&g_donors, &g_recipients, &g_inventory,
                                          &g_requests, &g_distributions, &g_waste, &summary);
                analytics_display_dashboard(&summary);
                break;
            }
        }
    } while (choice != 0);
}

static void handle_inventory_ops(void) {
    int choice = -1;
    do {
        printf("\n----------------- DONATION INVENTORY MANAGEMENT -----------------\n");
        printf("1. View All Food Donations (%d Batches)\n", g_inventory.count);
        printf("2. Register New Food Donation Batch\n");
        printf("3. Sort Inventory by Expiry Date (Ascending)\n");
        printf("4. Sort Inventory by Available Quantity\n");
        printf("5. Sort Inventory by Urgency Priority Score\n");
        printf("0. Return to Main Menu\n");
        printf("Select choice [0-5]: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                donation_display_all(&g_inventory);
                break;
            case 2: {
                FoodDonation d = {0};
                snprintf(d.donation_id, sizeof(d.donation_id), "DON-%ld", (long)time(NULL) % 10000);
                printf("\n--- Register New Donation (ID: %s) ---\n", d.donation_id);

                donor_display_all(&g_donors);
                printf("Enter Donor ID: ");
                if (fgets(d.donor_id, sizeof(d.donor_id), stdin)) trim_whitespace(d.donor_id);

                printf("Enter Food Item Name: ");
                if (fgets(d.food_name, sizeof(d.food_name), stdin)) trim_whitespace(d.food_name);

                printf("Select Category (0-7):\n");
                printf("  0. Grains & Staples  1. Cooked Meals  2. Dairy & Eggs  3. Fresh Produce\n  4. Bakery  5. Canned  6. Beverages  7. Other\nChoice: ");
                int cat = 0;
                if (scanf("%d", &cat) == 1) d.category = (FoodCategory)cat;
                clear_input_buffer();

                printf("Enter Initial Quantity: ");
                if (scanf("%lf", &d.initial_quantity) != 1 || d.initial_quantity <= 0.0) {
                    clear_input_buffer();
                    printf("[ERROR] Quantity must be positive.\n");
                    break;
                }
                clear_input_buffer();
                d.available_quantity = d.initial_quantity;

                printf("Select Unit (0:kg, 1:g, 2:packets, 3:pieces, 4:liters): ");
                int u = 0;
                if (scanf("%d", &u) == 1) d.unit = (FoodUnit)u;
                clear_input_buffer();

                get_current_date_str(d.prep_date, sizeof(d.prep_date));
                printf("Enter Expiry Date (YYYY-MM-DD): ");
                if (fgets(d.expiry_date, sizeof(d.expiry_date), stdin)) trim_whitespace(d.expiry_date);

                if (!is_valid_date_format(d.expiry_date)) {
                    printf("[ERROR] Invalid date format. Must be YYYY-MM-DD.\n");
                    break;
                }

                printf("Enter Storage Type (e.g. Ambient, Refrigerated, Frozen): ");
                if (fgets(d.storage_type, sizeof(d.storage_type), stdin)) trim_whitespace(d.storage_type);

                printf("Enter Notes: ");
                if (fgets(d.notes, sizeof(d.notes), stdin)) trim_whitespace(d.notes);

                d.status = STATUS_AVAILABLE;
                if (donation_add(&g_inventory, &d) == SUCCESS) {
                    printf("[SUCCESS] Food donation '%s' registered.\n", d.donation_id);
                    audit_record(&g_audit_log, OP_DONATION_CREATE, d.donation_id, "New Donation Registered", "Operator");
                    rebuild_all_indexes();
                    save_all_data();
                }
                break;
            }
            case 3:
                donation_sort_by_expiry(&g_inventory, true);
                printf("[SUCCESS] Sorted by earliest expiry date.\n");
                donation_display_all(&g_inventory);
                break;
            case 4:
                donation_sort_by_quantity(&g_inventory, false);
                printf("[SUCCESS] Sorted by available quantity (Descending).\n");
                donation_display_all(&g_inventory);
                break;
            case 5:
                donation_sort_by_urgency_score(&g_inventory);
                printf("[SUCCESS] Sorted by Urgency Score.\n");
                donation_display_all(&g_inventory);
                break;
        }
    } while (choice != 0);
}

static void handle_donor_recipient_ops(void) {
    int choice = -1;
    do {
        printf("\n------------- DONOR & RECIPIENT MANAGEMENT -------------\n");
        printf("1. View Donor Registry (%d Donors)\n", g_donors.count);
        printf("2. Register New Donor (with Duplicate Detection)\n");
        printf("3. View Recipient Registry (%d Recipients)\n", g_recipients.count);
        printf("4. Register New Recipient (with Duplicate Detection)\n");
        printf("0. Return to Main Menu\n");
        printf("Select choice [0-4]: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                donor_display_all(&g_donors);
                break;
            case 2: {
                Donor d = {0};
                snprintf(d.donor_id, sizeof(d.donor_id), "DNR-%ld", (long)time(NULL) % 10000);
                printf("\n--- Register Donor (ID: %s) ---\n", d.donor_id);

                printf("Enter Name / Contact Person: ");
                if (fgets(d.name, sizeof(d.name), stdin)) trim_whitespace(d.name);

                printf("Enter Organization: ");
                if (fgets(d.organization, sizeof(d.organization), stdin)) trim_whitespace(d.organization);

                printf("Enter Phone Contact: ");
                if (fgets(d.contact, sizeof(d.contact), stdin)) trim_whitespace(d.contact);

                printf("Checking for duplicate entries...\n");
                int dups = donor_check_duplicates(&g_donors, d.name, d.contact);
                if (dups > 0) {
                    char ans[16];
                    printf("Proceed with registration despite possible duplicates? (y/N): ");
                    if (fgets(ans, sizeof(ans), stdin)) {
                        trim_whitespace(ans);
                        if (ans[0] != 'y' && ans[0] != 'Y') {
                            printf("[CANCELLED] Donor registration aborted.\n");
                            break;
                        }
                    }
                }

                printf("Enter Email: ");
                if (fgets(d.email, sizeof(d.email), stdin)) trim_whitespace(d.email);

                printf("Enter Address: ");
                if (fgets(d.address, sizeof(d.address), stdin)) trim_whitespace(d.address);

                get_current_date_str(d.registered_date, sizeof(d.registered_date));
                d.is_active = true;

                if (donor_register(&g_donors, &d) == SUCCESS) {
                    printf("[SUCCESS] Donor '%s' registered.\n", d.donor_id);
                    audit_record(&g_audit_log, OP_DONOR_REGISTER, d.donor_id, "Donor Registered", "Operator");
                    rebuild_all_indexes();
                    save_all_data();
                }
                break;
            }
            case 3:
                recipient_display_all(&g_recipients);
                break;
            case 4: {
                Recipient r = {0};
                snprintf(r.recipient_id, sizeof(r.recipient_id), "RCP-%ld", (long)time(NULL) % 10000);
                printf("\n--- Register Beneficiary Recipient (ID: %s) ---\n", r.recipient_id);

                printf("Enter Organization Name: ");
                if (fgets(r.organization_name, sizeof(r.organization_name), stdin)) trim_whitespace(r.organization_name);

                printf("Enter Contact Person: ");
                if (fgets(r.contact_person, sizeof(r.contact_person), stdin)) trim_whitespace(r.contact_person);

                printf("Enter Phone: ");
                if (fgets(r.phone, sizeof(r.phone), stdin)) trim_whitespace(r.phone);

                int dups = recipient_check_duplicates(&g_recipients, r.organization_name, r.phone);
                if (dups > 0) {
                    char ans[16];
                    printf("Proceed with registration despite possible duplicates? (y/N): ");
                    if (fgets(ans, sizeof(ans), stdin)) {
                        trim_whitespace(ans);
                        if (ans[0] != 'y' && ans[0] != 'Y') {
                            printf("[CANCELLED] Recipient registration aborted.\n");
                            break;
                        }
                    }
                }

                printf("Enter Email: ");
                if (fgets(r.email, sizeof(r.email), stdin)) trim_whitespace(r.email);

                printf("Enter Address: ");
                if (fgets(r.address, sizeof(r.address), stdin)) trim_whitespace(r.address);

                printf("Enter Number of Beneficiaries: ");
                if (scanf("%d", &r.beneficiaries_count) != 1) r.beneficiaries_count = 1;
                clear_input_buffer();

                printf("Enter Dietary Requirements: ");
                if (fgets(r.dietary_requirements, sizeof(r.dietary_requirements), stdin)) trim_whitespace(r.dietary_requirements);

                r.is_verified = true;
                if (recipient_register(&g_recipients, &r) == SUCCESS) {
                    printf("[SUCCESS] Recipient '%s' registered.\n", r.recipient_id);
                    audit_record(&g_audit_log, OP_RECIPIENT_REGISTER, r.recipient_id, "Recipient Registered", "Operator");
                    rebuild_all_indexes();
                    save_all_data();
                }
                break;
            }
        }
    } while (choice != 0);
}

static void handle_search_ops(void) {
    int choice = -1;
    do {
        printf("\n------------------- ADVANCED SEARCH & FILTERS -------------------\n");
        printf("1. Keyword Search in Food Donations (Name, ID, Notes)\n");
        printf("2. Filter Donations by Food Category\n");
        printf("3. Search Donors by Name / Organization\n");
        printf("4. Search Recipients by Name / Organization\n");
        printf("5. Search Distributions by Status\n");
        printf("0. Return to Main Menu\n");
        printf("Select choice [0-5]: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: {
                char kw[MAX_STR_LEN];
                printf("Enter keyword: ");
                if (fgets(kw, sizeof(kw), stdin)) trim_whitespace(kw);
                search_donations_by_keyword(&g_inventory, kw);
                break;
            }
            case 2: {
                printf("Select Category (0-7): ");
                int c = 0;
                if (scanf("%d", &c) == 1) {
                    search_donations_by_category(&g_inventory, (FoodCategory)c);
                }
                clear_input_buffer();
                break;
            }
            case 3: {
                char q[MAX_STR_LEN];
                printf("Enter donor name query: ");
                if (fgets(q, sizeof(q), stdin)) trim_whitespace(q);
                search_donors_by_name(&g_donors, q);
                break;
            }
            case 4: {
                char q[MAX_STR_LEN];
                printf("Enter recipient name query: ");
                if (fgets(q, sizeof(q), stdin)) trim_whitespace(q);
                search_recipients_by_name(&g_recipients, q);
                break;
            }
            case 5: {
                printf("Select Status (0:Allocated, 1:In-Transit, 2:Delivered, 3:Cancelled): ");
                int st = 0;
                if (scanf("%d", &st) == 1) {
                    search_distributions_by_status(&g_distributions, (DistributionStatus)st);
                }
                clear_input_buffer();
                break;
            }
        }
    } while (choice != 0);
}

static void handle_export_ops(void) {
    printf("\n=================== ADVANCED REPORT EXPORT ===================\n");
    printf("1. Export All Reports (Donations, Recipients, Distributions, Expiry, Waste, Executive Summary)\n");
    printf("2. Export Single Report\n");
    printf("0. Cancel\n");
    printf("Choice: ");
    int c = 0;
    if (scanf("%d", &c) != 1) {
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    if (c == 1) {
        ensure_directory_exists("reports");
        export_donations_csv(&g_inventory, "reports/donations_report.csv");
        export_recipients_csv(&g_recipients, "reports/recipients_report.csv");
        export_distributions_csv(&g_distributions, "reports/distributions_report.csv");
        export_expiry_report_txt(&g_inventory, "reports/expiry_audit_report.txt");
        export_waste_report_csv(&g_waste, "reports/waste_ledger_report.csv");

        SystemAnalyticsSummary sum;
        analytics_compute_summary(&g_donors, &g_recipients, &g_inventory,
                                  &g_requests, &g_distributions, &g_waste, &sum);
        export_full_summary_report_txt(&sum, "reports/executive_summary.txt");

        printf("[SUCCESS] Generated 6 export reports in directory 'reports/':\n");
        printf("  - reports/donations_report.csv      (Excel compatible)\n");
        printf("  - reports/recipients_report.csv     (Excel compatible)\n");
        printf("  - reports/distributions_report.csv  (Excel compatible)\n");
        printf("  - reports/expiry_audit_report.txt   (Text summary)\n");
        printf("  - reports/waste_ledger_report.csv   (Excel compatible)\n");
        printf("  - reports/executive_summary.txt     (KPI Executive Report)\n");
    }
}

static void handle_backup_ops(void) {
    int choice = -1;
    do {
        printf("\n------------- BACKUP & DISASTER RECOVERY -------------\n");
        printf("1. Create New Timestamped System Backup Snapshot\n");
        printf("2. Display Available Backup Snapshots\n");
        printf("3. Restore System from Snapshot (with Confirmation Safeguard)\n");
        printf("0. Return to Main Menu\n");
        printf("Select choice [0-3]: ");
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1: {
                char bkp_id[MAX_STR_LEN];
                if (backup_create_snapshot(&g_backups, &g_donors, &g_inventory,
                                           &g_recipients, &g_requests, &g_distributions,
                                           &g_waste, &g_audit_log, bkp_id) == SUCCESS) {
                    audit_record(&g_audit_log, OP_BACKUP_CREATED, bkp_id, "Backup snapshot generated", "Operator");
                }
                break;
            }
            case 2:
                backup_display_all(&g_backups);
                break;
            case 3: {
                backup_display_all(&g_backups);
                if (g_backups.count == 0) break;

                char bkp_id[MAX_STR_LEN];
                printf("Enter Backup ID to restore: ");
                if (fgets(bkp_id, sizeof(bkp_id), stdin)) trim_whitespace(bkp_id);

                char confirm[16];
                printf("[WARNING] Restoring will OVERWRITE current in-memory data with the snapshot!\n");
                printf("Confirm restore of '%s'? (y/N): ", bkp_id);
                if (fgets(confirm, sizeof(confirm), stdin)) {
                    trim_whitespace(confirm);
                    if (confirm[0] == 'y' || confirm[0] == 'Y') {
                        backup_restore_snapshot(&g_backups, bkp_id, &g_donors, &g_inventory,
                                                &g_recipients, &g_requests, &g_distributions,
                                                &g_waste, &g_audit_log);
                        rebuild_all_indexes();
                    } else {
                        printf("[ABORTED] Restore cancelled.\n");
                    }
                }
                break;
            }
        }
    } while (choice != 0);
}

static void display_system_dashboard(void) {
    SystemAnalyticsSummary sum;
    analytics_compute_summary(&g_donors, &g_recipients, &g_inventory,
                              &g_requests, &g_distributions, &g_waste, &sum);

    printf("\n");
    printf("╔═════════════════════════════════════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                   SMART FOOD REDISTRIBUTION SYSTEM — PROFESSIONAL DSA EDITION                   ║\n");
    printf("║                 Enterprise Food Redistribution, Dynamic Allocation & Audit System               ║\n");
    printf("╠═════════════════════════════════════════════════════════════════════════════════════════════════════════╣\n");
    printf("║  COMMUNITY NETWORK:     Donors: %-4d | Beneficiary Recipients: %-4d                              ║\n",
           sum.total_donors, sum.total_recipients);
    printf("║  FOOD INVENTORY:        Total Batches: %-4d | Active Available: %-4d | Expired/At-Risk: %-4d        ║\n",
           sum.total_donations, sum.active_donations, sum.expired_donations + sum.near_expiry_donations);
    printf("║  MASS METRICS:          Donated: %8.1f kg | Available: %8.1f kg | Allocated: %8.1f kg      ║\n",
           sum.total_donated_kg, sum.total_available_kg, sum.total_allocated_kg);
    printf("║  DEMAND PIPELINE:       Queued Requests: %-4d | Fulfilled: %-4d | Partial: %-4d | Pending: %-4d   ║\n",
           sum.total_requests, sum.fulfilled_requests, sum.partial_requests, sum.unfulfilled_requests);
    printf("║  EFFICIENCY:            Waste Diversion Rate: %5.1f%% | Confirmed Loss: %7.1f kg                   ║\n",
           sum.redistribution_efficiency_rate, sum.total_wasted_kg);
    printf("║  DATA STRUCTURES ACTIVE: Binary Min-Heap (size %-3d) | Hash Table (N=%-3d) | Circular Queue (%-2d)    ║\n",
           g_priority_queue.size, g_hash_table.total_entries, g_requests.count);
    printf("╚═════════════════════════════════════════════════════════════════════════════════════════════════════════╝\n");
}

int main(void) {
    expiry_init_default_config(&g_expiry_cfg);
    backup_init(&g_backups);
    ht_init(&g_hash_table);
    load_all_data();

    int choice = -1;
    while (choice != 0) {
        display_system_dashboard();
        printf("\n=============================== MAIN OPERATIONAL MENU ===============================\n");
        printf(" 1. Smart Food Allocation Engine (Multi-Donation Greedy Matching & Review)\n");
        printf(" 2. Recipient Request Queue (Circular FIFO Queue DSA)\n");
        printf(" 3. Donation Priority Queue (Binary Min-Heap DSA: Insert / Extract-Min)\n");
        printf(" 4. Food Expiry Management Module (Dashboard, Critical Alerts & Thresholds)\n");
        printf(" 5. Allocation Cancellation & Transactional Rollback\n");
        printf(" 6. Food Waste Prevention Analytics & Confirmed Waste Ledger\n");
        printf(" 7. Donation Inventory Management (Browse, Add, Sort by Expiry/Qty)\n");
        printf(" 8. Donor & Recipient Registration (with Duplicate Contact Warning)\n");
        printf(" 9. Advanced Search & Query Filters\n");
        printf("10. Hash Table Diagnostics (djb2 Hash, Bucket Distribution & O(1) Lookup)\n");
        printf("11. Advanced Report Export (CSV & Text Reports for Excel)\n");
        printf("12. Backup & Disaster Recovery (Create & Restore Snapshots)\n");
        printf("13. System Audit Trail (Immutable Operation History)\n");
        printf("14. Load / Reset Demonstration Dataset (7 Evaluator Edge Cases)\n");
        printf(" 0. Save Database and Exit Application\n");
        printf("=====================================================================================\n");
        printf("Enter option [0-14]: ");

        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf("[ERROR] Invalid choice. Please enter a valid number.\n");
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:  handle_smart_allocation(); break;
            case 2:  handle_request_queue_ops(); break;
            case 3:  handle_priority_queue_ops(); break;
            case 4:  handle_expiry_ops(); break;
            case 5:  handle_rollback_ops(); break;
            case 6:  handle_waste_ops(); break;
            case 7:  handle_inventory_ops(); break;
            case 8:  handle_donor_recipient_ops(); break;
            case 9:  handle_search_ops(); break;
            case 10: ht_display_stats(&g_hash_table); break;
            case 11: handle_export_ops(); break;
            case 12: handle_backup_ops(); break;
            case 13: audit_display_recent(&g_audit_log, 20); break;
            case 14:
                demo_load_dataset(&g_donors, &g_inventory, &g_recipients, &g_requests,
                                  &g_distributions, &g_audit_log, &g_priority_queue);
                rebuild_all_indexes();
                save_all_data();
                break;
            case 0:
                save_all_data();
                printf("\n[SHUTDOWN] All entities safely saved. Exiting Smart Food Redistribution System.\n");
                break;
            default:
                printf("[ERROR] Unknown option '%d'. Please select from menu [0-14].\n", choice);
                break;
        }

        if (choice != 0) {
            printf("\nPress ENTER to continue back to menu...");
            char enter_buf[16];
            fgets(enter_buf, sizeof(enter_buf), stdin);
        }
    }

    ht_clear(&g_hash_table);
    return 0;
}
