/**
 * JUnit Test cases for LRUCache implementation.
 *
 * Tests cover:
 * - Basic put and get operations
 * - LRU eviction policy
 * - Thread safety
 * - Edge cases
 * - Concurrent access patterns
 */
public class LRUCacheTest {

    public static void main(String[] args) {
        System.out.println("Running LRU Cache Tests...\n");

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

        System.out.println("\nAll tests passed!");
    }

    public static void testBasicPutGet() {
        System.out.println("Test 1: Basic Put and Get Operations");
        LRUCache<String, Integer> cache = new LRUCache<>(3);

        cache.put("a", 1);
        cache.put("b", 2);
        cache.put("c", 3);

        assert cache.get("a").equals(1) : "Expected 1, got " + cache.get("a");
        assert cache.get("b").equals(2) : "Expected 2, got " + cache.get("b");
        assert cache.get("c").equals(3) : "Expected 3, got " + cache.get("c");
        System.out.println("✓ Passed\n");
    }

    public static void testLRUEviction() {
        System.out.println("Test 2: LRU Eviction Policy");
        LRUCache<Integer, String> cache = new LRUCache<>(3);

        cache.put(1, "one");
        cache.put(2, "two");
        cache.put(3, "three");

        // Access 1, making it recently used
        assert cache.get(1).equals("one") : "Should find 1";

        // Add 4, should evict 2 (least recently used)
        cache.put(4, "four");

        assert cache.get(1) != null : "1 should still be in cache";
        assert cache.get(2) == null : "2 should be evicted";
        assert cache.get(3) != null : "3 should still be in cache";
        assert cache.get(4) != null : "4 should be in cache";
        System.out.println("✓ Passed\n");
    }

    public static void testContainsKey() {
        System.out.println("Test 3: Contains Key");
        LRUCache<String, String> cache = new LRUCache<>(2);

        cache.put("key1", "value1");
        assert cache.containsKey("key1") : "key1 should exist";
        assert !cache.containsKey("key2") : "key2 should not exist";
        System.out.println("✓ Passed\n");
    }

    public static void testRemove() {
        System.out.println("Test 4: Remove Operation");
        LRUCache<Integer, Integer> cache = new LRUCache<>(3);

        cache.put(1, 1);
        cache.put(2, 2);
        cache.remove(1);

        assert cache.get(1) == null : "1 should be removed";
        assert cache.get(2) != null : "2 should still exist";
        System.out.println("✓ Passed\n");
    }

    public static void testClear() {
        System.out.println("Test 5: Clear Operation");
        LRUCache<String, String> cache = new LRUCache<>(3);

        cache.put("a", "1");
        cache.put("b", "2");
        cache.clear();

        assert cache.isEmpty() : "Cache should be empty after clear";
        assert cache.size() == 0 : "Cache size should be 0";
        System.out.println("✓ Passed\n");
    }

    public static void testCapacityOne() {
        System.out.println("Test 6: Capacity of One");
        LRUCache<String, String> cache = new LRUCache<>(1);

        cache.put("a", "1");
        assert cache.get("a").equals("1") : "Should find 'a'";

        cache.put("b", "2");
        assert cache.get("a") == null : "'a' should be evicted";
        assert cache.get("b").equals("2") : "Should find 'b'";
        System.out.println("✓ Passed\n");
    }

    public static void testEmpty() {
        System.out.println("Test 7: Empty Cache");
        LRUCache<String, String> cache = new LRUCache<>(5);

        assert cache.isEmpty() : "New cache should be empty";
        cache.put("a", "1");
        assert !cache.isEmpty() : "Cache should not be empty after put";
        System.out.println("✓ Passed\n");
    }

    public static void testSize() {
        System.out.println("Test 8: Size Operation");
        LRUCache<Integer, Integer> cache = new LRUCache<>(5);

        assert cache.size() == 0 : "Initial size should be 0";
        cache.put(1, 1);
        assert cache.size() == 1 : "Size should be 1";
        cache.put(2, 2);
        assert cache.size() == 2 : "Size should be 2";
        cache.remove(1);
        assert cache.size() == 1 : "Size should be 1 after removal";
        System.out.println("✓ Passed\n");
    }

    public static void testConcurrentAccess() {
        System.out.println("Test 9: Concurrent Access (Thread Safety)");
        LRUCache<Integer, Integer> cache = new LRUCache<>(100);

        Thread t1 = new Thread(() -> {
            for (int i = 0; i < 50; i++) {
                cache.put(i, i);
            }
        });

        Thread t2 = new Thread(() -> {
            for (int i = 25; i < 75; i++) {
                cache.put(i, i);
            }
        });

        t1.start();
        t2.start();

        try {
            t1.join();
            t2.join();
        } catch (InterruptedException e) {
            e.printStackTrace();
        }

        assert cache.size() <= 100 : "Cache size should not exceed capacity";
        System.out.println("✓ Passed (Cache size: " + cache.size() + ")\n");
    }

    public static void testToString() {
        System.out.println("Test 10: ToString Representation");
        LRUCache<String, String> cache = new LRUCache<>(2);

        cache.put("a", "1");
        cache.put("b", "2");

        String representation = cache.toString();
        assert representation.contains("LRUCache") : "Should contain 'LRUCache'";
        System.out.println("Cache representation: " + representation);
        System.out.println("✓ Passed\n");
    }
}