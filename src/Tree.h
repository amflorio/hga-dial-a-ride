#ifndef tree_h
#define tree_h

#include <memory>
#include <unordered_set>
#include <vector>

class Tree {
    public:
        double cost=0;
        std::vector<std::pair<size_t, size_t>> edges;
        Tree(double c, std::vector<std::pair<size_t, size_t>> edges) : cost{c},
                edges{std::move(edges)} {}
        std::unordered_set<size_t> nodes() const {
            std::unordered_set<size_t> nds;
            for (const auto& e : edges) {
                nds.insert(e.first);
                nds.insert(e.second);
            }
            return nds;
        }
};

#endif

