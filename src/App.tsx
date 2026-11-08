/**
 * @license
 * SPDX-License-Identifier: Apache-2.0
 */

import React, { useState } from 'react';
import {
  Download,
  Terminal as TerminalIcon,
  Cpu,
  Layers,
  FileCode,
  BookOpen,
  CheckCircle2,
  ExternalLink,
  ShieldCheck,
  FolderArchive,
  Code2,
  Sparkles,
  GitBranch,
} from 'lucide-react';
import { TerminalSimulator } from './components/TerminalSimulator';
import { HeapVisualizer } from './components/HeapVisualizer';
import { AllocationVisualizer } from './components/AllocationVisualizer';
import { CodeViewer } from './components/CodeViewer';
import { DocViewer } from './components/DocViewer';
import { ImageStudio } from './components/ImageStudio';

export default function App() {
  const [activeTab, setActiveTab] = useState<'terminal' | 'allocation' | 'heap' | 'images' | 'code' | 'docs'>('terminal');

  const handleDownloadZip = () => {
    const link = document.createElement('a');
    link.href = '/smart_food_redistribution_system_dsa_c.zip';
    link.download = 'smart_food_redistribution_system_dsa_c.zip';
    document.body.appendChild(link);
    link.click();
    document.body.removeChild(link);
  };

  return (
    <div className="min-h-screen bg-slate-100 text-slate-900 flex flex-col font-sans selection:bg-emerald-500 selection:text-white">
      {/* Top Banner & Header - Light Theme */}
      <header className="border-b border-slate-200 bg-white/95 backdrop-blur sticky top-0 z-50 shadow-xs">
        <div className="max-w-7xl mx-auto px-4 sm:px-6 py-3.5 flex flex-col md:flex-row items-start md:items-center justify-between gap-3">
          <div className="flex items-center gap-3">
            <div className="w-10 h-10 rounded-xl bg-gradient-to-br from-emerald-600 to-teal-700 flex items-center justify-center shadow-md shadow-emerald-950/10 text-white">
              <Code2 className="w-5 h-5 font-black" />
            </div>
            <div>
              <div className="flex items-center gap-2">
                <h1 className="font-bold text-base sm:text-lg text-slate-900 tracking-tight">
                  Smart Food Redistribution System
                </h1>
                <span className="px-2 py-0.5 rounded-full text-[10px] font-mono font-bold bg-cyan-50 text-cyan-700 border border-cyan-200">
                  DSA Edition
                </span>
                <span className="hidden sm:inline-block px-2 py-0.5 rounded-full text-[10px] font-mono font-bold bg-emerald-50 text-emerald-700 border border-emerald-200">
                  GCC C11
                </span>
              </div>
              <p className="text-xs text-slate-500 font-medium">
                Menu-Driven C Software • Binary Heap • djb2 Hash Table • Circular FIFO Queue • Greedy Knapsack
              </p>
            </div>
          </div>

          <div className="flex items-center gap-2">
            <span className="inline-flex items-center gap-1.5 px-3 py-1 rounded-full text-xs font-medium bg-emerald-50 text-emerald-700 border border-emerald-200">
              <span className="w-2 h-2 rounded-full bg-emerald-500 animate-pulse"></span>
              Core C Program Active
            </span>
          </div>
        </div>

        {/* Navigation Tabs */}
        <div className="max-w-7xl mx-auto px-4 sm:px-6 flex overflow-x-auto gap-2 border-t border-slate-200/80 pt-1 text-xs font-medium scrollbar-none">
          <button
            onClick={() => setActiveTab('terminal')}
            className={`px-3.5 py-2.5 border-b-2 flex items-center gap-2 transition-all cursor-pointer whitespace-nowrap ${
              activeTab === 'terminal'
                ? 'border-emerald-600 text-emerald-700 font-bold bg-emerald-50/50 rounded-t-lg'
                : 'border-transparent text-slate-600 hover:text-slate-900 hover:bg-slate-50'
            }`}
          >
            <TerminalIcon className="w-4 h-4 text-emerald-600" />
            <span>Interactive Terminal (GCC C11)</span>
          </button>

          <button
            onClick={() => setActiveTab('allocation')}
            className={`px-3.5 py-2.5 border-b-2 flex items-center gap-2 transition-all cursor-pointer whitespace-nowrap ${
              activeTab === 'allocation'
                ? 'border-emerald-600 text-emerald-700 font-bold bg-emerald-50/50 rounded-t-lg'
                : 'border-transparent text-slate-600 hover:text-slate-900 hover:bg-slate-50'
            }`}
          >
            <Cpu className="w-4 h-4 text-emerald-600" />
            <span>Smart Allocation Solver</span>
          </button>

          <button
            onClick={() => setActiveTab('heap')}
            className={`px-3.5 py-2.5 border-b-2 flex items-center gap-2 transition-all cursor-pointer whitespace-nowrap ${
              activeTab === 'heap'
                ? 'border-cyan-600 text-cyan-700 font-bold bg-cyan-50/50 rounded-t-lg'
                : 'border-transparent text-slate-600 hover:text-slate-900 hover:bg-slate-50'
            }`}
          >
            <Layers className="w-4 h-4 text-cyan-600" />
            <span>Binary Min-Heap Visualizer</span>
          </button>

          <button
            onClick={() => setActiveTab('images')}
            className={`px-3.5 py-2.5 border-b-2 flex items-center gap-2 transition-all cursor-pointer whitespace-nowrap ${
              activeTab === 'images'
                ? 'border-indigo-600 text-indigo-700 font-bold bg-indigo-50/50 rounded-t-lg'
                : 'border-transparent text-slate-600 hover:text-slate-900 hover:bg-slate-50'
            }`}
          >
            <Sparkles className="w-4 h-4 text-indigo-600" />
            <span>AI Image Studio (Create &amp; Edit)</span>
          </button>

          <button
            onClick={() => setActiveTab('code')}
            className={`px-3.5 py-2.5 border-b-2 flex items-center gap-2 transition-all cursor-pointer whitespace-nowrap ${
              activeTab === 'code'
                ? 'border-cyan-600 text-cyan-700 font-bold bg-cyan-50/50 rounded-t-lg'
                : 'border-transparent text-slate-600 hover:text-slate-900 hover:bg-slate-50'
            }`}
          >
            <FileCode className="w-4 h-4 text-cyan-600" />
            <span>Source Code Explorer (50 Files)</span>
          </button>

          <button
            onClick={() => setActiveTab('docs')}
            className={`px-3.5 py-2.5 border-b-2 flex items-center gap-2 transition-all cursor-pointer whitespace-nowrap ${
              activeTab === 'docs'
                ? 'border-cyan-600 text-cyan-700 font-bold bg-cyan-50/50 rounded-t-lg'
                : 'border-transparent text-slate-600 hover:text-slate-900 hover:bg-slate-50'
            }`}
          >
            <BookOpen className="w-4 h-4 text-cyan-600" />
            <span>Documentation &amp; Traceability</span>
          </button>
        </div>
      </header>

      {/* Main Content Area */}
      <main className="flex-1 max-w-7xl w-full mx-auto p-4 sm:p-6 space-y-6">
        {/* Quick Highlights Bar */}
        <div className="grid grid-cols-2 sm:grid-cols-4 gap-3 text-xs">
          <div className="bg-white border border-slate-200/90 rounded-xl p-3.5 flex items-center gap-3 shadow-xs">
            <div className="w-9 h-9 rounded-lg bg-emerald-50 border border-emerald-200 flex items-center justify-center text-emerald-600 shrink-0">
              <ShieldCheck className="w-4 h-4" />
            </div>
            <div>
              <div className="font-bold text-slate-800">100% C Architecture</div>
              <div className="text-[11px] text-slate-500">GCC C11 POSIX Compliant</div>
            </div>
          </div>

          <div className="bg-white border border-slate-200/90 rounded-xl p-3.5 flex items-center gap-3 shadow-xs">
            <div className="w-9 h-9 rounded-lg bg-cyan-50 border border-cyan-200 flex items-center justify-center text-cyan-600 shrink-0">
              <Layers className="w-4 h-4" />
            </div>
            <div>
              <div className="font-bold text-slate-800">Binary Min-Heap</div>
              <div className="text-[11px] text-slate-500">O(log N) Priority Queue</div>
            </div>
          </div>

          <div className="bg-white border border-slate-200/90 rounded-xl p-3.5 flex items-center gap-3 shadow-xs">
            <div className="w-9 h-9 rounded-lg bg-purple-50 border border-purple-200 flex items-center justify-center text-purple-600 shrink-0">
              <Sparkles className="w-4 h-4" />
            </div>
            <div>
              <div className="font-bold text-slate-800">Multi-Donation Match</div>
              <div className="text-[11px] text-slate-500">Greedy Knapsack Aggregation</div>
            </div>
          </div>

          <div className="bg-white border border-slate-200/90 rounded-xl p-3.5 flex items-center gap-3 shadow-xs">
            <div className="w-9 h-9 rounded-lg bg-amber-50 border border-amber-200 flex items-center justify-center text-amber-600 shrink-0">
              <FolderArchive className="w-4 h-4" />
            </div>
            <div>
              <div className="font-bold text-slate-800">VS Code Native</div>
              <div className="text-[11px] text-slate-500">Makefile + tasks.json</div>
            </div>
          </div>
        </div>

        {/* Tab Content Display */}
        {activeTab === 'terminal' && <TerminalSimulator onDownloadZip={handleDownloadZip} />}
        {activeTab === 'allocation' && <AllocationVisualizer />}
        {activeTab === 'heap' && <HeapVisualizer />}
        {activeTab === 'images' && <ImageStudio />}
        {activeTab === 'code' && <CodeViewer onDownloadZip={handleDownloadZip} />}
        {activeTab === 'docs' && <DocViewer />}
      </main>

      {/* Footer - Light Theme */}
      <footer className="border-t border-slate-200 bg-white py-4 text-center text-xs text-slate-500 font-mono">
        Smart Food Redistribution System • Professional DSA Edition • C11 Standard • GCC Build Pipeline Ready
      </footer>
    </div>
  );
}
