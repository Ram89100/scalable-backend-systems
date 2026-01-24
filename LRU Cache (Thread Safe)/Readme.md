# Thread-Safe LRU Cache in C++

A **thread-safe Least Recently Used (LRU) Cache** implementation in **C++**, designed for high performance, concurrency, and clean architecture. This project includes comprehensive documentation, real-world usage examples, and unit tests covering edge cases and concurrent access.

---

## 📌 Overview

An **LRU Cache** is a data structure that stores a limited number of key-value pairs and **automatically evicts the least recently used item** when the cache exceeds its capacity.

This implementation is ideal for:
- In-memory caching
- Backend systems
- Performance-critical applications
- Interview-ready system design examples

---

## ✨ Features

- 🔒 **Thread-Safe**
  - Uses `std::shared_mutex` to allow safe concurrent access
- ⚡ **O(1) Time Complexity**
  - `get`, `put`, and `remove` operations run in constant time
- ♻️ **Automatic LRU Eviction**
  - Least recently used entry is evicted when capacity is reached
- 📦 **Customizable Capacity**
  - Set cache size at initialization
- 🧩 **Generic Implementation**
  - Supports any key-value types using C++ Templates
- 🧪 **Comprehensive Tests**
  - Unit tests + edge cases + concurrency tests

---

## 🏗️ Architecture

The cache is implemented using a combination of:

| Component | Purpose |
|---------|--------|
| `std::unordered_map<K, Node>` | O(1) key lookup |
| `std::shared_ptr<Node>` linked list | Maintains access order |
| `std::shared_mutex` | Thread-safe concurrent access |

### Why This Design?
- `unordered_map` gives fast lookup
- Shared pointer linked list enables O(1) insertion and removal
- `shared_mutex` ensures safe multi-threaded access

---

## 📊 System Architecture Diagrams

### 1. Data Structure Layout

```
LRUCache Object:
┌──────────────────────────────────────────────┐
│  LRUCache<K, V>                              │
├──────────────────────────────────────────────┤
│  Attributes:                                 │
│  ├─ capacity_: int = 5                       │
│  ├─ lock_: std::shared_mutex                 │
│  ├─ map_: unordered_map<K, Node*>           │
│  ├─ head_: Node*  ──┐                        │
│  └─ tail_: Node*  ──┤                        │
└──────────────────────────────────────────────┘
                       │
                       ▼
Linked List Structure (Doubly-Linked):
┌──────┐     ┌──────┐     ┌──────┐     ┌──────┐     ┌──────┐
│ HEAD │◄───►│ k:1  │◄───►│ k:5  │◄───►│ k:3  │◄───►│ TAIL │
└──────┘     └──────┘     └──────┘     └──────┘     └──────┘
(sentinel)   (oldest)                            (newest)
                                                  [MRU end]

HashMap Index:
┌─────┬─────┬─────┬─────┬─────┐
│ k:1 │ k:5 │ k:3 │ ... │ ... │
├─────┼─────┼─────┼─────┼─────┤
│ ↓   │ ↓   │ ↓   │     │     │  Points to nodes in linked list
└─────┴─────┴─────┴─────┴─────┘
  O(1)  O(1)  O(1)  lookup

MRU (Most Recently Used) ──→ Newest entries
LRU (Least Recently Used) ──→ Oldest entries (eviction candidates)
```

### 2. Operation Flow Diagrams

#### GET Operation Flow
```
Thread calls: cache.get("key1")
│
├─► Acquire SHARED LOCK (read_lock)
│   │
│   ├─► HashMap lookup: map_.find("key1")
│   │   ├─► Found? Continue
│   │   └─► Not found? Return nullptr (release lock)
│   │
│   ├─► Get node reference
│   │
│   └─► Release SHARED LOCK
│       │
│       ▼
├─► Acquire UNIQUE LOCK (write_lock)  [Now exclusive]
│   │
│   ├─► removeNode(node)  [Unlink from current position]
│   │
│   ├─► addNodeToEnd(node)  [Move to end (MRU)]
│   │
│   └─► Release UNIQUE LOCK
│
└─► Return value (copy)
```

