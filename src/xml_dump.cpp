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
    xmlNewProp(n, (const unsigned char *)"name", (const unsigned char *)(std::to_string(id)).c_str());
    xmlNewProp(n, (const unsigned char *)"name", (const unsigned char *)name.c_str());


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
    xmlNodePtr components_root = xmlNewNode(NULL, BAD_CAST "components");
    xmlNodeSetContent(components_root, BAD_CAST "content");

    xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");

    xmlNodePtr n = root->CreateXmlSubtree();
    xmlAddChild(components_root, n);


    xmlDocSetRootElement(doc, components_root);

    /*
     * Dump the document to a buffer and print it
     * for demonstration purposes.
     */
    xmlChar *xmlbuff;
    int buffersize;
    xmlDocDumpFormatMemory(doc, &xmlbuff, &buffersize, 1);
    printf("%s", (char *) xmlbuff);

    /*
     * Free associated memory.
     */
    xmlFree(xmlbuff);
    xmlFreeDoc(doc);

    return 0;
}
