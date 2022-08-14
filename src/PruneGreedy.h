#ifndef prunegreedy_h
#define prunegreedy_h

#include <memory>
#include "Instance.h"
#include "Route.h"

class PruneGreedy {
    private:
        static std::shared_ptr<Instance> I;
        static void print(const Route& rt);
    public:
        static void setInstance(std::shared_ptr<Instance> i) {I=i;}
        static std::vector<Route> solve(const std::vector<Driver>& drivers,
                const std::vector<Request>& requests);
};

#endif

