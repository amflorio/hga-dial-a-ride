#ifndef partition_h
#define partition_h

#include <memory>
#include <vector>
#include "Group.h"
#include "Request.h"

class Partition {
    public:
        static std::vector<Group> partition();
        static void setInstance(std::shared_ptr<Instance> i) {I=i;}
        static double MST_st(const Group& g) {
            return MST_s(g)+MST_t(g);
        }
    private:
        typedef std::vector<Group> cluster_t;
        typedef std::vector<cluster_t> collection_t;
        struct wresult_t {
            enum class which_w {w1, w2, none};
            double val;
            which_w w;
            Group g1, g2;
        };
        static std::shared_ptr<Instance> I;
        static double MST_s(const Group& g);
        static double MST_t(const Group& g);
        static void printCollection(const collection_t& c);
        static void printGroup(const Group& g);
        static wresult_t w(const cluster_t& c1, const cluster_t& c2);
        static double w_val(const cluster_t& c1, const cluster_t& c2);
        static double w1(const Group& g1, const Group& g2);
        static double w2(const Group& g1, const Group& g2);
};

#endif

