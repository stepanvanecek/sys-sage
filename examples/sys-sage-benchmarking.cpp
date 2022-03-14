

#include <iostream>
//#include <hwloc.h>
#include <chrono>

#include "sys-sage.hpp"

////////////////////////////////////////////////////////////////////////
//PARAMS TO SET
#define TIMER_WARMUP 32
#define TIMER_REPEATS 128

////////////////////////////////////////////////////////////////////////
using namespace std::chrono;


//int hwloc_dump_xml(const char *filename);
uint64_t get_timer_overhead(int repeats, int warmup);

int main(int argc, char *argv[])
{
    Node* n = new Node(1);

    vector<Component*> nodesList;
    n->GetSubtreeNodeList(&nodesList);
    unsigned component_size=0, dataPathSize=0;
    cout << "EMPTY; num_elements; " << nodesList.size() << "; size[B]; " << n->GetTopologySize(&component_size, &dataPathSize) << "; component_size; " << component_size << "; dataPathSize; " << dataPathSize << endl;

    string topoPath = "example_data/skylake_hwloc.xml";

    high_resolution_clock::time_point t_start, t_end;
    uint64_t timer_overhead = get_timer_overhead(TIMER_REPEATS, TIMER_WARMUP);
    t_start = high_resolution_clock::now();
    int ret = parseHwlocOutput(n, topoPath);
    t_end = high_resolution_clock::now();
    uint64_t time_parseHwlocOutput = t_end.time_since_epoch().count()-t_start.time_since_epoch().count()-timer_overhead;
    if(ret != 0) {//adds topo to a next node
             return 1;
    }

    nodesList.clear();
    t_start = high_resolution_clock::now();
    n->GetSubtreeNodeList(&nodesList);
    t_end = high_resolution_clock::now();
    uint64_t time_GetSubtreeNodeList = t_end.time_since_epoch().count()-t_start.time_since_epoch().count()-timer_overhead;

    component_size=0;
    dataPathSize=0;
    cout << "HWLOC; num_elements; " << nodesList.size() << "; size[B]; " << n->GetTopologySize(&component_size, &dataPathSize) << "; component_size; " << component_size << "; dataPathSize; " << dataPathSize << endl;

    string bwPath = "example_data/skylake_caps_numa_benchmark.csv";
    t_start = high_resolution_clock::now();
    ret = parseCapsNumaBenchmark((Component*)n, bwPath, ";");
    t_end = high_resolution_clock::now();
    uint64_t time_parseCapsNumaBenchmark = t_end.time_since_epoch().count()-t_start.time_since_epoch().count()-timer_overhead;
    if(ret != 0)
    {
        cout << "failed parsing caps-numa-benchmark" << endl;
        return 1;
    }

    nodesList.clear();
    component_size=0;
    dataPathSize=0;
    n->GetSubtreeNodeList(&nodesList);
    cout << "HWLOC+DATAPATH; num_elements; " << nodesList.size() << "; size[B]; " << n->GetTopologySize(&component_size, &dataPathSize) << "; component_size; " << component_size << "; dataPathSize; " << dataPathSize << endl;
    cout << "time_parseHwlocOutput; " << time_parseHwlocOutput << "; time_parseCapsNumaBenchmark; " << time_parseCapsNumaBenchmark << "; time_GetSubtreeNodeList; " << time_GetSubtreeNodeList << "; components; " << nodesList.size() << endl;

    //for NUMA 0 get NUMA with min BW
    Numa * numa = (Numa*)n->FindSubcomponentById(0, SYS_SAGE_COMPONENT_NUMA);
    if(numa==NULL){ cerr << "numa 0 not found in sys-sage" << endl; return 1;}
    unsigned int max_bw = 0;
    Component* max_bw_component = NULL;
    t_start = high_resolution_clock::now();
    vector<DataPath*>* dp = numa->GetDataPaths(SYS_SAGE_DATAPATH_OUTGOING);
    for(auto it = std::begin(*dp); it != std::end(*dp); ++it) {
        if( (*it)->GetBw() > max_bw ){
            max_bw = (*it)->GetBw();
            max_bw_component = (*it)->GetTarget();
        }
    }
    t_end = high_resolution_clock::now();
    uint64_t time_getNumaMaxBw = t_end.time_since_epoch().count()-t_start.time_since_epoch().count()-timer_overhead;
    cout << "time_getNumaMaxBw; " << time_getNumaMaxBw << "; bw; " << max_bw << "; ComponentId; " << max_bw_component->GetId() << endl;

    return 0;
}

uint64_t get_timer_overhead(int repeats, int warmup)
{
    high_resolution_clock::time_point t_start, t_end;
    uint64_t time = 0;
    //uint64_t time_arr[10];
    for(int i=0; i<repeats+warmup; i++)
    {
        t_start = high_resolution_clock::now();
        t_end = high_resolution_clock::now();
        if(i>=warmup)
            time += t_end.time_since_epoch().count()-t_start.time_since_epoch().count();
        // if(i<10)
        //     time_arr[i]=t_end.time_since_epoch().count()-t_start.time_since_epoch().count();

    }
    // for(int i=0; i<10; i++)
    // {
    //     cout << time_arr[i] << "; ";
    // } //cout << endl;

    time = time/repeats;

    return time;
}
