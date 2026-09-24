#include <algolab/algorithms.hpp>
#include <algolab/structures.hpp>
#include <deque>
#include <iostream>
#include <random>
#include <unordered_map>

int checks = 0;
void check(bool condition, const char* expression, int line) {
    ++checks;
    if (!condition) throw std::runtime_error(std::string("Line ") + std::to_string(line) + ": " + expression);
}
#define CHECK(...) check((__VA_ARGS__), #__VA_ARGS__, __LINE__)
template<class E, class F> void expect_throw(F action) {
    bool caught = false;
    try { action(); } catch (const E&) { caught = true; }
    CHECK(caught);
}
struct ConstantHash { std::size_t operator()(int) const { return 0; } };

int main() {
    using namespace algolab;
    try {
        CHECK(lower_bound_index({}, 3) == 0);
        CHECK(lower_bound_index({1,2,2,4}, 2) == 1);
        CHECK(lower_bound_index({1,2}, 9) == 2);
        CHECK(balanced_brackets("a{b[c](d)}"));
        CHECK(balanced_brackets(""));
        CHECK(!balanced_brackets("([)]"));
        CHECK(!balanced_brackets("]"));
        CHECK(!balanced_brackets("(("));
        CHECK(longest_unique_substring("abba") == 2);
        CHECK(longest_unique_substring("") == 0);
        CHECK(longest_unique_substring(std::string("a\0b", 3)) == 3);
        CHECK(!maximum_subarray({}));
        CHECK(maximum_subarray({-5,-2,-9}) == -2);
        CHECK(maximum_subarray({-2,1,-3,4,-1,2,1,-5,4}) == 6);
        CHECK(maximum_subarray({std::numeric_limits<int>::max(), std::numeric_limits<int>::max()}) == 4294967294LL);
        expect_throw<std::overflow_error>([] { checked_add(std::numeric_limits<std::int64_t>::max(), 1); });
        CHECK(merge_intervals({{3,4},{1,3},{8,9}}) == std::vector<Interval>({{1,4},{8,9}}));
        CHECK(merge_intervals({}).empty());
        expect_throw<std::invalid_argument>([] { merge_intervals({{4,1}}); });
        CHECK(bfs_distances({{1},{0,2},{},{}}, 0) == std::vector<int>({0,1,2,-1}));
        CHECK(dfs_order({{1,2},{0,3},{3},{}}, 0) == std::vector<std::size_t>({0,1,3,2}));
        CHECK(!topological_sort({{1},{0}}));
        CHECK(!topological_sort({{0}}));
        CHECK(topological_sort({})->empty());
        CHECK(*topological_sort({{1,2},{3},{3},{}}) == std::vector<std::size_t>({0,1,2,3}));
        expect_throw<std::out_of_range>([] { bfs_distances({}, 0); });
        expect_throw<std::out_of_range>([] { dfs_order({{9}}, 0); });
        expect_throw<std::out_of_range>([] { topological_sort({{1}}); });
        CHECK(lis_length({10,9,2,5,3,7,101,18}) == 4);
        CHECK(lis_length({2,2,2}) == 1);
        CHECK(lis_length({}) == 0);
        CHECK(minimum_coins({1,3,4}, 6) == 2);
        CHECK(!minimum_coins({2}, 3));
        CHECK(minimum_coins({}, 0) == 0);
        expect_throw<std::invalid_argument>([] { minimum_coins({0}, 2); });
        expect_throw<std::invalid_argument>([] { minimum_coins({1}, -1); });
        CHECK(!two_sum({1}, 2));
        CHECK(two_sum({3,3}, 6) == std::make_pair(std::size_t{0}, std::size_t{1}));
        CHECK(two_sum({std::numeric_limits<int>::min(), std::numeric_limits<int>::max()}, -1).has_value());
        MinStack<int> minimum;
        expect_throw<std::out_of_range>([&] { minimum.minimum(); });
        minimum.push(3); minimum.push(1); minimum.push(1); minimum.push(5);
        CHECK(minimum.minimum() == 1); CHECK(minimum.pop() == 5);
        CHECK(minimum.pop() == 1); CHECK(minimum.minimum() == 1);
        CHECK(minimum.pop() == 1); CHECK(minimum.minimum() == 3);
        CHECK(minimum.pop() == 3); CHECK(minimum.empty());
        expect_throw<std::out_of_range>([&] { minimum.pop(); });

        // Fixed seed: failures are reproducible. Compare against independent reference solutions.
        std::mt19937 rng(20260924);
        std::uniform_int_distribution<int> value(-30, 30), length(0, 60), action(0, 2);
        for (int round = 0; round < 1000; ++round) {
            std::vector<int> a(static_cast<std::size_t>(length(rng)));
            for (int& v : a) v = value(rng);
            auto expected = a; std::sort(expected.begin(), expected.end());
            auto sorted = a; merge_sort(sorted); CHECK(sorted == expected);
            const int target = value(rng);
            CHECK(lower_bound_index(sorted, target) == static_cast<std::size_t>(std::lower_bound(sorted.begin(), sorted.end(), target) - sorted.begin()));
            bool pair_exists = false;
            for (std::size_t i = 0; i < a.size(); ++i) for (std::size_t j = i + 1; j < a.size(); ++j)
                if (a[i] + a[j] == target) pair_exists = true;
            auto pair = two_sum(a, target); CHECK(pair.has_value() == pair_exists);
            if (pair) { CHECK(pair->first != pair->second); CHECK(a[pair->first] + a[pair->second] == target); }
            std::optional<std::int64_t> brute;
            for (std::size_t i = 0; i < a.size(); ++i) {
                std::int64_t sum = 0;
                for (std::size_t j = i; j < a.size(); ++j) { sum += a[j]; if (!brute || sum > *brute) brute = sum; }
            }
            CHECK(maximum_subarray(a) == brute);
            std::vector<std::size_t> dp(a.size(), 1); std::size_t longest = 0;
            for (std::size_t i = 0; i < a.size(); ++i) {
                for (std::size_t j = 0; j < i; ++j) if (a[j] < a[i]) dp[i] = std::max(dp[i], dp[j] + 1);
                longest = std::max(longest, dp[i]);
            }
            CHECK(lis_length(a) == longest);
        }
        TwoStackQueue<int> queue; std::deque<int> reference_queue;
        HashMap<int,int,ConstantHash> map(0); std::unordered_map<int,int> reference_map;
        for (int i = 0; i < 5000; ++i) {
            int key = value(rng), v = value(rng), op = action(rng);
            if (op == 0) { queue.push(v); reference_queue.push_back(v); map.put(key, v); reference_map[key] = v; }
            else if (op == 1) {
                if (!reference_queue.empty()) { CHECK(queue.front() == reference_queue.front()); CHECK(queue.pop() == reference_queue.front()); reference_queue.pop_front(); }
                CHECK(map.erase(key) == (reference_map.erase(key) != 0));
            } else {
                auto found = reference_map.find(key); auto actual = map.get(key);
                CHECK(actual.has_value() == (found != reference_map.end()));
                if (actual) CHECK(*actual == found->second);
            }
            CHECK(map.size() == reference_map.size()); CHECK(queue.size() == reference_queue.size());
            CHECK(queue.empty() == reference_queue.empty());
        }
        while (!queue.empty()) queue.pop();
        expect_throw<std::out_of_range>([&] { queue.pop(); });
        expect_throw<std::out_of_range>([&] { queue.front(); });
        std::cout << "PASS: " << checks << " checks; seed=20260924\n";
    } catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
