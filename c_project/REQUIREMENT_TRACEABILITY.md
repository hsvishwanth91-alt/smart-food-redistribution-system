# ORIGINAL REQUIREMENT TRACEABILITY MATRIX
## Smart Food Redistribution System

| Req ID | Original Proposal Requirement | Specification in Proposal | C Header / Source File | Functions & Structs Implemented | Compliance Status |
|:-------|:------------------------------|:--------------------------|:-----------------------|:---------------------------------|:------------------|
| **REQ-ORIG-01** | Donor Registration & Profiling | Store Donor ID, Name, Organization, Phone, Email, Address, Registration Date | `include/donor.h`<br>`src/donor.c` | `Donor` struct, `donor_register()`, `donor_find_by_id()` | **100% Compliant** |
| **REQ-ORIG-02** | Food Donation Intake | Record Donation ID, Donor ID, Food Category, Quantity, Expiry Date | `include/donation.h`<br>`src/donation.c` | `FoodDonation` struct, `donation_add()`, `donation_find_by_id()` | **100% Compliant** |
| **REQ-ORIG-03** | Recipient Organization Profiling | Store Recipient ID, Organization Name, Contact Person, Phone, Capacity/Beneficiaries | `include/recipient.h`<br>`src/recipient.c` | `Recipient` struct, `recipient_register()`, `recipient_find_by_id()` | **100% Compliant** |
| **REQ-ORIG-04** | Category-Based Matching | Match donations where `donation.category == request.category` | `include/allocation.h`<br>`src/allocation.c` | `calculate_allocation_score()`, `generate_smart_allocation_plan()` | **100% Compliant** |
| **REQ-ORIG-05** | Distribution Records | Record which donation was dispatched to which recipient with timestamp | `include/distribution.h`<br>`src/distribution.c` | `DistributionRecord` struct, `dist_record_allocation()` | **100% Compliant** |
| **REQ-ORIG-06** | File Persistence | Store records in text/CSV files so state persists across program runs | `include/common.h`<br>`src/donor.c`<br>`src/donation.c`<br>`src/recipient.c`<br>`src/distribution.c` | `donor_save_to_csv()`, `donor_load_from_csv()`, `donation_save_to_csv()`, `dist_save_to_csv()` | **100% Compliant** |
| **REQ-ORIG-07** | Menu-Driven Console Interface | Interactive terminal menus using standard GCC C runtime (`stdio.h`, `stdlib.h`) | `src/main.c` | `main()` menu loop with numbered options, input validation, and clear buffers | **100% Compliant** |
| **REQ-ORIG-08** | Basic Stock Safeguards | Prevent negative available stock and invalid allocations | `src/donation.c`<br>`src/allocation.c` | Stock validation checks (`don->available_quantity >= cand->offered_quantity`) | **100% Compliant** |
