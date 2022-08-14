#ifndef disjointset_h
#define disjointset_h

#include <numeric>
#include <vector>

class DisjointSet {
    private:
        std::vector<size_t> parents;
        std::vector<size_t> sizes;
    public:
        DisjointSet(size_t N) : parents(N), sizes(N, 1) {
            std::iota(parents.begin(), parents.end(), 0);
        }
        /*
        // path compression
        size_t findSet(size_t i) {
            if (parents[i]==i)
                return i;
            parents[i]=findSet(parents[i]);
            return parents[i];
        }
        */
        // path compression - one pass
        size_t findSet(size_t x) {
            while (parents[x]!=x) {
                parents[x]=parents[parents[x]];
                x=parents[x];
            }
            return x;
        }
        size_t sizeSet(size_t x) {return sizes[findSet(x)];}
        void unionSet(size_t x, size_t y) {
            x=findSet(x);
            y=findSet(y);
            if (x!=y) {
                if (sizes[x]<sizes[y])
                    std::swap(x, y);
                parents[y]=x;
                sizes[x]+=sizes[y];
            }
        }
};

#endif