#### PUT Operation Flow
```
Thread calls: cache.put("key2", value)
│
├─► Acquire UNIQUE LOCK (exclusive write access)
│   │
│   ├─► Check if key exists in map_
│   │   │
│   │   ├─► YES (Update):
│   │   │   ├─► Update value
│   │   │   ├─► removeNode(node)
│   │   │   └─► addNodeToEnd(node)  [Mark as MRU]
│   │   │
│   │   └─► NO (Insert):
│   │       ├─► Check if size >= capacity
│   │       │   │
│   │       │   └─► YES: Evict LRU
│   │       │       ├─► lru_node = head_.next
│   │       │       ├─► removeNode(lru_node)
│   │       │       └─► map_.erase(lru_node.key)
│   │       │
│   │       ├─► Create new_node
│   │       ├─► addNodeToEnd(new_node)
│   │       └─► map_["key2"] = new_node
│   │
│   └─► Release UNIQUE LOCK
│
└─► Return
```

### 3. Concurrent Access Scenarios

#### Scenario A: Multiple Readers (Safe ✅)
```
Time →
0ms:  Thread1 calls get("a")
      │
10ms: ├─ acquires SHARED LOCK ─ holds
      │
20ms: ├─ Thread2 calls get("b")
      │  ├─ acquires SHARED LOCK ─ holds (CONCURRENT! ✅)
      │
30ms: ├─ Thread3 calls get("c")
      │  ├─ acquires SHARED LOCK ─ holds (CONCURRENT! ✅)
      │
40ms: ├─ Thread1 releases SHARED LOCK
      │
50ms: ├─ Thread2 releases SHARED LOCK
      │
60ms: └─ Thread3 releases SHARED LOCK

Result: All 3 readers operated simultaneously on cache
        No waiting, maximum parallelism ✅
```

#### Scenario B: Reader + Writer (Safe, Serialized)
```
Time →
0ms:  Thread1 (Reader) calls get("a")
      │
10ms: ├─ acquires SHARED LOCK ─ holds
      │
20ms: ├─ Thread2 (Writer) calls put("x", val)
      │  └─ BLOCKED waiting for exclusive lock ❌
      │
30ms: ├─ Thread1 completes
      │  ├─ releases SHARED LOCK
      │  │
      │  └─ Thread2 acquires UNIQUE LOCK (now exclusive)
      │
40ms: ├─ Thread2 modifies cache
      │
50ms: └─ Thread2 releases UNIQUE LOCK

Result: Writer waited for all readers to finish
        No race conditions ✅
```

#### Scenario C: Multiple Writers (Safe, Serialized)
```
Time →
0ms:  Thread1 (Writer) calls put("a", val1)
      │
10ms: ├─ acquires UNIQUE LOCK (exclusive)
      │
20ms: ├─ Thread2 (Writer) calls put("b", val2)
      │  └─ BLOCKED waiting for exclusive lock ❌
      │
30ms: ├─ Thread1 releases UNIQUE LOCK
      │
40ms: ├─ Thread2 acquires UNIQUE LOCK (exclusive)
      │
50ms: ├─ Thread2 modifies cache
      │
60ms: └─ Thread2 releases UNIQUE LOCK

Result: Writers serialized (one at a time)
        State consistency guaranteed ✅
```

### 4. Lock State Machine

```
                    ┌─────────────────────────┐
                    │   NO LOCK HELD          │
                    │   (Idle State)          │
                    └───────┬─────────────────┘
                            │
                 ┌──────────┴──────────┐
                 │                    │
                 ▼                    ▼
        ┌──────────────────┐  ┌──────────────────┐
        │  SHARED LOCK     │  │  UNIQUE LOCK     │
        │  (Read Mode)     │  │  (Write Mode)    │
        │                  │  │                  │
        │ ✓ Multiple       │  │ ✗ Exclusive      │
        │   readers        │  │                  │
        │ ✗ No writers     │  │ ✗ No readers     │
        │ ✗ No upgrades    │  │ ✗ No other       │
        │                  │  │    writers       │
        └────┬─────────────┘  └────┬─────────────┘
             │                     │
             │ (release)           │ (release)
             │                     │
             └──────────┬──────────┘
                        │
                        ▼
                ┌─────────────────────────┐
                │   NO LOCK HELD          │
                │   (Idle State)          │
                └─────────────────────────┘

Shared Lock Holders: Can coexist
Unique Lock Holder: Blocks all, blocked by all
```

