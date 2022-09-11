#include <algorithm>
#include <iostream>
#include <numeric>
#include "Arc.h"
#include "AssignmentPG.h"
#include "Config.h"
#include "Dijkstra.h"
#include "FESI.h"
#include "Instance.h"
#include "Partition.h"
#include "PartitionLambdaForest.h"
#include "PruneGreedy.h"
#include "Route.h"
#include "Stopwatch.h"

using namespace std;

void checkFeasible(shared_ptr<Instance> I, const vector<Route>& routes);
void csvOutput(shared_ptr<Instance> I, const string& algo,
        const vector<Route>& routes, double time_s);
void csvOutput(shared_ptr<Instance> I, const string& algo,
        const vector<vector<Route>>& v_routes, double time_s);
double makespan(const vector<Route>& routes);
void mycontinue();
void preprocessSPs(shared_ptr<Instance> I);
void shareInstance(shared_ptr<Instance> I);
double totalLatency(const vector<Route>& routes);

void checkFeasible(shared_ptr<Instance> I, const vector<Route>& routes) {
    set<size_t> all_reqs;
    for (const auto& req : I->requests())
        all_reqs.insert(req.id);
    for (const auto& rt : routes) {
        assert(rt.feasible());
        auto rt_reqs=rt.requests();
        for (auto id : rt_reqs) {
            assert(all_reqs.count(id)==1);
            all_reqs.erase(id);
        }
    }
    size_t nreqs=0;
    for (const auto& rt : routes) {
        assert(rt.size()%2==1);
        nreqs+=(rt.size()-1)/2;
    }
    if (nreqs!=I->requests().size()) {
        cout<<"nreqs: "<<nreqs<<endl;
        cout<<"I->requests().size(): "<<I->requests().size()<<endl;
    }
    assert(nreqs==I->requests().size());
    assert(all_reqs.empty());
}

void csvOutput(shared_ptr<Instance> I, const string& algo,
        const vector<Route>& routes, double time_s) {
    double c=accumulate(routes.begin(), routes.end(), 0.0,
            [](double a, const Route& rt){return a+rt.cost();});
    cout<<algo<<","<<I->code()<<","<<I->requests().size()<<","
        <<I->drivers().size()<<","<<I->capacity()<<","<<c<<","<<routes.size()
        <<","<<makespan(routes)<<","<<totalLatency(routes)<<","<<time_s<<endl;
}

void csvOutput(shared_ptr<Instance> I, const string& algo,
        const vector<vector<Route>>& v_routes, double time_s) {
    // cost, # routes, makespan, total latency
    tuple<double, size_t, double, double> acc_res {0, 0, 0, 0};
    for (const auto& routes : v_routes) {
        get<0>(acc_res)+=accumulate(routes.begin(), routes.end(), 0.0,
                [](double a, const Route& rt){return a+rt.cost();});
        get<1>(acc_res)+=routes.size();
        get<2>(acc_res)+=makespan(routes);
        get<3>(acc_res)+=totalLatency(routes);
    }
    const size_t N=v_routes.size();
    cout<<algo<<","<<I->code()<<","<<I->requests().size()<<","
        <<I->drivers().size()<<","<<I->capacity()<<","<<get<0>(acc_res)/N<<","
        <<get<1>(acc_res)/N<<","<<get<2>(acc_res)/N<<","<<get<3>(acc_res)/N<<","
        <<time_s/N<<endl;
}

