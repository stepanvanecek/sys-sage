#ifndef CAPS_NUMA_BENCHMARK
#define CAPS_NUMA_BENCHMARK

#include "Topology.hpp"
#include "DataPath.hpp"

int parseCapsNumaBenchmark(Component* rootComponent, string benchmarkPath, string delim);

class CSVReader
{
    std::string benchmarkPath;
    std::string delimiter;
public:
    CSVReader(std::string benchmarkPath, std::string delm = ";") : benchmarkPath(benchmarkPath), delimiter(delm) { }
    // Function to fetch data from a CSV File
    int getData(std::vector<std::vector<std::string> >*);
};

#endif
