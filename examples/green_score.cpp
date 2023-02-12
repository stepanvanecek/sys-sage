#ifdef CPUINFO


#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <cstring>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <limits>
#include <sstream>


#include <papi.h>
#include "sys-sage.hpp"

using namespace std;
using namespace std::chrono;

#define NUM_MY_PAPI_EVENTS 4
#define MEASURE_CHILD_THREADS 1

void usage(char* argv0)
{
    std::cerr << "usage: " << argv0 << " <hwloc xml path> <program_to_measure> [program params]" << std::endl;
    return;
}

class GreenEntry{
public:
    GreenEntry(uint64_t _time, int _threadId, double _freq, long long* _papi_counters)
    {
        time = _time;
        threadId = _threadId;
        frequency = _freq;
        memcpy(papi_counters, _papi_counters, NUM_MY_PAPI_EVENTS*sizeof(long long));
    };
    void PrintHeader()
    {
        cout << "time, thread, frequency, PAPI_TOT_INS, PAPI_L2_TCA, PAPI_L3_LDM, PAPI_L3_TCM" << endl;
    }
    void Print()
    {
        printf("-- %lld, %d, %f, %lld, %lld, %lld, %lld \n", time, threadId, frequency, papi_counters[0], papi_counters[1], papi_counters[2], papi_counters[3]);
        cout << time << ", " << threadId << ", " << frequency;
        cout << ", " << papi_counters[0] << ", " << papi_counters[1] << ", " << papi_counters[2] << ", " << papi_counters[3] << endl;
    };
    uint64_t time;
    int threadId;
    double frequency;
    long long papi_counters[4];
};

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void get_child_threads(int pid, int * tids, int * cores)
{
    string cmd_get_threadId = "ps -o tid,psr -p " + std::to_string(pid) + " -T | tail -n +2";
    string str_coreId = exec(cmd_get_threadId.c_str());

    for(int i = 0; i< MEASURE_CHILD_THREADS; i++)
    {
        cores[i] = -1;
        tids[i] = -1;
    }

    int index = 0;
    std::stringstream ss(str_coreId);
    std::string line;
    while (std::getline(ss, line)) {
        std::stringstream ss_line(line);
        ss_line >> tids[index];
        ss_line >> cores[index];

        std::cout << "tid " << tids[index] <<" on core " << cores[index] << std::endl;
        index++;
        if(index == MEASURE_CHILD_THREADS)
            return;
    }
    return;
}

