#pragma once
#include <algorithm>
#include <cstddef>
#include <functional>
#include <optional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace algolab {
// Amortized O(1) FIFO using two stacks. Elements move from input to output once.
template<class T> class TwoStackQueue {
    std::vector<T> input_, output_;
    void prepare() {
        if (output_.empty()) while (!input_.empty()) {
            output_.push_back(std::move(input_.back())); input_.pop_back();
        }
    }
public:
    void push(T value) { input_.push_back(std::move(value)); }
    bool empty() const { return input_.empty() && output_.empty(); }
    std::size_t size() const { return input_.size() + output_.size(); }
    const T& front() { prepare(); if (output_.empty()) throw std::out_of_range("empty queue"); return output_.back(); }
    T pop() { prepare(); if (output_.empty()) throw std::out_of_range("empty queue");
        T value = std::move(output_.back()); output_.pop_back(); return value; }
};

template<class T> class MinStack {
    std::vector<std::pair<T,T>> items_;
public:
    void push(const T& value) { items_.emplace_back(value, items_.empty() ? value : std::min(value, items_.back().second)); }
    bool empty() const { return items_.empty(); }
    const T& minimum() const { if (empty()) throw std::out_of_range("empty stack"); return items_.back().second; }
    T pop() { if (empty()) throw std::out_of_range("empty stack"); auto value = items_.back().first; items_.pop_back(); return value; }
};

// Separate chaining, load factor <= 1 after insertion. Educational value-returning API.
template<class K, class V, class Hash = std::hash<K>> class HashMap {
    using Entry = std::pair<K,V>;
    std::vector<std::vector<Entry>> buckets_;
    std::size_t size_ = 0;
    Hash hash_;
    void grow() {
        std::vector<std::vector<Entry>> next(buckets_.size() * 2);
        for (const auto& bucket : buckets_) for (const auto& entry : bucket)
            next[hash_(entry.first) % next.size()].push_back(entry);
        buckets_.swap(next);
    }
public:
    explicit HashMap(std::size_t capacity = 8) : buckets_(std::max(std::size_t{1}, capacity)) {}
    std::size_t size() const { return size_; }
    std::optional<V> get(const K& key) const {
        for (const auto& entry : buckets_[hash_(key) % buckets_.size()]) if (entry.first == key) return entry.second;
        return std::nullopt;
    }
    void put(const K& key, const V& value) {
        for (auto& entry : buckets_[hash_(key) % buckets_.size()]) if (entry.first == key) { entry.second = value; return; }
        if (size_ >= buckets_.size()) grow();
        buckets_[hash_(key) % buckets_.size()].emplace_back(key, value); ++size_;
    }
    bool erase(const K& key) {
        auto& bucket = buckets_[hash_(key) % buckets_.size()];
        for (auto it = bucket.begin(); it != bucket.end(); ++it) if (it->first == key) {
            bucket.erase(it); --size_; return true;
        }
        return false;
    }
};
} // namespace algolab
