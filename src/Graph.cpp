#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include "blossom/PerfectMatching.h"
#include "Config.h"
#include "DisjointSet.h"
#include "Graph.h"

using namespace std;

vector<pair<size_t, size_t>> Graph::bucketMatching() const {
    auto my_log=[](float x){
        return log(x)/log(1.05);    // use small base to yield more buckets
    };
    auto get_bucket=[&my_log](float c){
        assert(Config::EXACT_W1 || c>=0);
        return (int)my_log(1+max(c, 0.0f));
    };
    vector<pair<size_t, size_t>> matching;
    vector<size_t> matched(N, 0);
    int rem=N;
    cout<<"bucket-matching: remaining to match: "<<rem<<endl;
    vector<vector<pair<int, int>>> buckets;
    cout<<"bucket-matching: computing buckets ..."<<endl;
    for (size_t i=0; i<N; ++i)
        for (size_t j=i+1; j<N; ++j) {
            auto b=get_bucket(cost(i, j)/100);
            while (buckets.size()<b+1)
                buckets.push_back({});
            buckets[b].push_back({i, j});
        }
    cout<<"bucket-matching: finished computing buckets"<<endl;
    for (size_t b=0; rem!=0; ++b) {
        assert(b<buckets.size());
        auto& bucket=buckets[b];
        cout<<"buckets["<<b<<"].size(): "<<buckets[b].size()<<endl;
        for (const auto& edge : bucket)
            if (!matched[edge.first] && !matched[edge.second]) {
                matching.push_back(edge);
                matched[edge.first]=1;
                matched[edge.second]=1;
                rem-=2;
            }
        bucket.clear();
        cout<<"after bucket "<<b<<": remaining to match: "<<rem<<endl;
    }
    return matching;
}

Tree Graph::connectTrees(const vector<Tree>& trees) const {
    // create a graph with the contracted trees
    Graph g(trees.size());
    // keep a mapping to identify cheapest connecting edges between trees
    map<pair<size_t, size_t>, pair<size_t, size_t>> mymap;
    for (size_t i=0; i<trees.size(); ++i) {
        for (size_t j=i+1; j<trees.size(); ++j) {
            tuple<size_t, size_t, float> mincost
                    {0, 0, numeric_limits<float>::infinity()};
            for (auto node_i : trees[i].nodes())
                for (auto node_j : trees[j].nodes())
                    if (edgeCost(node_i, node_j)<get<2>(mincost))
                        mincost={node_i, node_j, edgeCost(node_i, node_j)};
            g.setEdgeCost(i, j, get<2>(mincost));
            mymap.insert({{i, j}, {get<0>(mincost), get<1>(mincost)}});
        }
    }
    // find the MST connecting all the trees
    Tree msttrees=g.MST();
    // create and return the final, connected tree
    Tree ans {0, {}};
    // insert into the final tree all input trees
    for (const auto& t : trees) {
        ans.cost+=t.cost;
        ans.edges.insert(ans.edges.end(), t.edges.begin(), t.edges.end());
    }
    // now add/include the cost/edges of the connecting tree
    ans.cost+=msttrees.cost;
    for (const auto& e : msttrees.edges)
        ans.edges.push_back(mymap.at(e));
    return ans;
}

Tree Graph::mergeCluster(const unordered_set<size_t>& active, size_t k) const {
    assert(!active.empty());
    assert(k>=5);
    Tree t {0, {}};
    DisjointSet dset(N);        // we could use much less memory here
    while (true) {
        // merge two components, Kruskal's style
        tuple<float,size_t,size_t> mincost
                {numeric_limits<float>::infinity(), 0, 0};
        for (const auto i : active)
            for (const auto j : active)
                if (dset.findSet(i)!=dset.findSet(j)) {
                    float c=edgeCost(i,j)/min(dset.sizeSet(i),dset.sizeSet(j));
                    if (c<get<0>(mincost))
                        mincost={c, i, j};      // TODO: make it i, j, c
                }
        t.cost+=get<0>(mincost);
        t.edges.emplace_back(get<1>(mincost), get<2>(mincost));
        dset.unionSet(get<1>(mincost), get<2>(mincost));
        if (dset.sizeSet(get<1>(mincost))>=k/4.0) {     // are we done?
            const auto set_id=dset.findSet(get<1>(mincost));
            // t may be a forest at this point, but we need just one tree
            // first, we update the tree/forest cost
            for (const auto& e : t.edges) {
                assert(dset.findSet(e.first)==dset.findSet(e.second));
                if (dset.findSet(e.first)!=set_id)  // not in our target tree?
                    t.cost-=edgeCost(e.first, e.second);
            }
            // next, we remove the edges that do not belong to our target tree
            t.edges.erase(remove_if(t.edges.begin(), t.edges.end(),
                    [&](const pair<size_t, size_t>& e)
                    {return dset.findSet(e.first)!=set_id;}), t.edges.end());
            assert(t.edges.size()>=1);
            return t;
        }
    }
}

