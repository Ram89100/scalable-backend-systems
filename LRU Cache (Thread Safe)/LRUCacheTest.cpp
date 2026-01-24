#include "LRUCache.h"
#include <iostream>
#include <cassert>
#include <thread>
#include <vector>

/**
 * Google Test-style test cases for LRUCache implementation.
 *
 * Tests cover:
 * - Basic put and get operations
 * - LRU eviction policy
 * - Thread safety
 * - Edge cases
 * - Concurrent access patterns
 */

void testBasicPutGet() {
    std::cout << "Test 1: Basic Put and Get Operations" << std::endl;
    LRUCache<std::string, int> cache(3);

    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("c", 3);

    assert(cache.get("a") != nullptr && *cache.get("a") == 1);
    assert(cache.get("b") != nullptr && *cache.get("b") == 2);
    assert(cache.get("c") != nullptr && *cache.get("c") == 3);
    std::cout << "✓ Passed\n" << std::endl;
}

void testLRUEviction() {
    std::cout << "Test 2: LRU Eviction Policy" << std::endl;
    LRUCache<int, std::string> cache(3);

    cache.put(1, "one");
    cache.put(2, "two");
    cache.put(3, "three");

    // Access 1, making it recently used
    assert(cache.get(1) != nullptr && *cache.get(1) == "one");

    // Add 4, should evict 2 (least recently used)
    cache.put(4, "four");

    assert(cache.get(1) != nullptr);
    assert(cache.get(2) == nullptr);
    assert(cache.get(3) != nullptr);
    assert(cache.get(4) != nullptr);
    std::cout << "✓ Passed\n" << std::endl;
}

void testContainsKey() {
    std::cout << "Test 3: Contains Key" << std::endl;
    LRUCache<std::string, std::string> cache(2);

    cache.put("key1", "value1");
    assert(cache.containsKey("key1"));
    assert(!cache.containsKey("key2"));
    std::cout << "✓ Passed\n" << std::endl;
}

void testRemove() {
    std::cout << "Test 4: Remove Operation" << std::endl;
    LRUCache<int, int> cache(3);

    cache.put(1, 1);
    cache.put(2, 2);
    cache.remove(1);

    assert(cache.get(1) == nullptr);
    assert(cache.get(2) != nullptr);
    std::cout << "✓ Passed\n" << std::endl;
}

void testClear() {
    std::cout << "Test 5: Clear Operation" << std::endl;
    LRUCache<std::string, std::string> cache(3);

    cache.put("a", "1");
    cache.put("b", "2");
    cache.clear();

    assert(cache.isEmpty());
    assert(cache.size() == 0);
    std::cout << "✓ Passed\n" << std::endl;
}

void testCapacityOne() {
    std::cout << "Test 6: Capacity of One" << std::endl;
    LRUCache<std::string, std::string> cache(1);

    cache.put("a", "1");
    assert(cache.get("a") != nullptr && *cache.get("a") == "1");

    cache.put("b", "2");
    assert(cache.get("a") == nullptr);
    assert(cache.get("b") != nullptr && *cache.get("b") == "2");
    std::cout << "✓ Passed\n" << std::endl;
}

void testEmpty() {
    std::cout << "Test 7: Empty Cache" << std::endl;
    LRUCache<std::string, std::string> cache(5);

    assert(cache.isEmpty());
    cache.put("a", "1");
    assert(!cache.isEmpty());
    std::cout << "✓ Passed\n" << std::endl;
}

void testSize() {
    std::cout << "Test 8: Size Operation" << std::endl;
    LRUCache<int, int> cache(5);

    assert(cache.size() == 0);
    cache.put(1, 1);
    assert(cache.size() == 1);
    cache.put(2, 2);
    assert(cache.size() == 2);
    cache.remove(1);
    assert(cache.size() == 1);
    std::cout << "✓ Passed\n" << std::endl;
}

void testConcurrentAccess() {
    std::cout << "Test 9: Concurrent Access (Thread Safety)" << std::endl;
    LRUCache<int, int> cache(100);

    auto worker1 = [&cache]() {
        for (int i = 0; i < 50; i++) {
            cache.put(i, i);
        }
    };

    auto worker2 = [&cache]() {
        for (int i = 25; i < 75; i++) {
            cache.put(i, i);
        }
    };

    std::thread t1(worker1);
    std::thread t2(worker2);

    t1.join();
    t2.join();

    assert(cache.size() <= 100);
    std::cout << "✓ Passed (Cache size: " << cache.size() << ")\n" << std::endl;
}

void testToString() {
    std::cout << "Test 10: ToString Representation" << std::endl;
    LRUCache<std::string, std::string> cache(2);

    cache.put("a", "1");
    cache.put("b", "2");

    std::string representation = cache.toString();
    assert(representation.find("LRUCache") != std::string::npos);
    std::cout << "Cache representation: " << representation << std::endl;
    std::cout << "✓ Passed\n" << std::endl;
}

int main() {
    std::cout << "Running LRU Cache Tests...\n" << std::endl;

    testBasicPutGet();
    testLRUEviction();
    testContainsKey();
    testRemove();
    testClear();
    testCapacityOne();
    testEmpty();
    testSize();
    testConcurrentAccess();
    testToString();

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}