int main(int argc, char* argv[]) {
    if (argc<5) {
        cerr<<"usage: "<<argv[0]<<" <network> <# reqs> <# drivers> <Lambda>"
                <<endl;
        cerr<<"where <network> is:"<<endl;
        cerr<<"\t'ny' for New York"<<endl;
        cerr<<"\t'sf' for San Francisco"<<endl;
        cerr<<"\t'sy-u' for synthetic data, uniformly distributed"<<endl;
        cerr<<"\t'sy-g' for synthetic data, mixed Gaussian model"<<endl;
        return 0;
    }
    auto I=make_shared<Instance>();
    shareInstance(I);
    Config::PrintConfigParameters();
    if (argv[1]==string("ny")) {
        I->setCode("NY");
        I->loadNetwork("../in/coords/Manhattan.xy", "../in/dists/Manhattan.d");
        I->loadRequests("../in/reqs/Manhattan.1.reqs", stoi(argv[2]));
        I->loadDrivers("../in/drivers/Manhattan.drv", stoi(argv[3]));
        preprocessSPs(I);
    } else if (argv[1]==string("sf")) {
        I->setCode("SF");
        I->loadNetwork("../in/coords/SanFrancisco.xy",
                "../in/dists/SanFrancisco.d");
        I->loadRequests("../in/reqs/SanFrancisco.1.reqs", stoi(argv[2]));
        I->loadDrivers("../in/drivers/SanFrancisco.drv", stoi(argv[3]));
        preprocessSPs(I);
    } else if (argv[1]==string("sy-u")) {
        I->setCode("SY-U");
        I->loadCoordinates("../in/coords/Synthetic-U.xy");
        I->loadSyntheticDataUniform(stoi(argv[2]), stoi(argv[3]));
    } else if (argv[1]==string("sy-g")) {
        if (argc!=7) {
            cerr<<"usage: "<<argv[0]<<" sy-g <# reqs> <# drivers> <Lambda> "
                    "<# clusters> <stddev>"<<endl;
            return 0;
        }
        I->setCode(string("SY-G-")+argv[5]+"-"+argv[6]);
        I->loadSyntheticDataGaussian(stoi(argv[2]), stoi(argv[3]),
                stoi(argv[5]), stof(argv[6]));
    } else {
        cerr<<"invalid network"<<endl;
        return 0;
    }
    I->setCapacity(stoi(argv[4]));
    // proposed partition algorithm
    Stopwatch sw;
    auto groups=Partition::partition();
    auto routes=AssignmentPG::assignment(groups);
    checkFeasible(I, routes);
    cout<<"kelinPG: "<<routes.size()<<" route(s)"<<endl;
    csvOutput(I, "HGA", routes, sw.elapsedSeconds());
    /* commenting-out partition from Lambda-forest as it's very inneficient
    // partition from Lambda-forest algorithm
    sw=Stopwatch();
    groups=PartitionLambdaForest::partition();
    routes=AssignmentPG::assignment(groups);
    checkFeasible(I, routes);
    cout<<"k-forest: "<<routes.size()<<" route(s)"<<endl;
    csvOutput(I, "k-forest", routes, sw.elapsedSeconds());
    */
    // prune-greedy DP
    sw=Stopwatch();
    routes=PruneGreedy::solve(I->drivers(), I->requests());
    checkFeasible(I, routes);
    cout<<"prune-greedy: "<<routes.size()<<" route(s)"<<endl;
    csvOutput(I, "pruneGDP", routes, sw.elapsedSeconds());
    // FESI
    fesi::loadInstance();
    cout<<"FESI: generating HSTs ... "<<flush;
    fesi::trainHST("../HSTs");
    cout<<"done!"<<endl;
    const size_t runs=10;
    vector<vector<Route>> v_routes;
    sw=Stopwatch();
    for (size_t i=0; i<runs; ++i) {
        auto hstfile="../HSTs/hst_"+I->code()+"_"
                +to_string(I->requests().size())+"_"
                +to_string(I->drivers().size())+"_"+to_string(I->capacity())+"_"
                +to_string(i)+".txt";
        fesi::loadInstance();
        fesi::readHSTFromFile(hstfile);
        routes=fesi::FESI();
        routes.erase(remove_if(routes.begin(), routes.end(),
                [](const Route& r){return r.stops().size()==1;}), routes.end());
        checkFeasible(I, routes);
        cout<<"FESI: "<<routes.size()<<" route(s)"<<endl;
        v_routes.push_back(move(routes));
    }
    csvOutput(I, "FESI", v_routes, sw.elapsedSeconds());
    return 0;
}

double makespan(const vector<Route>& routes) {
    double ms=0;
    for (const auto& rt : routes) {
        auto c=rt.cost();
        if (c>ms)
            ms=c;
    }
    return ms;
}

void mycontinue() {
    cout<<"press <ENTER> to continue"<<endl;
    cin.get();
}

void preprocessSPs(shared_ptr<Instance> I) {
    cout<<"pre-processing shortest-paths ..."<<endl;
    Dijkstra::init();
    for (size_t i=0; i<I->nodes(); ++i) {
        if (i&&i%2000==0)
            cout<<i<<"/"<<I->nodes()<<" done"<<endl;
        Dijkstra::solve(i);
    }
    cout<<"finished pre-processing shortest-paths"<<endl;
}

void shareInstance(shared_ptr<Instance> I) {
    AssignmentPG::setInstance(I);
    Dijkstra::setInstance(I);
    fesi::I=I;
    Partition::setInstance(I);
    PartitionLambdaForest::setInstance(I);
    PruneGreedy::setInstance(I);
    Route::setInstance(I);
}

double totalLatency(const vector<Route>& routes) {
    double lat=0;
    for (const auto& rt : routes)
        lat+=rt.latency();
    return lat;
}

