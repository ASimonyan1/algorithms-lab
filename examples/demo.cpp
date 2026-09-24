#include <algolab/algorithms.hpp>
#include <algolab/structures.hpp>
#include <iostream>

int main() {
    std::vector<int> values{7, 2, 9, 2, -1};
    algolab::merge_sort(values);
    std::cout << "Merge sort:";
    for (int value : values) std::cout << ' ' << value;
    std::cout << "\nLower bound of 2: " << algolab::lower_bound_index(values, 2);
    const auto distances = algolab::bfs_distances({{1, 2}, {3}, {3}, {}, {}}, 0);
    std::cout << "\nBFS distances:";
    for (auto distance : distances) std::cout << ' ' << distance;
    algolab::HashMap<std::string, int> counts;
    counts.put("Kotlin", 3); counts.put("C++", 5);
    std::cout << "\nHashMap[C++]: " << *counts.get("C++") << '\n';
}
