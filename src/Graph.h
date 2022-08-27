#ifndef graph_h
#define graph_h

#include <cassert>
#include <unordered_set>
#include <vector>
#include "Tree.h"

class Graph {
    private:
        size_t N;                               // number of nodes
        std::vector<std::vector<float>> csts;   // triangular cost matrix
        std::vector<std::pair<size_t, size_t>> bucketMatching() const;
        Tree connectTrees(const std::vector<Tree>& trees) const;
        Tree mergeCluster(const std::unordered_set<size_t>& active, size_t k)
                const;
        float cost(size_t i, size_t j) const {
            assert(i<j);
            return csts.at(i).at(j-i-1);
        }
    public:
        Graph(size_t N) : N{N} {
            for (size_t i=0; i<N; ++i)
                csts.push_back(std::vector<float>(N-i-1, 0));
        }
        float edgeCost(size_t i, size_t j) const
                {return i<j?cost(i, j):cost(j, i);}
        std::vector<std::pair<size_t, size_t>> minCostMatching();
        float MSTCost() const;
        Tree MST() const;
        void setEdgeCost(size_t i, size_t j, float c) {
            assert(i<j);
            csts.at(i).at(j-i-1)=c;
        }
        Tree unrootedKMST(const size_t k) const;
};

#endif

