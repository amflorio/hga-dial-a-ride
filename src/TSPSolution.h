#ifndef tspsolution_h
#define tspsolution_h

#include <vector>

class TSPSolution {
    private:
        std::vector<size_t> tour_;
        double cost_;
    public:
        TSPSolution(std::vector<size_t> t, double c) : tour_{std::move(t)},
                cost_{c} {}
        const std::vector<size_t>& tour() const {return tour_;}
        double cost() const {return cost_;}
};

#endif

