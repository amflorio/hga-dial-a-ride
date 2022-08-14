#include <cmath>
#include <iostream>
#include "Graph.h"
#include "Instance.h"
#include "PartitionLambdaForest.h"
#include "Request.h"

using namespace std;

shared_ptr<Instance> PartitionLambdaForest::I;

vector<Group> PartitionLambdaForest::partition() {
    vector<Group> grps;
    Group uncovered(I->requests().begin(), I->requests().end());
    while (!uncovered.empty()) {
        cout<<"PartitionLambdaForest: uncovered: "<<uncovered.size()<<endl;
        auto g=minRatioLambdaForest(uncovered);
        for (const auto& r : g)
            uncovered.erase(r);
        grps.push_back(move(g));
    }
    return grps;
}

Group PartitionLambdaForest::minRatioLambdaForest(const Group& g) {
    assert(!g.empty());
    pair<double,Group> bestgrp{numeric_limits<double>::infinity(),{*g.begin()}};
    for (size_t sqrt_q=2; sqrt_q<=sqrt(I->capacity()); ++sqrt_q) {
        cout<<"minRatioLambdaForest: sqrt_q: "<<sqrt_q<<endl;
        for (const auto& r1 : g) {
            double d=I->cost(r1.s, r1.t);
            vector<Request> reqs;
            for (const auto& r2 : g)    // this for loop could be avoided
                if (I->cost(r2.s, r2.t)<=d)
                    reqs.push_back(r2);
            Graph graph(reqs.size());
            for (size_t i=0; i<reqs.size(); ++i)
                for (size_t j=i+1; j<reqs.size(); ++j) {
                    graph.setEdgeCost(i, j, I->cost(reqs[i].s, reqs[j].s)
                            +I->cost(reqs[i].t, reqs[j].t));
                }
            auto kmst=graph.unrootedKMST(sqrt_q);
            // take account of (cheapest) edge connecting any source to sink
            double mincost=numeric_limits<double>::infinity();
            for (const auto& i : kmst.nodes())
                for (const auto& j : kmst.nodes())
                    if (I->cost(reqs[i].s, reqs[j].t)<mincost)
                        mincost=I->cost(reqs[i].s, reqs[j].t);
            kmst.cost+=mincost;
            kmst.cost/=kmst.nodes().size();
            //cout<<"k-mst cost: "<<kmst.cost<<endl;
            if (kmst.cost<bestgrp.first) {
                bestgrp={kmst.cost, {}};
                for (const auto& e : kmst.edges) {
                    bestgrp.second.insert(reqs[e.first]);
                    bestgrp.second.insert(reqs[e.second]);
                }
            }
        }
    }
    return bestgrp.second;
}

