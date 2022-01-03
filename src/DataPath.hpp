#ifndef DATAPATH
#define DATAPATH

#include "Topology.hpp"
#include <map>

#define SYS_TOPO_DATAPATH_NONE 1
#define SYS_TOPO_DATAPATH_OUTGOING 2
#define SYS_TOPO_DATAPATH_INCOMING 4
#define SYS_TOPO_DATAPATH_BIDIRECTIONAL 8
#define SYS_TOPO_DATAPATH_ORIENTED 16

#define SYS_TOPO_DATAPATH_TYPE_LOGICAL 32
#define SYS_TOPO_DATAPATH_TYPE_PHYSICAL 64

using namespace std;
class Component;

class DataPath {

public:
    DataPath(Component* _source, Component* _target);
    DataPath(Component* _source, Component* _target, double _bw, double _latency);
    DataPath(Component* _source, Component* _target, int _oriented);
    DataPath(Component* _source, Component* _target, int _oriented, double _bw, double _latency);

    void Print();
    map<string,void*> metadata;
private:
    Component * source;
    Component * target;
    int oriented;

    double bw;
    double latency;

    int dp_type;
};

#endif
