

#include <iostream>
#include <chrono>
#include <ctime>
#include <unistd.h>
#include <numaif.h>
#include <numa.h>


using namespace std;
using namespace std::chrono;

void fill_arr(uint64_t * arr, int num_elems)
{
    for(int i=0; i<num_elems; i++)
    {
        arr[i] = i;
    }
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

int main(int argc, char *argv[])
{
    uint64_t time, timer_overhead;
    uint64_t *arr, *mock_arr;
    high_resolution_clock::time_point t_start, t_end;

    int arrsz;
    int step=1;


    if(numa_available() == -1)
        return 1;

    cout << "numa_available(void) " << numa_available() << "  int numa_max_possible_node(void); " << numa_max_possible_node() << "  numa_num_possible_nodes() " << numa_num_possible_nodes() << "  numa_max_node(void) " << numa_max_node() << "  numa_num_configured_nodes() " << numa_num_configured_nodes() << endl;

    int numa_nodes = numa_num_configured_nodes();
    for(int n=0; n<numa_nodes; n++)
    {
        long long nodesz = numa_node_size(n, NULL);


        //arrsz = nodesz/4;
        arrsz = nodesz/4;
        step = 64/sizeof(uint64_t); //one elem per cache line
        int num_elems = arrsz/sizeof(uint64_t);
        mock_arr = (uint64_t*)numa_alloc_onnode(arrsz, n);
        arr = (uint64_t*)numa_alloc_onnode(arrsz, n);

        fill_arr(arr, num_elems);
        fill_arr(mock_arr, num_elems);

        timer_overhead = get_timer_overhead(1024, 64);

        t_start = high_resolution_clock::now();
        int sum = arr[0];
        t_end = high_resolution_clock::now();
        time = t_end.time_since_epoch().count()-t_start.time_since_epoch().count()-timer_overhead;

        cout << "node; " << n << "; size; " << nodesz << "; arrsz; " << arrsz << "; timer_ovh; " << timer_overhead << "; ldlat(ns); " << time;

        t_start = high_resolution_clock::now();
        for(int i = 0; i<num_elems; i+=step)
        {
            // if (i==0 || i==num_elems/8 || i==num_elems/2 || i==num_elems*7/8|| i==num_elems-1)
            // {
            //     /*here you should align ptr_to_check to page boundary */
            //     //t_start = high_resolution_clock::now();
            //     void* ptr_to_check = (void*)&(arr[i]);
            //     int status = -1;
            //     int ret_code;
            //     ret_code=move_pages(0 /*self memory */, 1, &ptr_to_check, NULL, &status, 0);
            //     //t_end = high_resolution_clock::now();
            //     //cout << /*arrsz << " ; " <<*/ t_end.time_since_epoch().count()-t_start.time_since_epoch().count() << "; ";//endl;
            //     //printf("    %d       Memory at %p is at %d node (retcode %d)\n", status, ptr_to_check, status, ret_code);
            // }
            sum+=arr[i];
        }
        t_end = high_resolution_clock::now();
        time = t_end.time_since_epoch().count()-t_start.time_since_epoch().count()-timer_overhead;
        cout << "; time; " << time << "; bw(MB/s); " << arrsz/(time/1000) << endl;

        numa_free(mock_arr, arrsz);
        numa_free(arr, arrsz);
    }

    //
    //
    // int max_arrsz=10000000;
    // int mocksz =  10000000;
    // mock_arr = (uint64_t*)malloc(sizeof(uint64_t)*mocksz);
    // for(;arrsz<=max_arrsz; arrsz*=2)
    // {
    //     arr = (uint64_t*)malloc(sizeof(uint64_t)*arrsz);
    //
    //     fill_arr(arr, arrsz);
    //     //fill_arr(mock_arr, mocksz);
    //     int sum = 0;
    //
    //     t_start = high_resolution_clock::now();
    //     for(int i = 0; i<arrsz; i+=step)
    //     {
    //         if (i==0 || i==arrsz/2 || i==arrsz-1)
    //         {
    //             int numa_node = -1;
    //             get_mempolicy(&numa_node, NULL, 0, (void*)&(arr[i]), MPOL_F_NODE | MPOL_F_ADDR);
    //             return numa_node;
    //         }
    //         sum+=arr[i];
    //     }
    //     t_end = high_resolution_clock::now();
    //     cout << /*arrsz << " ; " <<*/ t_end.time_since_epoch().count()-t_start.time_since_epoch().count() << "; ";//endl;
    //     free(arr);
    // }
    // cout << endl;
    // free(mock_arr);
    return 0;
}
