# DATA STRUCTURES AND ALGORITHMS (DSA) COMPLEXITY ANALYSIS
## Smart Food Redistribution System — Professional DSA Edition

This document provides rigorous, defensible asymptotic time and space complexity evaluations for every data structure and algorithmic routine implemented in this project. All claimed complexities match the exact C source code without exaggerated theoretical claims.

---

### 1. Binary Min-Heap (Priority Queue)
- **Source Code**: `include/priority_queue.h`, `src/priority_queue.c`
- **Application Context**: Manages food donation batches by shelf-life urgency. The root element `nodes[0]` always holds the donation batch with the smallest `days_to_expiry` (earliest expiry date), guaranteeing immediate access to batches closest to spoiling for dispatch prioritization.
- **Physical Memory Layout**: Complete binary tree flattened sequentially into a contiguous C array `HeapNode nodes[MAX_DONATIONS]`.
  - Parent of node at index $i$: $\lfloor(i - 1) / 2\rfloor$
  - Left child of node at index $i$: $2i + 1$
  - Right child of node at index $i$: $2i + 2$

#### Operation Complexities:
| Operation | Function in C | Best Case | Average Case | Worst Case | Auxiliary Space | Explanation |
|:----------|:--------------|:----------|:-------------|:-----------|:----------------|:------------|
| **Insert** | `pq_insert()` | $O(1)$ | $O(\log N)$ | $O(\log N)$ | $O(1)$ | Appends at index $N$, calls `sift_up()`. At most $\lfloor\log_2 N\rfloor$ levels traversed. Best case occurs when inserted key is $\ge$ parent. |
| **Extract Min** | `pq_extract_min()` | $O(\log N)$ | $O(\log N)$ | $O(\log N)$ | $O(1)$ | Swaps root with leaf at $N-1$, decrements size, calls `sift_down()` from root to deepest level. |
| **Peek Root** | `pq_peek()` | $O(1)$ | $O(1)$ | $O(1)$ | $O(1)$ | Direct indexed read at `pq->nodes[0]`. |
| **Heapify / Rebuild** | `pq_rebuild_from_inventory()` | $O(N)$ | $O(N)$ | $O(N)$ | $O(1)$ | Re-evaluates all active inventory batches and restores the min-heap invariant. |

---

### 2. Hash Table with Separate Chaining
- **Source Code**: `include/hash_table.h`, `src/hash_table.c`
- **Application Context**: Accelerates record lookup by primary ID string (`donor_id`, `donation_id`, `recipient_id`) from $O(N)$ linear scanning to $O(1)$ average time.
- **Hash Function**: Dan Bernstein's `djb2` hash algorithm:
  $$hash_{i} = ((hash_{i-1} \ll 5) + hash_{i-1}) + c = hash_{i-1} \times 33 + c$$
  Modulo reduction over prime bucket count: `bucket_index = hash % 127`.
- **Collision Resolution**: Separate chaining using dynamic linked list `HashNode* buckets[127]`.

#### Operation Complexities:
| Operation | Function in C | Best Case | Average Case ($\alpha \le 1.0$) | Worst Case (All Collisions) | Auxiliary Space |
|:----------|:--------------|:----------|:-------------------------------|:----------------------------|:----------------|
| **Insert** | `ht_insert()` | $O(1)$ | $O(1)$ | $O(K)$ | $O(1)$ per node |
| **Search by Key** | `ht_search()` | $O(1)$ | $O(1 + \alpha)$ | $O(K)$ | $O(1)$ |
| **Remove** | `ht_remove()` | $O(1)$ | $O(1 + \alpha)$ | $O(K)$ | $O(1)$ |
| **Clear** | `ht_clear()` | $O(M)$ | $O(M)$ | $O(M)$ | $O(1)$ |

*Note: $\alpha = N / M$ represents the load factor where $N$ is total entries and $M=127$ is bucket capacity. For typical demonstration datasets ($N \le 50$), $\alpha \approx 0.39$, yielding virtually constant $O(1)$ lookups.*

---

### 3. Circular FIFO Request Queue
- **Source Code**: `include/request_queue.h`, `src/request_queue.c`
- **Application Context**: Enforces First-Come-First-Served (FCFS) fairness for community shelters submitting bulk food assistance requests.
- **Ring Buffer Arithmetic**:
  - `enqueue`: `rear = (rear + 1) % capacity`
  - `dequeue`: `front = (front + 1) % capacity`
  - Current Count: Tracked explicitly via integer counter `count`.

#### Operation Complexities:
| Operation | Function in C | Time Complexity | Auxiliary Space | Ring Buffer Boundary Conditions |
|:----------|:--------------|:----------------|:----------------|:--------------------------------|
| **Enqueue** | `req_queue_enqueue()` | $O(1)$ | $O(1)$ | Full check: `count == capacity` |
| **Dequeue** | `req_queue_dequeue()` | $O(1)$ | $O(1)$ | Empty check: `count == 0` |
| **Peek Front**| `req_queue_peek()` | $O(1)$ | $O(1)$ | Reads `items[front]` |
| **Find by ID**| `req_queue_find_by_id()` | $O(K)$ where $K = count$ | $O(1)$ | Sequential scan from `front` wrapping around |

---

### 4. Smart Multi-Donation Allocation Algorithm (Greedy Heuristic)
- **Source Code**: `include/allocation.h`, `src/allocation.c`
- **Application Context**: Satisfies a requested food quantity by intelligently aggregating multiple compatible donation batches while strictly prioritizing items closest to expiration.
- **Deterministic Scoring Equation**:
  $$Score(D, R) = Base(50.0) + UrgencyBonus(D) + PriorityMultiplier(R) + SingleBatchBonus(D, R)$$
  Where:
  - $UrgencyBonus(D) = 60.0$ if $\text{days} \le 1.0$ (Critical)
  - $UrgencyBonus(D) = 40.0$ if $\text{days} \le 3.0$ (Near Expiry)
  - $UrgencyBonus(D) = 20.0$ if $\text{days} \le 7.0$
  - $PriorityMultiplier(R) = \text{Priority}(R) \times 10.0 \quad (\text{range: } 10.0 \text{ to } 40.0)$
  - $SingleBatchBonus(D, R) = 15.0$ if $Available(D) \ge RemainingRequested(R)$
- **Algorithm Steps**:
  1. Filter all inventory records by exact Category equality, exact FoodUnit equality, and unexpired status: $O(N)$.
  2. Score each candidate batch using deterministic scoring equation: $O(C)$ where $C \le N$.
  3. Sort candidates descending by score using Insertion Sort: $O(C^2)$ worst case, $O(C)$ best case.
  4. Greedily deduct stock from top candidates until either demand is satisfied or candidates are exhausted: $O(M)$ where $M \le 20$.
- **Overall Time Complexity**: $O(N + C \log C)$ (bounded by $O(N)$ for typical inventory sizes).
- **Space Complexity**: $O(C)$ temporary candidate buffer on stack.

---

### 5. In-Place Stable Sorting Routines
- **Source Code**: `src/donation.c`
- **Algorithm**: Straight Insertion Sort with in-place swaps.
- **Justification**:
  - Stable: Preserves the original ordering of batches entered on the same date.
  - Zero dynamic heap memory allocation overhead ($O(1)$ auxiliary space).
  - Highly optimal for small-to-medium datasets ($N \le 1000$).
- **Time Complexity**:
  - Best Case (Already sorted): $O(N)$
  - Average Case: $O(N^2)$
  - Worst Case (Reverse sorted): $O(N^2)$
- **Space Complexity**: $O(1)$ auxiliary in-place.
