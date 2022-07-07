#include <sstream>
#include <functional>

#include "xml_dump.hpp"
#include <libxml/parser.h>

std::function<int(string,void*,string*)> custom_search_attrib_key_fcn = NULL;

int print_attrib(map<string,void*> attrib, xmlNodePtr n)
{
    string attrib_value;
    for (auto const& [key, val] : attrib){
        int ret = 0;
        if(custom_search_attrib_key_fcn != NULL)
            ret=custom_search_attrib_key_fcn(key,val,&attrib_value);
        if(ret==0)
            ret = search_default_attrib_key(key, val, &attrib_value);

        if(ret==1)//attrib found
        {
            xmlNodePtr attrib_node = xmlNewNode(NULL, (const unsigned char *)"Attribute");
            xmlNewProp(attrib_node, (const unsigned char *)"name", (const unsigned char *)key.c_str());
            xmlNewProp(attrib_node, (const unsigned char *)"value", (const unsigned char *)attrib_value.c_str());
            xmlAddChild(n, attrib_node);
        }
    }

    return 1;
}

//for a specific key, return the value as a string to be printed in the xml
int search_default_attrib_key(string key, void* value, string* ret_value_str)
{
    if(!key.compare("CATcos") || !key.compare("CATL3mask"))
    {
        *ret_value_str=std::to_string(*(uint64_t*)value);
        return 1;
    }
    return 0;
}

xmlNodePtr Cache::CreateXmlSubtree()
{
    xmlNodePtr n = Component::CreateXmlSubtree();
    xmlNewProp(n, (const unsigned char *)"cache_level", (const unsigned char *)(std::to_string(cache_level)).c_str());
    xmlNewProp(n, (const unsigned char *)"cache_size", (const unsigned char *)(std::to_string(cache_size)).c_str());
    xmlNewProp(n, (const unsigned char *)"cache_associativity_ways", (const unsigned char *)(std::to_string(cache_associativity_ways)).c_str());
    return n;
}

xmlNodePtr Subdivision::CreateXmlSubtree()
{
    xmlNodePtr n = Component::CreateXmlSubtree();
    xmlNewProp(n, (const unsigned char *)"subdivision_type", (const unsigned char *)(std::to_string(type)).c_str());
    return n;
}

xmlNodePtr Numa::CreateXmlSubtree()
{
    xmlNodePtr n = Component::CreateXmlSubtree();
    if(size > 0)
        xmlNewProp(n, (const unsigned char *)"size", (const unsigned char *)(std::to_string(size)).c_str());
    return n;
}
xmlNodePtr Component::CreateXmlSubtree()
{
    xmlNodePtr n = xmlNewNode(NULL, (const unsigned char *)GetComponentTypeStr().c_str());
    xmlNewProp(n, (const unsigned char *)"id", (const unsigned char *)(std::to_string(id)).c_str());
    xmlNewProp(n, (const unsigned char *)"name", (const unsigned char *)name.c_str());
    std::ostringstream addr;
    addr << this;
    xmlNewProp(n, (const unsigned char *)"addr", (const unsigned char *)(addr.str().c_str()));

    print_attrib(attrib, n);

    for(auto it = begin(children); it != end(children); ++it)
    {
        xmlNodePtr child;
        if((*it)->GetComponentType() == SYS_SAGE_COMPONENT_CACHE)
            child = ((Cache*)*it)->CreateXmlSubtree();
        else if((*it)->GetComponentType() == SYS_SAGE_COMPONENT_NUMA)
            child = ((Numa*)*it)->CreateXmlSubtree();
        else if((*it)->GetComponentType() == SYS_SAGE_COMPONENT_SUBDIVISION)
            child = ((Subdivision*)*it)->CreateXmlSubtree();
        else
            child = (*it)->CreateXmlSubtree();
        xmlAddChild(n, child);
    }

    return n;
}

int exportToXml(Component* root, string path, std::function<int(string,void*,string*)> _custom_search_attrib_key_fcn)
{
    custom_search_attrib_key_fcn=_custom_search_attrib_key_fcn;

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");

    xmlNodePtr sys_sage_root = xmlNewNode(NULL, BAD_CAST "sys-sage");
    xmlDocSetRootElement(doc, sys_sage_root);
    xmlNodePtr components_root = xmlNewNode(NULL, BAD_CAST "components");
    xmlAddChild(sys_sage_root, components_root);
    xmlNodePtr data_paths_root = xmlNewNode(NULL, BAD_CAST "data-paths");
    xmlAddChild(sys_sage_root, data_paths_root);

    //build a tree for Components
    xmlNodePtr n = root->CreateXmlSubtree();
    xmlAddChild(components_root, n);

    //scan all Components for their DataPaths
    vector<Component*> components;
    root->GetSubtreeNodeList(&components);
    std::cout << "Number of components to export: " << components.size() << std::endl;
    for(Component* cPtr : components)
    {
        vector<DataPath*>* dpList = cPtr->GetDataPaths(SYS_SAGE_DATAPATH_INCOMING);
        vector<DataPath*> printed_dp;

        for(DataPath* dpPtr : *dpList)
        {
            //check if previously processed
            if (std::find(printed_dp.begin(), printed_dp.end(), dpPtr) == printed_dp.end())
            {
                xmlNodePtr dp_n = xmlNewNode(NULL, BAD_CAST "datapath");
                std::ostringstream src_addr;
                src_addr << dpPtr->GetSource();
                std::ostringstream target_addr;
                target_addr << dpPtr->GetTarget();
                xmlNewProp(dp_n, (const unsigned char *)"source", (const unsigned char *)(src_addr.str().c_str()));
                xmlNewProp(dp_n, (const unsigned char *)"target", (const unsigned char *)(target_addr.str().c_str()));
                xmlNewProp(dp_n, (const unsigned char *)"bw", (const unsigned char *)(std::to_string(dpPtr->GetBw())).c_str());
                xmlNewProp(dp_n, (const unsigned char *)"latency", (const unsigned char *)(std::to_string(dpPtr->GetLatency())).c_str());
                xmlAddChild(data_paths_root, dp_n);

                print_attrib(dpPtr->attrib, dp_n);

                printed_dp.push_back(dpPtr);
            }
        }
    }

    xmlSaveFormatFileEnc(path=="" ? "-" : path.c_str(), doc, "UTF-8", 1);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return 0;
}
