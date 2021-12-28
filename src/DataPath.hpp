#ifndef DATAPATH
#define DATAPATH

#include "Topology.hpp"
#include <map>

using namespace std;
class Component;

class DataPath {

public:
    DataPath(Component*, Component*);
    DataPath(Component*, Component*, double bw, double latency);

    map<string,void*> metadata;
private:
    Component * source;
    Component * target;

    double bw;
    double latency;
};

#endif
