#pragma once
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <optional>
#include <queue>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace algolab {
// Precondition: input is sorted in nondecreasing order. Half-open interval [lo, hi).
inline std::size_t lower_bound_index(const std::vector<int>& a, int target) {
    std::size_t lo = 0, hi = a.size();
    while (lo < hi) {
        const auto mid = lo + (hi - lo) / 2;
        if (a[mid] < target) lo = mid + 1; else hi = mid;
    }
    return lo;
}

inline void merge_sort(std::vector<int>& a) {
    std::vector<int> buffer(a.size());
    const auto sort = [&](const auto& self, std::size_t lo, std::size_t hi) -> void {
        if (hi - lo < 2) return;
        const auto mid = lo + (hi - lo) / 2;
        self(self, lo, mid); self(self, mid, hi);
        auto i = lo, j = mid, k = lo;
        while (i < mid && j < hi) buffer[k++] = a[i] <= a[j] ? a[i++] : a[j++];
        while (i < mid) buffer[k++] = a[i++];
        while (j < hi) buffer[k++] = a[j++];
        for (k = lo; k < hi; ++k) a[k] = buffer[k];
    };
    sort(sort, 0, a.size());
}

// Returns distinct indices, not values; the complement uses int64_t to avoid int overflow.
inline std::optional<std::pair<std::size_t, std::size_t>> two_sum(const std::vector<int>& a, int target) {
    std::unordered_map<std::int64_t, std::size_t> seen;
    for (std::size_t i = 0; i < a.size(); ++i) {
        const auto complement = static_cast<std::int64_t>(target) - a[i];
        const auto it = seen.find(complement);
        if (it != seen.end()) return std::make_pair(it->second, i);
        seen.emplace(a[i], i);
    }
    return std::nullopt;
}

inline bool balanced_brackets(const std::string& text) {
    std::vector<char> stack;
    for (char c : text) {
        if (c == '(' || c == '[' || c == '{') stack.push_back(c);
        else if (c == ')' || c == ']' || c == '}') {
            if (stack.empty()) return false;
            const char expected = c == ')' ? '(' : c == ']' ? '[' : '{';
            if (stack.back() != expected) return false;
            stack.pop_back();
        }
    }
    return stack.empty();
}

// Byte-oriented, not Unicode-code-point-oriented.
inline std::size_t longest_unique_substring(const std::string& text) {
    std::vector<std::size_t> next_start(256, 0);
    std::size_t left = 0, best = 0;
    for (std::size_t right = 0; right < text.size(); ++right) {
        const auto c = static_cast<unsigned char>(text[right]);
        left = std::max(left, next_start[c]);
        best = std::max(best, right - left + 1);
        next_start[c] = right + 1;
    }
    return best;
}

// int64_t accumulator; throws instead of overflowing for unrepresentable sums.
inline std::int64_t checked_add(std::int64_t a, std::int64_t b) {
    if ((b > 0 && a > std::numeric_limits<std::int64_t>::max() - b) ||
        (b < 0 && a < std::numeric_limits<std::int64_t>::min() - b))
        throw std::overflow_error("sum exceeds int64_t");
    return a + b;
}
inline std::optional<std::int64_t> maximum_subarray(const std::vector<int>& a) {
    if (a.empty()) return std::nullopt;
    std::int64_t ending = a[0], best = a[0];
    for (std::size_t i = 1; i < a.size(); ++i) {
        ending = std::max(static_cast<std::int64_t>(a[i]), checked_add(ending, a[i]));
        best = std::max(best, ending);
    }
    return best;
}

using Interval = std::pair<int, int>;
// Closed intervals: touching endpoints are merged.
inline std::vector<Interval> merge_intervals(std::vector<Interval> intervals) {
    for (const auto& interval : intervals)
        if (interval.first > interval.second) throw std::invalid_argument("reversed interval");
    std::sort(intervals.begin(), intervals.end());
    std::vector<Interval> result;
    for (const auto& interval : intervals) {
        if (result.empty() || result.back().second < interval.first) result.push_back(interval);
        else result.back().second = std::max(result.back().second, interval.second);
    }
    return result;
}

using Graph = std::vector<std::vector<std::size_t>>;
inline void validate_graph(const Graph& graph) {
    for (const auto& edges : graph) for (const auto v : edges)
        if (v >= graph.size()) throw std::out_of_range("invalid vertex");
}
inline std::vector<int> bfs_distances(const Graph& graph, std::size_t source) {
    validate_graph(graph);
    if (source >= graph.size()) throw std::out_of_range("invalid source");
    std::vector<int> distance(graph.size(), -1);
    std::queue<std::size_t> queue;
    distance[source] = 0; queue.push(source);
    while (!queue.empty()) {
        const auto u = queue.front(); queue.pop();
        for (const auto v : graph[u]) if (distance[v] == -1) {
            if (distance[u] == std::numeric_limits<int>::max()) throw std::overflow_error("distance overflow");
            distance[v] = distance[u] + 1; queue.push(v);
        }
    }
    return distance;
}
inline std::vector<std::size_t> dfs_order(const Graph& graph, std::size_t source) {
    validate_graph(graph);
    if (source >= graph.size()) throw std::out_of_range("invalid source");
    std::vector<bool> visited(graph.size(), false);
    std::vector<std::size_t> stack{source}, order;
    while (!stack.empty()) {
        const auto u = stack.back(); stack.pop_back();
        if (visited[u]) continue;
        visited[u] = true; order.push_back(u);
        for (auto it = graph[u].rbegin(); it != graph[u].rend(); ++it)
            if (!visited[*it]) stack.push_back(*it);
    }
    return order;
}
// Kahn's algorithm. A null result means the directed graph contains a cycle.
inline std::optional<std::vector<std::size_t>> topological_sort(const Graph& graph) {
    validate_graph(graph);
    std::vector<std::size_t> indegree(graph.size(), 0), order;
    for (const auto& edges : graph) for (auto v : edges) ++indegree[v];
    std::queue<std::size_t> queue;
    for (std::size_t v = 0; v < graph.size(); ++v) if (indegree[v] == 0) queue.push(v);
    while (!queue.empty()) {
        auto u = queue.front(); queue.pop(); order.push_back(u);
        for (auto v : graph[u]) if (--indegree[v] == 0) queue.push(v);
    }
    if (order.size() != graph.size()) return std::nullopt;
    return order;
}

inline std::size_t lis_length(const std::vector<int>& a) {
    std::vector<int> tails;
    for (int value : a) {
        auto it = std::lower_bound(tails.begin(), tails.end(), value);
        if (it == tails.end()) tails.push_back(value); else *it = value;
    }
    return tails.size();
}

inline std::optional<int> minimum_coins(std::vector<int> coins, int amount) {
    if (amount < 0) throw std::invalid_argument("negative amount");
    for (auto coin : coins) if (coin <= 0) throw std::invalid_argument("nonpositive coin");
    std::vector<int> best(static_cast<std::size_t>(amount) + 1, -1);
    best[0] = 0;
    for (std::size_t value = 1; value < best.size(); ++value) for (int coin : coins) {
        const auto c = static_cast<std::size_t>(coin);
        if (c <= value && best[value-c] >= 0 && (best[value] < 0 || best[value-c] + 1 < best[value]))
            best[value] = best[value-c] + 1;
    }
    return best.back() < 0 ? std::nullopt : std::optional<int>{best.back()};
}
} // namespace algolab
