
#include <iostream>
#include <algorithm>

#include "hwloc.hpp"

using namespace std;

vector<string> xmlRelevantNames
{
    "topology",
    "object"
};

vector<string> xmlRelevantObjectTypes
{
    "Machine",
    "Package",
    "Cache",
    "L3Cache",
    "L2Cache",
    "L1Cache",
    "NUMANode",
    "Core",
    "PU"
};



string xmlGetPropStr(xmlNode* node, string key)
{
    string val;
    xmlChar* valX = xmlGetProp(node, (xmlChar *)key.c_str());
    if(valX == NULL)
        val = "";
    else
        val = reinterpret_cast< char const* >(valX);
    xmlFree(valX);
    return val;
}

Component* createChildC(string type, xmlNode* node)
{
    Component* c = NULL;
    string s;
    if(!type.compare("Machine"))
    {
        c = (Component*)new Node();
    }
    else if(!type.compare("Package"))
    {
        s = xmlGetPropStr(node, "os_index");
        int id = stoi(s.empty()?"0":s);
        c = (Component*)new Chip(id);
    }
    else if(!type.compare("Cache") || !type.compare("L3Cache") || !type.compare("L2Cache") || !type.compare("L1Cache"))
    {
        s = xmlGetPropStr(node, "gp_index");
        int id = stoi(s.empty()?"0":s);
        s = xmlGetPropStr(node, "cache_size");
        unsigned long long size = stoul(s.empty()?"0":s);
        s = xmlGetPropStr(node, "depth");
        int cache_level = stoi(s.empty()?"0":s);
        s = xmlGetPropStr(node, "cache_associativity");
        int associativity = stoi(s.empty()?"0":s);

        c = (Component*)new Cache(id, cache_level, size, associativity);
    }
    else if(!type.compare("NUMANode"))
    {
        s = xmlGetPropStr(node, "os_index");
        int id = stoi(s.empty()?"0":s);
        s = xmlGetPropStr(node, "local_memory");
        long long size = stol(s.empty()?"0":s);

        c = (Component*)new Numa(id, size);
    }
    else if(!type.compare("Core"))
    {
        s = xmlGetPropStr(node, "os_index");
        int id = stoi(s.empty()?"0":s);
        c = (Component*)new Core(id);
    }
    else if(!type.compare("PU"))
    {
        s = xmlGetPropStr(node, "os_index");
        int id = stoi(s.empty()?"0":s);
        //cout << "adding thread " << id << endl;
        c = (Component*)new Thread(id);
    }
    else
    {
        c = new Component();
    }
    return c;
}

int xmlProcessChildren(Component* c, xmlNode* parent, int level)
{
    for (xmlNode* child = parent->children; child; child = child->next)
    {
        if(child->type == XML_ELEMENT_NODE)
        {
            string name(reinterpret_cast< char const* >(child->name));

            //interested in object or topology nodes
            if(find(xmlRelevantNames.begin(), xmlRelevantNames.end(), name) != xmlRelevantNames.end())
            {
                string type = xmlGetPropStr(child, "type");

                // for (int i = 0; i < level; ++i)
                //     cout << " " ;
                // cout << name <<  " - " << type << endl;

                //if relevant object, it will be inserted in the topology
                if(find(xmlRelevantObjectTypes.begin(), xmlRelevantObjectTypes.end(), type) != xmlRelevantObjectTypes.end())
                {
                    Component * childC;
                    if(!type.compare("Machine")) //node is already existing param
                    {
                        childC = c;
                        //cout << "already inserted " << type << endl;
                    }
                    else
                    {
                        //cout << "inserting " << type << " to " << c->GetName() << endl;
                        childC = createChildC(type, child);
                        c->InsertChild(childC);
                    }
                    xmlProcessChildren(childC, child, level+1);
                }
                else
                {
                    //cout << "not inserting " << type << " to " << c->GetName() << endl;
                    xmlProcessChildren(c, child, level+1);
                }
            }
        }
    }
    return 0;
}

//parses a hwloc output and adds it to topology
int parseHwlocOutput(Node* n, string topoPath)
{
    xmlDoc *document = xmlReadFile(topoPath.c_str(), NULL, 0);
    if (document == NULL) {
        cerr << "error: could not parse file " << topoPath.c_str() << endl;
        xmlFreeDoc(document);
        return 1;
    }

    xmlNode *root= xmlDocGetRootElement(document);

    int err = xmlProcessChildren((Component*)n, root, 0);

    xmlFreeDoc(document);
    return err;
}
