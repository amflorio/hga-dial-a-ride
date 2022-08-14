/**
	\Author: Trasier
	\Date:	2019/05/21
**/

#ifndef fesi_h
#define fesi_h

#include <memory>
#include <string>
#include <vector>
#include "Instance.h"
#include "Route.h"

namespace fesi {

std::vector<Route> FESI();
void trainHST(std::string des);
void loadInstance();
void readHSTFromFile(std::string hstFileName);

typedef long long LL;
typedef std::pair<int,int> pii;
typedef std::pair<double,double> pdd;
typedef std::pair<int,double> pid;
typedef std::pair<double,int> pdi;

typedef struct {
    int oid, did; 	// origin and destination
    double wei; 		// weight
    size_t req_id;      // Florio: add one extra field to identify request
} request_t;

typedef struct {
    int oid;		// initial location
    double cap;		// capacity
    std::vector<int> S;
} worker_t;

extern std::shared_ptr<Instance> I;     // Florio: instance information
extern int nV;
extern int H;
extern int *pi;
extern int *reverse_pi;
extern int **far;
extern double dmax;
extern double gama;
extern double *exp2s;
extern double *sum2s;
extern int* pi_bk;
extern double gamma_bk;
extern int nR;
extern int nW;
extern request_t* requests;
extern worker_t* workers;
extern const double speed;
extern const double EPS;
extern const double INF;
extern double usedTime;
extern int usedMemory;

void initLocation(std::string &fileName);
void initMemory(int n);
void freeMemory();
void printHST(int H);
void constructHST(bool load=false);
void constructHST_fast(bool load);
void constructHST_slow(bool load);
void randomization();
void calcDmax();
double distOnHST(int u, int v);
void readHST(std::string &fileName, bool flag=true);
void dumpHST(std::string &fileName, bool flag=true);
void loadHST();
void backupHST();
std::pair<int,int> getLCA(int u, int v);
int levelOfLCA(int u, int v);
double distAtLevel(int level);
double calcScore(request_t* R, int nR);
int dcmp(double x);
double dist(int x, int y);
void freeInput();

struct path_t {
    int rid;
    std::vector<int> vec;
    void initVec(int n) {
        vec.resize(n);
    }
    int size() {
        return vec.size();
    }
    void clear() {
        vec.clear();
    }
    bool empty() {
        return vec.empty();
    }
    bool operator< (const path_t& b) const {
        const std::vector<int>& avec = this->vec;
        const std::vector<int>& bvec = b.vec;
        int i = 0, j = 0;
        const int size = avec.size(), bsize = bvec.size();
        while(i<size && j<bsize) {
            if(avec[i] == bvec[j]) {
                ++i, ++j;
            } else {
                return avec[i] < bvec[j];
            }
        }
        if(i==size || j==bsize)
            return size < bsize;
        return rid < b.rid;
    }
};

struct node_t {
    int cnt, flow, dep;
    std::vector<int> req;
    int pop_req(int x) {
        int k = 0, n = req.size();
        for(int i=0; i<n; ++i) {
            if(req[i] != x)
                req[k++] = req[i];
        }
        if(k < n) {
            req.pop_back();
            return 1;
        } else {
            return 0;
        }
    }
    int pop_req(const std::vector<int>& xs) {
        if(xs.empty()) return 0;
        int k = 0, i = 0, j = 0;
        int n = req.size(), m = xs.size();
        while(i<n && j<m) {
            if(req[i] == xs[j]) {
                ++i, ++j;
            } else if(req[i] < xs[j]) {
                req[k++] = req[i++];
            } else {
                ++j;
            }
        }
        while(i < n)
            req[k++] = req[i++];
        if(k < n) {
            req.erase(req.begin()+k, req.end());
            return n-k;
        } else {
            return 0;
        }
    }
};

// I/O
void readFromFiles(std::string locFileName, std::string hstFileName, std::string inputFileName);
void dumpResult(std::string& fileName);
void dumpResult(std::string& fileName, const char *algo);

// Melloc & Free
void initGlobalMemory(int nV, int nW, int nR);
void freeGlobalMemory();

// Initilization
void initTree();
void initNodes();
void initWorkers();
void initRng();

// The import operations on Metric Space
inline double localDist(int a, int b) {
    return dist(a, b);
}
void insertDist(worker_t& w, int rid, int& oPos, int& dPos, double& inc);
void insert(worker_t& w, int rid, int& oPos, int& dPos);
void remove(worker_t& w, int rid);
double getRouteDist(worker_t& w);
double getRouteDist(int wid);
double getRemoveDist(worker_t& w, int rid);

// The import operations on HST
void sortPath(std::vector<int>& rids);
void genVec(int rid);
void genVec(std::vector<int>& rids);
void genLabel(worker_t& w, int maxH);
void dfsLabel(int rt, int vid, int l, int& cnt, int maxH);
int getMinHeight(int vid, int k);
void updateTree(std::vector<int>& rids);

// Other un-important operations on HST
void clearPath(std::vector<int>& rids);
void clearPath(int rid);
int getPathHeight(int u, int v);
int getPathLength(int u, int v);
int getMaxPathLength(std::vector<int>& rids);
int calcTreeSize();

}

#endif

