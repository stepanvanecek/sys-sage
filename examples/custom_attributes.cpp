#include "sys-sage.hpp"
#include <functional>


class My_core_attributes {
public:
    My_core_attributes(double t, int f): temperature(t), frequency(f){};
    double temperature;
    int frequency;
};

// define your own XML output function, where you define how your custom attribs will get printed.
// key is the attrib key, value is the void* to the value, and ret_value_str is an output parameter where you save the output string to put to the XML.
int print_my_attribs(string key, void* value, string* ret_value_str)
{
    if(!key.compare("codename") || !key.compare("info"))
    {
        *ret_value_str=*(string*)value;
        return 1;
    }
    else if(!key.compare("rack_no"))
    {
        *ret_value_str=std::to_string(*(int*)value);
        return 1;
    }
    else if(!key.compare("my_core_info"))
    {
        My_core_attributes c = *(My_core_attributes*)value;
        *ret_value_str="temperature='" + std::to_string(c.temperature) + " C' frequency='" + std::to_string(c.frequency) + " Hz'";
        return 1;
    }

    return 0;
}

void usage(char* argv0)
{
    std::cerr << "usage: " << argv0 << " <hwloc xml path> <caps-numa-benchmark csv path>" << std::endl;
    std::cerr << "       or" << std::endl;
    std::cerr << "       " << argv0 << " (uses predefined paths which may be incorrect.)" << std::endl;
    return;
}

int main(int argc, char *argv[])
{
    string topoPath;
    string bwPath;
    if(argc < 2){
        std::string path_prefix(argv[0]);
        std::size_t found = path_prefix.find_last_of("/\\");
        path_prefix=path_prefix.substr(0,found) + "/";
        topoPath = path_prefix + "example_data/skylake_hwloc.xml";
        bwPath = path_prefix + "example_data/skylake_caps_numa_benchmark.csv";
    }
    else if(argc == 3){
        topoPath = argv[1];
        bwPath = argv[2];
    }
    else{
        usage(argv[0]);
        return 1;
    }

    //first, populate sys-sage with some data
    Node* n = new Node(1);

    cout << "-- Parsing Hwloc output from file " << topoPath << endl;
    if(parseHwlocOutput(n, topoPath) != 0) { //adds topo to a next node
        usage(argv[0]);
        return 1;
    }
    cout << "-- End parseHwlocOutput" << endl;
    cout << "-- Parsing CapsNumaBenchmark output from file " << bwPath << endl;
    if(parseCapsNumaBenchmark((Component*)n, bwPath, ";") != 0) {
        cout << "failed parsing caps-numa-benchmark" << endl;
        usage(argv[0]);
        return 1;
    }
    cout << "-- End parseCapsNumaBenchmark" << endl;

    //let's add a few custom attributes
    string codename = "marsupial";
    int r = 15;
    n->attrib["codename"]=(void*)&codename;
    n->attrib["rack_no"]=(void*)&r;
    n->attrib["unknown_will_not_be_printed"]=(void*)&topoPath;

    My_core_attributes c1_attrib(38.222, 2000000000);
    Core* c1 = (Core*)n->FindSubcomponentById(1, SYS_SAGE_COMPONENT_CORE);
    if(c1 != NULL)
        c1->attrib["my_core_info"]=(void*)&c1_attrib;

    My_core_attributes c4_attrib(44.1, 1500000000);
    Core* c4 = (Core*)n->FindSubcomponentById(4, SYS_SAGE_COMPONENT_CORE);
    if(c4 != NULL)
        c4->attrib["my_core_info"]=(void*)&c4_attrib;

    string benchmark_info="measured with no load on 07.07.";
    Numa* n2 = (Numa*)n->FindSubcomponentById(2, SYS_SAGE_COMPONENT_NUMA);
    if(n2 != NULL){
        DataPath * dp = (*(n2->GetDataPaths(SYS_SAGE_DATAPATH_INCOMING)))[0];
        if(dp != NULL)
            dp->attrib["info"]=(void*)&benchmark_info;
        dp = (*(n2->GetDataPaths(SYS_SAGE_DATAPATH_INCOMING)))[2];
        if(dp != NULL)
            dp->attrib["info"]=(void*)&benchmark_info;
    }

    //export to xml
    string output_name = "sys-sage_custom_attributes.xml";
    std::cout << "-------- Exporting as XML to " << output_name << std::endl;
    exportToXml(n, output_name, print_my_attribs);

    return 0;
}
