# Scalable Backend Systems

> **Core Infrastructure Primitives for High-Performance Distributed Systems**

A comprehensive reference implementation of essential backend infrastructure components designed for distributed systems, demonstrating deep expertise in **concurrency, performance optimization, and system design trade-offs**.

---

## 📋 Table of Contents

- [Overview](#overview)
- [Project Philosophy](#project-philosophy)
- [Components](#components)
- [Architecture & Design](#architecture--design)
- [Performance Analysis](#performance-analysis)
- [Thread Safety & Concurrency](#thread-safety--concurrency)
- [Getting Started](#getting-started)
- [Implementation Details](#implementation-details)
- [Use Cases](#use-cases)
- [Design Patterns](#design-patterns)
- [Future Enhancements](#future-enhancements)
- [Contributing](#contributing)
- [License](#license)

---

## Overview

This repository implements **core backend infrastructure primitives from scratch** to demonstrate a deep understanding of **concurrency control, performance characteristics, and architectural trade-offs**. Rather than relying on external frameworks or managed services, each component is built with explicit synchronization, transparent performance models, and clear documentation of limitations.

### Who Should Use This?

- **System Design Interview Preparation**: Demonstrate knowledge of backend infrastructure
- **Performance-Critical Applications**: Understand and optimize concurrency patterns
- **Educational Purpose**: Learn how battle-tested components work internally
- **Single-Node Deployments**: Production-ready for monolithic services and microservices within a single host

### What This Is NOT

- ❌ A drop-in replacement for production frameworks (Redis, Kafka, etc.)
- ❌ Distributed system solution (single-node focused)
- ❌ A learning tool for distributed consensus algorithms
- ❌ Designed for horizontal scalability

---

## Project Philosophy

### Design Principles

| Principle | Rationale | Implementation |
|-----------|-----------|-----------------|
| **Explicit over Implicit** | Clarity helps understand trade-offs | No hidden thread pools or background operations |
| **Performance Transparency** | Know the cost of each operation | O(1), O(log n), O(n) clearly marked |
| **Safety by Default** | Thread-safety built-in from day one | All shared state protected by appropriate locks |
| **Production-Ready Code** | Not toy implementations | RAII, exception safety, proper resource management |
| **Documented Trade-offs** | Every design choice is justified | Pros, cons, and alternatives documented |

### Why Build from Scratch?

Modern backend engineers often use frameworks without understanding internals. This project bridges that gap by implementing:
- How hash maps maintain O(1) access under load
- Why distributed systems need rate limiting
- How reader-writer locks optimize for read-heavy workloads
- The interplay between data structures and concurrency

---

## Components

### 1. 🔒 Thread-Safe LRU Cache

**An in-memory cache with deterministic eviction policy and concurrent access support**

#### Key Features

- **O(1) Operations**: All fundamental operations (`get`, `put`, `remove`) run in constant time
- **Thread-Safe**: Uses `std::shared_mutex` for read-write lock optimization
- **Automatic Eviction**: Least Recently Used (LRU) strategy when capacity reached
- **Generic Implementation**: Template-based support for any key-value types
- **Production-Ready**: RAII patterns, exception safety, comprehensive tests

#### Core Concepts

| Concept | Implementation | Why It Matters |
|---------|-----------------|---|
| **HashMap** | `std::unordered_map<K, Node*>` | O(1) key lookup |
| **Linked List** | Doubly-linked via `shared_ptr` | O(1) insertion/removal + order maintenance |
| **Read-Write Lock** | `std::shared_mutex` | Multiple concurrent readers + exclusive writers |
| **Memory Management** | `shared_ptr` nodes | Exception-safe cleanup, no memory leaks |

#### Performance Profile

```
Operation              | Time      | Space  | Lock Type
-----------            | -----     | ------ | ----------
get(key)              | O(1)      | O(1)   | Read
put(key, value)       | O(1)      | O(1)   | Write
remove(key)           | O(1)      | O(1)   | Write
clear()               | O(n)      | O(1)   | Write
containsKey(key)      | O(1)      | O(1)   | Read
size()                | O(1)      | O(1)   | Read
```

#### Real-World Applications

- **Database Buffer Pool**: Cache frequently accessed pages
- **HTTP Caching**: Store responses for static content
- **Query Result Caching**: Reduce database load
- **Session Storage**: User session data in-memory

#### Thread Safety Guarantees

✅ **No Data Races** | ✅ **No Race Conditions** | ✅ **Deadlock-Free** | ✅ **Exception Safe**

[**Detailed Thread Safety Analysis** →](LRU%20Cache%20(Thread%20Safe)/Readme.md#-thread-safety-analysis)

---

### 2. ⏱️ Token Bucket Rate Limiter

**Time-based request throttling mechanism for API protection**

#### Key Features

- **Token-Based Rate Control**: Fixed-capacity bucket with time-based refills
- **Thread-Safe**: Synchronized critical sections for concurrent requests
- **Time-Aware Refill**: Proportional token generation based on elapsed time
- **O(1) Per-Request**: Constant-time decision making
- **Deterministic Behavior**: Predictable rate limiting without randomness

#### Algorithm Overview

```
Initialize:
  tokens = capacity
  last_refill_time = now()

Per Request:
  1. Calculate elapsed_time = now() - last_refill_time
  2. tokens_to_add = (elapsed_time * refill_rate) / 1_second
  3. tokens = min(capacity, tokens + tokens_to_add)
  4. last_refill_time = now()
  
  5. IF tokens > 0:
       tokens--
       ALLOW request ✅
     ELSE:
       REJECT request with HTTP 429 ❌
```

#### Configuration Parameters

| Parameter | Purpose | Example |
|-----------|---------|---------|
| `capacity` | Max tokens in bucket | 100 (requests) |
| `refill_rate` | Tokens per second | 10 (requests/sec) |
| `refill_interval` | Check interval | 100ms |

#### Performance Profile

```
Operation                 | Complexity | Lock Overhead
-----------               | -----      | ------
tryAcquire()             | O(1)       | Single mutex lock
getAvailableTokens()     | O(1)       | Single mutex lock
Refill calculation       | O(1)       | No lock needed
```

#### Real-World Applications

- **API Gateway Throttling**: Protect backend from overload
- **Database Query Rate Limiting**: Prevent resource exhaustion
- **Microservice Rate Limiting**: Inter-service traffic control
- **CDN Request Throttling**: Edge node load balancing

#### Limitation & Extensions

**Current Scope**: Single-node, single-bucket
**Distributed Extension**: Use Redis for shared bucket state

[**Detailed Implementation** →](Token%20Bucket%20Rate%20Limiter/Readme.md)

---

### 3. 📦 Simple Task Queue (Producer-Consumer)

**Lightweight asynchronous task execution framework**

#### Key Features

- **Producer-Consumer Pattern**: Decouple task creation from execution
- **Background Workers**: Thread pool for parallel task execution
- **Retry Mechanism**: Automatic retry for transient failures
- **Graceful Shutdown**: Safe termination with pending task completion
- **At-Least-Once Delivery**: Tasks guaranteed to execute (may execute multiple times)

#### Architecture

```
Producer Thread                Queue             Worker Threads
    │                          │                      │
    ├─ submit(task1)  ────────►├─────────────────────►├─ execute(task1)
    ├─ submit(task2)  ────────►├─ task1              │
    │                          ├─ task2  ────────────►├─ execute(task2)
    │                          ├─ task3              │
    │                    Retry ├─ task3  ────────────►├─ execute(task3)
    │                          │  (failed)            │
    │                          ├─ task3' ────────────►├─ execute(task3)
    │                          │  (retry)             │  [Success]
    │                          │                      │
    └──────────────────────────└──────────────────────┘
```

#### Delivery Semantics

| Semantic | Guarantee | Tradeoff |
|----------|-----------|----------|
| **At-Most-Once** | Execute ≤ 1 time | May lose tasks on failure |
| **At-Least-Once** | Execute ≥ 1 time | May execute multiple times |
| **Exactly-Once** | Execute = 1 time | Complex, requires coordination |

*This implementation uses **At-Least-Once** (default for Celery, Kafka, SQS)*

#### Performance Profile

```
Operation          | Complexity | Thread Type
-----------        | -----      | ---------
submit(task)       | O(1)       | Producer (any thread)
fetch(task)        | O(1)       | Worker thread
retry_enqueue()    | O(1)       | Worker thread
```

#### Real-World Applications

- **Email Sending**: Background email delivery with retries
- **Data Processing**: Long-running report generation
- **Webhooks**: Asynchronous external API calls
- **Batch Operations**: Bulk data transformations

#### Retry Strategy

```
Initial Submit → Execute
                 ↓
             Success → Complete ✅
                 ↓
             Failure → Retry Count--
                 ↓
             retry_count > 0 → Re-enqueue
                 ↓
             retry_count = 0 → Drop ❌
```

[**Detailed Design** →](Simple%20Task%20Queue/Readme.md)

---

## Architecture & Design

### System Layers

```
┌────────────────────────────────────────────────────────────────┐
│                      Application Layer                          │
│  (User code using cache, rate limiter, or task queue)           │
└──────────────────────────────────────────────────────────────┬─┘
                                                                 │
┌────────────────────────────────────────────────────────────┬──┘
│                    Infrastructure Layer                     │
├──────────────────────────────────────────────────────────┤
│  ┌─────────────────────┐  ┌─────────────────────┐         │
│  │   LRU Cache         │  │  Rate Limiter       │         │
│  │  (Data Structure)   │  │  (Flow Control)     │         │
│  └─────────────────────┘  └─────────────────────┘         │
│  ┌─────────────────────────────────────────────────┐       │
│  │        Task Queue (Async Execution)             │       │
│  └─────────────────────────────────────────────────┘       │
└────────────────────────────────────────────────────────────┘
                             │
┌────────────────────────────────────────────────────────────┐
│                 Concurrency Layer                           │
├────────────────────────────────────────────────────────────┤
│  ┌──────────────────┐  ┌──────────────────┐               │
│  │ Mutex/Lock       │  │ Atomic Variables │               │
│  │ (Synchronization)│  │ (Lock-free ops)  │               │
│  └──────────────────┘  └──────────────────┘               │
└────────────────────────────────────────────────────────────┘
                             │
┌────────────────────────────────────────────────────────────┐
│                    OS Thread Layer                          │
│  (std::thread, context switching, scheduling)              │
└────────────────────────────────────────────────────────────┘
```

### Component Interaction

```
                    Client Requests
                          │
                          ▼
                 ┌─────────────────┐
                 │  Rate Limiter   │  ← Protect backend
                 └────────┬────────┘
                          │
                   ┌──────┴──────┐
           Allowed │             │ Rejected
                   ▼             ▼
            ┌─────────────┐  HTTP 429
            │ LRU Cache   │  (Too Many Requests)
            └────┬────────┘
                 │
      ┌──────────┴──────────┐
   Hit │                    │ Miss
      ▼                     ▼
  Return             ┌─────────────────┐
  Cached          ─►│   Task Queue    │
  Value          Enqueue      │
                              ├─► Worker Thread 1
                              ├─► Worker Thread 2
                              └─► Worker Thread N
                              
                              Process & Cache Result
```

---

## Performance Analysis

### Comparative Performance

| Component | Time | Space | Scalability | Notes |
|-----------|------|-------|-------------|-------|
| **LRU Cache** | O(1) avg | O(capacity) | Single-node | Hash table + linked list |
| **Rate Limiter** | O(1) | O(1) | Single-node | Token-based throttling |
| **Task Queue** | O(1) enqueue | O(tasks) | Limited | Thread pool based |

### Throughput Estimates

```
LRU Cache (single thread):
  Reads:  ~5-10M ops/sec (shared lock overhead minimal)
  Writes: ~2-5M ops/sec (unique lock exclusive)
  Mixed:  ~3-7M ops/sec (depends on read/write ratio)

Rate Limiter (single thread):
  Tokens/sec: ~10M+ (only arithmetic operations)
  Decision time: ~100-500ns per request

Task Queue (with N workers):
  Throughput: ~(10K tasks/sec) * N workers
  Latency: ~1-10ms per task (queue + execution overhead)
```

### Bottleneck Analysis

| Scenario | Bottleneck | Mitigation |
|----------|-----------|-----------|
| Read-heavy cache | Lock contention minimal | Scale with shared locks ✅ |
| Write-heavy cache | Lock contention high | Use sharded locks or replicas |
| Rate limiting burst | Token arithmetic | Pre-calculate or batch |
| Task queue backlog | Worker availability | Add more worker threads |

---

## Thread Safety & Concurrency

### Concurrency Model

All components use **pessimistic concurrency control** (lock-based):

**Pros:**
- ✅ No lock-free complexity
- ✅ Exception-safe with RAII
- ✅ Clear semantics and debugging
- ✅ Works with any data type

**Cons:**
- ❌ Lock contention under high concurrency
- ❌ Potential priority inversion
- ❌ Possible writer starvation (LRU Cache)

### Thread Safety Matrix

| Component | Read Safety | Write Safety | Mixed Access |
|-----------|-------------|--------------|--------------|
| **LRU Cache** | ✅ Concurrent | ✅ Exclusive | ✅ Serialized |
| **Rate Limiter** | ✅ Atomic | ✅ Atomic | ✅ Atomic |
| **Task Queue** | ✅ Safe | ✅ Safe | ✅ Safe |

### Exception Safety

All components provide **Strong Exception Guarantee**:
- Operations either succeed completely or have no effect
- No partial state updates
- Resources properly cleaned up via RAII

---

## Getting Started

### Prerequisites

- **C++ Compiler**: C++17 or later (GCC 7+, Clang 5+, MSVC 2017+)
- **Build System**: CMake 3.10+ (optional, header-only for LRU Cache)
- **Operating System**: Linux, macOS, Windows

### Installation

```bash
# Clone the repository
git clone https://github.com/your-username/scalable-backend-systems.git
cd scalable-backend-systems

# LRU Cache (Header-only)
#include "LRU Cache (Thread Safe)/LRUCache.h"

# Token Bucket Rate Limiter (Header-only)
#include "Token Bucket Rate Limiter/TokenBucketRateLimiter.h"

# Task Queue (Python-based)
from Simple_Task_Queue.task_queue import TaskQueue
```

### Quick Examples

#### LRU Cache

```cpp
#include "LRUCache.h"
#include <iostream>

int main() {
    // Create cache with capacity of 3
    LRUCache<std::string, int> cache(3);
    
    // Insert items
    cache.put("user:1", 100);
    cache.put("user:2", 200);
    cache.put("user:3", 300);
    
    // Retrieve item (marks as recently used)
    auto value = cache.get("user:1");
    
    // Insert new item (evicts least recently used: "user:2")
    cache.put("user:4", 400);
    
    // Verify eviction
    assert(cache.get("user:2") == nullptr);  // Evicted
    assert(cache.size() == 3);
    
    return 0;
}
```

#### Rate Limiter

```cpp
#include "TokenBucketRateLimiter.h"
#include <iostream>

int main() {
    // 100 tokens, refill at 10 tokens/second
    TokenBucketRateLimiter limiter(100, 10);
    
    // Try to acquire tokens
    for (int i = 0; i < 105; i++) {
        if (limiter.tryAcquire()) {
            std::cout << "Request " << i << " allowed\n";
        } else {
            std::cout << "Request " << i << " rejected (HTTP 429)\n";
        }
    }
    
    return 0;
}
```

---

## Implementation Details

### LRU Cache Architecture

**Data Structures:**
- `HashMap`: `std::unordered_map<K, shared_ptr<Node>>`
- **Linked List**: Doubly-linked via `shared_ptr` pointers
- **Synchronization**: `std::shared_mutex` (read-write lock)

**Access Patterns:**
1. **Read (get)**: Acquire shared lock → lookup → upgrade to exclusive → move to end → release
2. **Write (put)**: Acquire exclusive lock → insert/update/evict → release
3. **Eviction**: Remove head node (oldest) → delete from map

**Memory Layout:**
```
┌─────────────────────────────────┐
│ Node {key, value, prev, next}  │
│  size: sizeof(K) + sizeof(V)   │
│        + 2 * sizeof(shared_ptr)│
└─────────────────────────────────┘
```

### Rate Limiter Algorithm

**Token Calculation:**
```
elapsed_nanos = now() - last_refill_time
tokens_to_add = (elapsed_nanos * refill_rate) / 1_000_000_000
new_tokens = min(capacity, current_tokens + tokens_to_add)
```

**Advantages:**
- ✅ Handles bursts (up to capacity)
- ✅ Smooth rate enforcement long-term
- ✅ No timer threads needed
- ✅ O(1) decision per request

### Task Queue Design

**Components:**
- **Producer Queue**: Thread-safe queue for task submission
- **Worker Pool**: Fixed number of background threads
- **Retry Logic**: Automatic re-enqueue on failure
- **Shutdown Handler**: Graceful termination

**Execution Flow:**
1. Producer calls `submit(task)`
2. Task enqueued with retry_count
3. Worker fetches task
4. Worker executes task
5. On failure: Retry count--, re-enqueue if count > 0
6. On success: Task complete

---

## Use Cases

### Enterprise Applications

| Use Case | Component | Why It Matters |
|----------|-----------|---|
| **Database Buffer Pool** | LRU Cache | Reduce disk I/O, improve query latency |
| **API Rate Limiting** | Rate Limiter | Prevent DOS, ensure fair resource usage |
| **Email Delivery** | Task Queue | Decouple API from slow operations |
| **Cache Warming** | LRU Cache + Task Queue | Background pre-loading of data |
| **Microservice Throttling** | Rate Limiter | Inter-service traffic control |

### Performance-Critical Systems

- **High-Frequency Trading**: Fast cache lookups for market data
- **Real-Time Analytics**: Stream processing with bounded memory
- **Social Media**: User feed caching with LRU eviction
- **Search Engines**: Query result caching and rate limiting

### Interview & Learning

- **System Design Interviews**: Complete implementation example
- **Performance Optimization**: Understanding concurrency trade-offs
- **Backend Education**: How production systems work internally

---

## Design Patterns

### 1. Composite Pattern
- **Where**: LRU Cache (HashMap + Linked List)
- **Benefit**: Leverage strengths of both data structures

### 2. Template Method
- **Where**: Generic `LRUCache<K, V>` with C++ templates
- **Benefit**: Type safety without code duplication

### 3. Lock-Based Synchronization
- **Where**: All components
- **Benefit**: Clear semantics, exception-safe with RAII

### 4. Reader-Writer Locking
- **Where**: LRU Cache with `shared_mutex`
- **Benefit**: Maximize concurrent reads, safe writes

### 5. Producer-Consumer
- **Where**: Task Queue
- **Benefit**: Decouple producers from consumers

### 6. Retry Pattern
- **Where**: Task Queue
- **Benefit**: Handle transient failures gracefully

---

## Future Enhancements

### LRU Cache

- [ ] **TTL Support**: Time-based expiration of entries
- [ ] **Write-Through Caching**: Persist updates to backing store
- [ ] **Cache Statistics**: Hit/miss ratios, eviction tracking
- [ ] **Custom Eviction Policies**: LFU, FIFO alternatives
- [ ] **Sharded Locks**: Reduce contention in write-heavy workloads
- [ ] **Overflow Handling**: Disk-based overflow for large datasets

### Rate Limiter

- [ ] **Distributed Rate Limiting**: Redis-backed shared state
- [ ] **Per-User Buckets**: Separate limits per API key
- [ ] **Weighted Requests**: Different costs for different operations
- [ ] **Sliding Window Algorithm**: Alternative to token bucket
- [ ] **Priority Queuing**: High-priority requests bypass limiter
- [ ] **Metrics Export**: Prometheus-compatible metrics

### Task Queue

- [ ] **Persistent Queue**: Redis/RabbitMQ backing
- [ ] **Scheduled Tasks**: Execute at specific times
- [ ] **Task Priorities**: Process high-priority tasks first
- [ ] **Dead Letter Queue**: Track and report failures
- [ ] **Distributed Execution**: Multi-node worker coordination
- [ ] **Monitoring Dashboard**: Web UI for queue management

---

## Contributing

Contributions are welcome! Areas for improvement:

1. **Lock-Free Implementations**: Atomic-based alternatives
2. **Performance Optimizations**: SIMD, cache-aware algorithms
3. **Extended Tests**: More edge cases and stress tests
4. **Documentation**: Additional examples and tutorials
5. **Distributed Extensions**: Redis/Kafka integrations

---

## License

MIT License - See LICENSE file for details

---

## Summary

| Aspect | LRU Cache | Rate Limiter | Task Queue |
|--------|-----------|--------------|-----------|
| **Use Case** | Data caching | Request throttling | Async execution |
| **Time Complexity** | O(1) | O(1) | O(1) |
| **Thread Safety** | ✅ Yes | ✅ Yes | ✅ Yes |
| **Production Ready** | ✅ Yes | ✅ Yes | ✅ Yes |
| **Scalability** | Single-node | Single-node | Single-node |
| **Learn Value** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ |

---

## Quick Links

- [LRU Cache Implementation](LRU%20Cache%20(Thread%20Safe)/) - Complete with thread safety analysis
- [Rate Limiter Implementation](Token%20Bucket%20Rate%20Limiter/) - Token bucket algorithm details
- [Task Queue Implementation](Simple%20Task%20Queue/) - Producer-consumer pattern
- [License](LICENSE) - MIT License

---

**Built with ❤️ for backend engineers and system design enthusiasts**