#include <algorithm>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <random>
#include <tuple>
#include "TSPHeuristic.h"

using namespace std;

TSPSolution TSPHeuristic::cheapestInsertion() const {
    tuple<size_t,size_t,double> cheapest {0, 0, numeric_limits<double>::max()};
    for (size_t i=0; i<costs.size(); ++i)
        for (size_t j=0; j<costs.size(); ++j)
            if (i!=j && costs[i][j]<get<2>(cheapest))
                cheapest={i, j, costs[i][j]};
    list<size_t> tour {get<0>(cheapest), get<1>(cheapest)};
    list<size_t> pending;
    for (size_t i=0; i<costs.size(); ++i)
        if (i!=tour.front() && i!=tour.back())
            pending.push_back(i);
    double totcost=costs[tour.front()][tour.back()]
            +costs[tour.back()][tour.front()];
    while (!pending.empty()) {
        tuple<list<size_t>::iterator, list<size_t>::iterator, double> best
                {pending.end(), tour.end(), numeric_limits<double>::max()};
        for (auto it_p=pending.begin(); it_p!=pending.end(); ++it_p)
            for (auto it_t=tour.begin(); it_t!=tour.end(); ++it_t) {
                size_t i=it_t==tour.begin()?tour.back():*prev(it_t);
                double cost=costs[i][*it_p]+costs[*it_p][*it_t]-costs[i][*it_t];
                if (cost<get<2>(best))
                    best={it_p, it_t, cost};
            }
        tour.insert(get<1>(best), *get<0>(best));
        pending.erase(get<0>(best));
        totcost+=get<2>(best);
    }
    return {vector<size_t>(tour.begin(), tour.end()), totcost};
}

TSPSolution TSPHeuristic::randomInsertion() const {
    default_random_engine g {42};
    vector<size_t> pending(costs.size());
    iota(pending.begin(), pending.end(), 0);
    shuffle(pending.begin(), pending.end(), g);
    list<size_t> tour {pending.end()[-1], pending.end()[-2]};
    pending.resize(pending.size()-2);
    double totcost=costs[tour.front()][tour.back()]
            +costs[tour.back()][tour.front()];
    while (!pending.empty()) {
        size_t r=pending.back();           // random node
        pending.pop_back();
        tuple<list<size_t>::iterator, double> best
                {tour.end(), numeric_limits<double>::max()};
        for (auto it_t=tour.begin(); it_t!=tour.end(); ++it_t) {
            size_t i=it_t==tour.begin()?tour.back():*prev(it_t);
            double cost=costs[i][r]+costs[r][*it_t]-costs[i][*it_t];
            if (cost<get<1>(best))
                best={it_t, cost};
        }
        tour.insert(get<0>(best), r);
        totcost+=get<1>(best);
    }
    return {vector<size_t>(tour.begin(), tour.end()), totcost};
}

