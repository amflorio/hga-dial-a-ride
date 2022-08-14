#ifndef partitionlambdaforest_h
#define partitionlambdaforest_h

#include <memory>
#include <vector>
#include "Group.h"

class PartitionLambdaForest {
    public:
        static std::vector<Group> partition();
        static void setInstance(std::shared_ptr<Instance> i) {I=i;}
    private:
        static std::shared_ptr<Instance> I;
        static Group minRatioLambdaForest(const Group& g);
};

#endif

