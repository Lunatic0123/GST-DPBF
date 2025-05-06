/**
* Created on time Tue May  6 20:01:47 CST 2025
* 
* @author : Yaxin Ma Group16.
*/
#include <vector>
#include <set>
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <climits>
struct Tree {
    int root;
    size_t keywords = 0;
    int cost;

    bool operator<(const Tree& other) const {
        if (cost != other.cost) return cost < other.cost;
        if (root != other.root) return root < other.root;
        return keywords < other.keywords;
    }
};
/**
 * Best First DP solution
 * @param v_instance_graph   original group
 * @param v_generated_group_graph   original group with keywords
 * @param generated_group_vertices   group of keywords
 */
int test(std::vector<std::vector<std::pair<int, int>>>& v_instance_graph,
    std::vector<std::vector<std::pair<int, int>>>& v_generated_group_graph,
    std::unordered_set<int>& generated_group_vertices) {

    int vert_num = v_instance_graph.size();
    std::set<Tree> Qt_set;
    std::vector<std::vector<int>> dpT(vert_num,
        std::vector<int>((1 << (v_generated_group_graph.size() - vert_num)), INT_MAX));

    // keywords_P[S] stores the set of root nodes of the tree containing the keyword set S
    std::vector<std::set<int>> keywords_P((1 << (v_generated_group_graph.size() - vert_num)));

    // Calculate all the mask of keywords
    size_t all_keywords = 0;
    for (auto i : generated_group_vertices) {
        all_keywords |= (1 << (i - vert_num));
    }

    // Init:Create tree for vertices in group keywords
    for (auto i : generated_group_vertices) {
        size_t keyword_bit = (1 << (i - vert_num));
        for (auto& edge : v_generated_group_graph[i]) {
            int u = edge.first;

            Tree tree;
            tree.root = u;
            tree.keywords = keyword_bit;
            tree.cost = 0;

            Qt_set.insert(tree);
            dpT[u][keyword_bit] = 0;
            keywords_P[keyword_bit].insert(u);
        }
    }

    while (!Qt_set.empty()) {
        Tree current = *Qt_set.begin();
        Qt_set.erase(Qt_set.begin());

        if (current.keywords == all_keywords) {
            return current.cost;
        }

        // Iterate all the neighbors
        for (auto& edge : v_instance_graph[current.root]) {
            int neighbor = edge.first;
            int edge_cost = edge.second;

            if (dpT[current.root][current.keywords] + edge_cost < dpT[neighbor][current.keywords]) {
                Tree old_tree;
                old_tree.root = neighbor;
                old_tree.keywords = current.keywords;
                old_tree.cost = dpT[neighbor][current.keywords];

                auto it = Qt_set.find(old_tree);
                if (it != Qt_set.end()) {
                    Qt_set.erase(it);
                }

                // Update dp
                dpT[neighbor][current.keywords] = dpT[current.root][current.keywords] + edge_cost;

                Tree new_tree;
                new_tree.root = neighbor;
                new_tree.keywords = current.keywords;
                new_tree.cost = dpT[neighbor][current.keywords];

                Qt_set.insert(new_tree);
                keywords_P[new_tree.keywords].insert(neighbor);
            }
        }

        // Merge two trees with the same root but different sets of keywords
        for (size_t k = 1; k < (1 << (v_generated_group_graph.size() - vert_num)); k++) {
            if (k == 0 || k == current.keywords || (k & current.keywords) != 0 || keywords_P[k].count(current.root) == 0) {
                continue;
            }

            // Merge
            if (dpT[current.root][current.keywords] + dpT[current.root][k] < dpT[current.root][current.keywords | k]) {
                size_t new_keywords = current.keywords | k;

                Tree old_tree;
                old_tree.root = current.root;
                old_tree.keywords = new_keywords;
                old_tree.cost = dpT[current.root][new_keywords];

                auto it = Qt_set.find(old_tree);
                if (it != Qt_set.end()) {
                    Qt_set.erase(it);
                }

                // Update dp
                dpT[current.root][new_keywords] = dpT[current.root][current.keywords] + dpT[current.root][k];

                Tree new_tree;
                new_tree.root = current.root;
                new_tree.keywords = new_keywords;
                new_tree.cost = dpT[current.root][new_keywords];

                Qt_set.insert(new_tree);
                keywords_P[new_keywords].insert(current.root);
            }
        }
    }

    return -1;
}