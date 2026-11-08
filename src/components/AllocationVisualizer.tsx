import React, { useState } from 'react';
import { Cpu, CheckCircle2, AlertCircle, Sparkles, Scale, Clock, ShieldCheck } from 'lucide-react';

interface CandidateDonation {
  id: string;
  name: string;
  available: number;
  unit: string;
  daysLeft: number;
  urgency: 'CRITICAL' | 'NEAR_EXPIRY' | 'NORMAL' | 'EXPIRED';
}

export const AllocationVisualizer: React.FC = () => {
  const [requestedQty, setRequestedQty] = useState<number>(25);
  const [category, setCategory] = useState<string>('Grains & Staples');
  const [priority, setPriority] = useState<number>(4); // Emergency

  // Inventory pool for Grains
  const donationsPool: CandidateDonation[] = [
    { id: 'DON-502', name: 'Basmati Rice Batch A', available: 10, unit: 'kg', daysLeft: 0.9, urgency: 'CRITICAL' },
    { id: 'DON-503', name: 'Basmati Rice Batch B', available: 8, unit: 'kg', daysLeft: 3.8, urgency: 'NEAR_EXPIRY' },
    { id: 'DON-504', name: 'Long Grain Rice Batch C', available: 12, unit: 'kg', daysLeft: 11.8, urgency: 'NORMAL' },
    { id: 'DON-506', name: 'Expired Stock Batch X', available: 20, unit: 'kg', daysLeft: -2.0, urgency: 'EXPIRED' },
  ];

  // Calculate allocation matches using deterministic algorithm
  const unexpiredPool = donationsPool.filter(d => d.daysLeft >= 0);
  
  // Scoring formula: Base(50) + UrgencyScore + Priority(10*P)
  const scored = unexpiredPool.map(d => {
    let score = 50 + priority * 10;
    if (d.daysLeft <= 1.0) score += 60;
    else if (d.daysLeft <= 3.0) score += 40;
    else if (d.daysLeft <= 7.0) score += 20;
    else score += 5;

    if (d.available >= requestedQty) score += 15;
    return { ...d, score };
  });

  // Sort descending by score
  scored.sort((a, b) => b.score - a.score);

  // Greedy match
  let remainingNeeded = requestedQty;
  const matches = [];

  for (const item of scored) {
    if (remainingNeeded <= 0.001) break;
    const take = Math.min(item.available, remainingNeeded);
    matches.push({
      ...item,
      allocated: take,
      remainingStock: item.available - take,
      reason: item.daysLeft <= 1.0
        ? 'Prioritized: Expiring within 24h (Critical Waste Prevention)'
        : item.daysLeft <= 3.0
        ? 'Near Expiry batch selected second to minimize storage risk'
        : 'Normal shelf-life batch utilized to fulfill remaining deficit',
    });
    remainingNeeded -= take;
  }

  const totalAllocated = matches.reduce((acc, cur) => acc + cur.allocated, 0);
  const shortage = Math.max(0, requestedQty - totalAllocated);
  const isSatisfied = shortage <= 0.001;

  return (
    <div className="bg-white border border-slate-200 rounded-xl p-5 shadow-xs space-y-6">
      {/* Header */}
      <div className="flex flex-col sm:flex-row items-start sm:items-center justify-between gap-4 border-b border-slate-100 pb-4">
        <div>
          <div className="flex items-center gap-2 text-emerald-700 font-bold text-sm">
            <Cpu className="w-4 h-4 text-emerald-600" />
            <span>Smart Multi-Donation Allocation Engine (Live Simulation)</span>
          </div>
          <p className="text-xs text-slate-500 mt-1">
            Simulates the greedy multi-donation knapsack matching engine. Automatically aggregates batches to fulfill requests while prioritizing at-risk stock.
          </p>
        </div>

        <div className="flex items-center gap-2 px-3 py-1 rounded-lg bg-emerald-50 border border-emerald-200 text-emerald-800 text-xs font-mono font-medium">
          <ShieldCheck className="w-3.5 h-3.5 text-emerald-600" />
          <span>Zero Black-Box AI • 100% Deterministic C</span>
        </div>
      </div>

      {/* Interactive Controls */}
      <div className="grid grid-cols-1 sm:grid-cols-3 gap-4 bg-slate-50/80 p-4 rounded-xl border border-slate-200 text-xs">
        <div className="space-y-1.5">
          <div className="flex justify-between font-medium text-slate-700">
            <span>Requested Quantity:</span>
            <span className="font-mono text-emerald-700 font-bold">{requestedQty} kg</span>
          </div>
          <input
            type="range"
            min={5}
            max={40}
            step={1}
            value={requestedQty}
            onChange={(e) => setRequestedQty(Number(e.target.value))}
            className="w-full accent-emerald-600 cursor-pointer"
          />
          <div className="flex justify-between text-[10px] text-slate-500 font-mono">
            <span>5 kg</span>
            <span>25 kg (Demo Target)</span>
            <span>40 kg</span>
          </div>
        </div>

        <div className="space-y-1.5">
          <span className="font-medium text-slate-700 block">Food Category Filter:</span>
          <select
            value={category}
            onChange={(e) => setCategory(e.target.value)}
            className="w-full bg-white border border-slate-300 rounded-lg p-2 text-slate-800 text-xs focus:outline-none focus:border-emerald-600 shadow-xs"
          >
            <option value="Grains & Staples">Grains &amp; Staples (Rice, Wheat)</option>
            <option value="Cooked Meals">Cooked Meals (Prepared Boxes)</option>
            <option value="Fresh Produce">Fresh Produce (Fruits, Veg)</option>
          </select>
        </div>

        <div className="space-y-1.5">
          <span className="font-medium text-slate-700 block">Recipient Priority Level:</span>
          <select
            value={priority}
            onChange={(e) => setPriority(Number(e.target.value))}
            className="w-full bg-white border border-slate-300 rounded-lg p-2 text-slate-800 text-xs focus:outline-none focus:border-emerald-600 font-mono shadow-xs"
          >
            <option value={4}>Level 4 — Emergency Shelter (+40 pts)</option>
            <option value={3}>Level 3 — High Need Mission (+30 pts)</option>
            <option value={2}>Level 2 — Medium (+20 pts)</option>
            <option value={1}>Level 1 — Standard (+10 pts)</option>
          </select>
        </div>
      </div>

      {/* Allocation Verdict Banner */}
      <div
        className={`p-4 rounded-xl border flex items-start gap-3 text-xs ${
          isSatisfied
            ? 'bg-emerald-50 border-emerald-200 text-emerald-900'
            : 'bg-amber-50 border-amber-200 text-amber-900'
        }`}
      >
        {isSatisfied ? (
          <CheckCircle2 className="w-5 h-5 text-emerald-600 shrink-0 mt-0.5" />
        ) : (
          <AlertCircle className="w-5 h-5 text-amber-600 shrink-0 mt-0.5" />
        )}

        <div className="space-y-1">
          <div className="font-bold flex items-center gap-2">
            <span>{isSatisfied ? 'Request Fully Satisfied (100% Demand Fulfilled)' : 'Partial Fulfillment / Stock Shortage Detected'}</span>
            <span className="font-mono px-2 py-0.5 rounded bg-white border border-slate-200 text-[11px] font-bold">
              {totalAllocated} / {requestedQty} kg
            </span>
          </div>
          <p className="text-slate-600">
            {isSatisfied
              ? `The engine combined ${matches.length} compatible donation batches to satisfy the full ${requestedQty} kg requirement without over-allocating.`
              : `Total eligible unexpired stock provides only ${totalAllocated} kg, leaving an unmet shortage deficit of ${shortage} kg.`}
          </p>
        </div>
      </div>

      {/* Match Table */}
      <div className="space-y-2">
        <h4 className="text-xs font-semibold text-slate-600 uppercase tracking-wider">
          Proposed Allocation Breakdown (Candidate Matches):
        </h4>

        <div className="overflow-x-auto rounded-lg border border-slate-200 shadow-xs">
          <table className="w-full text-left font-mono text-xs">
            <thead className="bg-slate-50 text-slate-600 text-[11px] uppercase border-b border-slate-200">
              <tr>
                <th className="p-3">#</th>
                <th className="p-3">Donation ID</th>
                <th className="p-3">Batch Name</th>
                <th className="p-3">Allocated</th>
                <th className="p-3">Remaining Stock</th>
                <th className="p-3">Days Left</th>
                <th className="p-3">Deterministic Score</th>
                <th className="p-3">Reason / Justification</th>
              </tr>
            </thead>
            <tbody className="divide-y divide-slate-100 bg-white">
              {matches.map((m, idx) => (
                <tr key={m.id} className="hover:bg-slate-50/80 transition-colors">
                  <td className="p-3 text-slate-400 font-bold">{idx + 1}</td>
                  <td className="p-3 font-semibold text-emerald-700">{m.id}</td>
                  <td className="p-3 text-slate-800">{m.name}</td>
                  <td className="p-3 font-bold text-slate-900">{m.allocated} kg</td>
                  <td className="p-3 text-slate-500">{m.remainingStock} kg</td>
                  <td className="p-3">
                    <span className={`px-2 py-0.5 rounded text-[10px] font-bold ${
                      m.daysLeft <= 1.0 ? 'bg-rose-50 text-rose-700 border border-rose-200' : 'bg-slate-100 text-slate-700'
                    }`}>
                      {m.daysLeft}d ({m.urgency})
                    </span>
                  </td>
                  <td className="p-3 font-bold text-cyan-700">{m.score.toFixed(1)}</td>
                  <td className="p-3 text-[11px] text-slate-600 font-sans">{m.reason}</td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      </div>
    </div>
  );
};
