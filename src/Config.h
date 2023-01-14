#ifndef config_h
#define config_h

#include <iostream>

class Config {
    public:
        // true: compute W1 via MSTs;  false: use a fast approximation
        static constexpr bool EXACT_W1=true;
        // true: exact matching via blossom algo;  false: approx bucket matching
        static constexpr bool EXACT_MATCHING=true;
        static void PrintConfigParameters() {
            std::cout<<"configuration parameters:"<<std::endl;
            std::cout<<"EXACT_W1: "<<EXACT_W1<<std::endl;
            std::cout<<"EXACT_MATCHING: "<<EXACT_MATCHING<<std::endl;
        }
};

#endif

