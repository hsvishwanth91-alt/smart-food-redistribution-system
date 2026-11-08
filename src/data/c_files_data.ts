/**
 * Project files catalog and documentation content for the interactive workbench.
 */

export interface ProjectFile {
  path: string;
  category: 'header' | 'source' | 'config' | 'data' | 'doc';
  description: string;
  size: string;
}

export const PROJECT_FILES: ProjectFile[] = [
  // Docs
  { path: 'README.md', category: 'doc', description: 'Comprehensive user guide, build instructions, and evaluator walkthrough', size: '12 KB' },
  { path: 'PAGE_BY_PAGE_ANALYSIS.md', category: 'doc', description: 'Page-by-page analysis of the original 7-page proposal', size: '4.8 KB' },
  { path: 'REQUIREMENT_TRACEABILITY.md', category: 'doc', description: 'Matrix tracing original requirements to C implementations', size: '3.2 KB' },
  { path: 'ENHANCEMENT_TRACEABILITY.md', category: 'doc', description: 'Matrix tracing all 17 advanced enhancements to C source code', size: '5.5 KB' },
  { path: 'DSA_COMPLEXITY.md', category: 'doc', description: 'Detailed time and space complexity proofs for Heap, Hash Table, Queue', size: '6.4 KB' },
  { path: 'PROJECT_ARCHITECTURE.md', category: 'doc', description: 'Layered architecture, state machines, and data validation tiers', size: '5.1 KB' },
  { path: 'TESTING.md', category: 'doc', description: 'Verified test matrix covering all 12 edge cases and assertions', size: '4.9 KB' },

  // Build & Config
  { path: 'Makefile', category: 'config', description: 'GNU Make build script with strict GCC C11 compiler flags', size: '1.4 KB' },
  { path: 'CMakeLists.txt', category: 'config', description: 'Cross-platform CMake configuration for VS Code CMake Tools', size: '0.6 KB' },
  { path: '.vscode/tasks.json', category: 'config', description: 'VS Code build task with GCC problem matchers', size: '1.1 KB' },
  { path: '.vscode/launch.json', category: 'config', description: 'VS Code GDB debugging launcher configuration', size: '0.8 KB' },
  { path: '.vscode/settings.json', category: 'config', description: 'VS Code IntelliSense and C11 standard configurations', size: '0.5 KB' },

  // Headers
  { path: 'include/common.h', category: 'header', description: 'Shared enums, food units, categories, and utility prototypes', size: '3.8 KB' },
  { path: 'include/donor.h', category: 'header', description: 'Donor entity registry and duplicate detection declarations', size: '1.6 KB' },
  { path: 'include/donation.h', category: 'header', description: 'Food donation inventory, statuses, and sorting prototypes', size: '2.1 KB' },
  { path: 'include/recipient.h', category: 'header', description: 'Beneficiary recipient organization profiles', size: '1.7 KB' },
  { path: 'include/request_queue.h', category: 'header', description: 'Circular array FIFO request queue operations (Enqueue/Dequeue)', size: '1.8 KB' },
  { path: 'include/priority_queue.h', category: 'header', description: 'Binary Min-Heap Priority Queue for shelf-life dispatch', size: '1.9 KB' },
  { path: 'include/hash_table.h', category: 'header', description: 'djb2 Hash Table with separate chaining for O(1) ID lookups', size: '1.5 KB' },
  { path: 'include/allocation.h', category: 'header', description: 'Smart Allocation Engine and multi-donation matching contracts', size: '2.2 KB' },
  { path: 'include/distribution.h', category: 'header', description: 'Distribution ledger and allocation rollback engine', size: '1.9 KB' },
  { path: 'include/expiry.h', category: 'header', description: 'Food expiry classification, threshold rules, and dashboard', size: '1.4 KB' },
  { path: 'include/analytics.h', category: 'header', description: 'Food waste tracking and CSV/TXT export prototypes', size: '2.8 KB' },
  { path: 'include/audit.h', category: 'header', description: 'System audit log and operation types', size: '1.8 KB' },
  { path: 'include/backup.h', category: 'header', description: 'Snapshot backup and disaster recovery prototypes', size: '1.7 KB' },
  { path: 'include/search.h', category: 'header', description: 'Multi-field search and query filter prototypes', size: '1.2 KB' },
  { path: 'include/demo.h', category: 'header', description: 'Evaluation demo dataset populator declarations', size: '1.1 KB' },

  // Sources
  { path: 'src/main.c', category: 'source', description: 'Main menu dispatcher, terminal dashboard, and operational submenus', size: '18 KB' },
  { path: 'src/allocation.c', category: 'source', description: 'Deterministic allocation scoring and greedy knapsack multi-donation match', size: '8.4 KB' },
  { path: 'src/priority_queue.c', category: 'source', description: 'Min-Heap sift-up, sift-down, extract-min, and heapify routines', size: '5.2 KB' },
  { path: 'src/hash_table.c', category: 'source', description: 'djb2 hash function, bucket chains, and O(1) lookup operations', size: '4.1 KB' },
  { path: 'src/request_queue.c', category: 'source', description: 'Circular array ring buffer with front/rear modulo pointer math', size: '4.8 KB' },
  { path: 'src/donation.c', category: 'source', description: 'Inventory management, urgency calculations, and in-place insertion sort', size: '7.2 KB' },
  { path: 'src/donor.c', category: 'source', description: 'Donor registration, duplicate contact checks, and CSV serialization', size: '5.1 KB' },
  { path: 'src/recipient.c', category: 'source', description: 'Recipient registry, duplicate detection, and CSV serialization', size: '4.9 KB' },
  { path: 'src/distribution.c', category: 'source', description: 'Transactional rollback engine, stock restoration, and ledger history', size: '6.8 KB' },
  { path: 'src/expiry.c', category: 'source', description: 'Threshold evaluations, expiry dashboard, and food safety governance', size: '4.3 KB' },
  { path: 'src/analytics.c', category: 'source', description: 'Food loss prevention metrics and Excel CSV / TXT report generator', size: '8.1 KB' },
  { path: 'src/audit.c', category: 'source', description: 'Append-only audit logger and historical inspection routines', size: '4.4 KB' },
  { path: 'src/backup.c', category: 'source', description: 'Atomic snapshot creation and rollback recovery', size: '5.2 KB' },
  { path: 'src/search.c', category: 'source', description: 'Case-insensitive substring search and category filters', size: '4.2 KB' },
  { path: 'src/demo.c', category: 'source', description: '7 predefined evaluation scenarios for viva and demonstrations', size: '6.7 KB' },
  { path: 'src/utils.c', category: 'source', description: 'Date math, ISO validation, safe string handling, and enum conversions', size: '5.6 KB' },

  // Data
  { path: 'data/donors.csv', category: 'data', description: 'Persistent CSV records for registered donors', size: '0.4 KB' },
  { path: 'data/donations.csv', category: 'data', description: 'Persistent CSV records for food inventory batches', size: '0.9 KB' },
  { path: 'data/recipients.csv', category: 'data', description: 'Persistent CSV records for beneficiary recipient organizations', size: '0.5 KB' },
  { path: 'data/requests.csv', category: 'data', description: 'Persistent CSV records for recipient food requests', size: '0.5 KB' },
  { path: 'data/distributions.csv', category: 'data', description: 'Persistent CSV records for completed and pending distributions', size: '0.2 KB' },
  { path: 'data/audit_log.csv', category: 'data', description: 'Persistent CSV records for immutable system audit trail', size: '0.6 KB' },
  { path: 'data/waste.csv', category: 'data', description: 'Persistent CSV records for confirmed food waste', size: '0.1 KB' }
];
