# Simple Task Queue (Producer–Consumer Model)

## Overview
This project implements a **lightweight in-memory task queue** to understand the core building blocks behind background job systems such as **Celery, Sidekiq, and Kafka-based workers**.

The goal of this project is **learning system design fundamentals**, not building a production-ready framework.

---

## Problem Statement
In backend systems, some operations are:
- Long-running
- Failure-prone
- Not suitable for synchronous API execution

Examples:
- Sending emails
- Generating reports
- Processing files
- Background data aggregation

To handle this, tasks must be **executed asynchronously** using a **task queue with workers**.

---

## High-Level Design

### Components
1. **Producer**
   - Submits tasks to the queue
2. **Task Queue**
   - Thread-safe in-memory queue
3. **Workers**
   - Background threads that execute tasks
4. **Retry Mechanism**
   - Failed tasks are retried limited times

### High Level Design

    Producer ──▶ Queue ──▶ Worker Threads ──▶ Task Execution
                             │
                             └── Retry on Failure

---

## Producer–Consumer Pattern ✅

### What it is
The **producer–consumer pattern** decouples task creation from task execution.

- **Producer**: submits tasks
- **Consumer (Worker)**: processes tasks independently

### Why it matters
- Improves API response time
- Prevents blocking main threads
- Enables parallel processing

### In this implementation
- Producers call `submit(task)`
- Workers continuously pull tasks from `queue.Queue`

---

## Worker Model ✅

### Design
- A fixed pool of worker threads is created at startup
- Each worker:
  - Waits for tasks
  - Executes them independently
  - Handles failures

### Benefits
- Controlled concurrency
- Prevents resource exhaustion
- Parallel task execution

### Limitation
- Thread-based (not process-based)
- Limited by Python GIL (acceptable for learning)

---

## Retry Logic ✅

### Why retries are needed
Tasks can fail due to:
- Temporary errors
- Network issues
- External service downtime

### Retry Strategy Used
- Each task is submitted with a **retry count**
- On failure:
  - Retry count is decremented
  - Task is re-enqueued
- Task is dropped when retries are exhausted

### Trade-offs
- Simple to implement
- No exponential backoff
- No dead-letter queue (DLQ)

---

## Failure Handling ✅

### How failures are handled
- Task execution is wrapped in `try/except`
- Exceptions do not crash workers
- Failed tasks are retried safely

### What this prevents
- Worker thread crashes
- Queue corruption
- System-wide failures

### Limitation
- No persistent failure tracking
- No alerting mechanism

---

## Exactly-Once vs At-Least-Once Semantics ✅

### At-Least-Once (Used Here)
- A task **may execute more than once**
- Ensures task is not lost
- Safer for unreliable systems

**Example**
If a worker crashes after executing but before acknowledgment, the task may run again.

### Exactly-Once (Hard Problem)
- Task executes **only once**
- Requires:
  - Persistent storage
  - Idempotent tasks
  - Distributed coordination

### Why At-Least-Once is preferred
- Simpler
- More reliable
- Widely used in real systems

**Celery, Kafka, SQS default to at-least-once delivery**

---

## Backpressure Handling

### What is backpressure
When producers generate tasks faster than workers can process them.

### How this system handles it
- `queue.Queue` blocks when full (configurable)
- Prevents memory explosion

### Production systems
- Use bounded queues
- Apply rate limiting
- Reject or delay tasks

---

## Graceful Shutdown

### Why it matters
Prevents:
- Task loss
- Half-executed jobs
- Zombie workers

### How it works here
- A shared `running` flag
- Workers stop polling when shutdown begins
- Threads are joined safely

---

## Time & Space Complexity

| Operation | Complexity |
|----------|------------|
| Submit task | O(1) |
| Fetch task | O(1) |
| Retry enqueue | O(1) |
| Memory usage | O(n tasks) |

---

## Limitations (Important for Interviews)

- In-memory only (data loss on crash)
- Single-node
- No persistence
- No task priority
- No delayed scheduling
- No monitoring UI

These limitations are **intentional** for learning.

---

## How Production Systems Improve This

| Feature | Production Solution |
|-------|---------------------|
| Persistence | Redis / RabbitMQ |
| Scalability | Multi-node workers |
| Reliability | Acknowledgements |
| Monitoring | Dashboards |
| Ordering | Partitioning |
| Fault tolerance | Replication |

---

## Relation to Celery

This project helped understand:
- Why brokers exist
- Why acknowledgements matter
- Why idempotent tasks are required
- How retries are implemented internally

Celery replaces:
- In-memory queue → Redis/RabbitMQ
- Threads → Processes
- Manual retries → Managed retries

---

## When to Use This Design
- Learning async systems
- Prototyping
- Understanding backend internals

## When NOT to Use
- Production workloads
- Distributed systems
- Critical data processing

---

