import React, { useState } from 'react';
import { Layers, ArrowDown, Sparkles, Clock, Scale } from 'lucide-react';

interface HeapNode {
  id: string;
  foodName: string;
  category: string;
  qty: number;
  unit: string;
  daysLeft: number;
  urgency: 'CRITICAL' | 'NEAR_EXPIRY' | 'NORMAL';
}

export const HeapVisualizer: React.FC = () => {
  const initialNodes: HeapNode[] = [
    { id: 'DON-505', foodName: 'Prepared Vegetable Curry', category: 'Cooked Meals', qty: 30, unit: 'pkts', daysLeft: 0.8, urgency: 'CRITICAL' },
    { id: 'DON-502', foodName: 'Basmati Rice Batch A', category: 'Grains & Staples', qty: 10, unit: 'kg', daysLeft: 0.9, urgency: 'CRITICAL' },
    { id: 'DON-501', foodName: 'Artisan Bread Loaves', category: 'Bakery', qty: 50, unit: 'pcs', daysLeft: 1.9, urgency: 'NEAR_EXPIRY' },
    { id: 'DON-503', foodName: 'Basmati Rice Batch B', category: 'Grains & Staples', qty: 8, unit: 'kg', daysLeft: 3.8, urgency: 'NEAR_EXPIRY' },
    { id: 'DON-507', foodName: 'Fresh Crisp Apples', category: 'Fresh Produce', qty: 15, unit: 'kg', daysLeft: 5.8, urgency: 'NORMAL' },
    { id: 'DON-504', foodName: 'Long Grain Rice Batch C', category: 'Grains & Staples', qty: 12, unit: 'kg', daysLeft: 11.8, urgency: 'NORMAL' },
  ];

  const [heap, setHeap] = useState<HeapNode[]>(initialNodes);
  const [lastAction, setLastAction] = useState<string>('Heap initialized with 6 active unexpired food batches.');

  const extractMin = () => {
    if (heap.length === 0) return;
    const min = heap[0];
    const remaining = [...heap];
    const last = remaining.pop();

    if (remaining.length > 0 && last) {
      remaining[0] = last;
      // Sift down
      let idx = 0;
      while (true) {
        let smallest = idx;
        const left = 2 * idx + 1;
        const right = 2 * idx + 2;

        if (left < remaining.length && remaining[left].daysLeft < remaining[smallest].daysLeft) {
          smallest = left;
        }
        if (right < remaining.length && remaining[right].daysLeft < remaining[smallest].daysLeft) {
          smallest = right;
        }

        if (smallest !== idx) {
          const temp = remaining[idx];
          remaining[idx] = remaining[smallest];
          remaining[smallest] = temp;
          idx = smallest;
        } else {
          break;
        }
      }
    }

    setHeap(remaining);
    setLastAction(`Extracted Root Min: ${min.id} (${min.foodName}, ${min.daysLeft}d left). Sift-down executed in O(log N) time.`);
  };

  const resetHeap = () => {
    setHeap(initialNodes);
    setLastAction('Binary Min-Heap restored to default evaluation dataset.');
  };

  return (
    <div className="bg-white border border-slate-200 rounded-xl p-5 shadow-xs space-y-6">
      {/* Header */}
      <div className="flex flex-col sm:flex-row items-start sm:items-center justify-between gap-4 border-b border-slate-100 pb-4">
        <div>
          <div className="flex items-center gap-2 text-cyan-700 font-bold text-sm">
            <Layers className="w-4 h-4 text-cyan-600" />
            <span>Binary Min-Heap Priority Queue (DSA Visualizer)</span>
          </div>
          <p className="text-xs text-slate-500 mt-1">
            Organizes donations by shelf-life urgency. Root element <code className="text-emerald-700 font-mono font-bold">nodes[0]</code> always guarantees immediate access to the earliest expiring batch.
          </p>
        </div>

        <div className="flex items-center gap-2">
          <button
            onClick={extractMin}
            disabled={heap.length === 0}
            className="px-3.5 py-1.5 bg-cyan-600 hover:bg-cyan-700 disabled:opacity-40 text-white rounded-lg text-xs font-semibold flex items-center gap-1.5 transition-colors cursor-pointer shadow-xs"
          >
            <Sparkles className="w-3.5 h-3.5" />
            Extract Min O(log N)
          </button>
          <button
            onClick={resetHeap}
            className="px-3 py-1.5 bg-white hover:bg-slate-50 text-slate-700 border border-slate-300 rounded-lg text-xs font-medium transition-colors cursor-pointer shadow-xs"
          >
            Reset Heap
          </button>
        </div>
      </div>

      {/* Action Notification */}
      <div className="bg-cyan-50 border border-cyan-200 rounded-lg px-3.5 py-2 text-xs text-cyan-800 font-mono flex items-center gap-2">
        <span className="w-2 h-2 rounded-full bg-cyan-600 animate-ping inline-block"></span>
        <span>{lastAction}</span>
      </div>

      {/* Tree Visualization */}
      <div className="space-y-4">
        <h4 className="text-xs font-semibold text-slate-600 uppercase tracking-wider">
          Complete Binary Tree Representation:
        </h4>

        {heap.length === 0 ? (
          <div className="text-center py-8 text-slate-400 text-xs font-mono">Heap is currently empty.</div>
        ) : (
          <div className="space-y-6">
            {/* Level 0: Root */}
            <div className="flex justify-center">
              <NodeCard node={heap[0]} index={0} isRoot />
            </div>

            {/* Level 1: Children of 0 (Indices 1, 2) */}
            {heap.length > 1 && (
              <div className="flex justify-center gap-12 sm:gap-24 relative">
                {heap[1] && <NodeCard node={heap[1]} index={1} />}
                {heap[2] && <NodeCard node={heap[2]} index={2} />}
              </div>
            )}

            {/* Level 2: Children of 1, 2 (Indices 3, 4, 5, 6) */}
            {heap.length > 3 && (
              <div className="grid grid-cols-2 sm:grid-cols-4 gap-4 justify-items-center">
                {heap[3] && <NodeCard node={heap[3]} index={3} />}
                {heap[4] && <NodeCard node={heap[4]} index={4} />}
                {heap[5] && <NodeCard node={heap[5]} index={5} />}
                {heap[6] && <NodeCard node={heap[6]} index={6} />}
              </div>
            )}
          </div>
        )}
      </div>

      {/* Array Memory Representation */}
      <div className="space-y-2 pt-2 border-t border-slate-100">
        <div className="flex items-center justify-between">
          <h4 className="text-xs font-semibold text-slate-600 uppercase tracking-wider">
            Sequential Array Memory Representation (Index Mapping):
          </h4>
          <span className="text-[11px] text-slate-500 font-mono">Parent: ⌊(i-1)/2⌋ | Left: 2i+1 | Right: 2i+2</span>
        </div>

        <div className="grid grid-cols-2 sm:grid-cols-6 gap-2 font-mono text-xs">
          {heap.map((node, idx) => (
            <div
              key={node.id}
              className={`p-2.5 rounded-lg border text-center ${
                idx === 0
                  ? 'bg-emerald-50 border-emerald-300 text-emerald-900 font-bold'
                  : 'bg-slate-50 border-slate-200 text-slate-800'
              }`}
            >
              <div className="text-[10px] text-slate-500 font-bold mb-1">Index #{idx} {idx === 0 && '(ROOT)'}</div>
              <div className="font-semibold truncate">{node.id}</div>
              <div className="text-[11px] text-cyan-700 font-bold">{node.daysLeft}d left</div>
              <div className="text-[10px] text-slate-500 mt-0.5">{node.qty} {node.unit}</div>
            </div>
          ))}
        </div>
      </div>
    </div>
  );
};

