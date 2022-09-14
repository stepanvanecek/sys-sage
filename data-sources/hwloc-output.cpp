
#include <iostream>
#include <hwloc.h>

#include "sys-sage.hpp"

/*! \file */
/// @private
int hwloc_dump_xml(const char *filename);

using namespace std;

/**
Binary (entrypoint) for generating hwloc topology XML output (to current directory)
\n usage: ./hwloc-output [output_filename]
@param filename of the output file (default: tmp_hwloc.xml)
*/
int main(int argc, char *argv[])
{
    if(argc < 2){
        hwloc_dump_xml("tmp_hwloc.xml");
    } else {
        hwloc_dump_xml(argv[1]);
    }

    return 0;
}

int hwloc_dump_xml(const char *filename)
{
    int err;
    unsigned long flags = 0; // don't show anything special
    hwloc_topology_t topology;

    err = hwloc_topology_init(&topology);
    if(err){
        std::cerr << "hwloc: Failed to initialize" << std::endl;return 1;
    }
    err = hwloc_topology_set_flags(topology, flags);
    if(err){
        std::cerr << "hwloc: Failed to set flags" << std::endl;return 1;
    }
    err = hwloc_topology_load (topology);
    if(err){
        std::cerr << "hwloc: Failed to load topology" << std::endl;return 1;
    }
    err = hwloc_topology_export_xml(topology, filename, flags);
    if(err){
        std::cerr << "hwloc: Failed to export xml" << std::endl; return 1;
    }
    else {
        std::cout << "Hwloc XML output exported to " << filename << std::endl;
    }
    return 0;
}
