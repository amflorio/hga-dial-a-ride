#ifndef group_h
#define group_h

#include <unordered_set>
#include <boost/functional/hash.hpp>

class Request;
typedef std::unordered_set<Request, boost::hash<Request>> Group;

std::ostream& operator<<(std::ostream& os, const Group& g);

#endif

