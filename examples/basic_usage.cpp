

#include <iostream>

#include "hwloc.hpp"
//#include "Topology.hpp"

int main(int argc, char *argv[])
{

    Topology* topo = new Topology();

    string topoPath = "/Users/stepan/Documents/phd/sv_memaxes/example_data/mitos_1626945486/hardware.xml";
    addParsedHwlocTopo(topo, topoPath, 1); //adds topo to a next node

    topo->PrintSubtree(0);
    Node* n = (Node*)topo->GetChild(0);
    n->PrintSubtree(9);

    cout << topo->GetNumThreads() << endl;
    cout << n->GetChild(0)->GetChild(0)->GetNumThreads() << endl;
    return 0;

}
