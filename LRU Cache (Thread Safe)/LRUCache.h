#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <unordered_map>
#include <list>
#include <shared_mutex>
#include <memory>
#include <stdexcept>
#include <sstream>

/**
 * Thread-safe LRU (Least Recently Used) Cache implementation.
 *
 * This cache stores a maximum of N key-value pairs. When the cache reaches
 * its capacity and a new item is added, the least recently used item is evicted.
 *
 * Time Complexity:
 * - get(const K& key): O(1)
 * - put(const K& key, const V& value): O(1)
 * - remove(const K& key): O(1)
 *
 * Space Complexity: O(capacity)
 *
 * @tparam K The type of keys maintained by this cache
 * @tparam V The type of mapped values
 */
template <typename K, typename V>
class LRUCache {
private:
    struct Node {
        K key;
        V value;
        std::shared_ptr<Node> prev;
        std::shared_ptr<Node> next;

        Node() : key(), value() {}
        Node(const K& k, const V& v) : key(k), value(v) {}
    };

    int capacity_;
    std::unordered_map<K, std::shared_ptr<Node>> map_;
    std::shared_ptr<Node> head_;  // sentinel node for beginning of list
    std::shared_ptr<Node> tail_;  // sentinel node for end of list
    mutable std::shared_mutex lock_;

    void removeNode(std::shared_ptr<Node> node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    void addNodeToEnd(std::shared_ptr<Node> node) {
        node->prev = tail_->prev;
        node->next = tail_;
        tail_->prev->next = node;
        tail_->prev = node;
    }

public:
    /**
     * Initializes an LRU Cache with the specified capacity.
     *
     * @param capacity The maximum number of entries the cache can hold
     * @throws std::invalid_argument if capacity <= 0
     */
    explicit LRUCache(int capacity) : capacity_(capacity) {
        if (capacity <= 0) {
            throw std::invalid_argument("Capacity must be greater than 0");
        }
        head_ = std::make_shared<Node>();
        tail_ = std::make_shared<Node>();
        head_->next = tail_;
        tail_->prev = head_;
    }

    /**
     * Retrieves the value associated with the given key.
     * Also marks the key as recently used by moving it to the end of the list.
     *
     * @param key The key whose value is to be retrieved
     * @return A shared_ptr to the value if found, nullptr otherwise
     */
    std::shared_ptr<V> get(const K& key) {
        std::shared_lock<std::shared_mutex> read_lock(lock_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return nullptr;
        }
        auto node = it->second;
        read_lock.unlock();

        std::unique_lock<std::shared_mutex> write_lock(lock_);
        removeNode(node);
        addNodeToEnd(node);
        return std::make_shared<V>(node->value);
    }

    /**
     * Inserts or updates a key-value pair in the cache.
     * If the key already exists, its value is updated and it becomes most recently used.
     * If the cache is at capacity, the least recently used entry is removed.
     *
     * @param key The key to be inserted or updated
     * @param value The value to be associated with the key
     */
    void put(const K& key, const V& value) {
        std::unique_lock<std::shared_mutex> write_lock(lock_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            auto node = it->second;
            node->value = value;
            removeNode(node);
            addNodeToEnd(node);
        } else {
            if (static_cast<int>(map_.size()) >= capacity_) {
                auto lru_node = head_->next;
                removeNode(lru_node);
                map_.erase(lru_node->key);
            }
            auto new_node = std::make_shared<Node>(key, value);
            addNodeToEnd(new_node);
            map_[key] = new_node;
        }
    }

    /**
     * Removes the entry associated with the given key.
     *
     * @param key The key of the entry to be removed
     */
    void remove(const K& key) {
        std::unique_lock<std::shared_mutex> write_lock(lock_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            auto node = it->second;
            removeNode(node);
            map_.erase(it);
        }
    }

    /**
     * Removes all entries from the cache.
     */
    void clear() {
        std::unique_lock<std::shared_mutex> write_lock(lock_);
        map_.clear();
        head_->next = tail_;
        tail_->prev = head_;
    }

    /**
     * Checks whether the given key is present in the cache.
     *
     * @param key The key to check
     * @return true if the key is in the cache, false otherwise
     */
    bool containsKey(const K& key) const {
        std::shared_lock<std::shared_mutex> read_lock(lock_);
        return map_.find(key) != map_.end();
    }

    /**
     * Returns the number of entries currently in the cache.
     *
     * @return The current size of the cache
     */
    int size() const {
        std::shared_lock<std::shared_mutex> read_lock(lock_);
        return static_cast<int>(map_.size());
    }

    /**
     * Checks whether the cache is empty.
     *
     * @return true if the cache contains no entries, false otherwise
     */
    bool isEmpty() const {
        std::shared_lock<std::shared_mutex> read_lock(lock_);
        return map_.empty();
    }

    /**
     * Returns the current capacity of the cache.
     *
     * @return The maximum number of entries this cache can hold
     */
    int getCapacity() const {
        return capacity_;
    }

    /**
     * Returns a string representation of the cache.
     *
     * @return String representation in format: LRUCache{key1=value1, key2=value2, ...}
     */
    std::string toString() const {
        std::shared_lock<std::shared_mutex> read_lock(lock_);
        std::ostringstream oss;
        oss << "LRUCache{";
        auto current = head_->next;
        bool first = true;
        while (current != tail_) {
            if (!first) {
                oss << ", ";
            }
            oss << current->key << "=" << current->value;
            current = current->next;
            first = false;
        }
        oss << "}";
        return oss.str();
    }

    ~LRUCache() = default;
};

#endif // LRU_CACHE_H