int main(int argc, char *argv[])
{
    string topoPath;

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

        //initialize a vector with GreenEntry results
        vector<GreenEntry>* greenEntries = new vector<GreenEntry>();
        //c->attrib["GreenEntries"] = (void*)greenEntries;

        //set up papi
        long long * papi_counters = new long long[MEASURE_CHILD_THREADS*NUM_MY_PAPI_EVENTS]();
        //for(int i = 0; i< NUM_MY_PAPI_EVENTS; i++) {papi_counters[i] = 0;}
        PAPI_library_init(PAPI_VER_CURRENT);
        int* eventset = new int [MEASURE_CHILD_THREADS]();
        int* measured_cores = new int [MEASURE_CHILD_THREADS]();
        int* measured_tids = new int [MEASURE_CHILD_THREADS]();
        PAPI_option_t* opts = new PAPI_option_t [MEASURE_CHILD_THREADS]();;
        int retval;
        for(int i = 0; i< MEASURE_CHILD_THREADS; i++)
        {
        }


        //start other process
        ptrace(PTRACE_CONT,child,0,0);
        int process_status, finished_pid;

        high_resolution_clock::time_point ts, ts_start = high_resolution_clock::now();
        // Wait until process exits
        do {
            n->RefreshCpuCoreFrequency();
            get_child_threads(child, measured_tids, measured_cores);

            for(int i = 0; i< MEASURE_CHILD_THREADS; i++)
            {
                if(measured_tids[i] != -1)
                {
                    eventset[i] = PAPI_NULL;
                    retval=PAPI_create_eventset(&(eventset[i]));
                    if (retval!=PAPI_OK) fprintf(stderr,"Error creating eventset! %s\n",PAPI_strerror(retval));
                    retval = PAPI_assign_eventset_component(eventset[i], 0 );
                    if (retval!=PAPI_OK) fprintf(stderr,"Error creating eventset! %s\n",PAPI_strerror(retval));

                    // Attach this event set to cpu 1
                    opts[i].cpu.eventset = eventset[i];
                    opts[i].cpu.cpu_num = measured_cores[i];
                    retval = PAPI_set_opt( PAPI_CPU_ATTACH, &(opts[i]) );
                    if (retval!=PAPI_OK) fprintf(stderr,"Error PAPI_set_opt! %s\n",PAPI_strerror(retval));

                    retval+=PAPI_add_event(eventset[i],PAPI_TOT_INS); // PAPI_TOT_INS 0x80000032  Yes   No   Instructions completed
                    retval+=PAPI_add_event(eventset[i],PAPI_L2_TCA); // PAPI_L2_DCA  0x80000041  Yes   No   Level 2 data cache accesses
                    retval+=PAPI_add_event(eventset[i],PAPI_L3_LDM); // PAPI_L3_LDM  0x8000000e  Yes   No   Level 3 load misses
                    retval+=PAPI_add_event(eventset[i],PAPI_L3_TCM); // PAPI_L3_TCM  0x80000008  Yes   No   Level 3 cache misses
                    if (retval!=PAPI_OK) fprintf(stderr,"Error eventset! %s\n",PAPI_strerror(retval));

                    // retval=PAPI_attach(eventset[i], measured_tids[i]);
                    // if (retval!=PAPI_OK) fprintf(stderr,"Error attaching papi: %s\n", PAPI_strerror(retval));
                    retval=PAPI_start(eventset[i]);
                    if (retval!=PAPI_OK) fprintf(stderr,"Error starting papi: %s\n", PAPI_strerror(retval));
                }
            }

            usleep(2000000);

            for(int i = 0; i< MEASURE_CHILD_THREADS; i++)
            {
                if(measured_tids[i] != -1)
                {
                    retval=PAPI_stop(eventset[i],&papi_counters[NUM_MY_PAPI_EVENTS*i]);
                    if (retval!=PAPI_OK) fprintf(stderr,"Error stopping:  %s\n", PAPI_strerror(retval));
                    // retval=PAPI_detach(eventset[i]);
                    // if (retval!=PAPI_OK) fprintf(stderr,"Error detaching:  %s\n", PAPI_strerror(retval));
                }
            }


            ts = high_resolution_clock::now();
            long long time = (ts.time_since_epoch().count()-ts_start.time_since_epoch().count())/1000000;

            //read PAPI measurements
            for(int i = 0; i< MEASURE_CHILD_THREADS; i++)
            {
                if(measured_cores[i] != -1)
                {
                    Thread * t = (Thread*)n->FindSubcomponentById(measured_cores[i], SYS_SAGE_COMPONENT_THREAD);
                    if(t==NULL)
                        return 1;
                    cout << "LIVE: Thread " << measured_cores[i] << ", tid " << measured_tids[i] << " Time : " << time << ", Frequency: " << t->GetFreq() << " ---- "<< papi_counters[NUM_MY_PAPI_EVENTS*i+0] << " " << papi_counters[NUM_MY_PAPI_EVENTS*i+1] << " " << papi_counters[NUM_MY_PAPI_EVENTS*i+2] << " " << papi_counters[NUM_MY_PAPI_EVENTS*i+3]<< endl;
                    greenEntries->push_back(GreenEntry(time, t->GetId(), t->GetFreq(), &papi_counters[NUM_MY_PAPI_EVENTS*i]));
                }
            }
            finished_pid = waitpid(child, &process_status, WNOHANG);
        } while(finished_pid <= 0);
        // while(!WIFEXITED(status));

        cout << "=========PRINT===============" << endl;
        (*greenEntries)[0].PrintHeader();
        for(GreenEntry g : *greenEntries)
        {
            g.Print();
        }

    }


    return 0;
}

#endif
