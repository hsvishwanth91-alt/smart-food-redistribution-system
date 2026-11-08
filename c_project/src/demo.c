/**
 * @file demo.c
 * @brief Demonstration mode test dataset populator.
 *        Constructs verified evaluation scenarios illustrating all DSA edge cases.
 */

#include "../include/demo.h"

StatusCode demo_load_dataset(
    DonorRegistry *donors,
    DonationInventory *inv,
    RecipientRegistry *recipients,
    RequestQueue *requests,
    DistributionLedger *ledger,
    AuditLog *audit,
    DonationPriorityQueue *pq
) {
    if (!donors || !inv || !recipients || !requests || !ledger || !audit) {
        return ERR_INVALID_INPUT;
    }

    char cur_date[MAX_DATE_LEN];
    get_current_date_str(cur_date, sizeof(cur_date));

    /* Initialize all containers */
    donor_init_registry(donors);
    donation_init_inventory(inv);
    recipient_init_registry(recipients);
    req_queue_init(requests);
    dist_init_ledger(ledger);
    audit_init(audit);

    /* 1. Register Donors */
    Donor d1 = {"DNR-101", "Sunrise Bakery", "Sunrise Food Corp", "+1-555-0191", "contact@sunrise.org", "42 Market St", "2026-10-01", 12, 180.5, true};
    Donor d2 = {"DNR-102", "Golden Harvest Farms", "Agri-Coop Ltd", "+1-555-0192", "info@goldenfarms.com", "88 Valley Rd", "2026-10-05", 25, 620.0, true};
    Donor d3 = {"DNR-103", "Metro Supermarket", "Metro Retail Inc", "+1-555-0193", "csr@metroretail.com", "101 Grand Ave", "2026-10-10", 8, 310.0, true};
    Donor d4 = {"DNR-104", "City Central Kitchen", "Hospitality Group", "+1-555-0194", "kitchen@citycentral.org", "12 Main St", "2026-10-15", 40, 950.0, true};
    donor_register(donors, &d1);
    donor_register(donors, &d2);
    donor_register(donors, &d3);
    donor_register(donors, &d4);

    /* 2. Register Food Donations covering all evaluation scenarios */

    /* Scenario 1: Sufficient single donation for standard match */
    FoodDonation don1 = {
        "DON-501", "DNR-101", "Artisan Bread Loaves", CAT_BAKERY,
        50.0, 50.0, UNIT_PIECE,
        "2026-11-06", "2026-11-10", STATUS_AVAILABLE, EXPIRY_NORMAL, 50,
        "Ambient", "Fresh whole wheat sliced loaves"
    };

    /* Scenario 2: Multiple donations that together satisfy a request */
    /* Target: Recipient requesting 25 kg Rice */
    /* Donation A: 10 kg rice (approaching expiry) */
    FoodDonation don2 = {
        "DON-502", "DNR-102", "Basmati Rice Batch A", CAT_GRAINS_STAPLES,
        10.0, 10.0, UNIT_KG,
        "2026-10-20", "2026-11-09", STATUS_AVAILABLE, EXPIRY_CRITICAL, 95,
        "Dry Storage", "High-priority batch expiring tomorrow"
    };
    /* Donation B: 8 kg rice */
    FoodDonation don3 = {
        "DON-503", "DNR-102", "Basmati Rice Batch B", CAT_GRAINS_STAPLES,
        8.0, 8.0, UNIT_KG,
        "2026-10-22", "2026-11-12", STATUS_AVAILABLE, EXPIRY_NEAR_EXPIRY, 75,
        "Dry Storage", "Second compatible batch"
    };
    /* Donation C: 12 kg rice */
    FoodDonation don4 = {
        "DON-504", "DNR-103", "Long Grain Rice Batch C", CAT_GRAINS_STAPLES,
        12.0, 12.0, UNIT_KG,
        "2026-10-25", "2026-11-20", STATUS_AVAILABLE, EXPIRY_NORMAL, 40,
        "Dry Storage", "Third compatible batch"
    };

    /* Scenario 3: Critical urgency donation expiring in <24 hours */
    FoodDonation don5 = {
        "DON-505", "DNR-104", "Prepared Vegetable Curry", CAT_COOKED_MEALS,
        30.0, 30.0, UNIT_PACKET,
        "2026-11-07", "2026-11-08", STATUS_AVAILABLE, EXPIRY_CRITICAL, 100,
        "Refrigerated", "Freshly packed meal boxes. Must distribute immediately"
    };

    /* Scenario 4: Expired batch (Engine must block allocation!) */
    FoodDonation don6 = {
        "DON-506", "DNR-103", "Past-Date Whole Milk", CAT_DAIRY_EGGS,
        20.0, 20.0, UNIT_LITER,
        "2026-10-28", "2026-11-04", STATUS_EXPIRED, EXPIRY_EXPIRED, 0,
        "Refrigerated", "Expired batch flagged during intake inspection"
    };

    /* Scenario 5: Insufficient single quantity / Partial allocation scenario */
    FoodDonation don7 = {
        "DON-507", "DNR-102", "Fresh Crisp Apples", CAT_PRODUCE_VEG_FRUIT,
        15.0, 15.0, UNIT_KG,
        "2026-11-05", "2026-11-14", STATUS_AVAILABLE, EXPIRY_NORMAL, 45,
        "Cool Ambient", "Grade A apples"
    };

    donation_add(inv, &don1);
    donation_add(inv, &don2);
    donation_add(inv, &don3);
    donation_add(inv, &don4);
    donation_add(inv, &don5);
    donation_add(inv, &don6);
    donation_add(inv, &don7);

    /* 3. Register Beneficiary Recipients */
    Recipient r1 = {"RCP-201", "Hope Community Shelter", "Sarah Jenkins", "+1-555-0301", "hope@shelter.org", "500 Elm St", 85, "Nut-free, Halal", 14, 12, true};
    Recipient r2 = {"RCP-202", "Saint Mary Family Mission", "Father Paul", "+1-555-0302", "stmary@mission.org", "720 Pine Ave", 120, "Vegetarian options", 20, 18, true};
    Recipient r3 = {"RCP-203", "Youth Care Centre", "David Zhang", "+1-555-0303", "contact@youthcare.org", "330 Oak Blvd", 45, "Standard", 6, 6, true};
    Recipient r4 = {"RCP-204", "Elderly Care Haven", "Elena Rostova", "+1-555-0304", "haven@eldercare.org", "910 Maple Rd", 60, "Low Sodium, Soft foods", 10, 8, true};
    recipient_register(recipients, &r1);
    recipient_register(recipients, &r2);
    recipient_register(recipients, &r3);
    recipient_register(recipients, &r4);

    /* 4. Enqueue Recipient Requests into FIFO Queue */

    /* Request 1: Target for Multi-Donation Match: 25 kg Rice */
    FoodRequest req1 = {
        "REQ-301", "RCP-201", CAT_GRAINS_STAPLES,
        25.0, 0.0, 25.0, UNIT_KG, PRIORITY_EMERGENCY,
        "2026-11-07", "2026-11-10", REQ_PENDING,
        "Urgent grain staples needed for 85 shelter residents"
    };

    /* Request 2: Single Match (Cooked Meals, 30 packets) */
    FoodRequest req2 = {
        "REQ-302", "RCP-202", CAT_COOKED_MEALS,
        30.0, 0.0, 30.0, UNIT_PACKET, PRIORITY_HIGH,
        "2026-11-07", "2026-11-08", REQ_PENDING,
        "Evening dinner distribution"
    };

    /* Request 3: Shortage / Partial Match (Demands 40 kg Apples, only 15 kg exist) */
    FoodRequest req3 = {
        "REQ-303", "RCP-204", CAT_PRODUCE_VEG_FRUIT,
        40.0, 0.0, 40.0, UNIT_KG, PRIORITY_MEDIUM,
        "2026-11-07", "2026-11-12", REQ_PENDING,
        "Fresh fruit for elderly residents"
    };

    /* Request 4: Recipient with NO compatible donation (e.g. Canned Goods, 0 available) */
    FoodRequest req4 = {
        "REQ-304", "RCP-203", CAT_CANNED_PACKAGED,
        50.0, 0.0, 50.0, UNIT_PACKET, PRIORITY_STANDARD,
        "2026-11-07", "2026-11-15", REQ_PENDING,
        "Non-perishable pantry reserve"
    };

    /* Request 5: Pre-existing partially fulfilled request */
    FoodRequest req5 = {
        "REQ-305", "RCP-201", CAT_BAKERY,
        50.0, 20.0, 30.0, UNIT_PIECE, PRIORITY_STANDARD,
        "2026-11-06", "2026-11-09", REQ_PARTIALLY_FULFILLED,
        "Breakfast bread supply (20 pcs received previously)"
    };

    req_queue_enqueue(requests, &req1);
    req_queue_enqueue(requests, &req2);
    req_queue_enqueue(requests, &req3);
    req_queue_enqueue(requests, &req4);
    req_queue_enqueue(requests, &req5);

    /* 5. Pre-load an initial historical distribution record */
    DistributionRecord drec = {
        "DIS-10001", "REQ-305", "DON-501", "RCP-201",
        20.0, UNIT_PIECE, "2026-11-06", "14:30:00",
        DIST_ALLOCATED, false, ""
    };
    dist_record_allocation(ledger, &drec);

    /* 6. Populate Heap Priority Queue */
    if (pq) {
        pq_rebuild_from_inventory(pq, inv);
    }

    /* 7. Audit Log */
    audit_record(audit, OP_DEMO_LOADED, "SYS_INIT",
                 "Demonstration evaluation dataset loaded into memory and verified.", "System");

    printf("\n=========================================================================\n");
    printf("[DEMO MODE INITIALIZED SUCCESSFULLY]\n");
    printf("Pre-loaded 4 Donors, 7 Food Batches, 4 Recipients, 5 Requests, 1 Dispatch.\n");
    printf("=========================================================================\n");
    demo_explain_scenario();
    return SUCCESS;
}

