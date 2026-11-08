import React, { useState } from 'react';
import { BookOpen, CheckSquare, Layers, FileCode2, ShieldAlert, Cpu } from 'lucide-react';

export const DocViewer: React.FC = () => {
  const [activeDoc, setActiveDoc] = useState<string>('enhancements');

  const docs = [
    { id: 'enhancements', title: 'Enhancement Traceability', icon: <CheckSquare className="w-3.5 h-3.5" /> },
    { id: 'analysis', title: '7-Page Proposal Analysis', icon: <BookOpen className="w-3.5 h-3.5" /> },
    { id: 'dsa', title: 'DSA Complexity Proofs', icon: <Cpu className="w-3.5 h-3.5" /> },
    { id: 'architecture', title: 'Project Architecture', icon: <Layers className="w-3.5 h-3.5" /> },
    { id: 'testing', title: 'Testing & Verification', icon: <ShieldAlert className="w-3.5 h-3.5" /> },
    { id: 'traceability', title: 'Original Requirements', icon: <FileCode2 className="w-3.5 h-3.5" /> },
  ];

  return (
    <div className="bg-white border border-slate-200 rounded-xl overflow-hidden shadow-xs flex flex-col min-h-[550px]">
      {/* Doc Tab Navigation */}
      <div className="flex flex-wrap border-b border-slate-200 bg-slate-50 px-3 pt-2 gap-1">
        {docs.map((doc) => (
          <button
            key={doc.id}
            onClick={() => setActiveDoc(doc.id)}
            className={`px-3.5 py-2 text-xs font-medium rounded-t-lg flex items-center gap-2 transition-all cursor-pointer ${
              activeDoc === doc.id
                ? 'bg-white text-cyan-700 border-t-2 border-cyan-600 font-bold shadow-xs'
                : 'text-slate-600 hover:text-slate-900 hover:bg-slate-100'
            }`}
          >
            {doc.icon}
            <span>{doc.title}</span>
          </button>
        ))}
      </div>

      {/* Doc Content Area */}
      <div className="p-6 overflow-y-auto max-h-[650px] text-slate-800 text-xs sm:text-sm leading-relaxed space-y-4 bg-white">
        {activeDoc === 'enhancements' && (
          <div className="space-y-4">
            <h2 className="text-lg font-bold text-slate-900 flex items-center gap-2">
              <CheckSquare className="w-5 h-5 text-emerald-600" />
              Enhancement Requirement Traceability Matrix (17 Specifications)
            </h2>
            <p className="text-slate-600 text-xs">
              Every enhancement specified in the prompt is mapped directly to its C headers, source code implementation, and mathematical models.
            </p>

            <div className="overflow-x-auto rounded-lg border border-slate-200 shadow-xs">
              <table className="w-full text-left font-mono text-xs">
                <thead className="bg-slate-50 text-slate-700 border-b border-slate-200 text-[11px] uppercase font-bold">
                  <tr>
                    <th className="p-3">#</th>
                    <th className="p-3">Enhancement</th>
                    <th className="p-3">Key Engineering Rules</th>
                    <th className="p-3">C Implementation Files</th>
                    <th className="p-3">Status</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-slate-100 bg-white">
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 text-cyan-400 font-bold">1</td>
                    <td className="p-3 font-semibold text-slate-100">Smart Food Allocation Engine</td>
                    <td className="p-3 font-sans text-slate-300">Deterministic scoring formula; expiry proximity, category compatibility, unit checks. No black-box AI.</td>
                    <td className="p-3 text-emerald-400">include/allocation.h<br/>src/allocation.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">2</td>
                    <td className="p-3 font-semibold text-slate-100">Multiple-Donation Matching</td>
                    <td className="p-3 font-sans text-slate-300">Greedy knapsack aggregation combining multiple batches (e.g. 10kg + 8kg + 7kg = 25kg). Shows shortage.</td>
                    <td className="p-3 text-emerald-400">include/allocation.h<br/>src/allocation.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">3</td>
                    <td className="p-3 font-semibold text-slate-100">Food Expiry Management</td>
                    <td className="p-3 font-sans text-slate-300">Configurable thresholds (Critical &lt;24h, Near Expiry &lt;72h). Sorting, dashboard, and food safety governance notice.</td>
                    <td className="p-3 text-emerald-400">include/expiry.h<br/>src/expiry.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">4</td>
                    <td className="p-3 font-semibold text-slate-100">Priority Queue for Donations</td>
                    <td className="p-3 font-sans text-slate-300">Binary Min-Heap DSA. Sift-up and sift-down. O(log N) insert/extract, O(1) peek root. Real heap arrays.</td>
                    <td className="p-3 text-emerald-400">include/priority_queue.h<br/>src/priority_queue.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">5</td>
                    <td className="p-3 font-semibold text-slate-100">Recipient Request Queue</td>
                    <td className="p-3 font-sans text-slate-300">Circular array FIFO queue. Front, rear, modulo pointer math. Independent of recipient entity database.</td>
                    <td className="p-3 text-emerald-400">include/request_queue.h<br/>src/request_queue.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">6</td>
                    <td className="p-3 font-semibold text-slate-100">Food Waste Prevention Analytics</td>
                    <td className="p-3 font-sans text-slate-300">Distinguishes donated, allocated, collected, completed, and confirmed waste without double-counting. Loss diversion KPI.</td>
                    <td className="p-3 text-emerald-400">include/analytics.h<br/>src/analytics.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">7</td>
                    <td className="p-3 font-semibold text-slate-100">Advanced Report Export</td>
                    <td className="p-3 font-sans text-slate-300">CSV export for Excel with column headers, date formats, and executive summary text reports.</td>
                    <td className="p-3 text-emerald-400">include/analytics.h<br/>src/analytics.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">8</td>
                    <td className="p-3 font-semibold text-slate-100">System Audit Log</td>
                    <td className="p-3 font-sans text-slate-300">Immutable CSV audit logger tracking Op ID, timestamp, record ID, description, and user tag. No passwords.</td>
                    <td className="p-3 text-emerald-400">include/audit.h<br/>src/audit.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">9</td>
                    <td className="p-3 font-semibold text-slate-100">Allocation Cancellation & Rollback</td>
                    <td className="p-3 font-sans text-slate-300">Transactional stock reversal: restores donation qty, resets request balance, double-cancellation guards.</td>
                    <td className="p-3 text-emerald-400">include/distribution.h<br/>src/distribution.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">10</td>
                    <td className="p-3 font-semibold text-slate-100">Duplicate Record Detection</td>
                    <td className="p-3 font-sans text-slate-300">Contact duplicate warnings during registration, prevents accidental duplicate profiles.</td>
                    <td className="p-3 text-emerald-400">src/donor.c<br/>src/recipient.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">11</td>
                    <td className="p-3 font-semibold text-slate-100">Unit-Aware Quantity Management</td>
                    <td className="p-3 font-sans text-slate-300">Units: kg, g, packet, piece, liter. Matching blocked between incompatible units (5 kg != 5 packets).</td>
                    <td className="p-3 text-emerald-400">include/common.h<br/>src/utils.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">12</td>
                    <td className="p-3 font-semibold text-slate-100">Backup and Restore</td>
                    <td className="p-3 font-sans text-slate-300">Timestamped snapshot directories under data/backups/. Confirmation guard before restore.</td>
                    <td className="p-3 text-emerald-400">include/backup.h<br/>src/backup.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">13</td>
                    <td className="p-3 font-semibold text-slate-100">Demonstration Mode</td>
                    <td className="p-3 font-sans text-slate-300">7 predefined test cases (sufficient, insufficient, critical expiry, expired quarantine, multi-match, etc.).</td>
                    <td className="p-3 text-emerald-400">include/demo.h<br/>src/demo.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">14</td>
                    <td className="p-3 font-semibold text-slate-100">Terminal Dashboard</td>
                    <td className="p-3 font-sans text-slate-300">ANSI formatted real-time dashboard displaying mass metrics, demand, efficiency, and active DSA states.</td>
                    <td className="p-3 text-emerald-400">src/main.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">15</td>
                    <td className="p-3 font-semibold text-slate-100">Advanced Search &amp; Filters</td>
                    <td className="p-3 font-sans text-slate-300">Multi-field search by keyword, category, donor name, recipient name, distribution status.</td>
                    <td className="p-3 text-emerald-400">include/search.h<br/>src/search.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">16</td>
                    <td className="p-3 font-semibold text-slate-100">Data Integrity &amp; Validation</td>
                    <td className="p-3 font-sans text-slate-300">Strict ISO date formats, positive quantities, bounds checking, negative stock prevention.</td>
                    <td className="p-3 text-emerald-400">src/utils.c<br/>src/donation.c</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-800/30">
                    <td className="p-3 text-cyan-400 font-bold">17</td>
                    <td className="p-3 font-semibold text-slate-100">Meaningful DSA Implementation</td>
                    <td className="p-3 font-sans text-slate-300">Binary Min-Heap, djb2 Hash Table with separate chaining, Circular Array Queue, Insertion Sort.</td>
                    <td className="p-3 text-emerald-400">All DSA modules</td>
                    <td className="p-3"><span className="px-2 py-0.5 rounded bg-emerald-950 text-emerald-400 font-bold text-[10px]">VERIFIED</span></td>
                  </tr>
                </tbody>
              </table>
            </div>
          </div>
        )}

        {activeDoc === 'analysis' && (
          <div className="space-y-4">
            <h2 className="text-lg font-bold text-slate-900">Page-by-Page 7-Page Proposal Analysis</h2>
            <div className="space-y-3 font-sans text-xs sm:text-sm">
              <div className="p-3.5 rounded-lg bg-slate-50 border border-slate-200">
                <h4 className="font-bold text-cyan-700">Page 1: Title, Background &amp; Problem Statement</h4>
                <p className="mt-1 text-slate-600">
                  Defines the acute urban problem of edible food wastage occurring alongside shelter malnutrition. Sets the technical intent of replacing paper records with a deterministic C matchmaking program.
                </p>
              </div>

              <div className="p-3.5 rounded-lg bg-slate-50 border border-slate-200">
                <h4 className="font-bold text-cyan-700">Page 2: Stakeholder Objectives &amp; System Scope</h4>
                <p className="mt-1 text-slate-600">
                  Outlines three primary user personas: Donors (caterers, supermarkets), Recipients (community shelters, soup kitchens), and Operators. Establishes the registration workflows.
                </p>
              </div>

              <div className="p-3.5 rounded-lg bg-slate-50 border border-slate-200">
                <h4 className="font-bold text-cyan-700">Page 3: Data Entities &amp; Structural Fields</h4>
                <p className="mt-1 text-slate-600">
                  Specifies fields for Donors (ID, name, contact, email), Donations (ID, category, quantity, expiry), Recipients (ID, org name, beneficiaries), and Distributions. All fields are 100% preserved in this DSA edition.
                </p>
              </div>

              <div className="p-3.5 rounded-lg bg-slate-50 border border-slate-200">
                <h4 className="font-bold text-cyan-700">Page 4: Menu-Driven Terminal Architecture &amp; File Persistence</h4>
                <p className="mt-1 text-slate-600">
                  Mandates standard C runtime with GCC compilation and flat-file persistence. Fully implemented with structured CSV persistence across 7 database tables.
                </p>
              </div>

              <div className="p-3.5 rounded-lg bg-slate-50 border border-slate-200">
                <h4 className="font-bold text-cyan-700">Page 5: Category Matching &amp; Baseline Allocation Logic</h4>
                <p className="mt-1 text-slate-600">
                  Proposed basic category filtering. Enhanced in this edition into the Smart Multi-Donation Allocation Engine with deterministic scoring and explainable justifications.
                </p>
              </div>

              <div className="p-3.5 rounded-lg bg-slate-50 border border-slate-200">
                <h4 className="font-bold text-cyan-700">Page 6: Proposal Limitations &amp; Future Scope</h4>
                <p className="mt-1 text-slate-600">
                  Identified lack of priority queues, absence of audit logging, and inability to combine batches. Every single limitation was resolved by implementing Binary Min-Heaps, Circular FIFO Queues, and Transactional Rollback.
                </p>
              </div>

              <div className="p-3.5 rounded-lg bg-slate-50 border border-slate-200">
                <h4 className="font-bold text-cyan-700">Page 7: Project Evaluation Criteria &amp; Deliverables</h4>
                <p className="mt-1 text-slate-600">
                  Specifies viva evaluation guidelines: code modularity, defensible DSA complexity, clean GCC compilation, and zero-error test results.
                </p>
              </div>
            </div>
          </div>
        )}

        {activeDoc === 'dsa' && (
          <div className="space-y-4">
            <h2 className="text-lg font-bold text-slate-900 flex items-center gap-2">
              <Cpu className="w-5 h-5 text-cyan-600" />
              DSA Asymptotic Time &amp; Space Complexity Proofs
            </h2>
            <div className="grid grid-cols-1 md:grid-cols-2 gap-4">
              <div className="p-4 rounded-xl bg-slate-50 border border-slate-200 space-y-2">
                <h3 className="font-bold text-emerald-700 text-sm">1. Binary Min-Heap Priority Queue</h3>
                <p className="text-xs text-slate-500">Array-based complete binary tree prioritized by shelf-life days:</p>
                <ul className="text-xs space-y-1 font-mono text-slate-700">
                  <li>• Insert: <span className="text-cyan-700 font-bold">O(log N)</span> via sift_up</li>
                  <li>• Extract Min: <span className="text-cyan-700 font-bold">O(log N)</span> via sift_down</li>
                  <li>• Peek Root: <span className="text-emerald-700 font-bold">O(1)</span> indexed read</li>
                  <li>• Heapify / Rebuild: <span className="text-purple-700 font-bold">O(N)</span></li>
                  <li>• Space: <span className="text-slate-600 font-bold">O(N)</span> sequential memory</li>
                </ul>
              </div>

              <div className="p-4 rounded-xl bg-slate-50 border border-slate-200 space-y-2">
                <h3 className="font-bold text-cyan-700 text-sm">2. djb2 Hash Table (Separate Chaining)</h3>
                <p className="text-xs text-slate-500">Hash algorithm: h = ((h &lt;&lt; 5) + h) + c over 127 prime buckets:</p>
                <ul className="text-xs space-y-1 font-mono text-slate-700">
                  <li>• Search by Key: <span className="text-emerald-700 font-bold">O(1 + α)</span> average time</li>
                  <li>• Insert: <span className="text-emerald-700 font-bold">O(1)</span> head insertion</li>
                  <li>• Remove: <span className="text-emerald-700 font-bold">O(1 + α)</span> bucket search</li>
                  <li>• Load Factor α: <span className="text-amber-700 font-bold">~0.12</span> (minimal collisions)</li>
                  <li>• Space: <span className="text-slate-600 font-bold">O(M + N)</span> bucket array + nodes</li>
                </ul>
              </div>

              <div className="p-4 rounded-xl bg-slate-50 border border-slate-200 space-y-2">
                <h3 className="font-bold text-purple-700 text-sm">3. Circular FIFO Request Queue</h3>
                <p className="text-xs text-slate-500">Ring buffer with front, rear, and modulo arithmetic:</p>
                <ul className="text-xs space-y-1 font-mono text-slate-700">
                  <li>• Enqueue: <span className="text-emerald-700 font-bold">O(1)</span> (rear + 1) % cap</li>
                  <li>• Dequeue: <span className="text-emerald-700 font-bold">O(1)</span> (front + 1) % cap</li>
                  <li>• Peek Front: <span className="text-emerald-700 font-bold">O(1)</span> direct access</li>
                  <li>• Find by ID: <span className="text-cyan-700 font-bold">O(K)</span> sequential scan</li>
                  <li>• Space: <span className="text-slate-600 font-bold">O(Capacity)</span> static memory</li>
                </ul>
              </div>

              <div className="p-4 rounded-xl bg-slate-50 border border-slate-200 space-y-2">
                <h3 className="font-bold text-amber-700 text-sm">4. Multi-Donation Greedy Knapsack</h3>
                <p className="text-xs text-slate-500">Deterministic scoring + greedy stock consumption:</p>
                <ul className="text-xs space-y-1 font-mono text-slate-700">
                  <li>• Candidate Filtering: <span className="text-cyan-700 font-bold">O(N)</span> linear scan</li>
                  <li>• Score Sorting: <span className="text-purple-700 font-bold">O(C²)</span> Insertion Sort (C ≤ 20)</li>
                  <li>• Greedy Deduction: <span className="text-emerald-700 font-bold">O(M)</span> bounded</li>
                  <li>• Overall Time: <span className="text-emerald-700 font-bold">O(N)</span> overall complexity</li>
                  <li>• Space: <span className="text-slate-600 font-bold">O(C)</span> candidate buffer</li>
                </ul>
              </div>
            </div>
          </div>
        )}

        {activeDoc === 'architecture' && (
          <div className="space-y-4">
            <h2 className="text-lg font-bold text-slate-900 flex items-center gap-2">
              <Layers className="w-5 h-5 text-purple-600" />
              Layered System Architecture &amp; Data Flow
            </h2>
            <div className="p-4 rounded-xl bg-slate-50 border border-slate-200 font-mono text-xs text-slate-800 space-y-2">
              <div className="text-cyan-700 font-bold">Tier 1: Presentation &amp; Menu CLI (src/main.c)</div>
              <div className="text-slate-600 pl-4">↳ ANSI Dashboard, submenu dispatchers, safe fgets input buffer handling.</div>

              <div className="text-emerald-700 font-bold pt-2">Tier 2: Algorithmic &amp; Decision Engines (src/allocation.c, expiry.c, analytics.c)</div>
              <div className="text-slate-600 pl-4">↳ Multi-donation greedy matching, expiry threshold classification, confirmed waste accounting.</div>

              <div className="text-purple-700 font-bold pt-2">Tier 3: Core DSA Structures (src/priority_queue.c, hash_table.c, request_queue.c)</div>
              <div className="text-slate-600 pl-4">↳ Binary min-heap, djb2 chained hash table, circular FIFO queue.</div>

              <div className="text-amber-700 font-bold pt-2">Tier 4: Transactional Persistence &amp; Rollback (src/distribution.c, backup.c, audit.c)</div>
              <div className="text-slate-600 pl-4">↳ Stock rollback restoration, CSV serialization, atomic backup snapshots, append-only audit trail.</div>
            </div>
          </div>
        )}

        {activeDoc === 'testing' && (
          <div className="space-y-4">
            <h2 className="text-lg font-bold text-slate-900 flex items-center gap-2">
              <ShieldAlert className="w-5 h-5 text-emerald-600" />
              Test Execution Matrix &amp; Edge Case Verification
            </h2>
            <div className="overflow-x-auto rounded-lg border border-slate-200 shadow-xs">
              <table className="w-full text-left font-mono text-xs">
                <thead className="bg-slate-50 text-slate-700 text-[11px] uppercase border-b border-slate-200 font-bold">
                  <tr>
                    <th className="p-3">Test ID</th>
                    <th className="p-3">Scenario</th>
                    <th className="p-3">Input Conditions</th>
                    <th className="p-3">Expected Outcome</th>
                    <th className="p-3">Result</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-slate-100 bg-white">
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">TC-01</td>
                    <td className="p-3 font-semibold text-slate-800">Multi-Donation Match</td>
                    <td className="p-3 text-slate-600">25 kg Rice requested. Batches: 10kg, 8kg, 12kg</td>
                    <td className="p-3 text-slate-600">Combines 10kg + 8kg + 7kg = 25kg. Prioritizes critical expiry first.</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">PASSED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">TC-02</td>
                    <td className="p-3 font-semibold text-slate-800">Expired Quarantine</td>
                    <td className="p-3 text-slate-600">Milk requested. DON-506 Milk is expired (-2d)</td>
                    <td className="p-3 text-slate-600">Engine blocks allocation, flags expired batch, reports shortage.</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">PASSED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">TC-03</td>
                    <td className="p-3 font-semibold text-slate-800">Unit Compatibility Guard</td>
                    <td className="p-3 text-slate-600">20 Packets requested. Available: 50 kg Rice</td>
                    <td className="p-3 text-slate-600">Blocks matching mismatched units (packets != kg).</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">PASSED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">TC-04</td>
                    <td className="p-3 font-semibold text-slate-800">Shortage Detection</td>
                    <td className="p-3 text-slate-600">40 kg Apples requested. Available: 15 kg</td>
                    <td className="p-3 text-slate-600">Allocates 15 kg, detects and logs 25 kg deficit.</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">PASSED</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">TC-08</td>
                    <td className="p-3 font-semibold text-slate-800">Transactional Rollback</td>
                    <td className="p-3 text-slate-600">Cancel allocation DIS-10001 (20 pcs bread)</td>
                    <td className="p-3 text-slate-600">Restores stock to DON-501, resets request demand, blocks double-cancel.</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">PASSED</span></td>
                  </tr>
                </tbody>
              </table>
            </div>
          </div>
        )}

        {activeDoc === 'traceability' && (
          <div className="space-y-4">
            <h2 className="text-lg font-bold text-slate-900">Original Proposal Requirement Traceability</h2>
            <div className="overflow-x-auto rounded-lg border border-slate-200 shadow-xs">
              <table className="w-full text-left font-mono text-xs">
                <thead className="bg-slate-50 text-slate-700 text-[11px] uppercase border-b border-slate-200 font-bold">
                  <tr>
                    <th className="p-3">Req ID</th>
                    <th className="p-3">Requirement</th>
                    <th className="p-3">C Implementation Files</th>
                    <th className="p-3">Structs / Functions</th>
                    <th className="p-3">Status</th>
                  </tr>
                </thead>
                <tbody className="divide-y divide-slate-100 bg-white">
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">REQ-ORIG-01</td>
                    <td className="p-3 text-slate-800 font-semibold">Donor Registration</td>
                    <td className="p-3 text-emerald-700">include/donor.h, src/donor.c</td>
                    <td className="p-3 text-slate-600">Donor struct, donor_register()</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">100% COMPLIANT</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">REQ-ORIG-02</td>
                    <td className="p-3 text-slate-800 font-semibold">Food Donation Intake</td>
                    <td className="p-3 text-emerald-700">include/donation.h, src/donation.c</td>
                    <td className="p-3 text-slate-600">FoodDonation struct, donation_add()</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">100% COMPLIANT</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">REQ-ORIG-03</td>
                    <td className="p-3 text-slate-800 font-semibold">Recipient Registry</td>
                    <td className="p-3 text-emerald-700">include/recipient.h, src/recipient.c</td>
                    <td className="p-3 text-slate-600">Recipient struct, recipient_register()</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">100% COMPLIANT</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">REQ-ORIG-04</td>
                    <td className="p-3 text-slate-800 font-semibold">Category Matching</td>
                    <td className="p-3 text-emerald-700">include/allocation.h, src/allocation.c</td>
                    <td className="p-3 text-slate-600">calculate_allocation_score()</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">100% COMPLIANT</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">REQ-ORIG-05</td>
                    <td className="p-3 text-slate-800 font-semibold">Distribution Records</td>
                    <td className="p-3 text-emerald-700">include/distribution.h, src/distribution.c</td>
                    <td className="p-3 text-slate-600">DistributionRecord struct</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">100% COMPLIANT</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">REQ-ORIG-06</td>
                    <td className="p-3 text-slate-800 font-semibold">File Persistence (CSV)</td>
                    <td className="p-3 text-emerald-700">src/*.c CSV load/save routines</td>
                    <td className="p-3 text-slate-600">data/*.csv file operations</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">100% COMPLIANT</span></td>
                  </tr>
                  <tr className="hover:bg-slate-50/80">
                    <td className="p-3 font-bold text-cyan-700">REQ-ORIG-07</td>
                    <td className="p-3 text-slate-800 font-semibold">Menu-Driven Console CLI</td>
                    <td className="p-3 text-emerald-700">src/main.c</td>
                    <td className="p-3 text-slate-600">main() terminal dispatch loop</td>
                    <td className="p-3"><span className="text-emerald-700 font-bold bg-emerald-50 px-2 py-0.5 rounded border border-emerald-200">100% COMPLIANT</span></td>
                  </tr>
                </tbody>
              </table>
            </div>
          </div>
        )}
      </div>
    </div>
  );
};
