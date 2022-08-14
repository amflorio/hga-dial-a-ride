#ifndef arc_h
#define arc_h

#include <vector>

class Arc {
    public:
        size_t idx;         // arc index in the set of arcs
        size_t s;           // index of the origin node
        size_t t;           // index of the destination node
        double d;           // arc distance
        Arc(size_t idx, size_t s, size_t t, double d)
                : idx{idx}, s{s}, t{t}, d{d} {}
};

#endif