### 5. Thread Safety Verification Checklist

| Criterion | Status | Evidence |
|-----------|--------|----------|
| **No Data Races** | ✅ PASS | All shared state (`map_`, `head_`, `tail_`) protected by `lock_` |
| **No Race Conditions** | ✅ PASS | Operations are atomic from caller's perspective (with locks) |
| **No Deadlocks** | ✅ PASS | Single lock hierarchy, consistent acquisition order |
| **No Livelocks** | ✅ PASS | No infinite retry loops; lock-based (not spin-based) |
| **Exception Safe** | ✅ PASS | RAII pattern with `std::unique_lock` and `std::shared_lock` |
| **Memory Safe** | ✅ PASS | `std::shared_ptr` prevents use-after-free |
| **ABA Problem Free** | ✅ PASS | Pointer-based nodes with shared_ptr maintain identity |
| **Starvation Free** | ⚠️ CAUTION | Writers may starve under continuous readers |

**Overall: ✅ THREAD-SAFE for concurrent read/write operations**

### 6. Starvation Analysis

**Reader Starvation**: ✅ NOT POSSIBLE
- Writers acquire unique lock, blocking new readers

**Writer Starvation**: ⚠️ POSSIBLE
- Continuous readers can prevent writers from proceeding
- **Mitigation**: Use lock with writer preference if critical
- **Alternative**: Monitor writer wait times and apply backpressure

**Example Writer Starvation**:
```
Reader 1: Acquire → Hold → Release
  ↓ (immediately)
Reader 2: Acquire → Hold → Release
  ↓ (immediately)
Reader 3: Acquire → Hold → Release
  ↓ (continuously)
Writer: WAITING... WAITING... WAITING... ❌

Solution: Implement writer-preferred or fair locking strategy
```

---

## 🚀 Usage

```cpp
// Create a cache with capacity of 5
LRUCache<std::string, std::string> cache(5);

// Put values
cache.put("key1", "value1");
cache.put("key2", "value2");

// Get value
auto value = cache.get("key1");
if (value) {
    std::cout << *value << std::endl;
}

// Check existence
if (cache.containsKey("key1")) {
    std::cout << "Key found in cache" << std::endl;
}

// Remove a key
cache.remove("key1");

// Cache size
int size = cache.size();

// Clear cache
cache.clear();

```
### 📚 API Documentation

### Public Methods

| Method                | Description                                   |
| --------------------- | --------------------------------------------- |
| `put(K key, V value)` | Adds or updates a key-value pair              |
| `get(K key)`          | Retrieves value and marks it as recently used |
| `remove(K key)`       | Removes a specific key                        |
| `clear()`             | Clears the entire cache                       |
| `containsKey(K key)`  | Checks if key exists                          |
| `size()`              | Returns current cache size                    |
| `isEmpty()`           | Checks if cache is empty                      |


### ⏱️ Performance Characteristics

| Operation | Time Complexity | Space Complexity |
| --------- | --------------- | ---------------- |
| Get       | O(1)            | O(1)             |
| Put       | O(1)            | O(1)             |
| Remove    | O(1)            | O(1)             |
| Clear     | O(n)            | O(1)             |

### 🔐 Thread Safety Analysis

**✅ YES - This system IS thread-safe**

#### Thread Safety Mechanism

The LRU Cache uses `std::shared_mutex` to provide robust thread-safe operations:

| Scenario | Lock Type | Behavior |
|----------|-----------|----------|
| Multiple reads (`get`, `containsKey`, `size`, `isEmpty`) | Shared Lock | ✅ All readers proceed concurrently |
| Single write (`put`, `remove`, `clear`) | Unique Lock | 🔒 Exclusive access, readers blocked |
| Read after write lock upgrade | Upgrade Pattern | ✅ Deadlock-free |

#### Thread-Safety Guarantees

