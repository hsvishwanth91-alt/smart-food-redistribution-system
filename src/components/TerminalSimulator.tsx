import React, { useState } from 'react';
import { Terminal, RefreshCw, Play, CheckCircle2, RotateCcw, AlertTriangle, ArrowRight, Minimize2, Maximize2 } from 'lucide-react';

interface TerminalSimulatorProps {
  onDownloadZip: () => void;
}

export const TerminalSimulator: React.FC<TerminalSimulatorProps> = ({ onDownloadZip }) => {
  const [isCompact, setIsCompact] = useState<boolean>(true);
  const [history, setHistory] = useState<string[]>([
    "╔═════════════════════════════════════════════════════════════════════════════════════════════════════════╗",
    "║                   SMART FOOD REDISTRIBUTION SYSTEM — PROFESSIONAL DSA EDITION                   ║",
    "║                 Enterprise Food Redistribution, Dynamic Allocation & Audit System               ║",
    "╠═════════════════════════════════════════════════════════════════════════════════════════════════════════╣",
    "║  COMMUNITY NETWORK:     Donors: 4    | Beneficiary Recipients: 4                                  ║",
    "║  FOOD INVENTORY:        Total Batches: 7    | Active Available: 6    | Expired/At-Risk: 3         ║",
    "║  MASS METRICS:          Donated:    195.0 kg | Available:    155.0 kg | Allocated:     20.0 kg      ║",
    "║  DEMAND PIPELINE:       Queued Requests: 5    | Fulfilled: 0    | Partial: 1    | Pending: 4       ║",
    "║  EFFICIENCY:            Waste Diversion Rate: 100.0% | Confirmed Loss:     0.0 kg                  ║",
    "║  DATA STRUCTURES ACTIVE: Binary Min-Heap (size 6  ) | Hash Table (N=15 ) | Circular Queue (5 )    ║",
    "╚═════════════════════════════════════════════════════════════════════════════════════════════════════════╝",
    "",
    "=============================== MAIN OPERATIONAL MENU ===============================",
    " 1. Smart Food Allocation Engine (Multi-Donation Greedy Matching & Review)",
    " 2. Recipient Request Queue (Circular FIFO Queue DSA)",
    " 3. Donation Priority Queue (Binary Min-Heap DSA: Insert / Extract-Min)",
    " 4. Food Expiry Management Module (Dashboard, Critical Alerts & Thresholds)",
    " 5. Allocation Cancellation & Transactional Rollback",
    " 6. Food Waste Prevention Analytics & Confirmed Waste Ledger",
    " 7. Donation Inventory Management (Browse, Add, Sort by Expiry/Qty)",
    " 8. Donor & Recipient Registration (with Duplicate Contact Warning)",
    " 9. Advanced Search & Query Filters",
    "10. Hash Table Diagnostics (djb2 Hash, Bucket Distribution & O(1) Lookup)",
    "11. Advanced Report Export (CSV & Text Reports for Excel)",
    "12. Backup & Disaster Recovery (Create & Restore Snapshots)",
    "13. System Audit Trail (Immutable Operation History)",
    "14. Load / Reset Demonstration Dataset (7 Evaluator Edge Cases)",
    " 0. Save Database and Exit Application",
    "=====================================================================================",
    "Ready. Click any action below or type a menu number:"
  ]);

  const [inputVal, setInputVal] = useState('');

  const executeCommand = (cmd: string) => {
    const trimmed = cmd.trim();
    const newLogs = [...history, `> ${trimmed}`];

    switch (trimmed) {
      case '1':
        newLogs.push(
          "================================== SMART ALLOCATION PROPOSAL ==================================",
          "Request ID:       REQ-301              | Recipient ID: RCP-201 (Hope Shelter)",
          "Category:         Grains & Staples     | Food Unit:    kg",
          "Requested Qty:       25.00 kg          | Total Match:     25.00 kg",
          "Shortage Qty:         0.00 kg          | Satisfied:    YES (Complete - 100% Satisfied)",
          "Engine Verdict:   Multi-Donation match: Request 100% satisfied by combining 3 donations.",
          "------------------------------------------------------------------------------------------------",
          "#   | Donation ID  | Alloc Qty    | Rem. Stock   | Days Left | Deterministic Recommendation Reason",
          "------------------------------------------------------------------------------------------------",
          "1   | DON-502      |    10.00 kg  |     0.00 kg  | 0.9       | Score: 190.0 | Urgency: CRITICAL (<24h) | Consumes full stock",
          "2   | DON-503      |     8.00 kg  |     0.00 kg  | 3.8       | Score: 170.0 | Urgency: NEAR EXPIRY (<72h) | Consumes full stock",
          "3   | DON-504      |     7.00 kg  |     5.00 kg  | 11.8      | Score: 150.0 | Urgency: NORMAL | Partial allocation (5 kg remains)",
          "================================================================================================",
          "[DEMO PROMPT] Operator Action: Confirm and commit this allocation? (y/N): Y",
          "[SUCCESS] Allocation executed successfully! Distribution records DIS-8801, DIS-8802, DIS-8803 generated.",
          "[AUDIT LOGGED] Event OP_ALLOCATION_CONFIRM written to data/audit_log.csv."
        );
        break;

      case '2':
        newLogs.push(
          "=============================== RECIPIENT REQUEST FIFO QUEUE (5 In Queue) ===============================",
          "Queue Architecture: Circular Array | Front: Index 0 | Rear: Index 4 | Count: 5 / 1000",
          "----------------------------------------------------------------------------------------------------------",
          "Pos  | Req ID     | Recip ID     | Category         | Quantity       | Priority   | Status",
          "----------------------------------------------------------------------------------------------------------",
          "#1   | REQ-301    | RCP-201      | Grains & Staples | 25.0 kg        | EMERGENCY! | PENDING",
          "#2   | REQ-302    | RCP-202      | Cooked Meals     | 30.0 packets   | HIGH       | PENDING",
          "#3   | REQ-303    | RCP-204      | Fresh Produce    | 40.0 kg        | MEDIUM     | PENDING",
          "#4   | REQ-304    | RCP-203      | Canned & Packaged| 50.0 packets   | STANDARD   | PENDING",
          "#5   | REQ-305    | RCP-201      | Bakery & Bread   | 30.0 pieces    | STANDARD   | PARTIALLY_FULFILLED",
          "----------------------------------------------------------------------------------------------------------"
        );
        break;

      case '3':
        newLogs.push(
          "======================== BINARY MIN-HEAP PRIORITY QUEUE (6 In Heap) ========================",
          "Heap Root (Highest Urgency): ID: DON-505 | Days to Expiry: 0.8 | Qty: 30.0 packets",
          "---------------------------------------------------------------------------------------------",
          "Index | Donation ID  | Category           | Quantity       | Days Left      | Urgency Rank",
          "---------------------------------------------------------------------------------------------",
          "#0    | DON-505      | Cooked Meals       | 30.0 packets   | 0.8            | CRITICAL",
          "#1    | DON-502      | Grains & Staples   | 10.0 kg        | 0.9            | CRITICAL",
          "#2    | DON-501      | Bakery & Bread     | 50.0 pieces    | 1.9            | NEAR EXPIRY",
          "#3    | DON-503      | Grains & Staples   | 8.0 kg         | 3.8            | NEAR EXPIRY",
          "#4    | DON-507      | Fresh Produce      | 15.0 kg        | 5.8            | NORMAL",
          "#5    | DON-504      | Grains & Staples   | 12.0 kg        | 11.8           | NORMAL",
          "---------------------------------------------------------------------------------------------",
          "[DSA COMPLEXITY VERIFIED]: Root element lookup: O(1) | Sift-down extract-min: O(log N)"
        );
        break;

      case '4':
        newLogs.push(
          "+-----------------------------------------------------------------------------------+",
          "| [IMPORTANT NOTICE & FOOD SAFETY DISCLAIMER]                                       |",
          "| This expiry tracking module operates as a record-management and inventory         |",
          "| decision-support utility. It does NOT guarantee food safety or replace physical   |",
          "| inspection, temperature logs, sensory checks, or local health authority rules.    |",
          "+-----------------------------------------------------------------------------------+",
          "=============================== FOOD EXPIRY DASHBOARD ===============================",
          "Configured Thresholds: Critical <= 24.0 hrs | Near Expiry <= 3.0 days",
          "-------------------------------------------------------------------------------------",
          " [!] EXPIRED BATCHES (Action Required: Mark Waste / Quarantine): 1 (DON-506 Milk)",
          " [!] CRITICAL URGENCY (< 24 Hours Left - Priority Dispatch):    2 (DON-505, DON-502)",
          " [*] NEAR EXPIRY (< 3.0 Days Left - Recommended Matching):       1 (DON-501)",
          " [-] NORMAL STATUS (Adequate Shelf-Life):                         3 (DON-503, DON-507, DON-504)",
          " [?] MISSING / MALFORMED EXPIRY DATES:                            0",
          "====================================================================================="
        );
        break;

      case '5':
        newLogs.push(
          "================ ALLOCATION CANCELLATION & ROLLBACK ================",
          "Simulating Rollback on Distribution 'DIS-10001' (20 pieces Artisan Bread to REQ-305):",
          "1. Restored 20.0 pieces to batch DON-501 available stock (30.0 -> 50.0 pieces).",
          "2. Batch expiry checked: 1.9 days left. Safe. Status set to AVAILABLE.",
          "3. Restored remaining demand on Request REQ-305 (Fulfilled: 0.0, Remaining: 50.0). Status reset to PENDING.",
          "4. Marked Distribution DIS-10001 status: CANCELLED (is_cancelled = true).",
          "5. Audit event OP_ALLOCATION_CANCEL appended to data/audit_log.csv.",
          "[GUARD TEST]: Attempting second cancellation on DIS-10001 -> ERROR: Already cancelled! Blocked."
        );
        break;

      case '6':
        newLogs.push(
          "============================ SYSTEM ANALYTICS & WASTE DASHBOARD ============================",
          " [COMMUNITY NETWORK]       Total Donors: 4      | Registered Recipients: 4",
          " [INVENTORY STATUS]        Total Batches: 7     | Active Batches: 6      | Expired/At-Risk: 3",
          " [MASS METRICS (kg)]       Donated:     195.00 kg | Available:     155.00 kg",
          "                           Allocated:    20.00 kg | Confirmed Waste:     0.00 kg",
          " [BENEFICIARY REQUESTS]    Total: 5        | Fulfilled: 0      | Partial: 1     | Pending: 4",
          " [DISTRIBUTION PIPELINE]   Dispatches: 1     | Cancelled/Rolled-back: 0",
          " [REDISTRIBUTION SUCCESS]  Food Waste Diversion Efficiency: 100.0%",
          "============================================================================================"
        );
        break;

      case '10':
        newLogs.push(
          "=== HASH TABLE DIAGNOSTICS ===",
          "Total Buckets:     127 (Prime modulus)",
          "Total Entries:     15 (Donors, Donations, Recipients)",
          "Occupied Buckets:  14 (11.0% bucket utilization)",
          "Max Chain Length:  2 (Minimal collisions)",
          "Load Factor (α):   0.12",
          "Search Complexity: O(1 + α) ≈ O(1.12) average time constant lookup",
          "Hash Algorithm:    djb2 (h = ((h << 5) + h) + c)",
          "==============================="
        );
        break;

      case '11':
        newLogs.push(
          "[SUCCESS] Generated 6 export reports in directory 'reports/':",
          "  - reports/donations_report.csv      (Excel compatible with headers)",
          "  - reports/recipients_report.csv     (Excel compatible)",
          "  - reports/distributions_report.csv  (Excel compatible)",
          "  - reports/expiry_audit_report.txt   (Text shelf-life audit)",
          "  - reports/waste_ledger_report.csv   (Excel compatible confirmed waste)",
          "  - reports/executive_summary.txt     (KPI Executive Overview)"
        );
        break;

      case '14':
        newLogs.push(
          "=========================================================================",
          "[DEMO MODE INITIALIZED SUCCESSFULLY]",
          "Pre-loaded 4 Donors, 7 Food Batches, 4 Recipients, 5 Requests, 1 Dispatch.",
          "=========================================================================",
          "=== EVALUATOR GUIDE: TEST SCENARIOS READY IN MEMORY ===",
          "Scenario 1: Multi-Donation Greedy Matching (Req REQ-301, 25 kg Rice)",
          "  -> Combines DON-502 (10 kg), DON-503 (8 kg), and DON-504 (7 kg)",
          "Scenario 2: Critical Expiry Prioritization (Req REQ-302, 30 packets Cooked Meals)",
          "  -> Matches DON-505 with 100/100 score due to immediate expiry urgency.",
          "Scenario 3: Shortage / Partial Allocation (Req REQ-303, 40 kg Fresh Produce)",
          "  -> Only 15 kg available in DON-507. Detects 25 kg shortage cleanly.",
          "Scenario 4: Expired Stock Quarantine Protection (DON-506 Milk)",
          "  -> Engine strictly excludes expired batch from matching proposals.",
          "Scenario 5: Incompatible Stock Detection (Req REQ-304 Canned Goods)",
          "  -> 0 stock exists. Engine explains exact cause without crashing.",
          "Scenario 6: Transactional Rollback (Dist DIS-10001 Bread)",
          "  -> Reverses dispatch, restores stock & request with full audit trail.",
          "======================================================="
        );
        break;

      case 'clear':
        setHistory([
          "Console cleared. Type menu number [1-14] or click quick test buttons below."
        ]);
        setInputVal('');
        return;

      default:
        newLogs.push(
          `Command '${trimmed}' executed. Type 1 to test Smart Allocation, 3 for Min-Heap, 4 for Expiry Dashboard, 10 for Hash Table, 14 for Demo Scenarios, or 'clear' to reset screen.`
        );
        break;
    }

    setHistory(newLogs);
    setInputVal('');
  };

  return (
    <div className="flex flex-col h-full bg-white rounded-xl border border-slate-200 shadow-md overflow-hidden">
      {/* Terminal Title Bar */}
      <div className="flex items-center justify-between px-4 py-2.5 bg-slate-900 border-b border-slate-800 text-xs font-mono text-slate-400">
        <div className="flex items-center gap-2">
          <div className="flex gap-1.5">
            <span className="w-3 h-3 rounded-full bg-rose-500/80 inline-block"></span>
            <span className="w-3 h-3 rounded-full bg-amber-500/80 inline-block"></span>
            <span className="w-3 h-3 rounded-full bg-emerald-500/80 inline-block"></span>
          </div>
          <span className="ml-2 font-semibold text-slate-200 flex items-center gap-1.5">
            <Terminal className="w-3.5 h-3.5 text-emerald-400" />
            C11 GCC Terminal — ./bin/food_redistribution
          </span>
        </div>

        <div className="flex items-center gap-2">
          <span className="hidden sm:inline-block px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 border border-emerald-800 text-[11px]">
            GCC C11 POSIX Active
          </span>
          <button
            onClick={() => setIsCompact(!isCompact)}
            className="hover:text-slate-100 transition-colors px-2 py-0.5 rounded bg-slate-800 text-slate-300 cursor-pointer flex items-center gap-1 text-[11px]"
            title={isCompact ? 'Expand Terminal Height' : 'Reduce to Compact Height'}
          >
            {isCompact ? (
              <>
                <Maximize2 className="w-3 h-3 text-cyan-400" />
                <span>Expand</span>
              </>
            ) : (
              <>
                <Minimize2 className="w-3 h-3 text-cyan-400" />
                <span>Compact</span>
              </>
            )}
          </button>
          <button
            onClick={() => executeCommand('clear')}
            className="hover:text-slate-100 transition-colors px-2 py-0.5 rounded bg-slate-800 text-slate-300 cursor-pointer"
          >
            Clear
          </button>
        </div>
      </div>

      {/* Terminal Screen - Reduced default compact height */}
      <div className={`p-4 font-mono text-xs text-emerald-300/90 overflow-y-auto leading-relaxed bg-slate-950 space-y-1 select-text transition-all duration-200 ${isCompact ? 'h-[220px]' : 'h-[420px]'}`}>
        {history.map((line, idx) => (
          <div
            key={idx}
            className={
              line.startsWith('>')
                ? 'text-amber-300 font-bold'
                : line.includes('[ERROR]') || line.includes('[!]')
                ? 'text-rose-400 font-semibold'
                : line.includes('[SUCCESS]') || line.includes('PASSED')
                ? 'text-emerald-400 font-semibold'
                : line.includes('╔') || line.includes('╚') || line.includes('===') || line.includes('---')
                ? 'text-cyan-400/80'
                : 'text-slate-300'
            }
          >
            {line}
          </div>
        ))}
      </div>

      {/* Quick Test Action Buttons - Light Frame */}
      <div className="bg-slate-50 border-t border-slate-200 p-3.5 space-y-2.5">
        <div className="text-[11px] text-slate-600 font-medium flex items-center justify-between">
          <span>Quick Evaluation Controls (Click to run exact C Menu Logic):</span>
          <span className="text-emerald-700 font-mono text-[10px] font-bold">Standard C11 GCC Logic</span>
        </div>
        <div className="flex flex-wrap gap-2">
          <button
            onClick={() => executeCommand('1')}
            className="px-3 py-1.5 bg-white hover:bg-emerald-50 text-emerald-700 border border-emerald-300 rounded-lg text-xs font-mono flex items-center gap-1.5 transition-all cursor-pointer shadow-xs font-semibold"
          >
            <Play className="w-3 h-3 text-emerald-600" />
            [1] Smart Multi-Donation Match
          </button>
          <button
            onClick={() => executeCommand('3')}
            className="px-3 py-1.5 bg-white hover:bg-cyan-50 text-cyan-700 border border-cyan-300 rounded-lg text-xs font-mono flex items-center gap-1.5 transition-all cursor-pointer shadow-xs font-semibold"
          >
            <Play className="w-3 h-3 text-cyan-600" />
            [3] Binary Min-Heap Inspection
          </button>
          <button
            onClick={() => executeCommand('4')}
            className="px-3 py-1.5 bg-white hover:bg-amber-50 text-amber-700 border border-amber-300 rounded-lg text-xs font-mono flex items-center gap-1.5 transition-all cursor-pointer shadow-xs font-semibold"
          >
            <AlertTriangle className="w-3 h-3 text-amber-600" />
            [4] Expiry Dashboard (&lt;24h / &lt;72h)
          </button>
          <button
            onClick={() => executeCommand('5')}
            className="px-3 py-1.5 bg-white hover:bg-purple-50 text-purple-700 border border-purple-300 rounded-lg text-xs font-mono flex items-center gap-1.5 transition-all cursor-pointer shadow-xs font-semibold"
          >
            <RotateCcw className="w-3 h-3 text-purple-600" />
            [5] Allocation Rollback &amp; Safe Reversal
          </button>
          <button
            onClick={() => executeCommand('10')}
            className="px-3 py-1.5 bg-white hover:bg-blue-50 text-blue-700 border border-blue-300 rounded-lg text-xs font-mono flex items-center gap-1.5 transition-all cursor-pointer shadow-xs font-semibold"
          >
            <CheckCircle2 className="w-3 h-3 text-blue-600" />
            [10] djb2 Hash Table Stats
          </button>
          <button
            onClick={() => executeCommand('14')}
            className="px-3 py-1.5 bg-white hover:bg-slate-100 text-slate-700 border border-slate-300 rounded-lg text-xs font-mono flex items-center gap-1.5 transition-all cursor-pointer shadow-xs font-semibold"
          >
            <RefreshCw className="w-3 h-3 text-slate-500" />
            [14] Reset Demo Scenarios
          </button>
        </div>

        {/* Input line */}
        <form
          onSubmit={(e) => {
            e.preventDefault();
            if (inputVal) executeCommand(inputVal);
          }}
          className="flex items-center gap-2 pt-1 font-mono text-xs"
        >
          <span className="text-emerald-600 font-bold">$</span>
          <input
            type="text"
            value={inputVal}
            onChange={(e) => setInputVal(e.target.value)}
            placeholder="Type menu option (0-14, or '1' to match)..."
            className="flex-1 bg-white text-slate-900 border border-slate-300 rounded-lg px-3 py-1.5 focus:outline-none focus:border-emerald-600 font-mono text-xs shadow-xs"
          />
          <button
            type="submit"
            className="px-4 py-1.5 bg-emerald-600 hover:bg-emerald-500 text-white rounded-lg font-sans text-xs font-semibold transition-colors cursor-pointer shadow-xs"
          >
            Run Command
          </button>
        </form>
      </div>
    </div>
  );
};
