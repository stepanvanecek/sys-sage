#ifdef CPUINFO


#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <cstring>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include <papi.h>
#include "sys-sage.hpp"

using namespace std;
using namespace std::chrono;

#define PAPI_NUM_EVENTS 4

void usage(char* argv0)
{
    std::cerr << "usage: " << argv0 << " <hwloc xml path> <program_to_measure> [program params]" << std::endl;
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
    // if(argc < 2){
    //     std::string path_prefix(argv[0]);
    //     std::size_t found = path_prefix.find_last_of("/\\");
    //     path_prefix=path_prefix.substr(0,found) + "/";
    //     topoPath = path_prefix + "example_data/skylake_hwloc.xml";
    // }
    // else if(argc == 2){
    //     topoPath = argv[1];
    // }
    // else{
    //     usage(argv[0]);
    //     return 1;
    // }
    if(argc < 3)
    {
        usage(argv[0]);
        return 1;
    }
    else
    {
        topoPath = argv[1];
    }

    //create root Topology and one node
    Topology* topo = new Topology();
    Node* n = new Node(topo, 1);

    if(parseHwlocOutput(n, topoPath) != 0) { //adds topo to a next node
        usage(argv[0]);
        return 1;
    }



    pid_t child = fork();
    if(child == 0)
    {
        ptrace(PTRACE_TRACEME,0,0,0);
        int thread_num = sched_getcpu();
        cout << "child running on thread " << thread_num << endl;
        int err = execvp(argv[2],&argv[2]);
        if(err)
        {
            perror("execvp");
        }
    }
    else if(child < 0)
    {
        std::cerr << "Error forking!" << std::endl;
    }
    else
    {
        int status;
        wait(&status);

        int thread_num = sched_getcpu();
        cout << "parent running on thread " << thread_num << endl;
        Thread * t = (Thread*)n->FindSubcomponentById(thread_num, SYS_SAGE_COMPONENT_THREAD);
        if(t==NULL)
            return 1;
        Core * c = (Core*)t->FindParentByType(SYS_SAGE_COMPONENT_CORE);
        if(c==NULL)
            return 1;

        //initialize a vector with GreenEntry results
        vector<GreenEntry>* greenEntries = new vector<GreenEntry>();
        c->attrib["GreenEntries"] = (void*)greenEntries;

        //set up papi
        long long * papi_counters = new long long[4]();
        //long long papi_counters[PAPI_NUM_EVENTS];
        PAPI_library_init(PAPI_VER_CURRENT);
    	int eventset=PAPI_NULL;
    	int retval=PAPI_create_eventset(&eventset);
    	if (retval!=PAPI_OK) fprintf(stderr,"Error creating eventset! %s\n",PAPI_strerror(retval));
    	//retval+=PAPI_add_event(eventset,PAPI_TOT_INS); // PAPI_TOT_INS 0x80000032  Yes   No   Instructions completed
    	retval+=PAPI_add_event(eventset,PAPI_L2_TCA); // PAPI_L2_DCA  0x80000041  Yes   No   Level 2 data cache accesses
    	retval+=PAPI_add_event(eventset,PAPI_L3_LDM); // PAPI_L3_LDM  0x8000000e  Yes   No   Level 3 load misses
    	retval+=PAPI_add_event(eventset,PAPI_L3_TCM); // PAPI_L3_TCM  0x80000008  Yes   No   Level 3 cache misses

        if (PAPI_attach(eventset, child) != PAPI_OK)
           exit(1);
        //retval+=PAPI_start(eventset);
        if (retval!=PAPI_OK) fprintf(stderr,"Error starting papi: %s\n", PAPI_strerror(retval));

        //start other process
        ptrace(PTRACE_CONT,child,0,0);
        int process_status, finished_pid;

        high_resolution_clock::time_point ts, ts_start = high_resolution_clock::now();
        // Wait until process exits
        do {
            n->RefreshCpuCoreFrequency();

            //start new PAPI measurements
            retval+=PAPI_start(eventset);
            //PAPI_reset(eventset);
            ts = high_resolution_clock::now();

            usleep(50000);

            //read PAPI measurements
            //retval=PAPI_read(eventset,papi_counters);
            retval=PAPI_stop(eventset,papi_counters);
            if (retval!=PAPI_OK) fprintf(stderr,"Error stopping:  %s\n", PAPI_strerror(retval));
            long long time = (ts.time_since_epoch().count()-ts_start.time_since_epoch().count())/1000000;
            cout << "LIVE: Time : " << time << ", Frequency: " << t->GetFreq() << " ---- "<< papi_counters[0] << " " << papi_counters[1] << " " << papi_counters[2] << " " << papi_counters[3]<< endl;
            //GreenEntry* g = new GreenEntry(time, t->GetFreq(), papi_counters);
            vector<GreenEntry>* ge = (vector<GreenEntry>*)c->attrib["GreenEntries"];
            ge->push_back(GreenEntry(time, t->GetFreq(), papi_counters));

            //wait(&status);

            finished_pid = waitpid(child, &process_status, WNOHANG);
        } while(finished_pid <= 0);
        // while(!WIFEXITED(status));
        retval=PAPI_stop(eventset,papi_counters);

        cout << "=========PRINT===============" << endl;
        for(GreenEntry g : *((vector<GreenEntry>*)c->attrib["GreenEntries"]))
        {
            g.Print();
        }

    }


    return 0;
}

#endif
