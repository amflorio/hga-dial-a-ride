#ifndef driver_h
#define driver_h

class Driver {
    public:
        size_t id;          // unique identifier
        size_t s;           // initial location
        Driver(size_t id, size_t s) : id{id}, s{s} {}
};

#endif

