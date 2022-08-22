#ifndef graph_h
#define graph_h

#include <cassert>
#include <unordered_set>
#include <vector>
#include "Tree.h"

class Graph {
    private:
        size_t N;                               // number of nodes
        std::vector<std::vector<double>> costs; // upper-triangular cost matrix
        std::vector<std::pair<size_t, size_t>> bucketMatching() const;
        Tree connectTrees(const std::vector<Tree>& trees) const;
        Tree mergeCluster(const std::unordered_set<size_t>& active, size_t k)
                const;
    public:
        Graph(size_t N) : N{N}, costs(N, std::vector<double>(N, 0)) {}
        double edgeCost(size_t i, size_t j) const
                {return i<j?costs[i][j]:costs[j][i];}
        std::vector<std::pair<size_t, size_t>> minCostMatching();
        double MSTCost() const;
        Tree MST() const;
        void setEdgeCost(size_t i, size_t j, double c) {
            assert(i<j);
            costs.at(i).at(j)=c;
        }
        Tree unrootedKMST(const size_t k) const;
};

#endif

