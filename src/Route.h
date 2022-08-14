#ifndef route_h
#define route_h

#include <memory>
#include <unordered_map>
#include <vector>
#include "Instance.h"

struct Stop {
    enum class Type {start, pickup, dropoff};
    Stop(size_t n, Type t, size_t req_id) : node{n}, type{t}, req_id{req_id} {}
    size_t node;
    Type type;
    size_t req_id;
};

class Route {
    private:
        static std::shared_ptr<Instance> I;
        std::vector<Stop> stps;
    public:
        Route(size_t s) {
            stps.emplace_back(s, Stop::Type::start, 0);
        }
        double cost() const {
            double c=0;
            for (size_t i=1; i<stps.size(); ++i)
                c+=I->cost(stps[i-1].node, stps[i].node);
            return c;
        }
        bool feasible() const {
            if (stps.size()<3)
                return false;
            if (stps[0].type!=Stop::Type::start)
                return false;
            std::set<size_t> loaded_reqs;
            for (size_t i=1; i<stps.size(); ++i)
                if (stps[i].type==Stop::Type::pickup) {
                    if (loaded_reqs.count(stps[i].req_id)>0)
                        return false;
                    loaded_reqs.insert(stps[i].req_id);
                    if (loaded_reqs.size()>I->capacity())
                        return false;
                } else if (stps[i].type==Stop::Type::dropoff) {
                    if (loaded_reqs.count(stps[i].req_id)!=1)
                        return false;
                    loaded_reqs.erase(stps[i].req_id);
                } else
                    return false;
            return true;
        }
        void insert(Stop s) {
            stps.push_back(std::move(s));
        }
        void insert(size_t i, Stop s) {
            stps.insert(stps.begin()+i, std::move(s));
        }
        double latency() const {
            double lat=0, tt=0;
            std::unordered_map<size_t, double> pickup_times;
            for (size_t i=1; i<stps.size(); ++i) {
                tt+=I->cost(stps[i-1].node, stps[i].node);
                if (stps[i].type==Stop::Type::pickup)
                    pickup_times[stps[i].req_id]=tt;
                else
                    lat+=tt-pickup_times.at(stps[i].req_id);
            }
            return lat;
        }
        std::set<size_t> requests() const {
            std::set<size_t> reqs;
            for (const auto& s : stps)
                if (s.type==Stop::Type::pickup)
                    reqs.insert(s.req_id);
            return reqs;
        }
        static void setInstance(std::shared_ptr<Instance> i) {I=i;}
        size_t size() const {
            return stps.size();
        }
        const std::vector<Stop>& stops() const {return stps;}
};

#endif

