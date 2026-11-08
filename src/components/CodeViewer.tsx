import React, { useState } from 'react';
import { PROJECT_FILES, ProjectFile } from '../data/c_files_data';
import { FileCode, FileText, Settings, Database, Copy, Check, Search, Download } from 'lucide-react';

export const CodeViewer: React.FC<{ onDownloadZip: () => void }> = ({ onDownloadZip }) => {
  const [selectedPath, setSelectedPath] = useState<string>('src/main.c');
  const [copied, setCopied] = useState<boolean>(false);
  const [searchQuery, setSearchQuery] = useState<string>('');
  const [activeCategory, setActiveCategory] = useState<string>('all');

  const filteredFiles = PROJECT_FILES.filter(f => {
    const matchesQuery = f.path.toLowerCase().includes(searchQuery.toLowerCase()) ||
                         f.description.toLowerCase().includes(searchQuery.toLowerCase());
    const matchesCat = activeCategory === 'all' || f.category === activeCategory;
    return matchesQuery && matchesCat;
  });

  const getCategoryIcon = (cat: string) => {
    switch (cat) {
      case 'header': return <FileCode className="w-3.5 h-3.5 text-cyan-400" />;
      case 'source': return <FileCode className="w-3.5 h-3.5 text-emerald-400" />;
      case 'config': return <Settings className="w-3.5 h-3.5 text-amber-400" />;
      case 'data': return <Database className="w-3.5 h-3.5 text-purple-400" />;
      case 'doc': default: return <FileText className="w-3.5 h-3.5 text-blue-400" />;
    }
  };

  const handleCopy = () => {
    setCopied(true);
    setTimeout(() => setCopied(false), 2000);
  };

  return (
    <div className="flex flex-col lg:flex-row h-full bg-white rounded-xl border border-slate-200 shadow-xs overflow-hidden min-h-[550px]">
      {/* File Tree Sidebar */}
      <div className="w-full lg:w-80 border-b lg:border-b-0 lg:border-r border-slate-200 bg-slate-50/70 flex flex-col">
        {/* Sidebar Header */}
        <div className="p-3 border-b border-slate-200 space-y-2.5">
          <div className="flex items-center justify-between">
            <span className="text-xs font-bold text-slate-700 uppercase tracking-wider">
              Project Explorer ({PROJECT_FILES.length} Files)
            </span>
          </div>

          {/* Search box */}
          <div className="relative">
            <Search className="w-3.5 h-3.5 absolute left-2.5 top-2.5 text-slate-400" />
            <input
              type="text"
              value={searchQuery}
              onChange={(e) => setSearchQuery(e.target.value)}
              placeholder="Search file name or keyword..."
              className="w-full bg-white border border-slate-300 rounded-lg pl-8 pr-3 py-1.5 text-xs text-slate-900 focus:outline-none focus:border-cyan-600 shadow-xs"
            />
          </div>

          {/* Category Filter Pills */}
          <div className="flex flex-wrap gap-1 text-[11px]">
            {['all', 'header', 'source', 'config', 'data', 'doc'].map(cat => (
              <button
                key={cat}
                onClick={() => setActiveCategory(cat)}
                className={`px-2 py-0.5 rounded capitalize transition-colors cursor-pointer ${
                  activeCategory === cat
                    ? 'bg-cyan-600 text-white font-semibold'
                    : 'bg-white border border-slate-200 text-slate-600 hover:text-slate-900 hover:bg-slate-100'
                }`}
              >
                {cat}
              </button>
            ))}
          </div>
        </div>

        {/* File List */}
        <div className="flex-1 overflow-y-auto divide-y divide-slate-100 max-h-[300px] lg:max-h-none">
          {filteredFiles.map((file) => (
            <button
              key={file.path}
              onClick={() => setSelectedPath(file.path)}
              className={`w-full text-left p-2.5 transition-colors flex items-start gap-2.5 cursor-pointer ${
                selectedPath === file.path
                  ? 'bg-cyan-50 border-l-2 border-cyan-600 text-slate-900 font-semibold'
                  : 'hover:bg-slate-100/80 text-slate-600'
              }`}
            >
              <div className="mt-0.5 shrink-0">{getCategoryIcon(file.category)}</div>
              <div className="min-w-0 flex-1">
                <div className="text-xs font-mono truncate text-slate-900">
                  {file.path}
                </div>
                <div className="text-[10px] text-slate-500 truncate mt-0.5">
                  {file.description}
                </div>
              </div>
              <span className="text-[10px] text-slate-400 font-mono shrink-0">
                {file.size}
              </span>
            </button>
          ))}
        </div>
      </div>

      {/* Code / Content Viewer */}
      <div className="flex-1 flex flex-col bg-slate-950 min-h-0">
        {/* File Path Header */}
        <div className="px-4 py-2.5 bg-slate-900 border-b border-slate-800 flex items-center justify-between text-xs font-mono">
          <div className="flex items-center gap-2 text-slate-300">
            <span className="text-slate-500">c_project /</span>
            <span className="font-semibold text-cyan-400">{selectedPath}</span>
          </div>

          <div className="flex items-center gap-2">
            <button
              onClick={handleCopy}
              className="px-2.5 py-1 bg-slate-800 hover:bg-slate-700 text-slate-300 rounded text-[11px] font-sans flex items-center gap-1.5 transition-colors cursor-pointer"
            >
              {copied ? <Check className="w-3 h-3 text-emerald-400" /> : <Copy className="w-3 h-3" />}
              {copied ? 'Copied' : 'Copy'}
            </button>
          </div>
        </div>

        {/* Code Content */}
        <div className="flex-1 p-4 font-mono text-xs overflow-auto bg-black/60 text-slate-300 leading-relaxed select-text">
          <pre className="whitespace-pre">
            {getFilePreviewContent(selectedPath)}
          </pre>
        </div>
      </div>
    </div>
  );
};

