#include <algorithm>
#include <queue>
#include <vector>
#include "Arc.h"
#include "Dijkstra.h"
#include "Instance.h"

using namespace std;

shared_ptr<Instance> Dijkstra::I;
vector<vector<double>> Dijkstra::costs;

double Dijkstra::cost(int s, int t) {
    if (costs[s][t]==-1)
        solve(s);
    assert(costs[s][t]!=-1);
    return costs[s][t];
}

void Dijkstra::init() {
    assert(I!=nullptr);
    costs=vector<vector<double>>(I->nodes(), vector<double>(I->nodes(), -1));
}

void Dijkstra::solve(int s) {
    vector<int> pred(I->nodes(), -1);
    vector<double> cost(I->nodes(), numeric_limits<double>::infinity());
    typedef pair<double, int> validxp;
    typedef priority_queue<validxp, vector<validxp>, greater<validxp>> mypq;
    mypq pq;
    pq.push({0, s});
    cost[s]=0;
    while (!pq.empty()) {
        auto next=pq.top();
        pq.pop();
        if (next.second!=s && cost[next.second]==0)
            continue;
        costs[s][next.second]=next.first;
        for (const auto& l : I->arcsFrom(next.second))
            if (next.first+l.d<cost[l.t]) {
                cost[l.t]=next.first+l.d;
                pred[l.t]=l.idx;
                pq.push({cost[l.t], l.t});
            }
        cost[next.second]=0;        // to indicate that it's visited
    }
}