vector<pair<size_t, size_t>> Graph::minCostMatching() {
    if (N<=10000 || Config::EXACT_MATCHING) {
        PerfectMatching pm(N, (N*N-N)/2);   // graph is complete and undirected
        for (size_t i=0; i<N; ++i)
            for (size_t j=i+1; j<N; ++j)
                pm.AddEdge(i, j, cost(i, j));
        pm.Solve();
        vector<pair<size_t, size_t>> mat;
        for (size_t i=0; i<N; ++i) {
            size_t mat_i=pm.GetMatch(i);
            if (i<mat_i)
                mat.emplace_back(i, mat_i);
        }
        return mat;
    } else
        return bucketMatching();
}

float Graph::MSTCost() const {      // Kruskal's, return only cost
    vector<pair<size_t, size_t>> edges;
    for (size_t i=0; i<N; ++i)
        for (size_t j=i+1; j<N; ++j)
            edges.emplace_back(i, j);
    sort(edges.begin(), edges.end(),
            [&](const pair<size_t, size_t>& e1, const pair<size_t, size_t>& e2)
            {return cost(e1.first, e1.second)<cost(e2.first, e2.second);});
    DisjointSet dset(N);
    float cst=0;
    for (size_t i=0, added=0; added<N-1; ++i)
        if (dset.findSet(edges[i].first)!=dset.findSet(edges[i].second)) {
            cst+=cost(edges[i].first, edges[i].second);
            dset.unionSet(edges[i].first, edges[i].second);
            added++;
        }
    return cst;
}

Tree Graph::MST() const {       // Kruskal's, return solution tree
    vector<pair<size_t, size_t>> edges;
    for (size_t i=0; i<N; ++i)
        for (size_t j=i+1; j<N; ++j)
            edges.emplace_back(i, j);
    sort(edges.begin(), edges.end(),
            [&](const pair<size_t, size_t>& e1, const pair<size_t, size_t>& e2)
            {return cost(e1.first, e1.second)<cost(e2.first, e2.second);});
    DisjointSet dset(N);
    Tree sol {0, {}};
    for (size_t i=0, added=0; added<N-1; ++i)
        if (dset.findSet(edges[i].first)!=dset.findSet(edges[i].second)) {
            sol.cost+=cost(edges[i].first, edges[i].second);
            sol.edges.emplace_back(edges[i].first, edges[i].second);
            dset.unionSet(edges[i].first, edges[i].second);
            added++;
        }
    return sol;
}

Tree Graph::unrootedKMST(const size_t k) const {
    assert(k>=2);
    if (k>N) {
        //cout<<"unrootedKMST: k="<<k<<" ;  N="<<N<<"  (infeasible)"<<endl;
        return {numeric_limits<float>::infinity(), {}};
    }
    if (k==N) {
        //cout<<"unrootedKMST: k="<<k<<" ;  N="<<N<<"  =>  MST"<<endl;
        return MST();
    }
    Tree bestmst {numeric_limits<float>::infinity(), {}};
    for (size_t root=0; root<N; ++root) {
        vector<float> root_dists;
        for (size_t i=0; i<N; ++i)
            root_dists.push_back(i!=root?edgeCost(i, root):0);
        nth_element(root_dists.begin(),root_dists.begin()+k-1,root_dists.end());
        const float theta=max(root_dists[k-1], 1e-5f);
        //cout<<"root="<<root<<": distance to "<<k<<"th nearest: "<<theta<<endl;
        for (float l=theta; l<=k*theta; l*=2) {
            //cout<<"guessing root="<<root<<" ;  l="<<l<<endl;
            unordered_set<size_t> active;
            for (size_t i=0; i<N; ++i)
                if ((i!=root?edgeCost(root, i):0)<=l)
                    active.insert(i);
            size_t k_rem=k;
            vector<Tree> trees;
            while (k_rem>=5) {
                auto tree=mergeCluster(active, k_rem);
                assert(tree.edges.size()+1<=k_rem);
                for (const auto& e : tree.edges) {
                    active.erase(e.first);
                    active.erase(e.second);
                }
                k_rem-=tree.edges.size()+1;
                trees.push_back(move(tree));
            }
            for (const auto i : active)
                trees.push_back(Tree(0, {{i, i}}));     // single-node trees
            Tree t=connectTrees(trees);
            if (t.cost<bestmst.cost)
                bestmst=t;
        }
    }
    return bestmst;
}

