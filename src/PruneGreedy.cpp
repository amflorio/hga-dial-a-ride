#include <algorithm>
#include <iostream>
#include <vector>
#include "PruneGreedy.h"

using namespace std;

shared_ptr<Instance> PruneGreedy::I;

void PruneGreedy::print(const Route& rt) {
    cout<<"[ ";
    for (const auto& stop : rt.stops())
        cout<<stop.node<<(stop.type==Stop::Type::start?"s"
                :stop.type==Stop::Type::pickup?"p":"d")<<" ";
    cout<<"]"<<endl;
}

vector<Route> PruneGreedy::solve(const vector<Driver>& drivers,
        const vector<Request>& requests) {
    const auto infty=numeric_limits<double>::infinity();
    // initialize one route for each driver
    vector<Route> routes;
    for (const auto& driver : drivers)
        routes.emplace_back(driver.s);
    double tot_cost=0;
    for (const auto& req : requests) {
        tuple<size_t, size_t, size_t, double> best {0, 0, 0, infty};//k,i,j,c
        for (size_t k=0; k<routes.size(); ++k) {
            const auto& rt=routes[k];
            // driver load when departing from the i-th node along the route
            vector<size_t> pick(rt.size(), 0);
            // distance increase when inserting req.s just after the i-th node
            // det_s[i] = det(l_i, s_r, l_i+1)
            vector<double> det_s(rt.size()-1, 0);
            // distance increase when inserting req.t just after the i-th node
            // det_t[i] = det(l_i, t_r, l_i+1)
            vector<double> det_t(rt.size()-1, 0);
            const auto& stops=rt.stops();
            for (size_t i=0; i<stops.size(); ++i) {
                if (i>0) {
                    // compute pick[i]
                    if (stops[i].type==Stop::Type::pickup) {
                        pick[i]=pick[i-1]+1;
                        assert(pick[i]<=I->capacity());
                    } else {
                        assert(stops[i].type==Stop::Type::dropoff);
                        assert(pick[i-1]>=1);
                        pick[i]=pick[i-1]-1;
                    }
                }
                if (i<stops.size()-1) {
                    // compute det_s[i]
                    det_s[i]=I->cost(stops[i].node, req.s)
                            +I->cost(req.s, stops[i+1].node)
                            -I->cost(stops[i].node, stops[i+1].node);
                    // compute det_t[i]
                    det_t[i]=I->cost(stops[i].node, req.t)
                            +I->cost(req.t, stops[i+1].node)
                            -I->cost(stops[i].node, stops[i+1].node);
                }
            }
            // naive implementation, sufficient for our cost-benchmark purpose
            for (size_t i=1; i<=rt.size(); ++i) {
                for (size_t j=i; j<=rt.size(); ++j) {
                    if (pick[j-1]==I->capacity())
                        break;
                    double c;
                    if (i==j && i==rt.size())
                        c=I->cost(stops[i-1].node, req.s)+I->cost(req.s, req.t);
                    else if (i==j)
                        c=I->cost(stops[i-1].node, req.s)+I->cost(req.s, req.t)
                                +I->cost(req.t, stops[i].node)
                                -I->cost(stops[i-1].node, stops[i].node);
                    else if (j==rt.size())      // i<j && j==N
                        c=det_s[i-1]+I->cost(stops[j-1].node, req.t);
                    else                        // i<j && j<N
                        c=det_s[i-1]+det_t[j-1];
                    if (c<get<3>(best))
                        best={k, i, j, c};
                }
            }
        }
        // unpack the tuple
        size_t i, j, k;
        double c;
        tie(k, i, j, c)=best;
        assert(i!=0 && j!=0);
        tot_cost+=c;
        routes[k].insert(i, Stop(req.s, Stop::Type::pickup, req.id));
        routes[k].insert(j+1, Stop(req.t, Stop::Type::dropoff, req.id));
    }
    // remove routes to which no request was assigned
    routes.erase(remove_if(routes.begin(), routes.end(),
            [](const Route& rt){return rt.stops().size()==1;}), routes.end());
    // consistency check
    double c=0;
    for (const auto& rt : routes)
        c+=rt.cost();
    if (abs(c-tot_cost)>1e-6)
        cout<<"warning: c="<<c<<" ;  tot_cost="<<tot_cost<<endl;
    return routes;
}

