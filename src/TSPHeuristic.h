#ifndef tspheuristic_h
#define tspheuristic_h

#include <memory>
#include <vector>
#include "TSPSolution.h"

class TSPHeuristic {
    private:
        std::vector<std::vector<double>> costs;
    public:
        TSPHeuristic(std::vector<std::vector<double>> csts)
                : costs{std::move(csts)} {}
        TSPSolution cheapestInsertion() const;
};

#endif

