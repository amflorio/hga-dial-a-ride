#ifndef request_h
#define request_h

#include <boost/functional/hash.hpp>

class Request {
    public:
        size_t id;          // unique identifier
        size_t s, t;        // origin and destination
        double d;
        friend size_t hash_value(const Request& r) {
            return r.id;
        }
        Request(size_t id, size_t s, size_t t) : id{id}, s{s}, t{t} {}
        bool operator==(const Request& other) const {
            return id==other.id;
        }
};

#endif

