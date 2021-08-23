#ifndef HWLOC
#define HWLOC

#include <vector>
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "Topology.hpp"

int addParsedHwlocTopo(Topology* t, std::string topoPath, int nodeId);
int xmlProcessChildren(Component* c, xmlNode* parent, int level);
Component* createChildC(std::string type, xmlNode* node);
std::string xmlGetPropStr(xmlNode* node, std::string key);
extern std::vector<std::string> xmlRelevantObjectTypes;
extern std::vector<std::string> xmlRelevantNames;


#endif
