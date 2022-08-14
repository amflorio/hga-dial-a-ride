#ifndef instance_h
#define instance_h

#include <cmath>
#include <string>
#include <vector>
#include "Driver.h"
#include "Request.h"

class Arc;
class Instance {
    private:
        std::vector<Request> reqs;                  // set of requests
        std::vector<Driver> drivs;                  // set of drivers
        size_t Q;                                   // vehicle capacity
        size_t N;                                   // # of nodes in the network
        size_t A;                                   // # of arcs in the network
        std::vector<double> xcoords, ycoords;       // node coordinates
        std::vector<Arc> arcs;                      // set of arcs
        std::vector<std::vector<Arc>> arcsFromN;    // arcs from a given vertex
        std::vector<std::vector<Arc>> arcsToN;      // arcs to a given vertex
        bool synthetic=false;                       // synthetic or real world
        std::string cod;                            // instance code
        double dist(size_t i, size_t j) const;
    public:
        const std::vector<Arc>& arcsFrom(size_t i) const {
            assert(i>=0 && i<arcsFromN.size());
            return arcsFromN[i];
        }
        const std::vector<Arc>& arcsTo(size_t i) const {
            assert(i>=0 && i<arcsToN.size());
            return arcsToN[i];
        }
        size_t capacity() const {return Q;}
        const std::string& code() const {return cod;}
        double cost(size_t i, size_t j) const {
            if (synthetic) {
                const double diff_x=xcoords[i]-xcoords[j];
                const double diff_y=ycoords[i]-ycoords[j];
                return std::sqrt(diff_x*diff_x + diff_y*diff_y);
            } else
                return (dist(i,j)+dist(j,i))/2;
        }
        const std::vector<Driver>& drivers() const {return drivs;}
        void loadCoordinates(const std::string& xy_file);
        void loadDrivers(const std::string& drv_file, size_t ndrv);
        void loadNetwork(const std::string& xy_file, const std::string& d_file);
        void loadRequests(const std::string& req_file, size_t nreqs);
        void loadSyntheticDataGaussian(size_t nreqs, size_t ndrv, size_t K,
                double sigma);
        void loadSyntheticDataUniform(size_t nreqs, size_t ndrv);
        size_t nodes() const {return N;}
        const std::vector<Request>& requests() const {return reqs;}
        void setCapacity(size_t q) {Q=q;}
        void setCode(std::string s) {cod=std::move(s);}
};

#endif

