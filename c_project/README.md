# Smart Food Redistribution System — Professional DSA Edition

An enterprise-grade, menu-driven C software system and Data Structures & Algorithms suite engineered for efficient, verifiable, and waste-preventive food redistribution across donors and community shelters.

---

## 🌟 Key Highlights

- **100% Native C (C11 standard)** with zero external graphical dependencies.
- **Core Data Structures & Algorithms**:
  - **Binary Min-Heap Priority Queue**: $O(\log N)$ insert and extract-min for shelf-life dispatch.
  - **djb2 Hash Table with Separate Chaining**: $O(1)$ average time record lookups by ID.
  - **Circular FIFO Request Queue**: Strict First-Come-First-Served beneficiary queuing.
  - **Greedy Multi-Donation Knapsack Matching**: Combines multiple donation batches to satisfy large recipient requests with explainable recommendation scores.
  - **In-Place Stable Insertion Sort**: Sorts inventory by expiry date, quantity, or urgency.
- **Transactional Rollback Engine**: Safely reverses allocations, restores stock and request balance, and guards against double-cancellations.
- **Food Expiry Management**: Configurable thresholds (Critical $<24$h, Near Expiry $<72$h), dashboard, and food safety governance.
- **Waste Prevention Analytics**: Distinguishes donated, allocated, collected, completed, and confirmed waste without double-counting.
- **Immutable System Audit Trail**: Logs all modifications to append-only CSV storage.
- **Disaster Recovery**: Timestamped backup snapshots with restore verification.
- **CSV & TXT Reports**: Generates Excel-compatible spreadsheets and executive text summaries.
- **Demonstration Mode**: One-click evaluation dataset pre-configured with 7 distinct testing scenarios.

---

## 📁 Project Directory Structure

```text
c_project/
├── include/                     # C Header Files (.h)
│   ├── common.h                 # Enums, constants, shared prototypes
│   ├── donor.h                  # Donor registry definitions
│   ├── donation.h               # Food donation inventory and sorting
│   ├── recipient.h              # Recipient organization definitions
│   ├── request_queue.h          # Circular FIFO Queue DSA
│   ├── priority_queue.h         # Binary Min-Heap Priority Queue DSA
│   ├── hash_table.h             # djb2 Separate Chaining Hash Table DSA
│   ├── allocation.h             # Smart Allocation & Multi-Donation Engine
│   ├── distribution.h           # Distribution ledger & Rollback Engine
│   ├── expiry.h                 # Expiry classification & dashboard
│   ├── analytics.h              # Waste tracking & report export
│   ├── audit.h                  # System audit logger
│   ├── backup.h                 # Snapshot backup & disaster recovery
│   ├── search.h                 # Multi-criteria search filters
│   └── demo.h                   # Evaluator test scenario populator
├── src/                         # Modular C Source Files (.c)
│   ├── main.c                   # Menu dispatcher & terminal dashboard
│   ├── donor.c                  # Donor management & duplicate detection
│   ├── donation.c               # Inventory management & sorting
│   ├── recipient.c              # Recipient management
│   ├── request_queue.c          # Circular FIFO queue operations
│   ├── priority_queue.c         # Heapify, sift-up, sift-down, extract-min
│   ├── hash_table.c             # djb2 hash algorithm & bucket chains
│   ├── allocation.c             # Deterministic scoring & multi-match
│   ├── distribution.c           # Transactional rollback & ledger
│   ├── expiry.c                 # Expiry tracking & thresholds
│   ├── analytics.c              # Loss prevention & CSV/TXT exports
│   ├── audit.c                  # Audit trail persistence
│   ├── backup.c                 # Atomic backup creation & restore
│   ├── search.c                 # Substring search & filtering
│   ├── demo.c                   # Predefined evaluation scenarios
│   └── utils.c                  # Date math, safe strings, validation
├── data/                        # Persistent Storage (CSV files)
│   ├── donors.csv
│   ├── donations.csv
│   ├── recipients.csv
│   ├── requests.csv
│   ├── distributions.csv
│   ├── audit_log.csv
│   └── waste.csv
├── .vscode/                     # VS Code Workspace Configuration
│   ├── tasks.json               # One-click GCC build task
│   ├── launch.json              # GDB interactive debugging configuration
│   └── settings.json            # C11 IntelliSense & formatter settings
├── Makefile                     # Professional GNU Make build script
├── CMakeLists.txt               # Cross-platform CMake build file
├── PAGE_BY_PAGE_ANALYSIS.md     # Proposal requirement evaluation
├── REQUIREMENT_TRACEABILITY.md  # Original requirements mapping
├── ENHANCEMENT_TRACEABILITY.md  # 17 enhancement specifications mapping
├── DSA_COMPLEXITY.md            # Asymptotic time/space complexity proofs
├── PROJECT_ARCHITECTURE.md      # Architecture, state machines, schemas
└── TESTING.md                   # Verified test matrix & results
```

