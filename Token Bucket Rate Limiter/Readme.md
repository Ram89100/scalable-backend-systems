# Token Bucket Rate Limiter

## Overview

This project implements a **Token Bucket Rate Limiter** to control the rate of incoming API requests.  
It is designed to be **thread-safe**, **time-based**, and suitable for **API throttling in a single-node C++ environment**.

The rate limiter ensures system stability by limiting how many requests can be processed within a given time window.

---

## Architecture Overview

### High-Level Flow

1. **Client** sends requests to the API
2. **API Layer** intercepts the request
3. **Token Bucket Rate Limiter** checks availability of tokens
4. **Decision Engine**
   - If token available → request proceeds
   - If no token → request rejected (HTTP 429)
5. **Time-Based Refill Mechanism** replenishes tokens periodically

---

## Architecture Diagram (Logical View)


---

## Component Breakdown

### 1. Client

- Represents external consumers (users, browsers, services)
- Can generate concurrent requests
- Not aware of rate limits
- All enforcement is server-side

---

### 2. API Layer

- Entry point for all incoming requests
- Integrates the `RateLimiter`
- Calls `tryAcquire()` before processing requests
- Acts as a gatekeeper to protect backend resources

---

### 3. Token Bucket Rate Limiter

The core component responsible for enforcing rate limits.

#### Internal State

- **Bucket Capacity**
  - Maximum number of tokens allowed in the bucket

- **Current Tokens**
  - Represents how many requests can be served immediately

- **Refill Rate**
  - Number of tokens added per second

- **Last Refill Timestamp**
  - Used to calculate elapsed time for refilling tokens

---

### 4. Time-Based Refill Mechanism

- Uses system time to calculate elapsed duration
- Replenishes tokens proportionally to elapsed time
- Ensures token count never exceeds capacity
- Prevents burst traffic beyond configured limits

---

### 5. Concurrency & Thread Safety

- `tryAcquire()` is guarded by `std::mutex`
- Ensures only one thread modifies token state at a time
- Prevents race conditions under concurrent access
- Safe for multi-threaded C++ environments within a single process

---

### 6. Decision Flow

- **Allow**
  - Token is available
  - Token is consumed
  - Request forwarded to API handler

- **Reject**
  - No token available
  - Request denied
  - Typically mapped to `HTTP 429 Too Many Requests`

---

## Requirement Coverage

| Topic | Covered | Explanation |
|------|--------|-------------|
| Token Bucket | ✅ | Fixed-capacity bucket with token consumption |
| Concurrency | ✅ | Handles multiple concurrent threads |
| Thread Safety | ✅ | Synchronized critical section |
| Time-Based Refill | ✅ | Tokens refilled based on elapsed time |
| API Throttling | ✅ | Requests allowed or rejected at API entry |

---

## Pros

- Simple and easy to understand
- Thread-safe for single-node deployments
- Accurate rate limiting using time-based refill
- Configurable capacity and refill rate
- Production-ready for monolithic services

---

## Cons / Limitations

- Not suitable for distributed systems
- State is stored in application memory
- Synchronization works only within a single C++ process
- Requires external coordination for multi-instance setups

---

## Scope for Extension

### 1. Distributed Rate Limiting
- Use Redis or another shared store
- Maintain a global token bucket
- Enforce limits across multiple service instances

### 2. Per-User / Per-API Key Rate Limiting
- Maintain separate token buckets per user or API key
- Enables fine-grained throttling

### 3. API Gateway Integration
- Move rate limiting logic to the gateway layer
- Protect backend services early in request flow

### 4. High-Performance Non-Blocking Design
- Replace `std::mutex` with lock-free atomic operations
- Improve throughput under high contention
- Improve throughput under high contention

### 5. Monitoring & Metrics
- Track allowed and rejected requests
- Integrate with monitoring systems like Prometheus

---

## When to Use This Architecture

- Single-node applications
- Monolithic services
- Internal tools and APIs
- Demonstration projects and interviews

---

## Summary

This Token Bucket Rate Limiter provides a **thread-safe, time-based, and configurable solution for API throttling**.  
While scoped for single-node deployments, it demonstrates strong understanding of rate-limiting algorithms, concurrency control, and system design trade-offs, with clear paths for extension to distributed systems.

