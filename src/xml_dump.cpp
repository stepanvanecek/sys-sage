#include <sstream>


#include "xml_dump.hpp"
#include <libxml/parser.h>


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

int exportToXml(Component* root, string path)
{
    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");

    xmlNodePtr sys_sage_root = xmlNewNode(NULL, BAD_CAST "sys-sage");
    xmlNodePtr components_root = xmlNewNode(NULL, BAD_CAST "components");
    xmlNodePtr data_paths_root = xmlNewNode(NULL, BAD_CAST "data-paths");
    //xmlNodeSetContent(components_root, BAD_CAST "content");

    xmlDocSetRootElement(doc, sys_sage_root);
    xmlAddChild(sys_sage_root, components_root);
    xmlAddChild(sys_sage_root, data_paths_root);

    xmlNodePtr n = root->CreateXmlSubtree();
    xmlAddChild(components_root, n);

    //scan all Components for their DataPaths
    vector<Component*> components;
    root->GetSubtreeNodeList(&components);
    std::cout << "comp " << components.size() << std::endl;
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
                printed_dp.push_back(dpPtr);
            }
        }
    }

    xmlSaveFormatFileEnc(path=="" ? "-" : path.c_str(), doc, "UTF-8", 1);

    xmlFreeDoc(doc);

    return 0;
}
