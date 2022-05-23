

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
    std::string path_prefix(argv[0]);
    std::size_t found = path_prefix.find_last_of("/\\");
    path_prefix=path_prefix.substr(0,found) + "/";
    string topoPath = "example_data/skylake_hwloc.xml";
    if(parseHwlocOutput(n, path_prefix+topoPath) != 0) //adds topo to a next node
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
    string bwPath = "example_data/skylake_caps_numa_benchmark.csv";
    if(parseCapsNumaBenchmark((Component*)n, path_prefix+bwPath, ";") != 0)
    {
        cout << "failed parsing caps-numa-benchmark" << endl;
        return 1;
    }
    cout << "---- end parseCapsNumaBenchmark" << endl;

    cout << "---------------- Printing all DataPaths ----------------" << endl;
    n->PrintAllDataPathsInSubtree();
    cout << "----------------                        ----------------" << endl;
    remove("tmp_hwloc.xml");

    exportToXml(topo, "test.xml");

    return 0;
}