---

## 🛠️ Compilation and Execution Instructions

### Option 1: Using GNU Make (Recommended)
In your terminal, navigate to the project directory and run:
```bash
make
./bin/food_redistribution
```

To clean previous build objects:
```bash
make clean
```

### Option 2: Direct GCC Single-Line Compilation
If `make` is not available, compile all modular units in one command:
```bash
mkdir -p bin data reports
gcc -std=c11 -Wall -Wextra -pedantic -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE -Iinclude src/*.c -o bin/food_redistribution
./bin/food_redistribution
```

### Option 3: Opening and Running in Visual Studio Code
1. Open Visual Studio Code.
2. Select **File -> Open Folder...** and choose the `c_project/` folder.
3. Install the standard **C/C++** extension (by Microsoft) if prompted.
4. Press `Ctrl + Shift + B` (or `Cmd + Shift + B` on macOS) to trigger the automated build task configured in `.vscode/tasks.json`.
5. Open the integrated terminal (`Ctrl + ~`) and launch the application:
   ```bash
   ./bin/food_redistribution
   ```
6. Alternatively, press `F5` to start interactive debugging via GDB with breakpoints configured in `.vscode/launch.json`.

---

## 🚀 Step-by-Step Evaluator Demonstration Guide

When presenting this project to evaluators, follow this recommended walkthrough:

### 1. View Initial Dashboard (Menu Option 14 -> Main Menu)
Select **Menu Option 14 (Load Demonstration Dataset)**. The system immediately loads 4 Donors, 7 Food Batches, 4 Recipients, 5 Queued Requests, and 1 Historical Dispatch. Notice the active ANSI terminal dashboard displaying inventory mass, queued demand, and active DSA structure counters.

### 2. Demonstrate Smart Multi-Donation Allocation (Menu Option 1)
- Select **Menu Option 1**.
- Press **ENTER** to process the front request `REQ-301` (Hope Community Shelter requesting 25 kg of Grains/Rice).
- Notice how the Smart Allocation Engine solves the request:
  - It identifies that single donations are insufficient.
  - It combines **DON-502** (10 kg), **DON-503** (8 kg), and **DON-504** (7 kg out of 12 kg).
  - It ranks **DON-502** first because its expiry date is tomorrow ($<24$h critical urgency), maximizing waste prevention.
  - It presents the explainable deterministic ranking score, displays the proposed remaining stock, and prompts for operator confirmation before committing.
- Confirm with `y`. The allocations are logged to the distribution ledger and audit trail!

### 3. Demonstrate Binary Min-Heap Operations (Menu Option 3)
- Select **Menu Option 3**.
- Choose **Option 1 (View Current Heap State)**: Notice how the root is always the item with the minimum days to expiry.
- Choose **Option 3 (Extract-Min O(log N))**: Watch the system remove the root and use `sift_down` to restore the min-heap property.
- Choose **Option 5**: View the mathematical asymptotic complexity proof.

### 4. Demonstrate Allocation Rollback & Transactional Safety (Menu Option 5)
- Select **Menu Option 5**.
- Input distribution ID `DIS-10001` (20 pieces of artisan bread).
- Confirm rollback. The system:
  - Restores 20 pieces to `DON-501` inventory.
  - Restores the remaining needed balance on request `REQ-305`.
  - Re-evaluates batch expiry date.
  - Updates distribution status to `CANCELLED`.
  - Logs the rollback to the audit trail.
  - Re-attempting to cancel `DIS-10001` is immediately blocked.

### 5. Demonstrate Food Expiry Dashboard (Menu Option 4)
- Select **Menu Option 4 -> Option 1**.
- View the real-time breakdown of expired batches, critical batches ($<24$h), and near-expiry batches ($<72$h).
- Choose **Option 5** to customize threshold hours/days dynamically.
- Choose **Option 7** to review the food safety governance policy.

### 6. Demonstrate Hash Table Diagnostics (Menu Option 10)
- Select **Menu Option 10**.
- Inspect total buckets (127), active entries, load factor ($\alpha \approx 0.12$), and maximum collision chain length.

### 7. Export Comprehensive Reports (Menu Option 11)
- Select **Menu Option 11 -> Option 1**.
- The system generates 6 report files in `reports/`:
  - `donations_report.csv` (Opens in Excel)
  - `recipients_report.csv` (Opens in Excel)
  - `distributions_report.csv` (Opens in Excel)
  - `waste_ledger_report.csv` (Opens in Excel)
  - `expiry_audit_report.txt` (Text summary)
  - `executive_summary.txt` (KPI overview)

---

## 📄 License and Governance
Developed under the MIT License for academic excellence, Data Structures & Algorithms rigor, and humanitarian food waste prevention.
