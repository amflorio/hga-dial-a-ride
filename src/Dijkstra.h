#ifndef dijkstra_h
#define dijkstra_h

#include <memory>
#include "Instance.h"

class Dijkstra {
    private:
        static std::shared_ptr<Instance> I;
        static std::vector<std::vector<double>> costs;
    public:
        static double cost(int s, int t);
        static void init();
        static void setInstance(std::shared_ptr<Instance> i) {I=i;}
        static void solve(int s);
};

#endif

