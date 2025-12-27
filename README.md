# scalable-backend-systems


## Overview

This repository implements **core backend infrastructure primitives** from scratch to demonstrate
a deep understanding of **concurrency, performance, and system design trade-offs**.

The project focuses on **how systems work internally**, rather than using existing frameworks
or managed services.

Implemented components mirror real-world building blocks used in:
- API Gateways
- Backend Services
- Distributed Systems (single-node implementations here)

Each module is isolated, documented, and designed to be **interview-ready**.

---

## Components

### 1. Thread-Safe LRU Cache
An in-memory cache with automatic eviction based on access recency.

**Key properties**
- O(1) `get`, `put`, `remove`
- Thread-safe using read–write locks
- Deterministic eviction behavior

**Concepts**
- HashMap + Doubly Linked List
- Lock-based concurrency control
- Cache eviction strategies

---

### 2. Task Queue (Producer–Consumer Model)
A lightweight asynchronous task execution framework inspired by Celery / Sidekiq.

**Key properties**
- Background worker threads
- Retry handling for transient failures
- Graceful shutdown support

**Concepts**
- Producer–Consumer pattern
- At-least-once delivery semantics
- Failure isolation and backpressure

---

### 3. Token Bucket Rate Limiter
A time-based rate limiter for API request throttling.

**Key properties**
- Constant-time request evaluation
- Controlled burst handling
- Thread-safe state updates

**Concepts**
- Token Bucket algorithm
- Time-based refill logic
- Concurrency correctness under contention

---

## High-Level System View

                                    Client
                                    │
                                    ▼
                                    Rate Limiter ──► Backend API
                                    │
                                    ▼
                                    LRU Cache
                                    │
                                    ▼
                                    Task Queue
                                    (Async Workers)



---

## Design Goals

- Explicit concurrency handling
- Predictable performance characteristics
- Clear separation of concerns
- Transparent trade-offs and limitations

Frameworks and distributed dependencies are intentionally avoided
to expose **low-level system mechanics**.

---

## Performance Summary

| Component | Time Complexity | Space |
|---------|----------------|-------|
| LRU Cache | O(1) ops | O(n) |
| Task Queue | O(1) enqueue/dequeue | O(n tasks) |
| Rate Limiter | O(1) per request | O(1) |

---

## Limitations (Intentional)

- In-memory only
- Single-node scope
- No persistence or replication

These constraints are deliberate to focus on **core correctness and design reasoning**.

---

## Possible Extensions

- Distributed cache using Redis
- Persistent queues (Kafka / RabbitMQ)
- Distributed rate limiting
- Metrics and observability
- Lock-free / CAS-based designs

---

## Repository Structure

                            ├── lru-cache/
                            ├── task-queue/
                            ├── rate-limiter/
                            └── README.md


---

## Tech Stack

- Java
- Multithreading & synchronization
- Core data structures
- System design patterns
- Python

---

## Author

Kumar Ram Krishna
