# PROJECT ARCHITECTURE & DESIGN SPECIFICATION
## Smart Food Redistribution System — Professional DSA Edition

### 1. Architectural Overview
The Smart Food Redistribution System is architectured as a high-integrity, modular, layered C application adhering to strict separation of concerns. The architecture comprises four distinct logical tiers:

```
┌────────────────────────────────────────────────────────────────────────┐
│                        PRESENTATION & CLI LAYER                        │
│   src/main.c (Menu Dispatcher, Dashboard Renderer, Input Controllers)   │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
┌───────────────────────────────────▼────────────────────────────────────┐
│                    ALGORITHMIC & DECISION ENGINE TIER                  │
│  src/allocation.c (Smart Multi-Donation Greedy Matching Engine)        │
│  src/expiry.c     (Threshold Classification & Shelf-Life Assessment)   │
│  src/search.c     (Multi-Field Search & Substring Query Engine)        │
│  src/analytics.c  (Waste Prevention & Community Impact Metrics)       │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
┌───────────────────────────────────▼────────────────────────────────────┐
│                      DATA STRUCTURES & INDEXING TIER                   │
│  src/priority_queue.c (Binary Min-Heap for Expiry-Urgency Dispatch)    │
│  src/request_queue.c  (Circular FIFO Ring Buffer for Demand)           │
│  src/hash_table.c     (djb2 Separate Chaining for O(1) ID Lookup)      │
│  src/donation.c       (In-Place Insertion Sort & State Transitions)    │
└───────────────────────────────────┬────────────────────────────────────┘
                                    │
┌───────────────────────────────────▼────────────────────────────────────┐
│                  DATA PERSISTENCE & TRANSACTIONAL RECOVERY             │
│  src/donor.c, recipient.c, distribution.c (CSV Serialization/Parsing)  │
│  src/distribution.c (Transactional Rollback & Quantity Restoration)    │
│  src/backup.c       (Atomic Snapshotting & Disaster Recovery)          │
│  src/audit.c        (Immutable Append-Only Compliance Log)             │
│  Files on Disk: data/*.csv, data/backups/, reports/*.txt, reports/*.csv│
└────────────────────────────────────────────────────────────────────────┘
```

---

### 2. Entity Relationship Model
```
┌─────────────────┐           1:N           ┌─────────────────┐
│      Donor      ├────────────────────────►│  FoodDonation   │
│  (DNR-xxxxxx)   │                         │  (DON-xxxxxx)   │
└─────────────────┘                         └────────┬────────┘
                                                     │
                                                     │ 1:N
                                                     ▼
┌─────────────────┐           1:N           ┌─────────────────┐
│    Recipient    ├────────────────────────►│  Distribution   │
│  (RCP-xxxxxx)   │                         │  (DIS-xxxxxx)   │
└────────┬────────┘                         └────────▲────────┘
         │                                           │
         │ 1:N                                       │ 1:N
         ▼                                           │
┌─────────────────┐                                  │
│   FoodRequest   ├──────────────────────────────────┘
│  (REQ-xxxxxx)   │
└─────────────────┘
```

---

### 3. Donation Lifecycle State Machine
```
           Intake
             │
             ▼
      [ AVAILABLE ] ─────────────────────────► [ EXPIRED ]
             │             Date Passed            ▲
  Partial    │ Allocation                         │ Date
  Allocation │                                    │ Passed
             ▼                                    │
 [ PARTIALLY_ALLOCATED ] ─────────────────────────┤
             │                                    │
   Full      │ Allocation                         │
   Remaining │                                    │
             ▼                                    │
  [ FULLY_ALLOCATED ]                             │
             │                                    │
  Operator   │ Rollback (if within shelf-life)    │
  Cancelled  ├────────────────────────────────────┘
             ▼
     [ CANCELLED ]   or   [ CONFIRMED_WASTE ] (Quarantined)
```

---

### 4. Safety Guardrails & Validation Policies
1. **Unit Compatibility Guard**:
   - The matching engine enforces strict unit equivalence: Kilograms can ONLY match Kilograms, Liters can ONLY match Liters, Packets can ONLY match Packets.
   - Prevents fatal unit confusion (e.g., fulfilling 10 kilograms with 10 single packets).
2. **Quarantine of Expired Stock**:
   - Batches where `calculate_days_until_expiry(expiry_date) < 0` are automatically excluded from the matching candidate pool and flagged `[!] EXPIRED`.
3. **Rollback Quantity Invariants**:
   - A distribution cannot be cancelled twice (`is_cancelled == true` check).
   - If a batch expired while allocated, rolling back restores the physical quantity for accounting purposes but sets status to `STATUS_EXPIRED`, preventing redistribution to recipients.
4. **Audit Trail Completeness**:
   - Every registration, allocation, rollback, waste confirmation, and backup snapshot logs an immutable entry to `data/audit_log.csv`.
