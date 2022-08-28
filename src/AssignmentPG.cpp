#include <iostream>
#include "AssignmentPG.h"
#include "DisjointSet.h"
#include "Graph.h"
#include "Instance.h"
#include "Partition.h"
#include "PruneGreedy.h"
#include "Tree.h"
#include "TSPHeuristic.h"

using namespace std;

shared_ptr<Instance> AssignmentPG::I;

// TODO: major refactoring
vector<Route> AssignmentPG::assignment(const vector<Group>& groups) {
    const auto D=I->drivers().size();
    const auto G=groups.size();
    cout<<"assignment PG algo: "<<D<<" drivers; "<<G<<" request groups"<<endl;
    cout<<"building auxiliary graph with "<<G+1<<" nodes ..."<<endl;
    Graph g(G+1);       // node 0 is D_0, nodes 1...G are request groups
    // setup the cost of edges connecting request groups
    for (size_t i=0; i<G; ++i) {
        for (size_t j=i+1; j<G; ++j) {
            double mincost=numeric_limits<double>::infinity();
            for (const auto& req_i : groups[i])
                for (const auto& req_j : groups[j])
                    if (I->cost(req_i.s, req_j.s)<mincost)
                        mincost=I->cost(req_i.s, req_j.s);
            g.setEdgeCost(i+1, j+1, mincost);
        }
    }
    // setup the cost of edges connecting D_0 (node 0) to request groups
    vector<size_t> driver(G, 0);        // closest driver to each request group
    for (size_t i=0; i<G; ++i) {
        double mincost=numeric_limits<double>::infinity();
        for (size_t k=0; k<I->drivers().size(); ++k) {
            const auto& d=I->drivers()[k];
            for (const auto& req : groups[i])
                if (I->cost(d.s, req.s)<mincost) {
                    mincost=I->cost(d.s, req.s);
                    driver[i]=k;
                }
        }
        g.setEdgeCost(0, i+1, mincost);
    }
    // find assignment of request groups to drivers
    auto MSTsol=g.MST();
    DisjointSet dset(D+G);  // nodes 0...D-1 drivers, D...D+G-1 request groups
    for (const auto& edge : MSTsol.edges) {
        assert(edge.first<edge.second);
        const auto set1=edge.first==0?driver[edge.second-1]:edge.first-1+D;
        const auto set2=edge.second-1+D;
        assert(set1>=D || set2>=D);     // one has to be a request group
        dset.unionSet(set1, set2);
    }
    // consistency check: drivers cannot belong to the same set
    for (size_t k1=0; k1<D; ++k1)
        for (size_t k2=k1+1; k2<D; ++k2)
            assert(dset.findSet(k1)!=dset.findSet(k2));
    // solve TSP for each driver assigned to at least one request group
    vector<Route> routes;
    set<size_t> g_set;              // for consistency check
    for (size_t i=0; i<G; ++i)
        g_set.insert(i);
    for (size_t k=0; k<D; ++k) {
        vector<size_t> groups_k;    // group indices associated with driver k
        for (size_t i=0; i<G; ++i)
            if (dset.findSet(k)==dset.findSet(D+i)) {
                assert(g_set.count(i)==1);
                groups_k.push_back(i);
                g_set.erase(i);
            }
        cout<<groups_k.size()<<" request group(s) assigned to driver "<<k<<endl;
        if (!groups_k.empty()) {
            // create cost matrix for the TSP over driver k and groups_k
            auto costs=vector<vector<double>>(1+groups_k.size(),
                    vector<double>(1+groups_k.size(), 0));
            for (size_t i=0; i<costs.size(); ++i)
                for (size_t j=i+1; j<costs.size(); ++j) {
                    costs[i][j]=i==0?g.edgeCost(0, groups_k.at(j-1)+1)
                            :g.edgeCost(groups_k.at(i-1)+1, groups_k.at(j-1)+1);
                    costs[j][i]=costs[i][j];
                }
            TSPHeuristic tsp(costs);
            auto tspsol=tsp.randomInsertion();
            cout<<"TSP cost: "<<tspsol.cost()<<endl;
            assert(tspsol.tour().size()==costs.size());
            auto tour=tspsol.tour();
            // rotate so that it starts with 0 (the driver)
            {
                size_t i=0;
                for (; tour[i]!=0; ++i)
                    tour.push_back(tour[i]);
                tour.erase(tour.begin(), tour.begin()+i);
            }
            assert(tour.size()==costs.size() && tour[0]==0);
            // dereference indices along tour
            for (size_t i=1; i<tour.size(); ++i)
                tour.at(i)=groups_k.at(i-1);
            cout<<"driver "<<k<<":"<<endl;
            cout<<"[ ";
            for (auto i : tour)
                cout<<" "<<i<<" ";
            cout<<" ]"<<endl;
            Route rt(I->drivers()[k].s);
            for (size_t i=1; i<tour.size(); ++i) {
                // create dummy driver from rt's last node
                Driver d(0, rt.stops().back().node);
                auto reqs=vector<Request>(groups.at(tour[i]).begin(),
                        groups[tour[i]].end());
                auto routes=PruneGreedy::solve({d}, reqs);
                assert(routes.size()==1);
                for (size_t j=1; j<routes[0].stops().size(); ++j)
                    rt.insert(routes[0].stops()[j]);
            }
            // check that rt is serving the required number of requests
            size_t nreqs=0;
            for (size_t i=1; i<tour.size(); ++i)
                nreqs+=groups[tour[i]].size();
            assert((rt.size()-1)/2==nreqs);
            cout<<"driver "<<k<<": "<<nreqs<<" request(s)"<<endl;
            routes.push_back(move(rt));
        }
    }
    assert(g_set.empty());
    return routes;
}

