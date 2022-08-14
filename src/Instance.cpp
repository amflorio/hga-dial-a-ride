#include <iostream>
#include <fstream>
#include <random>
#include "Arc.h"
#include "Dijkstra.h"
#include "Instance.h"

using namespace std;

double Instance::dist(size_t i, size_t j) const {
    return Dijkstra::cost(i, j);
}

void Instance::loadCoordinates(const string& xy_file) {
    ifstream fcoords(xy_file);
    assert(fcoords.is_open());
    fcoords>>N;
    xcoords=vector<double>(N, 0);
    ycoords=vector<double>(N, 0);
    size_t p;
    for (size_t i=0; i<N; ++i) {
        fcoords>>p>>xcoords.at(i)>>ycoords.at(i);
        assert(p==i);
    }
    assert(N==xcoords.size()&&N==ycoords.size());
}

void Instance::loadDrivers(const string& drv_file, size_t ndrv) {
    cout<<"loading drivers ... "<<flush;
    ifstream drvfile(drv_file);
    assert(drvfile.good());
    size_t id, s;
    for (size_t i=0; i<ndrv; ++i) {
        drvfile>>id>>s;
        drivs.emplace_back(id, s);
    }
    cout<<ndrv<<" drivers loaded"<<endl;
}

void Instance::loadNetwork(const string& xy_file, const string& d_file) {
    // first: read node coordinates from .xy file
    loadCoordinates(xy_file);
    // second: read arcs and distances from .d file
    ifstream fdists(d_file);
    assert(fdists.is_open());
    arcsFromN=vector<vector<Arc>>(N, vector<Arc>());
    arcsToN=vector<vector<Arc>>(N, vector<Arc>());
    fdists>>A;
    for (size_t l=0; l<A; ++l) {
        size_t s, t;
        double d;
        fdists>>s>>t>>d;
        arcs.emplace_back(l, s, t, d);
        arcsFromN.at(arcs.back().s).push_back(arcs.back());
        arcsToN.at(arcs.back().t).push_back(arcs.back());
    }
    // consistency check: no orphan nodes
    for (size_t i=0; i<N; ++i)
        assert(arcsFromN[i].size()!=0||arcsToN[i].size()!=0);
    cout<<"network loaded: "<<N<<" nodes and "<<A<<" arcs"<<endl;
}

void Instance::loadRequests(const string& req_file, size_t nreqs) {
    cout<<"loading requests ... "<<flush;
    ifstream reqfile(req_file);
    assert(reqfile.good());
    size_t id, s, t;
    for (size_t i=0; i<nreqs; ++i) {
        reqfile>>id>>s>>t;
        reqs.emplace_back(id, s, t);
    }
    cout<<nreqs<<" requests loaded"<<endl;
}

void Instance::loadSyntheticDataGaussian(size_t nreqs, size_t ndrv, size_t K,
        double stddev) {
    synthetic=true;
    std::mt19937 rd(nreqs+ndrv+K+stddev);
    // generate Gaussian means
    uniform_real_distribution<double> dist(0, 1000);
    vector<pair<double, double>> G_means;
    for (size_t k=0; k<K; ++k)
        G_means.push_back({dist(rd), dist(rd)});
    uniform_int_distribution<> k_dist(0, K-1);
    auto sample_2D_Gaussian=[&](const pair<double, double>& mean){
        normal_distribution<> x_dist(mean.first, stddev);
        normal_distribution<> y_dist(mean.second, stddev);
        return pair<double, double>(x_dist(rd), y_dist(rd));
    };
    while (reqs.size()!=nreqs) {
        // sample origin node from the mixed-Gaussian model
        auto s_coords=sample_2D_Gaussian(G_means[k_dist(rd)]);
        size_t s=xcoords.size();
        xcoords.push_back(s_coords.first);
        ycoords.push_back(s_coords.second);
        // sample destination node from the mixed-Gaussian model
        auto t_coords=sample_2D_Gaussian(G_means[k_dist(rd)]);
        size_t t=xcoords.size();
        xcoords.push_back(t_coords.first);
        ycoords.push_back(t_coords.second);
        // save request
        reqs.emplace_back(reqs.size(), s, t);
    }
    while (drivs.size()!=ndrv) {
        // sample initial location from the mixed-Gaussian model
        auto s_coords=sample_2D_Gaussian(G_means[k_dist(rd)]);
        size_t s=xcoords.size();
        xcoords.push_back(s_coords.first);
        ycoords.push_back(s_coords.second);
        // save driver
        drivs.emplace_back(drivs.size(), s);
    }
    N=xcoords.size();
}

void Instance::loadSyntheticDataUniform(size_t nreqs, size_t ndrv) {
    synthetic=true;
    std::mt19937 rd(nreqs+ndrv);
    // generate requests
    uniform_int_distribution<> int_dist(0, N-1);
    while (reqs.size()!=nreqs) {
        const size_t s=int_dist(rd);
        const size_t t=int_dist(rd);
        if (s!=t)
            reqs.emplace_back(reqs.size(), s, t);
    }
    // generate drivers
    while (drivs.size()!=ndrv)
        drivs.emplace_back(drivs.size(), int_dist(rd));
    cout<<"synthetic network loaded: "<<N<<" nodes"<<endl;
}

