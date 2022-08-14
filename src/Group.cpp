#include "Group.h"
#include "Request.h"

std::ostream& operator<<(std::ostream& os, const Group& g) {
    os<<"(";
    for (const auto& r : g)
        os<<" "<<r.id<<" ";
    os<<")";
    return os;
}