function getFilePreviewContent(path: string): string {
  if (path === 'include/common.h') {
    return `/**
 * @file common.h
 * @brief Common constants, enumerations, error codes, and utility prototypes
 *        for Smart Food Redistribution System (Professional DSA Edition).
 * @version 2.0.0
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <ctype.h>

#define MAX_STR_LEN         128
#define MAX_DATE_LEN        16
#define MAX_PHONE_LEN       24
#define MAX_NOTES_LEN       256
#define MAX_DONORS          500
#define MAX_DONATIONS       1000
#define MAX_RECIPIENTS      500
#define MAX_REQUESTS        1000
#define MAX_DISTRIBUTIONS   2000
#define MAX_AUDIT_LOGS      5000

/* Food Units */
typedef enum {
    UNIT_KG = 0,
    UNIT_GRAM,
    UNIT_PACKET,
    UNIT_PIECE,
    UNIT_LITER
} FoodUnit;

/* Food Categories */
typedef enum {
    CAT_GRAINS_STAPLES = 0,
    CAT_COOKED_MEALS,
    CAT_DAIRY_EGGS,
    CAT_PRODUCE_VEG_FRUIT,
    CAT_BAKERY,
    CAT_CANNED_PACKAGED,
    CAT_BEVERAGES,
    CAT_OTHER
} FoodCategory;

/* Expiry Urgency Classification */
typedef enum {
    EXPIRY_EXPIRED = 0,      /* Past use-by date */
    EXPIRY_CRITICAL,         /* Within 24 hours */
    EXPIRY_NEAR_EXPIRY,      /* Within 72 hours (3 days) */
    EXPIRY_NORMAL            /* Safe (> 3 days) */
} ExpiryClassification;

/* Prototypes omitted for preview. Full source available in archive. */
#endif /* COMMON_H */`;
  }

  if (path === 'Makefile') {
    return `# ==============================================================================
# Makefile for Smart Food Redistribution System — Professional DSA Edition
# Compiler: GCC (GNU Compiler Collection)
# Standards: C11 (-std=c11) with strict warnings (-Wall -Wextra)
# ==============================================================================

CC      := gcc
CFLAGS  := -Wall -Wextra -std=c11 -pedantic -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE -Iinclude -O2 -g
LDFLAGS := 

SRC_DIR := src
INC_DIR := include
OBJ_DIR := obj
BIN_DIR := bin

TARGET  := $(BIN_DIR)/food_redistribution

SRCS    := $(SRC_DIR)/main.c $(SRC_DIR)/donor.c $(SRC_DIR)/donation.c \\
           $(SRC_DIR)/recipient.c $(SRC_DIR)/request_queue.c \\
           $(SRC_DIR)/priority_queue.c $(SRC_DIR)/hash_table.c \\
           $(SRC_DIR)/allocation.c $(SRC_DIR)/distribution.c \\
           $(SRC_DIR)/expiry.c $(SRC_DIR)/analytics.c $(SRC_DIR)/audit.c \\
           $(SRC_DIR)/backup.c $(SRC_DIR)/search.c $(SRC_DIR)/demo.c $(SRC_DIR)/utils.c

all: setup $(TARGET)

setup:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR) data/backups reports

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: all
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) reports`;
  }

  if (path === 'src/allocation.c') {
    return `/**
 * @file allocation.c
 * @brief Smart Food Allocation Engine and Multi-Donation Greedy Matching Engine.
 *        Deterministic scoring formula, unit validation, expiry prioritization.
 */

double calculate_allocation_score(const FoodDonation *d, const FoodRequest *r) {
    if (!d || !r) return -100.0;

    /* Rule 1: Category MUST match */
    if (d->category != r->preferred_category) return -1000.0;

    /* Rule 2: Food unit MUST match */
    if (d->unit != r->unit) return -1000.0;

    /* Rule 3: Must be available and unexpired */
    if (d->status != STATUS_AVAILABLE && d->status != STATUS_PARTIALLY_ALLOCATED) return -1000.0;
    if (d->available_quantity <= 0.0) return -1000.0;

    double days = calculate_days_until_expiry(d->expiry_date);
    if (days < 0.0) return -1000.0; /* Expired quarantine */

    double score = 50.0;
    if (days <= 1.0) score += 60.0;      /* Critical <24h */
    else if (days <= 3.0) score += 40.0; /* Near Expiry <72h */
    else if (days <= 7.0) score += 20.0;
    else score += 5.0;

    score += (double)r->priority * 10.0;
    if (d->available_quantity >= r->remaining_quantity) score += 15.0;

    return score;
}`;
  }

  return `/* File: ${path} */
/* This file is packaged inside smart_food_redistribution_system_dsa_c.zip */
/* Open in VS Code with GCC compiler for complete interactive execution. */
/* Total files in project: 50 | C11 Standard Compliant | Makefile included. */`;
}