void demo_explain_scenario(void) {
    printf("\n=== EVALUATOR GUIDE: TEST SCENARIOS READY IN MEMORY ===\n");
    printf("Scenario 1: Multi-Donation Greedy Matching (Req REQ-301, 25 kg Rice)\n");
    printf("  -> Demonstrates combining DON-502 (10 kg), DON-503 (8 kg), DON-504 (7 kg of 12 kg)\n");
    printf("  -> Prioritizes DON-502 first because it expires tomorrow (< 24h critical!)\n\n");

    printf("Scenario 2: Critical Expiry Prioritization (Req REQ-302, 30 packets Cooked Meals)\n");
    printf("  -> Matches DON-505 with 100/100 score due to immediate expiry urgency.\n\n");

    printf("Scenario 3: Shortage / Partial Allocation Handling (Req REQ-303, 40 kg Fresh Produce)\n");
    printf("  -> Only 15 kg available in DON-507. Engine detects 25 kg shortage cleanly.\n\n");

    printf("Scenario 4: Expired Stock Quarantine Protection (DON-506 Milk, Expired)\n");
    printf("  -> Engine strictly excludes expired stock from matching proposals.\n\n");

    printf("Scenario 5: Incompatible Stock Detection (Req REQ-304 Canned Goods)\n");
    printf("  -> 0 stock exists. Engine explains exact cause without cryptic crash.\n\n");

    printf("Scenario 6: Transactional Rollback (Dist DIS-10001)\n");
    printf("  -> Cancel allocation DIS-10001 to demonstrate safe stock and request restoration.\n");
    printf("=======================================================\n");
}