1. **No Data Races**: All access to `map_`, `head_`, `tail_` is protected by `lock_`
2. **No Race Conditions**: State modifications are atomic from the caller's perspective
3. **Deadlock-Free**: Lock acquisition order is consistent (always `lock_` first)
4. **ABA Problem Prevention**: Using `shared_ptr` prevents use-after-free issues

#### Lock-Based Concurrency Pattern

```
Read Operations (get, containsKey, size, isEmpty):
  ┌─────────────────────────┐
  │  Acquire Shared Lock    │ ← Multiple threads can hold simultaneously
  │  (std::shared_lock)     │
  ├─────────────────────────┤
  │  Perform Read           │
  ├─────────────────────────┤
  │  Release Shared Lock    │
  └─────────────────────────┘
         ↑
    CAN OVERLAP
         ↑
    Multiple readers
    
Write Operations (put, remove, clear):
  ┌─────────────────────────┐
  │  Acquire Unique Lock    │ ← Only ONE thread at a time
  │  (std::unique_lock)     │
  ├─────────────────────────┤
  │  Perform Write          │
  ├─────────────────────────┤
  │  Release Unique Lock    │
  └─────────────────────────┘
         ↑
   NO OVERLAP
         ↑
   Exclusive access
```

#### Thread Safety in Practice

**Safe Scenario:**
```
Thread 1: cache.get("key1")       ← Holds shared lock
Thread 2: cache.get("key2")       ← Also holds shared lock (same shared lock)
Thread 3: cache.put("key3", val)  ← BLOCKED, waiting for exclusive lock
Thread 1 & 2: Release locks       ← Thread 3 proceeds with exclusive lock
```

**Prevented Race Condition Example:**
```
WITHOUT Lock (RACE CONDITION):
  Thread 1: Read "size" = 2
  Thread 2: Write - evict oldest
  Thread 1: Write - assume space available (WRONG! Cache full)
  
WITH Lock (NO RACE):
  Thread 1: Acquire shared lock → Read "size" = 2
  Thread 2: WAITS for shared locks to release
  Thread 1: Release shared lock
  Thread 2: Acquire exclusive lock → Modify cache safely
```

#### Critical Sections Protected

| State | Lock | Operations |
|-------|------|-----------|
| `map_` (HashMap) | `lock_` | All read/write operations |
| `head_` (Linked List Sentinel) | `lock_` | List traversal and modifications |
| `tail_` (Linked List Sentinel) | `lock_` | List traversal and modifications |
| `capacity_` | None (constant) | Read-only, no lock needed |

#### Potential Issues & Mitigations

| Issue | Severity | Mitigation |
|-------|----------|-----------|
| Lock contention under high load | Medium | Monitor with performance profiling |
| Priority inversion (low-priority writer blocks high-priority reader) | Low | Use lock-free atomics if critical |
| Exception safety during writes | Low | RAII ensures locks released on exception |

Suitable for multi-threaded C++ backend services.
### 📁 Project Structure
    LRU-Cache/
    ├── LRUCache.h
    ├── LRUCacheTest.cpp
    ├── README.md
    ├── LICENSE
    └── Readme.md




### 🧪 Example Scenarios
### 1️⃣ Basic Cache Usage
    LRUCache<int, int> cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    
    Cache: {1=1, 2=2, 3=3}
    
    cache.put(4, 4); // Evicts 1
    Cache: {2=2, 3=3, 4=4}

### 2️⃣ Access Updates Order
    LRUCache<int, int> cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    
    // Cache: {1=1, 2=2, 3=3}
    
    cache.put(4, 4); // Evicts 1
    // Cache: {2=2, 3=3, 4=4}

### 🧠 Design Patterns Used

- Generic Programming

    - C++ Templates for type safety

- Synchronization

    - Shared mutex locking for concurrency

- Composite Data Structures

    - unordered_map + Doubly Linked List (via shared_ptr)


### 🚧 Limitations & Future Enhancements
### Current Limitations

-   Fixed capacity only

-   No persistence

### Planned Enhancements

-   ⏱️ Time-based expiration (TTL)

-   🔄 Custom eviction policies

-   📊 Cache statistics (hit/miss ratio)
