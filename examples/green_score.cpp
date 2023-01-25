#ifdef CPUINFO


#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <cstring>

#include <papi.h>
#include "sys-sage.hpp"

using namespace std;
using namespace std::chrono;

#define PAPI_NUM_EVENTS 4

void usage(char* argv0)
{
    std::cerr << "usage: " << argv0 << " <hwloc xml path> <caps-numa-benchmark csv path>" << std::endl;
    std::cerr << "       or" << std::endl;
    std::cerr << "       " << argv0 << " (uses predefined paths which may be incorrect.)" << std::endl;
    return;
}

class GreenEntry{
public:
    GreenEntry(uint64_t _time, double _freq, long long* _papi_counters)
    {
        time = _time;
        frequency = _freq;
        memcpy(papi_counters, _papi_counters, PAPI_NUM_EVENTS*sizeof(long long));
    };
    void Print()
    {
        cout << "GreenEntry, time " << time << " frequency " << frequency;
        cout << " papi_counters " << papi_counters[0] << " " << papi_counters[1] << " " << papi_counters[2] << " " << papi_counters[3] << endl;
    };
    uint64_t time;
    double frequency;
    long long papi_counters[4];
};

int main(int argc, char *argv[])
{
    string topoPath;
    if(argc < 2){
        std::string path_prefix(argv[0]);
        std::size_t found = path_prefix.find_last_of("/\\");
        path_prefix=path_prefix.substr(0,found) + "/";
        topoPath = path_prefix + "example_data/skylake_hwloc.xml";
    }
    else if(argc == 2){
        topoPath = argv[1];
    }
    else{
        usage(argv[0]);
        return 1;
    }

    //create root Topology and one node
    Topology* topo = new Topology();
    Node* n = new Node(topo, 1);

    if(parseHwlocOutput(n, topoPath) != 0) { //adds topo to a next node
        usage(argv[0]);
        return 1;
    }

    //set up papi
    long long papi_counters[PAPI_NUM_EVENTS];
    PAPI_library_init(PAPI_VER_CURRENT);
	int eventset=PAPI_NULL;
	int retval=PAPI_create_eventset(&eventset);
	if (retval!=PAPI_OK) fprintf(stderr,"Error creating eventset! %s\n",PAPI_strerror(retval));
	retval+=PAPI_add_event(eventset,PAPI_TOT_INS); // PAPI_TOT_INS 0x80000032  Yes   No   Instructions completed
	retval+=PAPI_add_event(eventset,PAPI_L2_TCA); // PAPI_L2_DCA  0x80000041  Yes   No   Level 2 data cache accesses
	retval+=PAPI_add_event(eventset,PAPI_L3_LDM); // PAPI_L3_LDM  0x8000000e  Yes   No   Level 3 load misses
	retval+=PAPI_add_event(eventset,PAPI_L3_TCM); // PAPI_L3_TCM  0x80000008  Yes   No   Level 3 cache misses

    int thread_num = sched_getcpu();
    Thread * t = (Thread*)n->FindSubcomponentById(thread_num, SYS_SAGE_COMPONENT_THREAD);
    if(t==NULL)
        return 1;
    Core * c = (Core*)t->FindParentByType(SYS_SAGE_COMPONENT_CORE);
    if(c==NULL)
        return 1;

    //initialize a vector with GreenEntry results
    vector<GreenEntry>* greenEntries = new vector<GreenEntry>();
    c->attrib["GreenEntries"] = (void*)greenEntries;

    high_resolution_clock::time_point ts, ts_start = high_resolution_clock::now();
    for(int i=0; i<10; i++)
    {
        n->RefreshCpuCoreFrequency();

        //start PAPI measurements
        PAPI_reset(eventset);
		retval+=PAPI_start(eventset);
		if (retval!=PAPI_OK) fprintf(stderr,"Error starting papi: %s\n", PAPI_strerror(retval));
        ts = high_resolution_clock::now();

        // ... start computation
        usleep(1000000);

        //read PAPI measurements
        retval=PAPI_stop(eventset,papi_counters);
        if (retval!=PAPI_OK) fprintf(stderr,"Error stopping:  %s\n", PAPI_strerror(retval));
        long long time = (ts.time_since_epoch().count()-ts_start.time_since_epoch().count())/1000000;
        cout << "LIVE: Time : " << time << ", Frequency: " << t->GetFreq() << endl;
        //GreenEntry* g = new GreenEntry(time, t->GetFreq(), papi_counters);
        vector<GreenEntry>* ge = (vector<GreenEntry>*)c->attrib["GreenEntries"];
        ge->push_back(GreenEntry(time, t->GetFreq(), papi_counters));

    }
cout << "========================" << endl;
    for(GreenEntry g : *((vector<GreenEntry>*)c->attrib["GreenEntries"]))
    {
        g.Print();
    }

    return 0;
}

#endif
