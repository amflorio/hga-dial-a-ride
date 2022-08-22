#include <cmath>
#include <iomanip>
#include <iostream>
#include "Config.h"
#include "Instance.h"
#include "Graph.h"
#include "Partition.h"

using namespace std;

shared_ptr<Instance> Partition::I;

double Partition::MST_s(const Group& g) {
    size_t N=g.size();
    vector<size_t> v_s(N);
    size_t i=0;
    for (const auto& r : g) {
        v_s[i]=r.s;
        i++;
    }
    Graph graph(N);
    for (size_t i=0; i<N; ++i)
        for (size_t j=i+1; j<N; ++j)
            graph.setEdgeCost(i, j, I->cost(v_s[i], v_s[j]));
    return graph.MSTCost();
}

double Partition::MST_t(const Group& g) {
    size_t N=g.size();
    vector<size_t> v_t(N);
    size_t i=0;
    for (const auto& r : g) {
        v_t[i]=r.t;
        i++;
    }
    Graph graph(N);
    for (size_t i=0; i<N; ++i)
        for (size_t j=i+1; j<N; ++j)
            graph.setEdgeCost(i, j, I->cost(v_t[i], v_t[j]));
    return graph.MSTCost();
}

vector<Group> Partition::partition() {
    cout<<"partition algorithm: # of requests: "<<I->requests().size()<<endl;
    collection_t M;
    for (const auto& r : I->requests())
        M.push_back({{r}});
    //printCollection(M);
    for (size_t iter=0; ++iter<=log2(I->capacity());) {
        cout<<"iteration: "<<iter<<" ;  M.size(): "<<M.size()<<endl;
        if (M.size()%2==1)
            M.push_back(cluster_t());       // virtual
        Graph G(M.size());
        for (size_t i=0; i<M.size(); ++i)
            for (size_t j=i+1; j<M.size(); ++j)
                G.setEdgeCost(i, j, w_val(M[i], M[j]));
        cout<<"finding minimum cost matching ..."<<endl;
        auto mcm=G.minCostMatching();
        collection_t newM;
        for (const auto& match : mcm) {
            const auto& a=match.first;
            const auto& b=match.second;
            auto w_ab=w(M[a], M[b]);
            /*
            cout<<a<<" -- "<<b<<"  type: "<<(w_ab.w==
                    wresult_t::which_w::w1?"w1":w_ab.w==
                    wresult_t::which_w::w2?"w2":"virtual")<<"  op1: "
                    <<w_ab.g1<<"  op2: "<<w_ab.g2<<endl;
            */
            if (w_ab.w==wresult_t::which_w::w1) {
                newM.push_back({w_ab.g1});
                for (const auto& r : w_ab.g2)
                    newM.back()[0].insert(r);
                for (const auto& g : M[a])
                    if (!g.count(*w_ab.g1.begin()))
                        newM.back().push_back(g);
                for (const auto& g : M[b])
                    if (!g.count(*w_ab.g2.begin()))
                        newM.back().push_back(g);
            } else if (w_ab.w==wresult_t::which_w::w2) {
                newM.push_back(M[a]);
                newM.back().insert(newM.back().end(), M[b].begin(), M[b].end());
            } else {        // matched with virtual cluster
                newM.push_back(M[a]);       // M[b] is the virtual cluster
            }
        }
        swap(M, newM);
        //printCollection(M);
    }
    vector<Group> grps;
    unordered_set<Request, boost::hash<Request>> check;
    for (const auto& c : M)
        for (const auto& g : c) {
            grps.push_back(g);
            for (const auto& r : g)
                check.insert(r);
        }
    cout<<"(check) number of requests: "<<check.size()<<endl;
    assert(check.size()==I->requests().size());
    return grps;
}

void Partition::printCollection(const collection_t& c) {
    cout<<"{"<<endl;
    for (size_t i=0; i<c.size(); ++i) {
        cout<<setw(2)<<i<<":  [";
        for (const auto& group : c[i])
            cout<<group;
        cout<<"]"<<endl;
    }
    cout<<"}"<<endl;
}

Partition::wresult_t Partition::w(const cluster_t& c1, const cluster_t& c2) {
    assert(!c1.empty());
    if (c2.empty())     // dummy?
        return {0, wresult_t::which_w::none};
    wresult_t minimum={numeric_limits<double>::infinity(),
            wresult_t::which_w::none};
    for (const auto& g1 : c1) {
        assert(!g1.empty());
        for (const auto& g2 : c2) {
            assert(!g2.empty());
            double val_w1=w1(g1, g2);
            double val_w2=w2(g1, g2);
            if (val_w1<minimum.val && val_w1<val_w2) {
                minimum={val_w1, wresult_t::which_w::w1, g1, g2};
            } else if (val_w2<minimum.val && val_w2<=val_w1) {
                minimum={val_w2, wresult_t::which_w::w2, g1, g2};
            }
        }
    }
    return minimum;
}

double Partition::w_val(const cluster_t& c1, const cluster_t& c2) {
    assert(!c1.empty());
    if (c2.empty())     // dummy?
        return 0;
    double min=numeric_limits<double>::infinity();
    for (const auto& g1 : c1) {
        assert(!g1.empty());
        for (const auto& g2 : c2) {
            assert(!g2.empty());
            double val_w1=w1(g1, g2);
            double val_w2=w2(g1, g2);
            if (val_w1<min && val_w1<val_w2)
                min=val_w1;
            else if (val_w2<min && val_w2<=val_w1)
                min=val_w2;
        }
    }
    return min;
}

double Partition::w1(const Group& g1, const Group& g2) {
    if (Config::EXACT_W1) {
        Group un=g1;
        for (const auto& r : g2)
            un.insert(r);
        return MST_st(un)-MST_st(g1)-MST_st(g2);
    } else {
        double min_s=numeric_limits<double>::infinity();
        double min_t=numeric_limits<double>::infinity();
        for (const auto& r1 : g1)
            for (const auto& r2 : g2) {
                if (I->cost(r1.s, r2.s)<min_s)
                    min_s=I->cost(r1.s, r2.s);
                if (I->cost(r1.t, r2.t)<min_t)
                    min_t=I->cost(r1.t, r2.t);
            }
        return min_s+min_t;
    }
}

double Partition::w2(const Group& g1, const Group& g2) {
    double min_g1=numeric_limits<double>::infinity();
    for (const auto& r : g1)
        if (I->cost(r.s, r.t)<min_g1)
            min_g1=I->cost(r.s, r.t);
    double min_g2=numeric_limits<double>::infinity();
    for (const auto& r : g2)
        if (I->cost(r.s, r.t)<min_g2)
            min_g2=I->cost(r.s, r.t);
    return min_g1+min_g2;
}

