

#include <iostream>
//#include <hwloc.h>

#include "sys-sage.hpp"


int hwloc_dump_xml(const char *filename);
int main(int argc, char *argv[])
{
    //create root Topology and one node
    Topology* topo = new Topology();
    Node* n = new Node(1);
    n->SetParent((Component*)topo);
    topo->InsertChild((Component*)n);

    vector<Component*> nodesList;
    n->GetSubtreeNodeList(&nodesList);
    cout << "=====EMPTY num_elements: " << nodesList.size() << ", size[B] " << n->GetTopologySize() << endl;


    cout << "---- start parseHwlocOutput" << endl;
    string topoPath = "example_data/skylake_hwloc.xml";
    if(parseHwlocOutput(n, topoPath) != 0) //adds topo to a next node
    {
             return 1;
    }
    cout << "---- end parseHwlocOutput" << endl;

    nodesList.clear();
    n->GetSubtreeNodeList(&nodesList);
    cout << "=====HWLOC num_elements: " << nodesList.size() << ", size[B] " << n->GetTopologySize() << endl;

    cout << "---- start parseCapsNumaBenchmark" << endl;
    string bwPath = "example_data/skylake_caps_numa_benchmark.csv";
    if(parseCapsNumaBenchmark((Component*)n, bwPath, ";") != 0)
    {
        cout << "failed parsing caps-numa-benchmark" << endl;
        return 1;
    }
    cout << "---- end parseCapsNumaBenchmark" << endl;

    nodesList.clear();
    n->GetSubtreeNodeList(&nodesList);
    cout << "=====HWLOC+DATAPATH num_elements: " << nodesList.size() << ", size[B] " << n->GetTopologySize() << endl;

    return 0;
}




// int hwloc_dump_xml(const char *filename)
// {
//     int err;
//     unsigned long flags = 0; // don't show anything special
//     hwloc_topology_t topology;
//
//     err = hwloc_topology_init(&topology);
//     if(err){
//         std::cerr << "hwloc: Failed to initialize" << std::endl;return 1;
//     }
//     err = hwloc_topology_set_flags(topology, flags);
//     if(err){
//         std::cerr << "hwloc: Failed to set flags" << std::endl;return 1;
//     }
//     err = hwloc_topology_load (topology);
//     if(err){
//         std::cerr << "hwloc: Failed to load topology" << std::endl;return 1;
//     }
//     err = hwloc_topology_export_xml(topology, filename, flags);
//     if(err){
//         std::cerr << "hwloc: Failed to export xml" << std::endl; return 1;
//     }
//     return 0;
// }
