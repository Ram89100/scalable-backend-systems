import java.util.*;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * Thread-safe LRU (Least Recently Used) Cache implementation.
 *
 * This cache stores a maximum of N key-value pairs. When the cache reaches
 * its capacity and a new item is added, the least recently used item is evicted.
 *
 * Time Complexity:
 * - get(K key): O(1)
 * - put(K key, V value): O(1)
 * - remove(K key): O(1)
 *
 * Space Complexity: O(capacity)
 *
 * @param <K> The type of keys maintained by this cache
 * @param <V> The type of mapped values
 */
public class LRUCache<K, V> {
    private static final class Node<K, V> {
        K key;
        V value;
        Node<K, V> prev;
        Node<K, V> next;

        Node(K key, V value) {
            this.key = key;
            this.value = value;
        }
    }

    private final int capacity;
    private final Map<K, Node<K, V>> map;
    private final Node<K, V> head; // sentinel node for beginning of list
    private final Node<K, V> tail; // sentinel node for end of list
    private final ReentrantReadWriteLock lock = new ReentrantReadWriteLock();

    /**
     * Initializes an LRU Cache with the specified capacity.
     *
     * @param capacity The maximum number of entries the cache can hold
     * @throws IllegalArgumentException if capacity <= 0
     */
    public LRUCache(int capacity) {
        if (capacity <= 0) {
            throw new IllegalArgumentException("Capacity must be greater than 0");
        }
        this.capacity = capacity;
        this.map = new HashMap<>(capacity);
        this.head = new Node<>(null, null);
        this.tail = new Node<>(null, null);
        head.next = tail;
        tail.prev = head;
    }

    /**
     * Retrieves the value associated with the given key.
     * Also marks the key as recently used by moving it to the end of the list.
     *
     * @param key The key whose value is to be retrieved
     * @return The value associated with the key, or null if not found
     */
    public V get(K key) {
        lock.readLock().lock();
        try {
            Node<K, V> node = map.get(key);
            if (node == null) {
                return null;
            }
            lock.readLock().unlock();
            lock.writeLock().lock();
            try {
                // Move to end (most recently used)
                removeNode(node);
                addNodeToEnd(node);
                return node.value;
            } finally {
                lock.writeLock().unlock();
            }
        } finally {
            if (lock.getReadLockCount() > 0) {
                lock.readLock().unlock();
            }
        }
    }

    /**
     * Inserts or updates a key-value pair in the cache.
     * If the key already exists, its value is updated and it becomes most recently used.
     * If the cache is at capacity, the least recently used entry is removed.
     *
     * @param key The key to be inserted or updated
     * @param value The value to be associated with the key
     */
    public void put(K key, V value) {
        lock.writeLock().lock();
        try {
            if (map.containsKey(key)) {
                Node<K, V> node = map.get(key);
                node.value = value;
                removeNode(node);
                addNodeToEnd(node);
            } else {
                if (map.size() >= capacity) {
                    Node<K, V> lruNode = head.next;
                    removeNode(lruNode);
                    map.remove(lruNode.key);
                }
                Node<K, V> newNode = new Node<>(key, value);
                addNodeToEnd(newNode);
                map.put(key, newNode);
            }
        } finally {
            lock.writeLock().unlock();
        }
    }

    /**
     * Removes the entry associated with the given key.
     *
     * @param key The key of the entry to be removed
     */
    public void remove(K key) {
        lock.writeLock().lock();
        try {
            Node<K, V> node = map.remove(key);
            if (node != null) {
                removeNode(node);
            }
        } finally {
            lock.writeLock().unlock();
        }
    }

    /**
     * Removes all entries from the cache.
     */
    public void clear() {
        lock.writeLock().lock();
        try {
            map.clear();
            head.next = tail;
            tail.prev = head;
        } finally {
            lock.writeLock().unlock();
        }
    }

    /**
     * Checks whether the given key is present in the cache.
     *
     * @param key The key to check
     * @return true if the key is in the cache, false otherwise
     */
    public boolean containsKey(K key) {
        lock.readLock().lock();
        try {
            return map.containsKey(key);
        } finally {
            lock.readLock().unlock();
        }
    }

    /**
     * Returns the number of entries currently in the cache.
     *
     * @return The current size of the cache
     */
    public int size() {
        lock.readLock().lock();
        try {
            return map.size();
        } finally {
            lock.readLock().unlock();
        }
    }

    /**
     * Checks whether the cache is empty.
     *
     * @return true if the cache contains no entries, false otherwise
     */
    public boolean isEmpty() {
        lock.readLock().lock();
        try {
            return map.isEmpty();
        } finally {
            lock.readLock().unlock();
        }
    }

    /**
     * Returns the current capacity of the cache.
     *
     * @return The maximum number of entries this cache can hold
     */
    public int getCapacity() {
        return capacity;
    }

    // Private helper methods

    private void removeNode(Node<K, V> node) {
        node.prev.next = node.next;
        node.next.prev = node.prev;
    }

    private void addNodeToEnd(Node<K, V> node) {
        node.prev = tail.prev;
        node.next = tail;
        tail.prev.next = node;
        tail.prev = node;
    }

    @Override
    public String toString() {
        lock.readLock().lock();
        try {
            StringBuilder sb = new StringBuilder("LRUCache{");
            Node<K, V> current = head.next;
            while (current != tail) {
                sb.append(current.key).append("=").append(current.value);
                current = current.next;
                if (current != tail) {
                    sb.append(", ");
                }
            }
            sb.append("}");
            return sb.toString();
        } finally {
            lock.readLock().unlock();
        }
    }
}