const NodeCard: React.FC<{ node: HeapNode; index: number; isRoot?: boolean }> = ({ node, index, isRoot }) => {
  const urgencyColors = {
    CRITICAL: 'border-rose-300 bg-rose-50 text-rose-900',
    NEAR_EXPIRY: 'border-amber-300 bg-amber-50 text-amber-900',
    NORMAL: 'border-slate-200 bg-slate-50 text-slate-800',
  };

  return (
    <div
      className={`relative w-44 p-3 rounded-xl border shadow-xs transition-all ${
        isRoot ? 'ring-2 ring-emerald-500 shadow-emerald-100' : ''
      } ${urgencyColors[node.urgency]}`}
    >
      <div className="flex items-center justify-between text-[10px] font-mono mb-1">
        <span className="px-1.5 py-0.2 rounded bg-white/80 border border-slate-200 text-slate-700 font-bold">
          Idx #{index} {isRoot && '★ ROOT'}
        </span>
        <span className="font-bold flex items-center gap-1">
          <Clock className="w-2.5 h-2.5" />
          {node.daysLeft}d left
        </span>
      </div>

      <div className="font-bold text-xs truncate text-slate-900">{node.foodName}</div>
      <div className="text-[11px] text-slate-500 truncate mt-0.5">{node.id} ({node.category})</div>

      <div className="mt-2 pt-1.5 border-t border-slate-200/60 flex items-center justify-between text-[11px] font-mono">
        <span className="flex items-center gap-1 text-slate-700 font-semibold">
          <Scale className="w-3 h-3 text-cyan-600" />
          {node.qty} {node.unit}
        </span>
        <span className="text-[10px] px-1.5 py-0.5 rounded bg-white/80 border border-slate-200 font-semibold">
          {node.urgency}
        </span>
      </div>
    </div>
  );
};
