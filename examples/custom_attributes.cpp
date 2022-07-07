#include "sys-sage.hpp"
#include <functional>


class My_core_attributes {
public:
    My_core_attributes(double t, int f): temperature(t), frequency(f){};
    double temperature;
    int frequency;
};

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

int main(int argc, char *argv[])
{
    //first, populate sys-sage with some data
    Node* n = new Node(1);
    cout << "---- start parse example input" << endl;
    std::string path_prefix(argv[0]);
    std::size_t found = path_prefix.find_last_of("/\\");
    path_prefix=path_prefix.substr(0,found) + "/";
    string topoPath = "example_data/skylake_hwloc.xml";
    if(parseHwlocOutput(n, path_prefix+topoPath) != 0){
        std::cout << "error parsing hwloc in path " << path_prefix+topoPath << std::endl;
        return 1;
    }
    string bwPath = "example_data/skylake_caps_numa_benchmark.csv";
    if(parseCapsNumaBenchmark((Component*)n, path_prefix+bwPath, ";") != 0){
        std::cout << "failed parsing caps-numa-benchmark in path " << path_prefix+bwPath << std::endl;
        return 1;
    }
    cout << "---- end parse example input" << endl;

    //let's add a few custom attributes
    string codename = "marsupial";
    int r = 15;
    n->attrib["codename"]=(void*)&codename;
    n->attrib["rack_no"]=(void*)&r;
    n->attrib["unknown_will_not_be_printed"]=(void*)&topoPath;

    My_core_attributes c1_attrib(38.222, 2000000000);
    Core* c1 = (Core*)n->FindSubcomponentById(1, SYS_SAGE_COMPONENT_CORE);
    c1->attrib["my_core_info"]=(void*)&c1_attrib;

    My_core_attributes c4_attrib(44.1, 1500000000);
    Core* c4 = (Core*)n->FindSubcomponentById(4, SYS_SAGE_COMPONENT_CORE);
    c4->attrib["my_core_info"]=(void*)&c4_attrib;

    string benchmark_info="measured with no load on 07.07.";
    Numa* n2 = (Numa*)n->FindSubcomponentById(2, SYS_SAGE_COMPONENT_NUMA);
    DataPath * dp = (*(n2->GetDataPaths(SYS_SAGE_DATAPATH_INCOMING)))[0];
    dp->attrib["info"]=(void*)&benchmark_info;
    dp = (*(n2->GetDataPaths(SYS_SAGE_DATAPATH_INCOMING)))[2];
    dp->attrib["info"]=(void*)&benchmark_info;

    //export to xml
    string output_name = "sys-sage_custom_attributes.xml";
    cout << "-------- Exporting as XML to " << output_name << " --------" << endl;
    exportToXml(n, output_name, print_my_attribs);

    return 0;
}
