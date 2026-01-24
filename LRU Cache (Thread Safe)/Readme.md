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

### 🔐 Thread Safety

This cache supports safe concurrent access using `std::shared_mutex`:

- Multiple readers can access simultaneously via shared locks

- Writes are exclusive via unique locks

- Prevents race conditions and data corruption

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

-   🧩 Implement java.util.Map interface

-    🔌 Optional async eviction