# Thread-Safe LRU Cache in Java

A **thread-safe Least Recently Used (LRU) Cache** implementation in **Java**, designed for high performance, concurrency, and clean architecture. This project includes comprehensive documentation, real-world usage examples, and unit tests covering edge cases and concurrent access.

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
  - Uses `ReentrantReadWriteLock` to allow safe concurrent access
- ⚡ **O(1) Time Complexity**
  - `get`, `put`, and `remove` operations run in constant time
- ♻️ **Automatic LRU Eviction**
  - Least recently used entry is evicted when capacity is reached
- 📦 **Customizable Capacity**
  - Set cache size at initialization
- 🧩 **Generic Implementation**
  - Supports any key-value types using Java Generics
- 🧪 **Comprehensive Tests**
  - Unit tests + edge cases + concurrency tests

---

## 🏗️ Architecture

The cache is implemented using a combination of:

| Component | Purpose |
|---------|--------|
| `HashMap<K, CacheNode>` | O(1) key lookup |
| `Doubly Linked List` | Maintains access order |
| `ReentrantReadWriteLock` | Thread-safe concurrent access |

### Why This Design?
- HashMap gives fast lookup
- Doubly linked list enables O(1) insertion and removal
- Locks ensure safe multi-threaded access

---

## 🚀 Usage

```java
// Create a cache with capacity of 5
LRUCache<String, String> cache = new LRUCache<>(5);

// Put values
cache.put("key1", "value1");
cache.put("key2", "value2");

// Get value
String value = cache.get("key1");

// Check existence
if (cache.containsKey("key1")) {
    System.out.println("Key found in cache");
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

### 🔐 Thread Safety

This cache supports safe concurrent access using ReentrantReadWriteLock:

- Multiple readers can access simultaneously

-   Writes are exclusive

-   Prevents race conditions and data corruption

Suitable for multi-threaded backend services.
### 📁 Project Structure
    LRU-Cache/
    ├── src/
    │   ├── LRUCache.java
    │   ├── CacheNode.java
    │   └── CacheEntry.java
    ├── test/
    │   └── LRUCacheTest.java
    ├── README.md
    ├── LICENSE
    └── .gitignore




### 🧪 Example Scenarios
### 1️⃣ Basic Cache Usage
    LRUCache<Integer, Integer> cache = new LRUCache<>(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    
    Cache: {1=1, 2=2, 3=3}
    
    cache.put(4, 4); // Evicts 1
    Cache: {2=2, 3=3, 4=4}

### 2️⃣ Access Updates Order
    LRUCache<Integer, Integer> cache = new LRUCache<>(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    
    // Cache: {1=1, 2=2, 3=3}
    
    cache.put(4, 4); // Evicts 1
    // Cache: {2=2, 3=3, 4=4}

### 🧠 Design Patterns Used

- Generic Programming

    - Java Generics for type safety

- Synchronization

    - Read-write locking for concurrency

- Composite Data Structures

    - HashMap + Doubly Linked List


### 🚧 Limitations & Future Enhancements
### Current Limitations

-   Fixed capacity only

-   No persistence

### Planned Enhancements

-   ⏱️ Time-based expiration (TTL)

-   🔄 Custom eviction policies

-   📊 Cache statistics (hit/miss ratio)

-   🧩 Implement java.util.Map interface

-    🔌 Optional async eviction