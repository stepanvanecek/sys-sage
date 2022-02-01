

#include <iostream>
//#include <hwloc.h>

#include "sys-sage.hpp"


int main(int argc, char *argv[])
{
    //create root Topology and one node
    Topology* topo = new Topology();
    Node* n = new Node(1);
    n->SetParent((Component*)topo);
    topo->InsertChild((Component*)n);

    cout << "---- start parseHwlocOutput" << endl;
    string topoPath = "example_data/skylake_hwloc.xml";
    if(parseHwlocOutput(n, topoPath) != 0) //adds topo to a next node
    {   //parsing failed -> try creating new hwloc output and parsing it
        // hwloc_dump_xml("tmp_hwloc.xml");
        // if(parseHwlocOutput(n, "tmp_hwloc.xml") != 0){
        //     cout << "failed parsing hwloc output" << endl;
        //     return 1;
        // }
    }
    cout << "---- end parseHwlocOutput" << endl;


    cout << "Total num HW threads: " << topo->GetNumThreads() << endl;
    cout << "---------------- Printing whole tree ----------------" << endl;
    topo->PrintSubtree(2);
    cout << "----------------                     ----------------" << endl;


    cout << "---- start parseCapsNumaBenchmark" << endl;
    string bwPath = "example_data/skylake_caps_numa_benchmark.out";
    if(parseCapsNumaBenchmark((Component*)n, bwPath, ";") != 0)
    {
        cout << "failed parsing caps-numa-benchmark" << endl;
        return 1;
    }
    cout << "---- end parseCapsNumaBenchmark" << endl;

    cout << "---------------- Printing all DataPaths ----------------" << endl;
    n->PrintAllDataPathsInSubtree();
    cout << "----------------                        ----------------" << endl;
    remove("tmp_hwloc.xml");
    return 0;
}
