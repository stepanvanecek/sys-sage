

#include <iostream>
#include <hwloc.h>

#include "sys-topo.hpp"


int dump_hardware_xml(const char *filename);
int main(int argc, char *argv[])
{
    //create root Topology and one node
    Topology* topo = new Topology();
    Node* n = new Node(1);
    n->SetParent((Component*)topo);
    topo->InsertChild((Component*)n);

    cout << "---- start parseHwlocOutput" << endl;
    string topoPath = "hwloc.xml";
    parseHwlocOutput(n, topoPath); //adds topo to a next node
    cout << "---- end parseHwlocOutput" << endl;

    cout << "Total num HW threads: " << topo->GetNumThreads() << endl;
    cout << "---------------- Printing whole tree ----------------" << endl;
    topo->PrintSubtree(2);
    cout << "----------------                     ----------------" << endl;

    cout << "---- start parseCapsNumaBenchmark" << endl;
    string bwPath = "test-caps-numa-bench-numa-only.csv";
    parseCapsNumaBenchmark((Component*)n, bwPath, ";");
    cout << "---- end parseCapsNumaBenchmark" << endl;

    cout << "---------------- Printing all DataPaths ----------------" << endl;
    vector<Component*> subtreeList;
    n->GetSubtreeNodeList(&subtreeList);
    for(auto it = std::begin(subtreeList); it != std::end(subtreeList); ++it)
    {
        vector<DataPath*>* dp_in = (*it)->GetDataPaths(SYS_TOPO_DATAPATH_INCOMING);
        vector<DataPath*>* dp_out = (*it)->GetDataPaths(SYS_TOPO_DATAPATH_OUTGOING);
        if(dp_in->size() > 0 || dp_out->size() > 0 )
        {
            cout << "DataPaths regarding Component (" << (*it)->GetComponentTypeStr() << ") id " << (*it)->GetId() << endl;
            for(auto it2 = std::begin(*dp_out); it2 != std::end(*dp_out); ++it2)
            {
                cout << "    ";
                (*it2)->Print();
            }
            for(auto it2 = std::begin(*dp_in); it2 != std::end(*dp_in); ++it2)
            {
                cout << "    ";
                (*it2)->Print();
            }
        }
    }
    cout << "----------------                        ----------------" << endl;
    //remove(topoPath.c_str());
    return 0;
}




int hwloc_dump_xml(const char *filename)
{
    // int err;
    // unsigned long flags = 0; // don't show anything special
    // hwloc_topology_t topology;
    //
    // err = hwloc_topology_init(&topology);
    // if(err)
    // {
    //     std::cerr << "hwloc: Failed to initialize" << std::endl;
    //     return 1;
    // }
    //
    // err = hwloc_topology_set_flags(topology, flags);
    // if(err)
    // {
    //     std::cerr << "hwloc: Failed to set flags" << std::endl;
    //     return 1;
    // }
    //
    // err = hwloc_topology_load (topology);
    // if(err)
    // {
    //     std::cerr << "hwloc: Failed to load topology" << std::endl;
    //     return 1;
    // }
    //
    // err = hwloc_topology_export_xml(topology, "hardware.xml", flags);
    // if(err)
    // {
    //     std::cerr << "hwloc: Failed to export xml" << std::endl;
    //     return 1;
    // }

    return 0;
}
