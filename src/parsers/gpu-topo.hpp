#ifndef GPU_TOPO
#define GPU_TOPO

#include "Topology.hpp"
#include "DataPath.hpp"

int parseGpuTopo(Component* parent, string dataSourcePath, int gpuId, string delim = ";");
int parseGpuTopo(Chip* gpu, string dataSourcePath, string delim = ";");

class GpuTopo
{
public:
    GpuTopo(Chip* gpu, string dataSourcePath, string delim = ";");

    int ParseBenchmarkData();
private:
    int ReadBenchmarkFile();
    map<string,vector<string> > benchmarkData;
    string dataSourcePath;
    string delim;
    Chip* root;
    bool L2_shared_on_gpu;
    int parseGPU_INFORMATION();
    int parseCOMPUTE_RESOURCE_INFORMATION();
    int parseREGISTER_INFORMATION();
    int parseMAIN_MEMORY();
    int parseCaches(string header_name, string cache_name);
};

const std::string whiteSpaces( " \f\n\r\t\v" );
void trimRight( std::string& str,const std::string& trimChars = whiteSpaces );
void trimLeft( std::string& str,const std::string& trimChars = whiteSpaces );
void trim( std::string& str, const std::string& trimChars = whiteSpaces );
#endif
