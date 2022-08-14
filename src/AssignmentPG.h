#ifndef assignmentpg_h
#define assignmentpg_h

#include <memory>
#include <vector>
#include "Group.h"
#include "Route.h"

class Instance;
class AssignmentPG {
    private:
        static std::shared_ptr<Instance> I;
    public:
        static std::vector<Route> assignment(const std::vector<Group>& groups);
        static void setInstance(std::shared_ptr<Instance> i) {I=i;}
};

#endif

